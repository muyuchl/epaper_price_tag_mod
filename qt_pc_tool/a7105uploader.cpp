#include "a7105uploader.h"
#include <QDebug>
#include <QTimer>
#include "command.h"

// sector aligned
// include 96 byte notes to read
const int FLASH_IMAGE_SIZE = 4096;

// data to read for one packet, should be fit in wireless packet
const int FLASH_ONE_READ_SIZE = 48;

const int RESPONSE_TIMEOUT = 1000;
static const int RETRY_MAX = 3;

A7105Uploader::A7105Uploader(QObject *parent) : QObject(parent),
    sectorStartAddress(0),
    curSeq(0),
    totalCount(0),
    progInPercent(0),
    responseTimer(new QTimer(this)),
    retryLeft(RETRY_MAX),
    statTimeoutCount(0)
{
    bin.reserve(FLASH_IMAGE_SIZE);

    responseTimer->setInterval(RESPONSE_TIMEOUT);
    responseTimer->setSingleShot(true);
    connect(responseTimer, &QTimer::timeout,
            this, &A7105Uploader::sltTimeout);
}

void A7105Uploader::start(int imageIndex)
{
    Q_ASSERT( imageIndex >= 0 && imageIndex < 64);

    qDebug() << __PRETTY_FUNCTION__ << " " << imageIndex;

    bin.clear();

    sectorStartAddress = static_cast<quint32>(imageIndex) * FLASH_IMAGE_SIZE;

    retryLeft = RETRY_MAX;
    statTimeoutCount = 0;
    curSeq = 0;
    progInPercent = 0;
    if (0 == (FLASH_IMAGE_SIZE % FLASH_ONE_READ_SIZE)) {
        totalCount = FLASH_IMAGE_SIZE / FLASH_ONE_READ_SIZE;
    } else {
        totalCount = FLASH_IMAGE_SIZE / FLASH_ONE_READ_SIZE + 1;
    }

    Q_ASSERT(totalCount > 0);
    qDebug() << "read total count: " << totalCount;

    emit sgnProgressChange(progInPercent);
    sendReadRequest();
}

void A7105Uploader::stop()
{
    curSeq = 0;
    progInPercent = 0;
    totalCount = 0;
    emit sgnProgressChange(progInPercent);

}

const QByteArray &A7105Uploader::getBinData() const
{
    return bin;
}

void A7105Uploader::sltFrameReceived(QByteArray frame)
{
    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_RF_DATARECEIVED == cmdid) {
        parseRxedRFFrame(frame);
    }
}

void A7105Uploader::sltTimeout()
{
    statTimeoutCount++;
    retryLeft--;
    qWarning() << __PRETTY_FUNCTION__ << " retryLeft: " << retryLeft;

    if (retryLeft > 0) {
        sendReadRequest();
    } else {
        // failed
        emit sgnFailed();
    }

}

void A7105Uploader::sendReadRequest()
{
    quint32 longReadAddr = sectorStartAddress + curSeq * FLASH_ONE_READ_SIZE;

    quint32 readSize = FLASH_ONE_READ_SIZE;

    // check if last partial packet
    if (curSeq + 1 == totalCount
            && 0 != (FLASH_IMAGE_SIZE % FLASH_ONE_READ_SIZE)) {
        readSize = FLASH_IMAGE_SIZE % FLASH_ONE_READ_SIZE;

        qDebug() << "reading last packet with size: " << readSize;
    }

    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_READ_FLASH_DATA;
    buf[2] =  static_cast<quint8> ((longReadAddr >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((longReadAddr >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((longReadAddr >> 0) & 0xff);

    buf[5] =  static_cast<quint8> ((readSize >> 8) & 0xff);
    buf[6] =  static_cast<quint8> ((readSize >> 0) & 0xff);

    responseTimer->start();

    QByteArray ba((char*)buf, 7);
    emit sgnSendFrame(ba);
}

void A7105Uploader::parseRxedRFFrame(const QByteArray &frame)
{
    quint8 rfcmdid = static_cast<quint8>(frame.at(1));
    switch (rfcmdid) {
    //    case RFCMD_PING_RESP:
    //        parsePingResponse(frame);
    //        break;
    //    case RFCMD_READ_FLASH_JEDECID_RESP:
    //        parseJedecIdResponse(frame);
    //        break;
    case RFCMD_READ_FLASH_DATA_RESP:
        parseReadDataResponse(frame);
        break;
    }
}

void A7105Uploader::parseReadDataResponse(const QByteArray &frame)
{
    responseTimer->stop();
    retryLeft = RETRY_MAX;

    if (frame.size() < 7) {
        qDebug() << __PRETTY_FUNCTION__ << " invalid length: " << frame.size();
        return;
    }

    quint8 buf[7] = {0};
    memcpy(buf, frame.constData(), 7);

    quint16 readLen = (buf[5] << 8) + (buf[6]);
    if (readLen > 64) {
        qDebug() << __PRETTY_FUNCTION__ << " invalid readLen: " << readLen;
        return;
    }

    QByteArray data = frame.mid(2, readLen);
    //qDebug() << "read data: " << QString(data.toHex(' '));

    bin.append(data);

    if (curSeq + 1 == totalCount) {
        // finish
        curSeq = 0;
        progInPercent = 100;
        emit sgnProgressChange(progInPercent);
        qDebug() << "statTimeoutCount: " << statTimeoutCount;
    } else {
        curSeq++;
        int newPercent = static_cast<int>(curSeq * 100 / totalCount);
        if (newPercent != progInPercent) {
            progInPercent = newPercent;
            emit sgnProgressChange(progInPercent);
        }

        sendReadRequest();
    }
}
