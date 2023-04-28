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

    class MainWindow : public QMainWindow {

    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);

        void styleTabBar();

        void addTab(ThingyTabInnerWindow *widget);

        inline ThingyTabInnerWindow* currentTab() {
            QWidget *currentWidget = mCentralStackedLayout.currentWidget();
            if (currentWidget == nullptr) {
                return nullptr;
            }
            return dynamic_cast<ThingyTabInnerWindow*>(currentWidget);
        }

    public slots:
        void onTabChanged(int index);

        void onTitleChanged(ThingyTabInnerWindow *innerWidget, QString title);

        void showOnScreenKeyboard();

        void newDocument();

        void newBrowser();

    private:
        QTabBar mTabBar;

        QStackedLayout mCentralStackedLayout;
        QWidget mCentralStackedLayoutContainer;

        QVBoxLayout mWindowLayout;
        QWidget mWindowContainer;

        QHBoxLayout mTabBarLayout;

        QPushButton mNewDocumentButton;
        QPushButton mKeyboardButton;
    };

}


#endif //TEXMACS_WINDOW_HPP