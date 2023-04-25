//
// Created by lizab on 21/02/2023.
//

#include "ThingyTabInnerWindow.hpp"
#include "Application.hpp"

#include <QScrollBar>
#include <QPainter>
#include <QBoxLayout>
#include <QPaintEvent>
#include <QStyle>
#include <QApplication>
#include <QScroller>

texmacs::ThingyTabInnerWindow::ThingyTabInnerWindow(QWidget *parent) : QWidget(parent), mTitle("Untitled") {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    setLayout(&mOuterWindowLayout);
    mOuterWindowLayout.addWidget(&mCentralWindow);

    mCentralWindow.setCentralWidget(&mInnerWindowWidget);
    mInnerWindowWidget.setLayout(&mInnerWindowLayout);
    mInnerWindowLayout.addWidget(&mScrollArea);

    mCentralWindow.setCentralWidget(&mInnerWindowWidget);
    mScrollArea.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mCentralWindow.show();

    mLayout.setStackingMode(QStackedLayout::StackAll);
    mLayout.setContentsMargins(0, 0, 0, 0);
    mLayoutContainer.setLayout(&mLayout);
    mScrollArea.setViewport(&mLayoutContainer);

    // kinetic scrolling
    mScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QScroller::grabGesture(&mScrollArea, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(&mScrollArea, QScroller::TouchGesture);

    // when scrolling, update the central widget
    connect(mScrollArea.horizontalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        if (mCentralWidget != nullptr) {
            mCentralWidget->update();
        }
    });
    connect(mScrollArea.verticalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        if (mCentralWidget != nullptr) {
            mCentralWidget->update();
        }
    });
}

void texmacs::ThingyTabInnerWindow::setOrigin(QPoint newOrigin) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

   mScrollArea.horizontalScrollBar()->setSliderPosition(newOrigin.x());
   mScrollArea.verticalScrollBar()->setSliderPosition(newOrigin.y());

    if (mCentralWidget != nullptr) {
        mCentralWidget->update();
    }
}

void texmacs::ThingyTabInnerWindow::setExtents (QRect newExtents ) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    //QWidget *_viewport = QAbstractScrollArea::viewport();
    //cout << "Inside  " << _viewport->width() << ", " << _viewport->height() << "\n";
    //cout << "Extents " << newExtents.width() << ", " << newExtents.height() << "\n";
    if (newExtents.width()  < 0) newExtents.setWidth (0);
    if (newExtents.height() < 0) newExtents.setHeight(0);
    if (p_extents != newExtents) {
        p_extents = newExtents;
        updateScrollBars();
    }

    if (mCentralWidget != nullptr) {
        mCentralWidget->update();
    }
}

/*! Scrolls contents so that the given point is visible. */
void texmacs::ThingyTabInnerWindow::ensureVisible (int cx, int cy, int mx, int my ) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    QWidget *_viewport = mScrollArea.viewport();
    int w = _viewport->width();
    int h = _viewport->height();

    int dx = - p_origin.x();
    int dy = - p_origin.y();
    int cw = p_extents.width();
    int ch = p_extents.height();

    if (w < mx * 2) mx = w / 2;
    if (h < my * 2) my = h / 2;

    if (cw <= w) { mx = 0; dx = 0; }

    if (ch <= h) { my = 0; dy = 0; }

    if (cx < mx - dx) dx = mx - cx;
    else if (cx >= w - mx - dx) dx  = w - mx - cx;

    if (cy < my - dy) dy = my - cy;
    else if (cy >= h - my - dy) dy  = h - my - cy;

    if (dx > 0) dx = 0;
    else if (dx < w - cw && cw > w) dx = w - cw;

    if (dy > 0) dy = 0;
    else if (dy < h - ch && ch > h) dy = h - ch;

    setOrigin (QPoint(-dx, -dy));

    if (mCentralWidget != nullptr) {
        mCentralWidget->update();
    }
}

