//
// Created by lizab on 28/02/2023.
//

#ifndef TEXMACS_WEBBROWSER_H
#define TEXMACS_WEBBROWSER_H

#include "ThingyTabInnerWindow.hpp"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QtWebView>
#include <QQuickWidget>

namespace texmacs {

    class WebBrowser : public ThingyTabInnerWindow {

    public:
        WebBrowser(QWidget *parent);
        ~WebBrowser();

        void loadUrl(const QString &url);

    private:
        QQuickWidget mWebEngineView;
        QLineEdit mUrlLineEdit;

        QWidget mLayoutContainer;
        QVBoxLayout mLayout;


    };

}


#endif //TEXMACS_WEBBROWSER_H
