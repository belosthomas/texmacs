#include "Application.hpp"
#include "DocumentWidget.hpp"

#include <QDebug>
#include <QMessageBox>

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

void texmacs::Application::withCurrentEditor(std::function<void(edit_interface_rep* e)> f) {
    MainWindow *currentWindow = nullptr;
    QWidget *qtwindow = activeWindow();
    if (qtwindow != nullptr) {
        currentWindow = dynamic_cast<MainWindow*>(qtwindow);
    }
    if (currentWindow == nullptr) {
        return;
    }

    ThingyTabInnerWindow *currentTab = currentWindow->currentTab();

    QWidget *centralWidget = currentTab->centralWidget();

    DocumentWidget *documentWidget = dynamic_cast<DocumentWidget*>(centralWidget);

    if (documentWidget == nullptr) {
        return;
    }

    QTimer::singleShot(0, this, [this, documentWidget, f]() {
        f(documentWidget->editor());
    });

}

void texmacs::Application::showSchemeImplementationChooserWidget() {
    mWelcomeWidget = new WelcomeWidget();
    mWelcomeWidget->show();

    if (wantedSchemeImplementation() == "") {
        auto allSchemes = get_scheme_factories();
        for (auto scheme: allSchemes) {
            mWelcomeWidget->addVersion(QString::fromStdString(scheme));
        }
    } else {
        mWelcomeWidget->addVersion(QString::fromStdString(wantedSchemeImplementation()));
    }

    connect(mWelcomeWidget, &WelcomeWidget::launch, this, [this](QString version) {
        setWantedSchemeImplementation(version.toStdString());
        showSplashScreenAndLoadTeXMacs();
    });

    connect(&mResourceExtractorThread, &ResourcesExtractor::progress, mWelcomeWidget, &WelcomeWidget::setProgress);
    connect(&mResourceExtractorThread, &ResourcesExtractor::ready, mWelcomeWidget, &WelcomeWidget::setReady);

    mResourceExtractorThread.start();

}

void texmacs::Application::showSplashScreenAndLoadTeXMacs() {
    connect(this, &Application::initializationMessage, this, [this](const QString& message) {
        mWelcomeWidget->setStatus(message);
    });
    connect(this, &Application::initialized, this, [this]() {
        mWelcomeWidget->hide();
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

    QString login = qgetenv("USER");
    if (login.isEmpty()) {
        login = qgetenv("USERNAME");
    }
    if (login.isEmpty()) {
        login = qgetenv("LOGNAME");
    }
    if (login.isEmpty()) {
        login = qgetenv("USERDOMAIN");
    }
    if (login.isEmpty()) {
        login = qgetenv("HOSTNAME");
    }
    if (login.isEmpty()) {
        login = "unknown";
    }
    mEnvironment.login = login;

    mEnvironment.homePath = QDir::homePath() + "/.TeXmacs";
    mEnvironment.path = QDir::homePath() + "/.TeXmacs/TeXmacs";
    mEnvironment.progsPath = QDir::homePath() + "/.TeXmacs/TeXmacs/progs-" + QString::fromStdString(std::string(scheme().scheme_dialect().data(), N(scheme().scheme_dialect())));
    mEnvironment.pluginsPath = QDir::homePath() + "/.TeXmacs/TeXmacs/plugins";

    qputenv("TEXMACS_HOME_PATH", mEnvironment.homePath.toUtf8());
    qputenv("TEXMACS_PATH", mEnvironment.path.toUtf8());
    qputenv("TEXMACS_PROGS_PATH", mEnvironment.progsPath.toUtf8());
    qputenv("TEXMACS_PLUGINS_PATH", mEnvironment.pluginsPath.toUtf8());

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

    scheme().install_procedure("texmacs::application::set-initialization-message", [this](texmacs::abstract_scheme* scheme, tmscm args) {
        string status = args->car()->to_string();
        QString qstatus = QString::fromStdString(std::string(status.data(), N(status)));
        emit initializationMessage("Initializing Scheme (" + qstatus + ")...");
        return scheme->tmscm_unspefied();
    }, 1, 0);

    scheme().install_procedure("getlogin", [this](texmacs::abstract_scheme* scheme, tmscm args) {
        string loginString = string(mEnvironment.login.toUtf8().constData(), mEnvironment.login.toUtf8().size());
        return scheme->string_to_tmscm(loginString);
    }, 0, 0);

    scheme().install_procedure("texmacs::application::texmacs-path", [this](texmacs::abstract_scheme* scheme, tmscm args) {
        string pathString = string(mEnvironment.path.toUtf8().constData(), mEnvironment.path.toUtf8().size());
        return scheme->string_to_tmscm(pathString);
    }, 0, 0);

    scheme().install_procedure("texmacs::application::texmacs-progs-path", [this](texmacs::abstract_scheme* scheme, tmscm args) {
        string progsPathString = string(mEnvironment.progsPath.toUtf8().constData(), mEnvironment.progsPath.toUtf8().size());
        return scheme->string_to_tmscm(progsPathString);
    }, 0, 0);

    scheme().install_procedure("texmacs::application::texmacs-plugins-path", [this](texmacs::abstract_scheme* scheme, tmscm args) {
        string pluginsPathString = string(mEnvironment.pluginsPath.toUtf8().constData(), mEnvironment.pluginsPath.toUtf8().size());
        return scheme->string_to_tmscm(pluginsPathString);
    }, 0, 0);

    scheme().install_procedure("texmacs::application::texmacs-home-path", [this](texmacs::abstract_scheme* scheme, tmscm args) {
        string homePathString = string(mEnvironment.homePath.toUtf8().constData(), mEnvironment.homePath.toUtf8().size());
        return scheme->string_to_tmscm(homePathString);
    }, 0, 0);

    string tm_init_file = "$TEXMACS_PROGS_PATH/init-texmacs.scm";
    string my_init_file = "$TEXMACS_PROGS_PATH/my-init-texmacs.scm";

    bench_start ("initialize scheme");
    if (exists (tm_init_file)) exec_file (tm_init_file);
    if (exists (my_init_file)) exec_file (my_init_file);
    bench_cumul ("initialize scheme");
    if (my_init_cmds != "") {
        my_init_cmds= "(begin" * my_init_cmds * ")";
        exec_delayed (scheme_cmd (my_init_cmds), my_init_cmds);
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
