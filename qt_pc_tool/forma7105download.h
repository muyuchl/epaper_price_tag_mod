#ifndef FORMA7105DOWNLOAD_H
#define FORMA7105DOWNLOAD_H

#include <QWidget>
#include <QTime>

namespace Ui {
class FormA7105Download;
}

class A7105Downloader;

class FormA7105Download : public QWidget
{
    Q_OBJECT

public:
    explicit FormA7105Download(QWidget *parent = nullptr);
    ~FormA7105Download();

    void setDownloader(A7105Downloader *a7105Downloader);

private slots:
    void sltProgressChange(int progress);
    void sltFailed();
    void on_pushButtonBrowse_clicked();

    void on_pushButtonDownload_clicked();

    void on_pushButtonStop_clicked();

private:
    Ui::FormA7105Download *ui;
    QByteArray baData;
    A7105Downloader *downloader;
    QTime pingTime;
};

#endif // FORMA7105DOWNLOAD_H
