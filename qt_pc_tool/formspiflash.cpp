#include "formspiflash.h"
#include "ui_formspiflash.h"
#include "command.h"

#include <QMessageBox>
#include <QDebug>
#include <QFile>

const int FLASH_PAGE_SIZE = 256;

const int FLASH_ONE_READ_MAX = 16;

const int FLASH_ONE_WRITE_MAX = 16;

const quint32 FLASH_LAST_ADDR = 256*1024 - 1;

FormSPIFlash::FormSPIFlash(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSPIFlash),
    readingChip(false),
    readingAddr(0)
{
    ui->setupUi(this);
}

FormSPIFlash::~FormSPIFlash()
{
    delete ui;
}

void FormSPIFlash::sltFrameReceived(QByteArray frame)
{
    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_FLASH_READ_JEDEC_ID == cmdid) {
        parseJedecId(frame);
    } else if (CMD_FLASH_READ_DATA == cmdid) {
        parseReadData(frame);
    } else if (CMD_FLASH_WRITE_DATA == cmdid) {
        ui->labelFlashStatus->setText("data written");
    } else if (CMD_FLASH_SECTOR_ERASE == cmdid) {
        ui->labelFlashStatus->setText("sector erased");
    }
}

void FormSPIFlash::parseJedecId(const QByteArray &frame)
{
    if (frame.size() != 5) {
        ui->labelFlashStatus->setText("invalid response length");
        return;
    }

    quint8 cmd_status = static_cast<quint8>(frame[1]);
    if (0 != cmd_status) {
        ui->labelFlashStatus->setText("command failed");
        return;
    }

    quint8 manu = static_cast<quint8>(frame[2]);

    quint8 idHigh = static_cast<quint8>(frame[3]);
    quint8 idLow = static_cast<quint8>(frame[4]);

    QString text = QString("Jedec id: %1 %2 %3").arg(manu, 2, 16, QChar(' '))
            .arg(idHigh, 2, 16, QChar(' '))
            .arg(idLow, 2, 16, QChar(' '));

    ui->labelFlashStatus->setText(text);
}

void FormSPIFlash::parseReadData(const QByteArray &frame)
{
    QByteArray data = frame.mid(2);
    qDebug() << "read data: " << QString(data.toHex(' '));

    if (readingChip) {
        baReadData.append(data);

        readingAddr += FLASH_ONE_READ_MAX;
        if (readingAddr <= FLASH_LAST_ADDR) {
            sendReadChipCmd();
        } else {
            // finish
            saveReadChipData();
        }

    } else {




        if (data.size() > 16) {
            ui->labelFlashStatus->setText(QString(data.mid(0, 16).toHex(' ')));
        } else {
            ui->labelFlashStatus->setText(QString(data.toHex(' ')));
        }
    }

}

void FormSPIFlash::sendReadChipCmd()
{
    ui->labelFlashStatus->setText(QString("reading flash %1")
                                  .arg(readingAddr, 6, 16)) ;

    quint8 buf[6];
    buf[0] = CMD_FLASH_READ_DATA;
    buf[1] =  static_cast<quint8> ((readingAddr >> 16) & 0xff);
    buf[2] =  static_cast<quint8> ((readingAddr >> 8) & 0xff);
    buf[3] =  static_cast<quint8> ((readingAddr >> 0) & 0xff);

    buf[4] =  static_cast<quint8> ((FLASH_ONE_READ_MAX >> 8) & 0xff);
    buf[5] =  static_cast<quint8> ((FLASH_ONE_READ_MAX >> 0) & 0xff);

    QByteArray ba((char*)buf, 6);

    emit sgnSendFrame(ba);
}

void FormSPIFlash::saveReadChipData()
{
    readingChip = false;

    QFile binFile("dump.bin");
    if (binFile.open(QIODevice::WriteOnly)) {
        binFile.write(baReadData);
        binFile.close();

        qDebug() << "written to dump.bin";
        ui->labelFlashStatus->setText("written to dump.bin");
    }
}

void FormSPIFlash::on_pushButtonJedecId_clicked()
{
    ui->labelFlashStatus->setText("reading jedec");

    char buf[1];
    buf[0] = static_cast<char>(CMD_FLASH_READ_JEDEC_ID);
    QByteArray ba(buf, 1);

    emit sgnSendFrame(ba);
}

void FormSPIFlash::on_pushButtonReadData_clicked()
{
    QString addr = ui->lineEditReadAddr->text();
    bool ok = false;
    quint32 addrValue = addr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid read address");
        return;
    }

//    if ((addrValue % FLASH_PAGE_SIZE) != 0) {
//        QMessageBox::about(this, "notice", "address will be aligned to page size (256)");
//        addrValue = addrValue - (addrValue % FLASH_PAGE_SIZE);
//    }

//    ui->lineEditReadAddr->setText(QString("%1").arg(addrValue, 2, 16, QChar(' ')));

    quint32 readLen = ui->lineEditReadSize->text().toUInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid read len");
        return;
    }

    if (readLen > FLASH_ONE_READ_MAX) {
        QMessageBox::warning(this, "error",
                             QString("read len should not exceed %1").arg(FLASH_ONE_READ_MAX)
                             );
        return;
    }


    quint8 buf[6];
    buf[0] = CMD_FLASH_READ_DATA;
    buf[1] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[2] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    buf[4] =  static_cast<quint8> ((readLen >> 8) & 0xff);
    buf[5] =  static_cast<quint8> ((readLen >> 0) & 0xff);

    QByteArray ba((char*)buf, 6);

    emit sgnSendFrame(ba);
}

void FormSPIFlash::on_pushButtonReadAll_clicked()
{
    readingChip = true;
    readingAddr = 0;
    baReadData.clear();

    sendReadChipCmd();


}

void FormSPIFlash::on_pushButtonWritePage_clicked()
{
    uint8_t start_value = 0;

    quint32 pageIndex = ui->lineEditPageIndex->text().toUInt();
    qDebug() << "page index: " << pageIndex;



}

void FormSPIFlash::on_pushButtonTest_clicked()
{


}

void FormSPIFlash::on_pushButtonWriteData_clicked()
{
    QString addr = ui->lineEditWriteAddr->text();
    bool ok = false;
    quint32 addrValue = addr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid write address");
        return;
    }

    qDebug() << "writing to addr " << addrValue;

   const  quint16 dataLen = 16;

   quint8 buf[6 + dataLen] = {0};
    buf[0] = CMD_FLASH_WRITE_DATA;
    buf[1] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[2] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    buf[4] =  static_cast<quint8> ((dataLen >> 8) & 0xff);
    buf[5] =  static_cast<quint8> ((dataLen >> 0) & 0xff);

    for (uint8_t i = 0; i < dataLen; i++) {
        buf[6 + i] = (i);
    }

    QByteArray ba((char*)buf, 6 + dataLen);

    emit sgnSendFrame(ba);
}

void FormSPIFlash::on_pushButtonSectorErase_clicked()
{
    QString sectorIndexStr = ui->lineEditSectorEraseIndex->text();
    bool ok = false;
    quint32 sectorIndex = sectorIndexStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid sector index");
        return;
    }

    quint32 addrValue = sectorIndex * 4 * 1024;

    qDebug() << "sector index " << sectorIndex
             << "  addr: 0x " << QString("%1").arg(addrValue, 0, 16);


    quint8 buf[4];
    buf[0] = CMD_FLASH_SECTOR_ERASE;
    buf[1] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[2] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    QByteArray ba((char*)buf, 4);

    emit sgnSendFrame(ba);
}
