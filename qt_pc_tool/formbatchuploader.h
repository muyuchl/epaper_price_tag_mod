#ifndef FORMBATCHUPLOADER_H
#define FORMBATCHUPLOADER_H

#include <QWidget>

namespace Ui {
class FormBatchUploader;
}

class BatchUploader;

class FormBatchUploader : public QWidget
{
    Q_OBJECT

    enum {
        COL_PICTURE_INDEX,
        COL_PROGRESS
    };
public:
    explicit FormBatchUploader(QWidget *parent = nullptr);
    ~FormBatchUploader();
    void setBatchUploader(BatchUploader *uploader);

private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();

    void sltFailed();
    void sltAllDone();
    void sltProgressChange(int picIndex, int progress);

private:
    void initTableWidget();

private:
    Ui::FormBatchUploader *ui;
    BatchUploader *batchUploader;

    int startIdx;
};

#endif // FORMBATCHUPLOADER_H
