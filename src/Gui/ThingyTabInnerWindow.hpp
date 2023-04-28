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

        const QString &title() const;

        void setCentralWidget(QWidget *widget);

        void addStackedWidget(QWidget *widget);

        QWidget *centralWidget() const;

        QMenuBar *menuBar();

        void addToolBar(QToolBar *toolBar);

        void addToolBarBreak();

        void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockWidget);

        QStatusBar *statusBar();

        QString switchStack();

        void updateFocus();

        QPoint origin ();

        QRect extents ();

        QScrollBar *horizontalScrollBar();

        QScrollBar *verticalScrollBar();

        void setOrigin(QPoint newOrigin);

        void setExtents(QRect newExtents);

        void ensureVisible(int cx, int cy, int mx, int my);

        void updateScrollBars();

        void scrollContentsBy(int dx, int dy);

        bool eventFilter(QObject *obj, QEvent *event) override;

        bool wheelEventFilter(QWheelEvent *event);

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