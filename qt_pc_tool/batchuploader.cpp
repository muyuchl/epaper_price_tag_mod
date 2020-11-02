#include "batchuploader.h"
#include "a7105uploader.h"

BatchUploader::BatchUploader(QObject *parent) : QObject(parent),
    a7105Uploader(nullptr),
    uploadStartIndex(0),
    uploadCount(0),
    curIdxTask(0)
{

}

void BatchUploader::setUploader(A7105Uploader *uploader)
{
    a7105Uploader = uploader;
    connect(a7105Uploader, &A7105Uploader::sgnFailed,
            this, &BatchUploader::sltFailed);
    connect(a7105Uploader, &A7105Uploader::sgnProgressChange,
            this, &BatchUploader::sltProgressChange);

}

void BatchUploader::start(int startIndex, int count)
{
    uploadStartIndex = startIndex;
    uploadCount = count;
    curIdxTask = 0;

    bins.resize(count);

    if (count) {
        startUploading();
    }
}

void BatchUploader::stop()
{
    uploadStartIndex = 0;
    uploadCount = (0);
    curIdxTask = (0);
    a7105Uploader->stop();
}

void BatchUploader::sltProgressChange(int progress)
{
    if (uploadCount == 0) {
        return;
    }

    emit sgnProgressChange(uploadStartIndex + curIdxTask, progress);

    if (progress == 100) {
        // save bin
        bins[curIdxTask] = a7105Uploader->getBinData();

        if (curIdxTask == uploadCount - 1) {
            emit sgnAllDone();
        } else if (uploadCount > 0) {
            curIdxTask++;
            startUploading();
        }

    }

}

void BatchUploader::sltFailed()
{
    uploadStartIndex = 0;
    uploadCount = (0);
    curIdxTask = (0);

    emit sgnFailed();
}

void BatchUploader::startUploading()
{
    Q_ASSERT(a7105Uploader);

    a7105Uploader->start(uploadStartIndex + curIdxTask);
}
