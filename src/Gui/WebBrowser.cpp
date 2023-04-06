//
// Created by lizab on 28/02/2023.
//

#include "WebBrowser.hpp"

#include <QQuickItem>

texmacs::WebBrowser::WebBrowser(QWidget *parent) : ThingyTabInnerWindow(parent) {

    //mWebEngineView.setSource(QUrl::fromLocalFile(":/TeXmacs/quickwidgets/webview.qml"));
    //mWebEngineView.setResizeMode(QQuickWidget::SizeRootObjectToView);

    mLayout.addWidget(&mUrlLineEdit);
    //mLayout.addWidget(&mWebEngineView);
    mLayoutContainer.setLayout(&mLayout);

    connect(&mUrlLineEdit, &QLineEdit::returnPressed, this, [this](){
        loadUrl(mUrlLineEdit.text());
    });

    loadUrl("https://www.texmacs.org/");
    mUrlLineEdit.setText("https://www.texmacs.org/");

    setCentralWidget(&mLayoutContainer);
}

texmacs::WebBrowser::~WebBrowser() {
}

void texmacs::WebBrowser::loadUrl(const QString &url) {
    //mWebEngineView.rootObject()->setProperty("url", url);
}