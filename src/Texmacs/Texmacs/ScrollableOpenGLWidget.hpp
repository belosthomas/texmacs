//
// Created by lizab on 21/02/2023.
//

#ifndef TEXMACS_SCROLLABLEOPENGLWIDGET_HPP
#define TEXMACS_SCROLLABLEOPENGLWIDGET_HPP

#include <iostream>

#include <QOpenGLWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QPainter>
#include <QAbstractScrollArea>
#include <QWheelEvent>

namespace texmacs {

    class ScrollableOpenGLWidget : public QScrollArea {
        Q_OBJECT

    public:
        class InnerOpenGLWidget : public QOpenGLWidget {

            friend class ScrollableOpenGLWidget;

        public:
            InnerOpenGLWidget(ScrollableOpenGLWidget &parent) : QOpenGLWidget(&parent), mParent(parent) {
                setAttribute(Qt::WA_NoSystemBackground);
                setAttribute(Qt::WA_StaticContents);
                //p_surface->setAttribute(Qt::WA_MacNoClickThrough);
                setAutoFillBackground(false);
                setBackgroundRole(QPalette::NoRole);
                setAttribute(Qt::WA_OpaquePaintEvent);
                setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            }

            void initializeGL() override {
                mParent.initializeGL();
            }

            void resizeGL(int w, int h) override {
                mParent.resizeGL(w, h);
            }

            void paintGL() override {
                if (!isVisible()) {
                    return;
                }

                QPainter painter(this);
                mParent.paintGL(painter);
            }

            // forward events to parent
            void wheelEvent(QWheelEvent *event) override {
                mParent.wheelEvent(event);
            }



        private:
            ScrollableOpenGLWidget &mParent;

        };

    public:
        ScrollableOpenGLWidget(QWidget *parent = nullptr) : QScrollArea(parent), mOpenGLWidget(*this) {
            setViewport(&mOpenGLWidget);
        }

        virtual void initializeGL() = 0;

        virtual void resizeGL(int w, int h) = 0;

        virtual void paintGL(QPainter &painter) = 0;

        InnerOpenGLWidget &surface() {
            return mOpenGLWidget;
        }

        void resizeEvent(QResizeEvent * event) final
        {
            mOpenGLWidget.resizeEvent(event);
        }

        void paintEvent(QPaintEvent *event) final
        {
            mOpenGLWidget.paintEvent(event);
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

        void wheelEvent(QWheelEvent *event) {
            scrollContentsBy(0, event->angleDelta().y() / 8);
        }


    private:
        InnerOpenGLWidget mOpenGLWidget;

        QRect    p_extents;   // The size of the virtual area where things are drawn.
        QPoint    p_origin;   // The offset into that area


    };

}


#endif //TEXMACS_SCROLLABLEOPENGLWIDGET_HPP
