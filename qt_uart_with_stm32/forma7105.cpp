#include "forma7105.h"
#include "ui_forma7105.h"

#include <QDebug>
#include <QMessageBox>
#include "command.h"
#include <QFile>
#include <QFileDialog>

const int LONG_READ_WRITE_LEN = 16;

FormA7105::FormA7105(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormA7105),
    longReadAddr(0)
{
    ui->setupUi(this);
}

FormA7105::~FormA7105()
{
    delete ui;
}

void FormA7105::sltFrameReceived(QByteArray frame)
{
    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_RF_DATARECEIVED == cmdid) {
        parseRxedRFFrame(frame);
    }
}

void FormA7105::parseRxedRFFrame(const QByteArray &frame)
{
    quint8 rfcmdid = static_cast<quint8>(frame.at(1));
    switch (rfcmdid) {
    case RFCMD_PING_RESP:
        parsePingResponse(frame);
        break;
    case RFCMD_READ_FLASH_JEDECID_RESP:
        parseJedecIdResponse(frame);
        break;
    case RFCMD_READ_FLASH_DATA_RESP:
        parseReadDataResponse(frame);
        break;
    case RFCMD_FLASH_SECTOR_ERASE_RESP:
        parseSectorEraseResponse(frame);
        break;
    case RFCMD_WRITE_FLASH_DATA_RESP:
        parseWriteDataResponse(frame);
        break;
    case RFCMD_FLASH_CHIP_ERASE_RESP:
        parseChipEraseResponse(frame);
        break;
    default:
        qDebug()<< "unknow rfcmdid: " << rfcmdid;
        break;
    }
}

void FormA7105::parsePingResponse(const QByteArray &frame)
{
    QByteArray ba = frame.mid(2, 4);
    qDebug() << "rf ping response: " << ba.toHex(' ');
    QString str(ba);
    ui->labelPingResponse->setText(str);
}

void FormA7105::parseJedecIdResponse(const QByteArray &frame)
{
    QByteArray ba = frame.mid(2, 3);
    qDebug() << "rf Jedec response: " << ba.toHex(' ');
    QString str(ba.toHex(' '));
    ui->labelFlashJedecId->setText(str);
}

void FormA7105::parseReadDataResponse(const QByteArray &frame)
{
    unsigned char buf[128] = {0};
    memcpy(buf, frame.constData(), qMin(128, frame.size()));

    quint32 addr = (buf[2] << 16)
            + (buf[3] << 8) + buf[4];

    quint16 readLen = (buf[5] << 8) + (buf[6]);

    qDebug() << "addr : " << addr << "  len: " << readLen;

    if (readLen > 32) {
        readLen = 32;
    }

    if (addr >= longReadAddr && addr < longReadAddrEnd) {
        // long reading,
        baLongRead.append(frame.mid(7, readLen));
        longReadAddr += LONG_READ_WRITE_LEN;
        if (longReadAddr >= longReadAddrEnd) {
            qDebug() << "all read end, saving";
            ui->labelStatus->setText("long read finish");
            saveLongReadToFile();
        } else {
            sendLongReadRequest();
        }

    } else {
        QByteArray ba = frame.mid(7, readLen);

        ui->lineEditDataRead->setText(QString(ba.toHex(' ')));
    }


}

void FormA7105::parseSectorEraseResponse(const QByteArray &frame)
{
    if (baLongWriting.size() && writeIndex  == 0) {
        sendLongWriteRequest();
    } else {
        ui->labelSectorEraseState->setText("done");
    }
}

void FormA7105::parseWriteDataResponse(const QByteArray &frame)
{
    qDebug() << "write data, got response";

    if (writeIndex < baLongWriting.size()) {
        sendLongWriteRequest();
    } else {
        baLongWriting.clear();
        writeIndex = 0;
        ui->labelStatus->setText("write 4KB finish");
    }

}

void FormA7105::parseChipEraseResponse(const QByteArray &frame)
{
    ui->labelStatus->setText("chip erase done");
}

