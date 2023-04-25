//
// Created by lizab on 06/02/2023.
//

#include "MainWindow.hpp"
#include "Application.hpp"

texmacs::MainWindow::MainWindow(QWidget *parent) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    mWindowContainer.setLayout(&mWindowLayout);
    setCentralWidget(&mWindowContainer);

    mWindowLayout.setContentsMargins(0, 0, 0, 0);
    mWindowLayout.setSpacing(0);

    mWindowLayout.addLayout(&mTabBarLayout);

    styleTabBar();
    mTabBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mTabBarLayout.addWidget(&mTabBar);

    mNewDocumentButton.setText("+");
    mNewDocumentButton.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(&mNewDocumentButton, &QPushButton::clicked, this, &MainWindow::newDocument);
    mTabBarLayout.addWidget(&mNewDocumentButton);

    //mKeyboardButton.setText("Show On-Screen Keyboard");
    //connect(&mKeyboardButton, &QPushButton::clicked, this, &MainWindow::showOnScreenKeyboard);
    mTabBarLayout.addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    //mTabBarLayout.addWidget(&mKeyboardButton);

    mCentralStackedLayoutContainer.setLayout(&mCentralStackedLayout);
    mCentralStackedLayoutContainer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mWindowLayout.addWidget(&mCentralStackedLayoutContainer);

    // Disable the title bar
    // setWindowFlags(Qt::FramelessWindowHint);
    setUnifiedTitleAndToolBarOnMac(true);

    // Set the window size
    setMinimumSize(800, 600);

    // Connect the tab changed signal
    connect(&mTabBar, &QTabBar::currentChanged, this, &MainWindow::onTabChanged);
}

void texmacs::MainWindow::styleTabBar() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    QScreen *screen = QGuiApplication::primaryScreen();
    double dpi = screen->physicalDotsPerInch();

    // Set the tab bar style size
    mTabBar.setStyleSheet("QTabBar::tab {"
                          "padding: " + QString::number(8 * dpi / 96) + "px; "
                          "margin-top: " + QString::number(8 * dpi / 96) +  "px;"
                          " max-width: " + QString::number(200 * dpi / 96) + "px;"
                          " min-width: " + QString::number(100 * dpi / 96) + "px;"
                          "border-right: 1px solid #e0e0e0;"
                          "border-bottom: 1px solid #e0e0e0;"
                          " background-color: transparent;"
                          "}"
                          "QTabBar::tab:selected { "
                          "background-color: #e0e0e0;"
                          "border-bottom: 0px solid transparent;"
                          " }");

    mNewDocumentButton.setStyleSheet("QPushButton {"
                                     "padding: " + QString::number(8 * dpi / 96) + "px; "
                                     "margin-top: " + QString::number(8 * dpi / 96) +  "px;"
                                     "border-right: 1px solid #e0e0e0;"
                                     "border-bottom: 1px solid #e0e0e0;"
                                     " background-color: transparent;"
                                     "}"
                                     "QPushButton:hover {"
                                     "background-color: #e0e0e0;"
                                     "}"
                                     "QPushButton:pressed {"
                                     "background-color: #c0c0c0;"
                                     "}");

}

void texmacs::MainWindow::addTab(ThingyTabInnerWindow *widget) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    mCentralStackedLayout.addWidget(widget);
    mTabBar.addTab(widget->title());

    connect(widget, &ThingyTabInnerWindow::titleChanged, this, &MainWindow::onTitleChanged);

    mTabBar.setCurrentIndex(mTabBar.count() - 1);
}

void texmacs::MainWindow::onTabChanged(int index) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    auto widget = dynamic_cast<ThingyTabInnerWindow *>(mCentralStackedLayout.currentWidget());
    mCentralStackedLayout.setCurrentIndex(index);
    widget = dynamic_cast<ThingyTabInnerWindow *>(mCentralStackedLayout.widget(index));
    if (widget != nullptr) {
        setWindowTitle(widget->title());
    }
}

void texmacs::MainWindow::onTitleChanged(ThingyTabInnerWindow *innerWidget, QString title) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    setWindowTitle(title);
}

void texmacs::MainWindow::showOnScreenKeyboard() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

}

void texmacs::MainWindow::newDocument() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    open_window();
}

void texmacs::MainWindow::newBrowser() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    WebBrowser *browser = new WebBrowser(this);
    browser->addStackedWidget(new DrawBoard(*browser->horizontalScrollBar(), *browser->verticalScrollBar(), browser));
    addTab(browser);
}
