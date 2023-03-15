//
// Created by lizab on 02/03/2023.
//

#include "DrawBoard.hpp"

#include <QPainter>
#include <QMouseEvent>


texmacs::DrawBoard::DrawBoard(QScrollBar &horizontalScrollBar, QScrollBar &verticalScrollBar, QWidget *parent) : QWidget(parent), mHorizontalScrollBar(horizontalScrollBar), mVerticalScrollBar(verticalScrollBar) {
    setMouseTracking(true);
}

void texmacs::DrawBoard::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 2));

    QPoint scroll(mHorizontalScrollBar.value(), mVerticalScrollBar.value());
    for (auto &points : mPoints) {
        for (int i = 1; i < points.size(); ++i) {
            painter.drawLine(points[i - 1] - scroll, points[i] - scroll);
        }
    }
}

void texmacs::DrawBoard::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mIsDrawing = true;
        mPoints.append(QList<QPoint>());
        mPoints.last().append(event->pos() + QPoint(mHorizontalScrollBar.value(), mVerticalScrollBar.value()));
    }
}

void texmacs::DrawBoard::mouseMoveEvent(QMouseEvent *event) {
    if (mIsDrawing) {
        mPoints.last().append(event->pos() + QPoint(mHorizontalScrollBar.value(), mVerticalScrollBar.value()));
        update();
    }
}

void texmacs::DrawBoard::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mIsDrawing = false;
    }
}

void texmacs::DrawBoard::simplify(QList<QPoint> &points) {
    // todo
}
