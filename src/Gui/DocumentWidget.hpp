//
// Created by lizab on 08/02/2023.
//

#ifndef TEXMACS_DOCUMENTWIDGET_H
#define TEXMACS_DOCUMENTWIDGET_H

#include <QGuiApplication>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QCoreApplication>
#include <QOpenGLFunctions>
#include <QTextEdit>
#include <QInputMethodEvent>
#include <QDebug>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QGestureEvent>

#include "ThingyTabInnerWindow.hpp"

#include <iostream>

class qt_simple_widget_rep;
class edit_interface_rep;

namespace texmacs {

    class DocumentWidget : public QWidget {

    Q_OBJECT
    public:
        DocumentWidget(qt_simple_widget_rep *drawer, edit_interface_rep *editor, ThingyTabInnerWindow *parent);

        void paintEvent(QPaintEvent *event) override {
            if (!isVisible()) {
                return;
            }

            QPainter painter(this);
            paint(painter);
        }

        void paint(QPainter &painter);

        bool event(QEvent *event) override;

        bool eventFilter(QObject *obj, QEvent *event) override;

        bool keyPressEventFilter(QKeyEvent *event);

        void mousePressEvent(QMouseEvent *event) override;

        void mouseReleaseEvent(QMouseEvent *event) override;

        bool inputMethodEventFilter(QInputMethodEvent *event);

        QVariant inputMethodQueryFilter(Qt::InputMethodQuery query) const;


        void mouseMoveEvent (QMouseEvent* event) override;
        void tabletEvent (QTabletEvent* event) override;
        void dragEnterEvent(QDragEnterEvent *event) override;
        void dropEvent(QDropEvent *event) override;

        QPoint origin () {
            return mParent->origin();
        }

        QRect extents () {
            return mParent->extents();
        }

        void setOrigin(QPoint newOrigin) {
            return mParent->setOrigin(newOrigin);
        }

        void setExtents(QRect newExtents) {
            return mParent->setExtents(newExtents);
        }

        void ensureVisible(int cx, int cy, int mx, int my) {
            return mParent->ensureVisible(cx, cy, mx, my);
        }

        void updateScrollBars() {
            return mParent->updateScrollBars();
        }

        void scrollContentsBy(int dx, int dy) {
            return mParent->scrollContentsBy(dx, dy);
        }

        QWidget &surface() {
            return *this;
        }

        QScrollBar *horizontalScrollBar() {
            return mParent->horizontalScrollBar();
        }

        QScrollBar *verticalScrollBar() {
            return mParent->verticalScrollBar();
        }

        inline edit_interface_rep *editor() {
            return mEditor;
        }

    public slots:
        void updateText();

    private:
        ThingyTabInnerWindow *mParent;
        qt_simple_widget_rep *mDrawer;
        edit_interface_rep *mEditor;

        QVBoxLayout mLayout;
        QPlainTextEdit mTextEdit;


    };

}


#endif //TEXMACS_DOCUMENTWIDGET_H
