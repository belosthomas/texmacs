#include "Application.hpp"
#include <QtWebView>

int main(int argc, char** argv) {
    QtWebView::initialize();
    auto app = texmacs::Application(argc, argv);
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    return app.exec();
}
