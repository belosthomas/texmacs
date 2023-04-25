#ifndef TEXMACS_GUI_RESOURCESEXTRACTOR
#define TEXMACS_GUI_RESOURCESEXTRACTOR

#include <QThread>

namespace texmacs {

    class ResourcesExtractor : public QThread {
    Q_OBJECT

    public:
        ResourcesExtractor(QObject *parent = nullptr) : QThread(parent) {}

        void run() final;

    signals:
        void progress(int);
        void ready();

    };

}

#endif