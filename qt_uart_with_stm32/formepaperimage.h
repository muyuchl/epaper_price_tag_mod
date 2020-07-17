#ifndef FORMEPAPERIMAGE_H
#define FORMEPAPERIMAGE_H

#include <QWidget>

namespace Ui {
class FormEpaperImage;
}

class FormEpaperImage : public QWidget
{
    Q_OBJECT

public:
    explicit FormEpaperImage(QWidget *parent = nullptr);
    ~FormEpaperImage();


public slots:
    void sltFrameReceived(QByteArray frame);

signals:
    void sgnSendFrame(QByteArray frame);

private slots:
    void on_pushButtonFill_clicked();

    void on_pushButtonLoadFromFlash_clicked();

    void on_pushButtonEpdOff_clicked();

    void on_pushButtonEpdOn_clicked();

private:
    void parseEpaperFillResponse(const QByteArray &frame);

private:
    Ui::FormEpaperImage *ui;
};

#endif // FORMEPAPERIMAGE_H
