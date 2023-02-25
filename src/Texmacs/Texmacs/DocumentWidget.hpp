//
// Created by lizab on 08/02/2023.
//

#ifndef TEXMACS_DOCUMENTWIDGET_H
#define TEXMACS_DOCUMENTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QCoreApplication>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "ScrollableOpenGLWidget.hpp"

#include <iostream>

#include "Plugins/Qt/qt_renderer.hpp"
#include "Plugins/Qt/qt_simple_widget.hpp"

namespace texmacs {

    class DocumentWidget : public ScrollableOpenGLWidget {

        Q_OBJECT
    public:
        DocumentWidget(qt_simple_widget_rep *drawer, QWidget *parent = nullptr) : ScrollableOpenGLWidget(parent), mDrawer(drawer) {
            assert(drawer != nullptr);

            drawer->associatedDocumentWidget = this;

            QTimer *timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), &surface(), SLOT(update()));
            timer->start(16);
        }

        void initializeGL() override {
            std::cout << "initializeGL" << std::endl;

            QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
            f->initializeOpenGLFunctions();

       /*     f->glEnable(GL_MULTISAMPLE);
            f->glEnable(GL_POINT_SMOOTH);
            f->glEnable(GL_LINE_SMOOTH);
            f->glEnable(GL_POLYGON_SMOOTH);
            f->glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            f->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
            f->glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
*/
            f->glViewport(0, 0, width(), height());
        }

        void resizeGL(int w, int h) override {
            std::cout << "resizeGL" << std::endl;
            QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
            f->glViewport(0, 0, w, h);
        }

        void paintGL(QPainter &painter) override {
            std::cout << "paintEvent" << std::endl;

            // Enable antialiasing
            painter.setRenderHint(QPainter::Antialiasing, true);

            // Fix blurry images on high DPI screens
            painter.setRenderHint(QPainter::SmoothPixmapTransform, true);


            qt_renderer_rep ren(&painter, painter.device()->width(), painter.device()->height());
            mDrawer->invalidate_all();
            mDrawer->repaint_invalid_regions(&ren);

        }

    private:
        qt_simple_widget_rep *mDrawer;


    };

}


#endif //TEXMACS_DOCUMENTWIDGET_H
