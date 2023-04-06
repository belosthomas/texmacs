#ifndef TEXMACS_THINGYTABINNERWINDOW_HPP
#define TEXMACS_THINGYTABINNERWINDOW_HPP

#include <QApplication>
#include <QWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QStatusBar>
#include <QStackedLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QKeyEvent>
#include <QMainWindow>

#include <QQuickPaintedItem>
#include <QQuickItem>

namespace texmacs {

    class ThingyTabInnerWindow : public QWidget {

        Q_OBJECT

        friend class MainWindow;

    public:
        ThingyTabInnerWindow(QWidget *parent);

        const QString &title() const {
            return mTitle;
        }

        void setCentralWidget(QWidget *widget) {

            if (mCentralWidget != nullptr) {
                mLayout.removeWidget(mCentralWidget);
            }

            mCentralWidget = widget;
            mCentralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            mLayout.addWidget(mCentralWidget);
        }

        void addStackedWidget(QWidget *widget) {
            widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            mLayout.addWidget(widget);
            mLayout.setCurrentWidget(widget);
            updateFocus();
        }

        QWidget *centralWidget() const {
            return mCentralWidget;
        }

        QMenuBar *menuBar() {
            return mCentralWindow.menuBar();
        }

        void addToolBar(QToolBar *toolBar) {
            toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            mCentralWindow.addToolBar(toolBar);
        }

        void addToolBarBreak() {
            mCentralWindow.addToolBarBreak();
        }

        void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockWidget) {
            mCentralWindow.addDockWidget(area, dockWidget);
        }

        QStatusBar *statusBar() {
            return mCentralWindow.statusBar();
        }

        QString switchStack() {
            mCurrentIndex = (mCurrentIndex + 1) % mLayout.count();
            updateFocus();
            return mLayout.widget(mCurrentIndex)->objectName();
        }

        void updateFocus() {
            for (int i = 0; i < mLayout.count(); i++) {
                mLayout.widget(i)->setAttribute(Qt::WA_TransparentForMouseEvents, i != mCurrentIndex);
            }
            mLayout.widget(mCurrentIndex)->setFocus();
        }

        QPoint origin () {
            return p_origin;
        }

        QRect extents () {
            return p_extents;
        }

        QScrollBar *horizontalScrollBar() {
            return mScrollArea.horizontalScrollBar();
        }

        QScrollBar *verticalScrollBar() {
            return mScrollArea.verticalScrollBar();
        }

        void setOrigin(QPoint newOrigin);

        void setExtents(QRect newExtents);

        void ensureVisible(int cx, int cy, int mx, int my);

        void updateScrollBars();

        void scrollContentsBy(int dx, int dy);

    public slots:
        void setTitle(const QString &title) {
            mTitle = title;
            emit titleChanged(this, title);
        }

    signals:
        void titleChanged(ThingyTabInnerWindow *innerWidget, const QString &title);

    private:
        QMainWindow mCentralWindow;
        QWidget mInnerWindowWidget;
        QVBoxLayout mOuterWindowLayout, mInnerWindowLayout;

        QString mTitle;
        QWidget *mCentralWidget = nullptr;

        QScrollArea mScrollArea;

        QWidget mLayoutContainer;
        QStackedLayout mLayout;

        int mCurrentIndex = 0;

        QRect    p_extents;   // The size of the virtual area where things are drawn.
        QPoint    p_origin;   // The offset into that area
    };

}

#endif // TEXMACS_THINGYTABINNERWINDOW_HPP