/*! Scrollbar stabilization */
void texmacs::ThingyTabInnerWindow::updateScrollBars() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    QWidget *_viewport = mScrollArea.viewport();
    QScrollBar *_hScrollBar = mScrollArea.horizontalScrollBar();
    QScrollBar *_vScrollBar = mScrollArea.verticalScrollBar();

    int xw = p_extents.width();
    int xh = p_extents.height();
    int w  = _viewport->width() ; // -2
    int h  = _viewport->height(); // -2
    int sbw= qApp->style()->pixelMetric (QStyle::PM_ScrollBarExtent);
    if (_hScrollBar->maximum() > _hScrollBar->minimum()) h += sbw;
    if (_vScrollBar->maximum() > _vScrollBar->minimum()) w += sbw;
    if (xw > w) h -= sbw;
    if (xh > h) w -= sbw;
    /*if (!editor_flag) {
        if (xw < w) xw= w;
        if (xh < h) xh= h;
    }*/

    int cw = (xw > w ? xw - w : 0);
    if (_hScrollBar->sliderPosition() > cw)
        _hScrollBar->setSliderPosition(cw);
    _hScrollBar->setRange(0, cw);
    _hScrollBar->setSingleStep((w >> 4) + 1);
    _hScrollBar->setPageStep(w);

    int ch = (xh > h ? xh - h : 0);
    if (_vScrollBar->sliderPosition() > ch)
        _vScrollBar->setSliderPosition(ch);
    _vScrollBar->setRange(0, ch);
    _vScrollBar->setSingleStep((h >> 4) + 1);
    _vScrollBar->setPageStep(h);

    //surface().setMinimumWidth (w < xw? w: xw);
    //surface().setMinimumHeight(h < xh? h: xh);

    // we may need a relayout if the surface width is changed
    //updateGeometry();
    if (mCentralWidget != nullptr) {
        mCentralWidget->update();
    }
}

/*! Scroll area updater */
void texmacs::ThingyTabInnerWindow::scrollContentsBy (int dx, int dy ) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    if (dx) p_origin.setX(p_origin.x() - dx);
    if (dy) p_origin.setY(p_origin.y() - dy);

    if (mCentralWidget != nullptr) {
        mCentralWidget->update();
    }
}

const QString &texmacs::ThingyTabInnerWindow::title() const {
    return mTitle;
}

void texmacs::ThingyTabInnerWindow::setCentralWidget(QWidget *widget) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    if (mCentralWidget != nullptr) {
        mLayout.removeWidget(mCentralWidget);
    }

    mCentralWidget = widget;
    mCentralWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mLayout.addWidget(mCentralWidget);
}

void texmacs::ThingyTabInnerWindow::addStackedWidget(QWidget *widget) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();

    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mLayout.addWidget(widget);
    mLayout.setCurrentWidget(widget);
    updateFocus();
}

QWidget *texmacs::ThingyTabInnerWindow::centralWidget() const {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    return mCentralWidget;
}

QMenuBar *texmacs::ThingyTabInnerWindow::menuBar() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    return mCentralWindow.menuBar();
}

void texmacs::ThingyTabInnerWindow::addToolBar(QToolBar *toolBar) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mCentralWindow.addToolBar(toolBar);
}

void texmacs::ThingyTabInnerWindow::addToolBarBreak() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    mCentralWindow.addToolBarBreak();
}

void texmacs::ThingyTabInnerWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockWidget) {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    // mCentralWindow.addDockWidget(area, dockWidget);
}

QStatusBar *texmacs::ThingyTabInnerWindow::statusBar() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    return mCentralWindow.statusBar();
}

QString texmacs::ThingyTabInnerWindow::switchStack() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    mCurrentIndex = (mCurrentIndex + 1) % mLayout.count();
    updateFocus();
    return mLayout.widget(mCurrentIndex)->objectName();
}

void texmacs::ThingyTabInnerWindow::updateFocus() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    for (int i = 0; i < mLayout.count(); i++) {
        mLayout.widget(i)->setAttribute(Qt::WA_TransparentForMouseEvents, i != mCurrentIndex);
    }
    mLayout.widget(mCurrentIndex)->setFocus();
}

QPoint texmacs::ThingyTabInnerWindow::origin () {
    return QPoint(mScrollArea.horizontalScrollBar()->value(), mScrollArea.verticalScrollBar()->value());
}

QRect texmacs::ThingyTabInnerWindow::extents () {
    return p_extents;
}

QScrollBar *texmacs::ThingyTabInnerWindow::horizontalScrollBar() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    return mScrollArea.horizontalScrollBar();
}

QScrollBar *texmacs::ThingyTabInnerWindow::verticalScrollBar() {
    TEXMACS_APP_ASSERT_APPLICATION_THREAD();
    return mScrollArea.verticalScrollBar();
}