void FormA7105::sendLongReadRequest()
{
    ui->labelStatus->setText(QString("reading %1").arg(longReadAddr, 0, 16));

    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_READ_FLASH_DATA;
    buf[2] =  static_cast<quint8> ((longReadAddr >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((longReadAddr >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((longReadAddr >> 0) & 0xff);

    buf[5] =  static_cast<quint8> ((LONG_READ_WRITE_LEN >> 8) & 0xff);
    buf[6] =  static_cast<quint8> ((LONG_READ_WRITE_LEN >> 0) & 0xff);


    QByteArray ba((char*)buf, 7);

    emit sgnSendFrame(ba);
}

void FormA7105::sendLongWriteRequest()
{
    quint32 writeAddrThisTx = longWriteStartAddr + writeIndex;

    ui->labelStatus->setText(QString("writing %1").arg(writeAddrThisTx, 0, 16));


    quint8 buf[80 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_WRITE_FLASH_DATA;
    buf[2] =  static_cast<quint8> ((writeAddrThisTx >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((writeAddrThisTx >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((writeAddrThisTx >> 0) & 0xff);

    buf[5] =  static_cast<quint8> ((LONG_READ_WRITE_LEN >> 8) & 0xff);
    buf[6] =  static_cast<quint8> ((LONG_READ_WRITE_LEN >> 0) & 0xff);

    // fill data
    for (int i = 0; i < LONG_READ_WRITE_LEN; i++) {

        buf[7 + i] = static_cast<quint8>(baLongWriting[writeIndex + i]);
    }

    QByteArray ba((char*)buf, 7 + LONG_READ_WRITE_LEN);

    writeIndex += LONG_READ_WRITE_LEN;

    emit sgnSendFrame(ba);

}

void FormA7105::saveLongReadToFile()
{
    QFile binFile("dump.bin");
    if (binFile.open(QIODevice::WriteOnly)) {
        binFile.write(baLongRead);
        binFile.close();

        qDebug() << "written to dump.bin";
        ui->labelStatus->setText("read data written to dump.bin");
    }
}

void FormA7105::on_pushButtonPing_clicked()
{
    ui->labelPingResponse->setText("");

    QByteArray ba = ui->lineEditPingData->text().toLatin1();
    if (ba.size() < 4) {
        int toAdd = 4 - ba.size();
        for (int i = 0; i < toAdd; i++) {
            ba.append(' ');
        }
    }

    quint8 buf[6];
    buf[0] = CMD_RF_TXDATA;
    buf[1] =  static_cast<quint8> (RFCMD_PING);

    buf[2] =  static_cast<quint8> (ba[0]);
    buf[3] =  static_cast<quint8> (ba[1]);
    buf[4] =  static_cast<quint8> (ba[2]);
    buf[5] =  static_cast<quint8> (ba[3]);

    QByteArray bauart((char*)buf, 6);

    emit sgnSendFrame(bauart);
}

void FormA7105::on_pushButtonReadFlashJedecId_clicked()
{
    ui->labelFlashJedecId->setText("wait");

    quint8 buf[6];
    buf[0] = CMD_RF_TXDATA;
    buf[1] =  static_cast<quint8> (RFCMD_READ_FLASH_JEDECID);

    QByteArray bauart((char*)buf, 2);

    emit sgnSendFrame(bauart);
}

void FormA7105::on_pushButtonReadData_clicked()
{
    ui->lineEditDataRead->setText("");

    QString addrStr = ui->lineEditReadAddr->text();
    bool ok = false;
    quint32 addrValue = addrStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid read address");
        return;
    }

    qDebug() << "reading  addr " << addrValue;

    QString lenStr = ui->lineEditReadLen->text();

    quint32 readLen = lenStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid read len");
        return;
    }
    if (readLen <= 0 || readLen > 32) {
        QMessageBox::warning(this, "error", "read len out of range (1, 32)");
        return;
    }

    longReadAddr = addrValue;
    longReadAddrEnd = longReadAddr + readLen;
    baLongRead.clear();

    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_READ_FLASH_DATA;
    buf[2] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    buf[5] =  static_cast<quint8> ((readLen >> 8) & 0xff);
    buf[6] =  static_cast<quint8> ((readLen >> 0) & 0xff);


    QByteArray ba((char*)buf, 7);

    emit sgnSendFrame(ba);

}

void FormA7105::on_pushButtonRead4KB_clicked()
{
    QString addrStr = ui->lineEditReadAddr->text();
    bool ok = false;
    quint32 addrValue = addrStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid read address");
        return;
    }

    qDebug() << "reading  addr " << addrValue;

    longReadAddr = addrValue;
    longReadAddrEnd = longReadAddr + 4 * 1024;
    baLongRead.clear();

    sendLongReadRequest();
}


void FormA7105::on_pushButtonSectorErase_clicked()
{
    ui->labelSectorEraseState->setText("wait");

    QString addrStr = ui->lineEditSectorEraseAddr->text();
    bool ok = false;
    quint32 addrValue = addrStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid sector address");
        return;
    }

    if ((addrValue & 0xfff) != 0)
    {
        QMessageBox::warning(this, "error", "not 4KB aligned, will ignore lower bits");
        addrValue =  addrValue & 0xfffff000;
        ui->lineEditSectorEraseAddr->setText(QString("%1").arg(addrValue, 0, 16));
    }

    qDebug() << "erase sector  addr " << addrValue;


    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_FLASH_SECTOR_ERASE;
    buf[2] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    QByteArray ba((char*)buf, 5);

    emit sgnSendFrame(ba);
}

void FormA7105::on_pushButtonWriteData_clicked()
{
    QString addrStr = ui->lineEditWriteAddr->text();
    bool ok = false;
    quint32 addrValue = addrStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid write address");
        return;
    }

    qDebug() << "write  addr " << addrValue;


    QString dataStr = ui->lineEditWriteData->text();
    dataStr = dataStr.trimmed();
    dataStr = dataStr.replace(QChar(' '), QLatin1String(""));

    if (dataStr.size() % 2) {
        dataStr = QChar('0') + dataStr;
    }

    ui->lineEditWriteData->setText(dataStr);

    quint16 dataLen = dataStr.size() / 2;


    quint8 buf[80 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_WRITE_FLASH_DATA;
    buf[2] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    buf[5] =  static_cast<quint8> ((dataLen >> 8) & 0xff);
    buf[6] =  static_cast<quint8> ((dataLen >> 0) & 0xff);

    // fill data
    for (int i = 0; i < dataLen; i++) {
        QString strByte = dataStr.mid(2*i, 2);
        quint32 byteValue = strByte.toUInt(&ok, 16);
        buf[7 + i] = static_cast<quint8>(byteValue);
    }

    QByteArray ba((char*)buf, 7 + dataLen);

    emit sgnSendFrame(ba);

}


void FormA7105::on_pushButtonWrite4KB_clicked()
{
    QString addrStr = ui->lineEditWriteAddr->text();
    bool ok = false;
    quint32 addrValue = addrStr.toUInt(&ok, 16);
    if (!ok) {
        QMessageBox::warning(this, "error", "invalid write address");
        return;
    }

    qDebug() << "write  addr " << addrValue;

    QString fileName = QFileDialog::getOpenFileName(this, "Select bin File", QString());
    qDebug() << __PRETTY_FUNCTION__ << fileName;

    if (fileName.isEmpty())
    {
        return;
    }

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        baLongWriting = f.readAll();
        writeIndex = 0;

        longWriteStartAddr = addrValue;


        // erase sector first
        ui->labelStatus->setText(QString("erasing sector"));

        quint8 buf[7 ] = {0};
        buf[0] = CMD_RF_TXDATA;
        buf[1] = RFCMD_FLASH_SECTOR_ERASE;
        buf[2] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
        buf[3] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
        buf[4] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

        QByteArray ba((char*)buf, 5);

        emit sgnSendFrame(ba);
    }

}

void FormA7105::on_pushButtonChipErase_clicked()
{
    ui->labelStatus->setText("erasing flash");

    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_FLASH_CHIP_ERASE;

    QByteArray ba((char*)buf, 2);

    emit sgnSendFrame(ba);
}

void FormA7105::on_pushButtonWriteBin_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Select bin File", QString());
    qDebug() << __PRETTY_FUNCTION__ << fileName;

    if (fileName.isEmpty())
    {
        return;
    }

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        baLongWriting = f.readAll();
        writeIndex = 0;

        longWriteStartAddr = 0x00;

    sendLongWriteRequest();

    }

}
