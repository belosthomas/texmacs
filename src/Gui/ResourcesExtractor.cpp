#include "ResourcesExtractor.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

void texmacs::ResourcesExtractor::run() {
    QString destination = QDir::homePath() + "/.TeXmacs";

    // Test if the resources are already extracted
    //if (QFile(destination + "/.resources_extracted").exists()) {
    //    emit mWelcomeWidget->setProgress(100);
    //    emit mWelcomeWidget->setReady();
    //    return;
    //}
    QDir().mkpath(destination);

    // Count the number of files to extract
    int count = 0;
    for (QDirIterator it(":", QDirIterator::Subdirectories); it.hasNext(); it.next()) {
        count++;
    }

    // Extract the files
    int i = 0;
    for (QDirIterator it(":", QDirIterator::Subdirectories); it.hasNext(); ) {
        QString source_filename = it.next();
        QString destination_filename = source_filename;
        destination_filename.replace(":", destination);
        qDebug() << "Copying " << source_filename << " to " << destination_filename;
        if (it.fileInfo().isDir()) {
            QDir().mkpath(destination_filename);
        } else {
            QDir().mkpath(QFileInfo(destination_filename).dir().absolutePath());
            if (!QFile::copy(source_filename, destination_filename)) {
                qDebug() << "Error: Could not copy " << source_filename << " to " << destination_filename;
            }
        }
        i++;
        emit progress(i * 100 / count);
    }

    // Create a dummy file to indicate that the resources have been extracted
    QFile f(destination + "/.resources_extracted");
    f.open(QIODevice::WriteOnly);
    f.close();

    emit progress(100);
    emit ready();
}