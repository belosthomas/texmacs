/******************************************************************************
* MODULE     : Application.hpp
* DESCRIPTION: Main Application of TeXmacs
* COPYRIGHT  : (C) 2023 Liza Belos
*******************************************************************************
* This software falls under the GNU general public license version 3 or later.
* It comes WITHOUT ANY WARRANTY WHATSOEVER. For details, see the file LICENSE
* in the root directory or <http://www.gnu.org/licenses/gpl-3.0.html>.
******************************************************************************/

#ifndef TEXMACS_TEXMACSAPPLICATION_H
#define TEXMACS_TEXMACSAPPLICATION_H

#include <list>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
#include <QSplashScreen>
#include <QTimer>
#include <QScreen>
#include <QDir>
#include <QDirIterator>
#include <QThread>

#include "MainWindow.hpp"
#include "WelcomeWidget.hpp"
#include "PixmapManager.hpp"
#include "WelcomeWidget.hpp"
#include "ResourcesExtractor.hpp"

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

#define TEXMACS_APP_ASSERT_APPLICATION_THREAD() \
    texmacs::getApplication().assertApplicationThread(__FUNCTION__)

namespace texmacs {

    /**
     * @brief The TeXMacs Application class
     * This class is the main class of the application, responsible for the initialization of the application.
     */
    class Application : public QApplication {
        Q_OBJECT

    public:
        Application(int &argc, char **argv);

        /**
         * @brief Get the Qt on which the application is running.
         */
        const QThread &applicationThread() const {
            return *mApplicationThread;
        }

        /**
         * @brief Assert that the current thread is the application thread.
         */
        void assertApplicationThread(QString functionName = "") const {
            qDebug() << "Asserting that " << functionName << " is called from the application thread.";
            if (QThread::currentThread() != mApplicationThread) {
                qDebug() << "Error: " << functionName << " is not called from the application thread.";
               // abort();
            }
        }

        /**
         * @brief Get the pixmap manager.
         */
        const PixmapManager &pixmapManager() const {
            return mPixmapManager;
        }

        /**
         * @brief Set the wanted scheme implementation.
         */
        inline void setWantedSchemeImplementation(const std::string &wantedSchemeImplementation) {
            mWantedScemeImplementation = wantedSchemeImplementation;
        }

        /**
         * @brief Get the wanted scheme implementation.
         */
        inline const std::string &wantedSchemeImplementation() const {
            return mWantedScemeImplementation;
        }

    public slots:
        /**
         * @brief Add a tab to the current window, or to a new window if there is no current window.
         * @param centralWidget The central widget of the tab.
         */
        void addTab(ThingyTabInnerWindow *centralWidget);

        /**
         * @brief Load the welcome widget.
         */
        void showSchemeImplementationChooserWidget();

        /**
         * @brief Load and show the splash screen.
         */
        void showSplashScreenAndLoadTeXMacs();

    signals:
        /**
         * @brief This signal is emitted when the application is ready to be shown.
         * It will close the splash screen and show the main window.
         */
        void initialized();

        /**
         * @brief This signal allows to change the splash screen message.
         */
        void initializationMessage(const QString& message);

    protected:
        /**
         * @brief Reset TeXmacs by removing the ~/.TeXmacs directory.
         */
        void resetTeXmacs();

        /**
         * @brief Initialize the environment variable TEXMACS_*.
         */
        void initializeEnvironmentVariables();

        /**
         * @brief Initialize the scheme interpreter.
         */
        void initializeScheme();

    protected slots:
        /**
         * @brief This function is called when Qt has finish to initialize the application. This function initialize TeXmacs.
         */
        void onApplicationStarted();

    private:
        const QThread *mApplicationThread;
        ResourcesExtractor mResourceExtractorThread;

        WelcomeWidget *mWelcomeWidget;
        QSplashScreen mSlpashScreen;
        server *mServer = nullptr;
        std::list<MainWindow*> mWindows;
        PixmapManager mPixmapManager;
        std::string mWantedScemeImplementation = "";

    };

    inline Application &getApplication() {
        QApplication *app = qApp;
        Application *texmacsApp = dynamic_cast<Application*>(app);
        assert(texmacsApp != nullptr);
        return *texmacsApp;
    }

}

#endif //TEXMACS_TEXMACSAPPLICATION_H
