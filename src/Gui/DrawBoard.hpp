//
// Created by lizab on 02/03/2023.
//

#ifndef TEXMACS_DRAWBOARD_H
#define TEXMACS_DRAWBOARD_H

#include <QWidget>
#include <QScrollBar>

namespace texmacs {

    class DrawBoard : public QWidget {

    public:
        explicit DrawBoard(QScrollBar &horizontalScrollBar, QScrollBar &verticalScrollBar, QWidget *parent = nullptr);

        void paintEvent(QPaintEvent *event) override;

        void mousePressEvent(QMouseEvent *event) override;

        void mouseMoveEvent(QMouseEvent *event) override;

        void mouseReleaseEvent(QMouseEvent *event) override;

        void simplify(QList<QPoint> &points);

    private:
        QList<QList<QPoint>> mPoints;
        bool mIsDrawing = false;
        QScrollBar &mHorizontalScrollBar;
        QScrollBar &mVerticalScrollBar;

    };

}


#endif //TEXMACS_DRAWBOARD_H
