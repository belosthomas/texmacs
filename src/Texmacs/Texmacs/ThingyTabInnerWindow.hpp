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

namespace texmacs {

    class ThingyTabInnerWindow : public QScrollArea {

        Q_OBJECT

        friend class Window;

    public:
        ThingyTabInnerWindow(QWidget *parent) : QScrollArea(parent), mTitle("Untitled") {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            mTopLayout.setContentsMargins(0, 0, 0, 0);
            mTopLayout.setSpacing(0);

            mTopContainer.setLayout(&mTopLayout);
            mTopContainer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

            mMenuBar.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            mTopLayout.addWidget(&mMenuBar);

            mLayout.setStackingMode(QStackedLayout::StackAll);
            mLayout.setContentsMargins(0, 0, 0, 0);
            mLayoutContainer.setLayout(&mLayout);
            setViewport(&mLayoutContainer);
        }

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
            return &mMenuBar;
        }

        void addToolBar(QToolBar *toolBar) {
            toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            mTopLayout.addWidget(toolBar);
        }

        void addToolBarBreak() {
        }

        void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockWidget) {
            //mTopBar.addDockWidget(area, dockWidget);
        }

        QStatusBar *statusBar() {
            return &mStatusBar;
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

        void setOrigin(QPoint newOrigin);

        void setExtents(QRect newExtents);

        void ensureVisible(int cx, int cy, int mx, int my);

        void updateScrollBars();

        void scrollContentsBy(int dx, int dy) override;

    public slots:
        void setTitle(const QString &title) {
            mTitle = title;
            emit titleChanged(this, title);
        }

    signals:
        void titleChanged(ThingyTabInnerWindow *innerWidget, const QString &title);

    protected:
        QWidget &topWidget() {
            return mTopContainer;
        }

        QStatusBar &bottomWidget() {
            return mStatusBar;
        }

    private:
        QString mTitle;
        QWidget *mCentralWidget = nullptr;

        QVBoxLayout mTopLayout;
        QWidget mTopContainer;

        QMenuBar mMenuBar;
        QStatusBar mStatusBar;

        QWidget mLayoutContainer;
        QStackedLayout mLayout;

        int mCurrentIndex = 0;

        QRect    p_extents;   // The size of the virtual area where things are drawn.
        QPoint    p_origin;   // The offset into that area
    };

}

#endif // TEXMACS_THINGYTABINNERWINDOW_HPP