#include "batchdownloader.h"
#include "a7105downloader.h"

#include <QDebug>
#include <QFile>

BatchDownloader::BatchDownloader(QObject *parent) : QObject(parent),
    a7105Downloader(nullptr),
    batchStartIndex(0),
    curIdxDownloading(0)
{

}

void BatchDownloader::setA7105Downloader(A7105Downloader *downloader)
{
    a7105Downloader = downloader;
    connect(a7105Downloader, &A7105Downloader::sgnProgressChange,
            this, &BatchDownloader::sltProgressChange);
    connect(a7105Downloader, &A7105Downloader::sgnFailed,
            this, &BatchDownloader::sgnFailed);

}

void BatchDownloader::start(int startIndex, const QStringList &files)
{
    batchStartIndex = startIndex;
    binFiles = files;
    curIdxDownloading = 0;

    startDownloading();
}

void BatchDownloader::stop()
{
    a7105Downloader->stop();
}

void BatchDownloader::sltProgressChange(int progress)
{
    emit sgnProgressChange(curIdxDownloading, progress);
    if (progress == 100) {
        curIdxDownloading++;

        if (curIdxDownloading == binFiles.size()) {
            emit sgnAllDone();
            return;
        } else {
            // next file
            startDownloading();
        }

    }
}

void BatchDownloader::sltFailed()
{
    qWarning() << __PRETTY_FUNCTION__ ;
    emit sgnFailed();
}

void BatchDownloader::startDownloading()
{
    qDebug() << "start downloading " << binFiles[curIdxDownloading];

    if (curIdxDownloading < binFiles.size()) {
        QFile binFile(binFiles[curIdxDownloading]);
        if (!binFile.open(QIODevice::ReadOnly)) {
            emit sgnFailed();
            qWarning() << "BatchDownloader failed to read file " << binFiles[curIdxDownloading];
            return;
        } else {
            QByteArray ba = binFile.readAll();
            a7105Downloader->start(batchStartIndex + curIdxDownloading, ba);
        }
    }
}
