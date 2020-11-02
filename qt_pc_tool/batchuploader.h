#ifndef BATCHUPLOADER_H
#define BATCHUPLOADER_H

#include <QObject>
#include <QVector>

class A7105Uploader;

class BatchUploader : public QObject
{
    Q_OBJECT
public:
    explicit BatchUploader(QObject *parent = nullptr);

    void setUploader(A7105Uploader *uploader);

    void start(int startIndex, int count);
    void stop();

signals:
    void sgnFailed();
    void sgnAllDone();
    void sgnProgressChange(int picIndex, int progress);

public slots:
    void sltProgressChange(int progress);
    void sltFailed();

private:
    void startUploading();

private:
    A7105Uploader *a7105Uploader;
    int uploadStartIndex;
    int uploadCount;
    int curIdxTask; // index in tasks, [0,uploadcount-1]

    QVector<QByteArray> bins;   // same size as uploadCount

};

#endif // BATCHUPLOADER_H
