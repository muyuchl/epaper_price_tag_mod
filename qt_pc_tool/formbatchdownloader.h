#ifndef FORMBATCHDOWNLOADER_H
#define FORMBATCHDOWNLOADER_H

#include <QWidget>

namespace Ui {
class FormBatchDownloader;
}

class BatchDownloader;

class FormBatchDownloader : public QWidget
{
    Q_OBJECT

    enum {
        COL_PICTURE_INDEX,
        COL_FILENAME,
        COL_PROGRESS
    };

public:
    explicit FormBatchDownloader(QWidget *parent = nullptr);
    ~FormBatchDownloader();
    void setBatchDownloader(BatchDownloader *downloader);

private slots:
    void on_pushButtonBrowse_clicked();

    void sltFailed();
    void sltAllDone();
    void sltProgressChange(int fileIndex, int progress);

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

private:
    void initTableWidget();

private:
    Ui::FormBatchDownloader *ui;

    QStringList filesToDownload;
    BatchDownloader *batchDownloader;
};

#endif // FORMBATCHDOWNLOADER_H
