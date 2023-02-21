#include "Application.hpp"

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
        mSlpashScreen.showMessage(message, Qt::AlignBottom | Qt::AlignCenter, Qt::black);
    });
    connect(this, &Application::initialized, this, [this]() {
        int n = 0;
        init_plugins ();
        gui_open (n, nullptr);
        mServer = new server();
        open_window ();
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

void texmacs::Application::initializeEnvironmentVariables() {
    emit initializationMessage("Initializing environment variable...");
    QString texmacsHomePath = QDir::homePath() + "/.TeXmacs";
    QString texmacsPath = QDir::homePath() + "/.TeXmacs/TeXmacs";

    set_env("TEXMACS_HOME_PATH", string(texmacsHomePath.toUtf8().constData(), texmacsHomePath.toUtf8().size()));
    set_env("TEXMACS_PATH", string(texmacsPath.toUtf8().constData(), texmacsPath.toUtf8().size()));

    original_path= get_env ("PATH");
    load_user_preferences ();
}

void texmacs::Application::initializeScheme() {
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

void texmacs::Application::onApplicationStarted() {
    // resetTeXmacs();
    extractResources();
    initializeEnvironmentVariables();

    the_et     = tuple ();
    the_et->obs= ip_observer (path ());
    cache_initialize ();
    bench_start ("initialize texmacs");
    init_texmacs ();
    bench_cumul ("initialize texmacs");

    initializeScheme();
    emit initialized();
}