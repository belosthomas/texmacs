#include "Application.hpp"

#include <QDebug>
#include <QMessageBox>

texmacs::Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
    // Set the application informations
    setWindowIcon(QIcon(":/TeXmacs/images/texmacs.png"));
    setApplicationName("TeXmacs");
    setApplicationVersion(TEXMACS_REVISION);
    setOrganizationName("TeXmacs");
    setOrganizationDomain("texmacs.org");

    // Load the splash screen
    loadSplashScreen();

    // When Qt is ready, initialize TeXmacs
    QTimer::singleShot(0, this, SLOT(onApplicationStarted()));
}

void texmacs::Application::addTab(ThingyTabInnerWindow *centralWidget) {
    Window *currentWindow = nullptr;
    QWidget *qtwindow = activeWindow();
    if (qtwindow != nullptr) {
        currentWindow = dynamic_cast<Window *>(qtwindow); // dynamic_cast returns nullptr if the cast is not possible
    }
    if (currentWindow == nullptr) {
        currentWindow = &mWindows.emplace_back();
    }
    currentWindow->addTab(centralWidget);
    currentWindow->show();
}

void texmacs::Application::loadSplashScreen() {
    mSlpashScreen.setPixmap(QPixmap(":/TeXmacs/misc/images/splash.png").scaledToWidth(primaryScreen()->size().width() / 4, Qt::SmoothTransformation));
    mSlpashScreen.show();
    connect(this, &Application::initializationMessage, this, [this](const QString& message) {
        qInfo().noquote() << message;
        mSlpashScreen.showMessage(message, Qt::AlignBottom | Qt::AlignCenter, Qt::black);
    });
    connect(this, &Application::initialized, this, [this]() {
        mSlpashScreen.finish(this->activeWindow());
    });
}

void texmacs::Application::resetTeXmacs() {
    emit initializationMessage("Resetting TeXmacs...");
    QDir dir(QDir::homePath() + "/.TeXmacs");
    if (dir.exists()) {
        dir.removeRecursively();
    }
}

void texmacs::Application::extractResources() {
    emit initializationMessage("Extracting resources...");
    QString destination = QDir::homePath() + "/.TeXmacs";

    // Test if the resources are already extracted
    if (QFile(destination + "/.resources_extracted").exists()) {
        return;
    }
    QDir().mkpath(destination);

    // Count the number of files to extract
    int count = 0;
    for (QDirIterator it(":", QDirIterator::Subdirectories); it.hasNext(); it.next()) {
        count++;
    }

    // Extract the files
    int i = 0;
    for (QDirIterator it(":", QDirIterator::Subdirectories); it.hasNext(); ) {
        QString source_filename = it.next();
        QString destination_filename = source_filename;
        destination_filename.replace(":", destination);
        qDebug() << "Copying " << source_filename << " to " << destination_filename;
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

void texmacs::Application::initializeEnvironmentVariables() {
    emit initializationMessage("Initializing environment variable...");
    QString texmacsHomePath = QDir::homePath() + "/.TeXmacs";
    QString texmacsPath = QDir::homePath() + "/.TeXmacs/TeXmacs";
    QString texmacsProgsPath = QDir::homePath() + "/.TeXmacs/TeXmacs/progs-";

    set_env("TEXMACS_HOME_PATH", string(texmacsHomePath.toUtf8().constData(), texmacsHomePath.toUtf8().size()));
    set_env("TEXMACS_PATH", string(texmacsPath.toUtf8().constData(), texmacsPath.toUtf8().size()));
    set_env("TEXMACS_PROGS_PATH", string(texmacsProgsPath.toUtf8().constData(), texmacsProgsPath.toUtf8().size()) * scheme().scheme_dialect());

    original_path= get_env ("PATH");
    load_user_preferences ();
}

void texmacs::Application::initializeScheme() {
    register_all_scheme();
    auto allSchemes = get_scheme_factories();
    if (allSchemes.empty()) {
        qDebug() << "Error: No scheme factories registered";
        QApplication::exit(1);
    }

    if (!mWantedScemeImplementation.empty()) {
        emit initializationMessage("Initializing Scheme (" + QString::fromStdString(mWantedScemeImplementation) + ")...");
        use_scheme(mWantedScemeImplementation);
    } else {
        emit initializationMessage("Initializing Scheme (" + QString::fromStdString(allSchemes[0]) + " by default)...");
        use_scheme(allSchemes[0]);
    }
}

void texmacs::Application::onApplicationStarted() {
    // resetTeXmacs();
    extractResources();

    try {
        initializeScheme();
    } catch (std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize Scheme: " + QString(e.what()));
        QApplication::exit(1);
    }

    emit initializationMessage("Initializing Environment Variables...");
    initializeEnvironmentVariables();

    emit initializationMessage("Loading Pixmaps...");
    mPixmapManager.loadAll();

    emit initializationMessage("Initializing TeXmacs...");
    try {
        the_et = tuple();
        the_et->obs = ip_observer(path());
        cache_initialize();
        init_texmacs();
    } catch (std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize TeXmacs: " + QString(e.what()));
        QApplication::exit(1);
    }

    emit initializationMessage("Initializing Plugins...");
    try {
        init_plugins();
    } catch (std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize plugins: " + QString(e.what()));
        QApplication::exit(1);
    }

    emit initializationMessage("Opening Gui...");
    try {
        int n = 0;
        gui_open(n, nullptr);
    } catch (std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not open GUI: " + QString(e.what()));
        QApplication::exit(1);
    }

    emit initializationMessage("Initializing Server...");
    try {
        mServer = new server();
    } catch (std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize server: " + QString(e.what()));
        QApplication::exit(1);
    }

    emit initializationMessage("Opening Window...");
    try {
        open_window();
    } catch (std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not open window: " + QString(e.what()));
        QApplication::exit(1);
    }

    emit initializationMessage("Initialization complete.");
    emit initialized();
}
