#ifndef TEXMACS_PIXMAPMANAGER_HPP
#define TEXMACS_PIXMAPMANAGER_HPP

#include <QPixmap>
#include <QIcon>
#include <QDir>
#include <QMap>
#include <QDirIterator>
#include <QRegularExpression>

namespace texmacs {

    class PixmapManager {

    public:
        PixmapManager(QString path=":/TeXmacs/misc/pixmaps") : mPath(path) {

        }

        void loadAll() {
            loadAll(QStringList() << "*.svg");
            loadAll(QStringList() << "*_x4.png");
            loadAll(QStringList() << "*_x2.png");
            loadAll(QStringList() << "*.png");
            loadAll(QStringList() << "*.xpm");
        }

        void loadAll(QStringList filters) {
            QDirIterator it(mPath, filters, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                load(it.next());
            }
        }

        void load(QString path) {
            QString name = QFileInfo(path).baseName();
            // name can have the form tm_prefs_convert_x4, or tm_prefs_convert_x2 of tm_prefs_convert
            // we want to keep only the first part
            name = name.replace(QRegularExpression("_x[24]$"), "");
            if (mPixmaps.contains(name)) {
                return;
            }
            qDebug() << "Loading pixmap " << name << " from " << path;
            QIcon pixmap(path);
            if (pixmap.isNull() || pixmap.pixmap(QSize(64,64)).isNull()) {
                qDebug() << "Failed to load pixmap " << name << " from " << path;
                return;
            }
            mPixmaps[name] = pixmap;
        }

        QIcon getIcon(QString name) const {
            name = name.replace(QRegularExpression("\\.xpm$"), "");
            if (mPixmaps.contains(name)) {
                return mPixmaps[name];
            }
            qDebug() << "Can't find icon " << name;
            return QIcon();
        }

        QPixmap getPixmap(QString name, QSize size) const {
            name = name.replace(QRegularExpression("\\.xpm$"), "");
            if (mPixmaps.contains(name)) {
                return mPixmaps[name].pixmap(size);
            }
            qDebug() << "Can't find pixmap " << name;
            return QPixmap();
        }

    private:
        QString mPath;
        QMap<QString, QIcon> mPixmaps;
    };

}

#endif
