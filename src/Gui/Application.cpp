#include "Application.hpp"

#include <QDebug>
#include <QMessageBox>

using namespace Qt::StringLiterals;

texmacs::Application::Application(int &argc, char **argv) : QApplication(argc, argv), mApplicationThread(QThread::currentThread()) {
    // Set the application informations
    setWindowIcon(QIcon(":/TeXmacs/images/texmacs.png"));
    setApplicationName("TeXmacs");
    setApplicationVersion(TEXMACS_REVISION);
    setOrganizationName("TeXmacs");
    setOrganizationDomain("texmacs.org");
}

void texmacs::Application::addTab(ThingyTabInnerWindow *centralWidget) {
    MainWindow *currentWindow = nullptr;
    QWidget *qtwindow = activeWindow();
    if (qtwindow != nullptr) {
        currentWindow = dynamic_cast<MainWindow*>(qtwindow); // dynamic_cast returns nullptr if the cast is not possible
    }
    if (currentWindow == nullptr) {
        currentWindow = mWindows.emplace_back(new MainWindow());
    }
    currentWindow->addTab(centralWidget);
    currentWindow->show();
}

void texmacs::Application::showSchemeImplementationChooserWidget() {
    mWelcomeWidget = new WelcomeWidget();
    mWelcomeWidget->show();

    auto allSchemes = get_scheme_factories();
    for (auto scheme : allSchemes) {
        mWelcomeWidget->addVersion(QString::fromStdString(scheme));
    }

    connect(mWelcomeWidget, &WelcomeWidget::launch, this, [this](QString version) {
        setWantedSchemeImplementation(version.toStdString());
        mWelcomeWidget->hide();
        showSplashScreenAndLoadTeXMacs();
    });

    connect(&mResourceExtractorThread, &ResourcesExtractor::progress, mWelcomeWidget, &WelcomeWidget::setProgress);
    connect(&mResourceExtractorThread, &ResourcesExtractor::ready, mWelcomeWidget, &WelcomeWidget::setReady);

    mResourceExtractorThread.start();

}

void texmacs::Application::showSplashScreenAndLoadTeXMacs() {
    mSlpashScreen.setPixmap(QPixmap(":/TeXmacs/misc/images/splash.png").scaledToWidth(primaryScreen()->size().width() / 4, Qt::SmoothTransformation));
    mSlpashScreen.show();
    connect(this, &Application::initializationMessage, this, [this](const QString& message) {
        qInfo().noquote() << message;
        mSlpashScreen.showMessage(message, Qt::AlignBottom | Qt::AlignCenter, Qt::black);
    });
    connect(this, &Application::initialized, this, [this]() {
        mSlpashScreen.hide();
    });
    QTimer::singleShot(0, this, SLOT(onApplicationStarted()));
}

void texmacs::Application::resetTeXmacs() {
    emit initializationMessage("Resetting TeXmacs...");
    QDir dir(QDir::homePath() + "/.TeXmacs");
    if (dir.exists()) {
        dir.removeRecursively();
    }
}

void texmacs::Application::initializeEnvironmentVariables() {
    emit initializationMessage("Initializing environment variable...");
    QString texmacsHomePath = QDir::homePath() + "/.TeXmacs";
    QString texmacsPath = QDir::homePath() + "/.TeXmacs/TeXmacs";
    QString texmacsProgsPath = QDir::homePath() + "/.TeXmacs/TeXmacs/progs-";
    QString texmacsPluginsPath = QDir::homePath() + "/.TeXmacs/TeXmacs/plugins";

    set_env("TEXMACS_HOME_PATH", string(texmacsHomePath.toUtf8().constData(), texmacsHomePath.toUtf8().size()));
    set_env("TEXMACS_PATH", string(texmacsPath.toUtf8().constData(), texmacsPath.toUtf8().size()));
    set_env("TEXMACS_PROGS_PATH", string(texmacsProgsPath.toUtf8().constData(), texmacsProgsPath.toUtf8().size()) * scheme().scheme_dialect());
    set_env("TEXMACS_PLUGINS_PATH", string(texmacsPluginsPath.toUtf8().constData(), texmacsPluginsPath.toUtf8().size()));

    original_path= get_env ("PATH");
    load_user_preferences ();
}

void texmacs::Application::initializeScheme() {
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
        use_scheme("S7");
    }

}

void texmacs::Application::onApplicationStarted() {
    try {
        initializeScheme();
    } catch (const std::exception& e) {
        // Open an error dialog
        qDebug() << "catched exception";
        QMessageBox::critical(nullptr, "Error", "Could not initialize Scheme: " + QString(e.what()));
        QApplication::exit(1);
        return;
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
    } catch (const std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize TeXmacs: " + QString(e.what()));
        QApplication::exit(1);
        return;
    }

    emit initializationMessage("Initializing Plugins...");
    try {
        init_plugins();
    } catch (const std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize plugins: " + QString(e.what()));
        QApplication::exit(1);
        return;
    }

    emit initializationMessage("Opening Gui...");
    try {
        int n = 0;
        gui_open(n, nullptr);
    } catch (const std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not open GUI: " + QString(e.what()));
        QApplication::exit(1);
        return;
    }

    emit initializationMessage("Initializing Server...");
    try {
        mServer = new server();
        mServer->init();
    } catch (const std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not initialize server: " + QString(e.what()));
        QApplication::exit(1);
        return;
    }

    emit initializationMessage("Opening Window...");
    try {
        open_window();
    } catch (const std::exception& e) {
        // Open an error dialog
        QMessageBox::critical(nullptr, "Error", "Could not open window: " + QString(e.what()));
        QApplication::exit(1);
        return;
    }

    emit initializationMessage("Initialization complete.");
    emit initialized();
}
