#ifndef BATCHDOWNLOADER_H
#define BATCHDOWNLOADER_H

#include <QObject>

class A7105Downloader;

class BatchDownloader : public QObject
{
    Q_OBJECT
public:
    explicit BatchDownloader(QObject *parent = nullptr);
    void setA7105Downloader(A7105Downloader * downloader);

    void start(int startIndex, const QStringList &files);
    void stop();

signals:
    void sgnFailed();
    void sgnAllDone();
    void sgnProgressChange(int fileIndex, int progress);

public slots:
    void sltProgressChange(int progress);
    void sltFailed();

private:
    void startDownloading();

private:
    A7105Downloader *a7105Downloader;
    int batchStartIndex;
    QStringList binFiles;
    int curIdxDownloading;  // index to binFiles, not flash sector
};

#endif // BATCHDOWNLOADER_H
