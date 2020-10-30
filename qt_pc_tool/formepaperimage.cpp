#include "formepaperimage.h"
#include "ui_formepaperimage.h"
#include <QDebug>
#include "command.h"
#include <QMessageBox>

FormEpaperImage::FormEpaperImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEpaperImage)
{
    ui->setupUi(this);
}

FormEpaperImage::~FormEpaperImage()
{
    delete ui;
}

void FormEpaperImage::sltFrameReceived(QByteArray frame)
{
    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_RF_DATARECEIVED != cmdid) {
        return ;
    }

    quint8 rfcmdid = static_cast<quint8>(frame.at(1));
    switch (rfcmdid) {
    case RFCMD_EPAPER_FILL_RESP:
        parseEpaperFillResponse(frame);
        break;
    }
}

void FormEpaperImage::on_pushButtonFill_clicked()
{
    ui->labelFillState->setText("wait");

    quint8 fillValue = 0x00;
    if (ui->radioButtonAllF->isChecked()) {
        fillValue = 0xFF;
    }

    quint8 buf[6];
    buf[0] = CMD_RF_TXDATA;
    buf[1] =  static_cast<quint8> (RFCMD_EPAPER_FILL);
    buf[2] = fillValue;

    QByteArray bauart((char*)buf, 3);

    emit sgnSendFrame(bauart);
}

void FormEpaperImage::parseEpaperFillResponse(const QByteArray &frame)
{
    ui->labelFillState->setText("done");
}

void FormEpaperImage::on_pushButtonLoadFromFlash_clicked()
{
    QString addrStr = ui->lineEditFlashLoadAddr->text();
    bool ok = false;
    quint32 addr = addrStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid read address");
        return;
    }

    qDebug() << "load image from flash  addr " << addr;


    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_LOAD_IMAGE_FROM_FLASH;
    buf[2] =  static_cast<quint8> ((addr >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((addr >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((addr >> 0) & 0xff);


    QByteArray ba((char*)buf, 5);

    emit sgnSendFrame(ba);
}

void FormEpaperImage::on_pushButtonEpdOff_clicked()
{
    quint8 buf[6];
    buf[0] = CMD_RF_TXDATA;
    buf[1] =  static_cast<quint8> (RFCMD_EPD_OFF);

    QByteArray bauart((char*)buf, 2);
    emit sgnSendFrame(bauart);
}

void FormEpaperImage::on_pushButtonEpdOn_clicked()
{
    quint8 buf[6];
    buf[0] = CMD_RF_TXDATA;
    buf[1] =  static_cast<quint8> (RFCMD_EPD_ON);

    QByteArray bauart((char*)buf, 2);
    emit sgnSendFrame(bauart);
}
