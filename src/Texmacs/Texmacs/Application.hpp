//
// Created by lizab on 04/02/23.
//

#ifndef TEXMACS_TEXMACSAPPLICATION_H
#define TEXMACS_TEXMACSAPPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QSplashScreen>
#include <QTimer>
#include <QScreen>
#include <QDir>
#include <QDirIterator>
#include <QThread>

#include "server.hpp"
#include "string.hpp"
#include "sys_utils.hpp"
#include "data_cache.hpp"
#include "boot.hpp"

extern url    tm_init_file;
extern url    tm_init_buffer_file;
extern string my_init_cmds;
extern string original_path;

extern int geometry_w, geometry_h;
extern int geometry_x, geometry_y;

extern tree the_et;
extern bool texmacs_started;

namespace texmacs {

    class Application : public QApplication {
        Q_OBJECT

    public:
        Application(int &argc, char **argv) : QApplication(argc, argv) {
            setWindowIcon(QIcon(":/TeXmacs/images/texmacs.png"));
            setApplicationName("TeXmacs");
            setApplicationVersion(TEXMACS_REVISION);
            setOrganizationName("TeXmacs");
            setOrganizationDomain("texmacs.org");

            loadSplashScreen();
            QTimer::singleShot(0, this, SLOT(onApplicationStarted()));
        }

    signals:
        void initialized();
        void initializationMessage(const QString& message);

    protected:
        void loadSplashScreen() {
            mSlpashScreen.setPixmap(QPixmap(":/TeXmacs/misc/images/splash.png").scaledToWidth(primaryScreen()->size().width() / 4, Qt::SmoothTransformation));
            mSlpashScreen.show();
            connect(this, &Application::initializationMessage, this, [this](const QString& message) {
                mSlpashScreen.showMessage(message, Qt::AlignBottom | Qt::AlignCenter, Qt::black);
            });
            connect(this, &Application::initialized, this, [this]() {
                int n = 0;
                gui_open (n, nullptr);
                mServer = new server();
                open_window ();
                mSlpashScreen.finish(this->activeWindow());
            });
        }

        void initialize() {
            resetTeXmacs();
            extractResources();
            initializeEnvironmentVariables();
            initializeScheme();
            emit initialized();
        }

        void resetTeXmacs() {
            emit initializationMessage("Resetting TeXmacs...");
            QDir dir(QDir::homePath() + "/.TeXmacs");
            if (dir.exists()) {
                dir.removeRecursively();
            }
        }

        void extractResources() {
            emit initializationMessage("Extracting resources...");
            QString destination = QDir::homePath() + "/.TeXmacs";
            if (QFile(destination + "/.resources_extracted").exists()) {
                return;
            }
            QDir().mkpath(destination);

            int count = 0;
            for (QDirIterator it(":", QDirIterator::Subdirectories); it.hasNext(); it.next()) {
                count++;
            }

            int i = 0;
            for (QDirIterator it(":", QDirIterator::Subdirectories); it.hasNext(); ) {
                QString source_filename = it.next();
                QString destination_filename = source_filename;
                destination_filename.replace(":", destination);
                // qDebug() << "Copying " << source_filename << " to " << destination_filename;
                if (it.fileInfo().isDir()) {
                    QDir().mkpath(destination_filename);
                } else {
                    if (!QFile::copy(source_filename, destination_filename)) {
                        throw std::runtime_error("Could not copy " + source_filename.toStdString() + " to " + destination_filename.toStdString());
                    }
                }
                i++;
                emit initializationMessage("Extracting resources..." + QString::number(i * 100 / count) + "%");
            }
            // Create a dummy file to indicate that the resources have been extracted
            QFile f(destination + "/.resources_extracted");
            f.open(QIODevice::WriteOnly);
            f.close();
        }

        void initializeEnvironmentVariables() {
            emit initializationMessage("Initializing environment variable...");
            QString texmacsHomePath = QDir::homePath() + "/.TeXmacs";
            QString texmacsPath = QDir::homePath() + "/.TeXmacs/TeXmacs";

            set_env("TEXMACS_HOME_PATH", string(texmacsHomePath.toUtf8().constData(), texmacsHomePath.toUtf8().size()));
            set_env("TEXMACS_PATH", string(texmacsPath.toUtf8().constData(), texmacsPath.toUtf8().size()));

            original_path= get_env ("PATH");
            load_user_preferences ();

            the_et     = tuple ();
            the_et->obs= ip_observer (path ());
            cache_initialize ();
            bench_start ("initialize texmacs");
            init_texmacs ();
            bench_cumul ("initialize texmacs");
        }

        void initializeScheme() {
            emit initializationMessage("Initializing Scheme...");
            register_all_scheme();
            auto allSchemes = get_scheme_factories();
            if (allSchemes.size() == 0) {
                qDebug() << "TeXmacs] Error: no scheme factories registered";
                QApplication::exit(1);
            }
            qDebug() << "Found Scheme : ";
            for (auto scheme : allSchemes) {
                qDebug() << " - " << QString::fromStdString(scheme);
            }
            qDebug() << "Using " << QString::fromStdString(allSchemes[0]) << " by default";
            use_scheme(allSchemes[0]);
        }


    protected slots:
        void onApplicationStarted() {
            initialize();
        }

    private:
        QSplashScreen mSlpashScreen;
        server *mServer = nullptr;

    };

}

#endif //TEXMACS_TEXMACSAPPLICATION_H
