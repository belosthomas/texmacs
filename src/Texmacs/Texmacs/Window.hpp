//
// Created by lizab on 06/02/2023.
//

#ifndef TEXMACS_WINDOW_HPP
#define TEXMACS_WINDOW_HPP

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QTabBar>
#include <QStatusBar>
#include <QToolBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QSpacerItem>
#include <QScreen>

#include "ThingyTabInnerWindow.hpp"
#include "WebBrowser.hpp"
#include "DrawBoard.hpp"

#include "new_window.hpp"

namespace texmacs {

    class Window : public QMainWindow {

        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr) {
            mWindowContainer.setLayout(&mWindowLayout);
            setCentralWidget(&mWindowContainer);

            mWindowLayout.setContentsMargins(0, 0, 0, 0);
            mWindowLayout.setSpacing(0);

            mWindowLayout.addLayout(&mTabBarLayout);

            styleTabBar();
            mTabBarLayout.addWidget(&mTabBar);

            mNewDocumentButton.setText("+D");
            connect(&mNewDocumentButton, &QPushButton::clicked, this, &Window::newDocument);
            mTabBarLayout.addWidget(&mNewDocumentButton);

            mNewBrowserButton.setText("+B");
            connect(&mNewBrowserButton, &QPushButton::clicked, this, &Window::newBrowser);
            mTabBarLayout.addWidget(&mNewBrowserButton);

            mModeButton.setText("Mode");
            connect(&mModeButton, &QPushButton::clicked, this, &Window::changeMode);
            mTabBarLayout.addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
            mTabBarLayout.addWidget(&mModeButton);

            mCentralStackedLayoutContainer.setLayout(&mCentralStackedLayout);
            mCentralStackedLayoutContainer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            mWindowLayout.addWidget(&mCentralStackedLayoutContainer);

            // Disable the title bar
            // setWindowFlags(Qt::FramelessWindowHint);
            setUnifiedTitleAndToolBarOnMac(true);

            // Set the window size
            setMinimumSize(800, 600);

            // Connect the tab changed signal
            connect(&mTabBar, &QTabBar::currentChanged, this, &Window::onTabChanged);
        }

        void styleTabBar() {
            QScreen *screen = QGuiApplication::primaryScreen();
            double dpi = screen->physicalDotsPerInch();

            // Set the tab bar style size
            mTabBar.setStyleSheet("QTabBar::tab {"
                                                     "padding: " + QString::number(8 * dpi / 96) + "px; "
                                                     "margin-right: " + QString::number(8 * dpi / 96) +  "px;"
                                                     "border-radius: 0px;"
                                                     " background-color: black;"
                                                     " color: white; "
                                                "}"
                                                "QTabBar::tab:selected { border-bottom: 2px solid red; }");

        }

        void addTab(ThingyTabInnerWindow *widget) {
            mCentralStackedLayout.addWidget(widget);
            mTabBar.addTab(widget->title());

            connect(widget, &ThingyTabInnerWindow::titleChanged, this, &Window::onTitleChanged);

            mTabBar.setCurrentIndex(mTabBar.count() - 1);
        }

    public slots:
        void onTabChanged(int index) {
            auto widget = dynamic_cast<ThingyTabInnerWindow *>(mCentralStackedLayout.currentWidget());
            if (widget != nullptr) {
                mWindowLayout.removeWidget(&widget->topWidget());
                mWindowLayout.removeWidget(&widget->bottomWidget());

                widget->topWidget().setParent(nullptr);
                widget->bottomWidget().setParent(nullptr);

                widget->topWidget().hide();
                widget->bottomWidget().hide();
            }

            mCentralStackedLayout.setCurrentIndex(index);

            widget = dynamic_cast<ThingyTabInnerWindow *>(mCentralStackedLayout.widget(index));
            if (widget != nullptr) {
                widget->topWidget().setParent(&mWindowContainer);
                widget->bottomWidget().setParent(&mWindowContainer);

                mWindowLayout.insertWidget(0, &widget->topWidget());
                mWindowLayout.insertWidget(mWindowLayout.count(), &widget->bottomWidget());

                widget->topWidget().show();
                widget->bottomWidget().show();

                setWindowTitle(widget->title());
            }
        }

        void onTitleChanged(ThingyTabInnerWindow *innerWidget, QString title) {
            setWindowTitle(title);
        }

        void changeMode() {
            ThingyTabInnerWindow *widget = dynamic_cast<ThingyTabInnerWindow *>(mCentralStackedLayout.currentWidget());
            mModeButton.setText(widget->switchStack());
        }

        void newDocument() {
            open_window();
        }

        void newBrowser() {
            WebBrowser *browser = new WebBrowser(this);
            browser->addStackedWidget(new DrawBoard(*browser->horizontalScrollBar(), *browser->verticalScrollBar(), browser));
            addTab(browser);
        }

    private:
        QTabBar mTabBar;

        QStackedLayout mCentralStackedLayout;
        QWidget mCentralStackedLayoutContainer;

        QVBoxLayout mWindowLayout;
        QWidget mWindowContainer;

        QHBoxLayout mTabBarLayout;

        QPushButton mNewDocumentButton;
        QPushButton mNewBrowserButton;
        QPushButton mModeButton;
    };

}


#endif //TEXMACS_WINDOW_HPP
