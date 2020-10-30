#ifndef FORMA7105UPLOADER_H
#define FORMA7105UPLOADER_H

#include <QWidget>
#include <QTime>

namespace Ui {
class FormA7105Uploader;
}

class A7105Uploader;

class FormA7105Uploader : public QWidget
{
    Q_OBJECT

public:
    explicit FormA7105Uploader(QWidget *parent = nullptr);
    ~FormA7105Uploader();
    void setUploader(A7105Uploader *a7105Uploader);

private slots:
    void on_pushButtonUpload_clicked();
    void on_pushButtonStop_clicked();
    void sltProgressChange(int progress);
    void sltFailed();

    void on_pushButtonSaveBin_clicked();

private:
    Ui::FormA7105Uploader *ui;
    A7105Uploader *uploader;
    QTime pingTime;
};

#endif // FORMA7105UPLOADER_H
