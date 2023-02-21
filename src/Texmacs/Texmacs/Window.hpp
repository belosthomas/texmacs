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

namespace texmacs {

    class ThingyTabInnerWindow : public QMainWindow {

        Q_OBJECT

    public:
        ThingyTabInnerWindow(QWidget *parent) : QMainWindow(parent), mTitle("Untitled") {

        }

        const QString &title() const {
            return mTitle;
        }

    public slots:
        void setTitle(const QString &title) {
            mTitle = title;
            emit titleChanged(title);
        }

    signals:
        void titleChanged(const QString &title);

    private:
        QString mTitle;
    };

    class Window : public QMainWindow {

        Q_OBJECT

    public:
        Window(QWidget *parent = nullptr) {
            setCentralWidget(&mCentralWidget);

            // Style the tab bar to have a Chrome like look
            mCentralWidget.tabBar()->setStyleSheet("QTabBar::tab { height: 25px; width: 150px; }");
            mCentralWidget.tabBar()->setTabsClosable(true);
            mCentralWidget.tabBar()->setMovable(true);

            // Disable the title bar
            // setWindowFlags(Qt::FramelessWindowHint);
            setUnifiedTitleAndToolBarOnMac(true);

            // Allow the window to be moved from the tab bar
            mCentralWidget.tabBar()->installEventFilter(this);

            // Set the window size
            setMinimumSize(800, 600);

            // Connect the tab changed signal
            connect(&mCentralWidget, &QTabWidget::currentChanged, this, &Window::onTabChanged);

        }

        void addTab(ThingyTabInnerWindow *widget) {
            mCentralWidget.addTab(widget, widget->title());
        }

    public slots:
        void onTabChanged(int index) {
            ThingyTabInnerWindow *widget = dynamic_cast<ThingyTabInnerWindow *>(mCentralWidget.widget(index));
            if (widget == nullptr) {
                return;
            }
            setWindowTitle(widget->title());
        }


    private:
        QTabWidget mCentralWidget;


    };

}


#endif //TEXMACS_WINDOW_HPP
