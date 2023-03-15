//
// Created by lizab on 21/02/2023.
//

#include "ThingyTabInnerWindow.hpp"

#include <QScrollBar>
#include <QPainter>
#include <QBoxLayout>
#include <QPaintEvent>
#include <QStyle>
#include <QApplication>


void texmacs::ThingyTabInnerWindow::setOrigin(QPoint newOrigin) {
    if (newOrigin.x() != p_origin.x())
        QAbstractScrollArea::horizontalScrollBar()->setSliderPosition(newOrigin.x());
    if (newOrigin.y() != p_origin.y())
        QAbstractScrollArea::verticalScrollBar()->setSliderPosition(newOrigin.y());
}

void texmacs::ThingyTabInnerWindow::setExtents (QRect newExtents ) {
    //QWidget *_viewport = QAbstractScrollArea::viewport();
    //cout << "Inside  " << _viewport->width() << ", " << _viewport->height() << "\n";
    //cout << "Extents " << newExtents.width() << ", " << newExtents.height() << "\n";
    if (newExtents.width()  < 0) newExtents.setWidth (0);
    if (newExtents.height() < 0) newExtents.setHeight(0);
    if (p_extents != newExtents) {
        p_extents = newExtents;
        updateScrollBars();
    }
}

/*! Scrolls contents so that the given point is visible. */
void texmacs::ThingyTabInnerWindow::ensureVisible (int cx, int cy, int mx, int my ) {
    QWidget *_viewport = QAbstractScrollArea::viewport();
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
}

/*! Scrollbar stabilization */
void texmacs::ThingyTabInnerWindow::updateScrollBars() {
    QWidget *_viewport = QAbstractScrollArea::viewport();
    QScrollBar *_hScrollBar = QAbstractScrollArea::horizontalScrollBar();
    QScrollBar *_vScrollBar = QAbstractScrollArea::verticalScrollBar();

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
}

/*! Scroll area updater */
void texmacs::ThingyTabInnerWindow::scrollContentsBy (int dx, int dy ) {
    if (dx) p_origin.setX(p_origin.x() - dx);
    if (dy) p_origin.setY(p_origin.y() - dy);
}