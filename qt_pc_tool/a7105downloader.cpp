#include "a7105downloader.h"

#include <QDebug>
#include <QTimer>
#include "command.h"

namespace  {
// sector aligned
// include 96 byte notes to read
const int FLASH_IMAGE_SIZE = 4096;

// data to read for one packet, should be fit in wireless packet
const int FLASH_ONE_WRITE_SIZE = 48;

const int RESPONSE_TIMEOUT_ERASE = 10000;
const int RESPONSE_TIMEOUT_WRITE = 1000;
static const int RETRY_MAX = 3;
}


A7105Downloader::A7105Downloader(QObject *parent) : QObject(parent),
    sectorStartAddress(0),
    curSeq(0),
    totalCount(0),
    progInPercent(0),
    sectorErased(false),
    responseTimer(new QTimer(this)),
    retryLeft(RETRY_MAX),
    statTimeoutCount(0)
{

    responseTimer->setSingleShot(true);
    connect(responseTimer, &QTimer::timeout,
            this, &A7105Downloader::sltTimeout);
}

void A7105Downloader::start(int imageIndex, const QByteArray &data)
{
    Q_ASSERT( imageIndex >= 0 && imageIndex < 64);
    qDebug() << __PRETTY_FUNCTION__ << " " << imageIndex;

    bin = data;
    if (bin.size() != FLASH_IMAGE_SIZE) {
        // its normal, if picture data is 4000, but sector is 4096 byte
        qDebug() << "resize data from " << bin.size() << " to " << FLASH_IMAGE_SIZE;
        bin.resize(FLASH_IMAGE_SIZE);
    }

    sectorStartAddress = static_cast<quint32>(imageIndex) * FLASH_IMAGE_SIZE;

    retryLeft = RETRY_MAX;
    statTimeoutCount = 0;
    curSeq = 0;
    progInPercent = 0;
    sectorErased = false;
    if (0 == (FLASH_IMAGE_SIZE % FLASH_ONE_WRITE_SIZE)) {
        totalCount = FLASH_IMAGE_SIZE / FLASH_ONE_WRITE_SIZE;
    } else {
        totalCount = FLASH_IMAGE_SIZE / FLASH_ONE_WRITE_SIZE + 1;
    }

    Q_ASSERT(totalCount > 0);
    qDebug() << "write total count: " << totalCount;

    emit sgnProgressChange(progInPercent);

    sendEraseSectorRequest();
}

void A7105Downloader::stop()
{
    responseTimer->stop();

    curSeq = 0;
    progInPercent = 0;
    totalCount = 0;
    sectorErased = false;
    emit sgnProgressChange(progInPercent);
}

void A7105Downloader::sltFrameReceived(QByteArray frame)
{
    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_RF_DATARECEIVED == cmdid) {
        parseRxedRFFrame(frame);
    }
}

void A7105Downloader::sltTimeout()
{
    statTimeoutCount++;
    retryLeft--;
    qWarning() << __PRETTY_FUNCTION__ << " retryLeft: " << retryLeft;

    if (retryLeft > 0) {
        if (sectorErased) {
            sendWriteRequest();
        } else {
            sendEraseSectorRequest();
        }

    } else {
        // failed
        emit sgnFailed();
    }
}

void A7105Downloader::sendEraseSectorRequest()
{
    qDebug() << __PRETTY_FUNCTION__;
    quint32 addrValue = sectorStartAddress ;


    responseTimer->start(RESPONSE_TIMEOUT_ERASE);

    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_FLASH_SECTOR_ERASE;
    buf[2] =  static_cast<quint8> ((addrValue >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((addrValue >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((addrValue >> 0) & 0xff);

    QByteArray ba((char*)buf, 5);

    emit sgnSendFrame(ba);
}

void A7105Downloader::sendWriteRequest()
{
    quint32 longWriteAddr = sectorStartAddress + curSeq * FLASH_ONE_WRITE_SIZE;

    quint32 writeSize = FLASH_ONE_WRITE_SIZE;

    // check if last partial packet
    if (curSeq + 1 == totalCount
            && 0 != (FLASH_IMAGE_SIZE % FLASH_ONE_WRITE_SIZE)) {
        writeSize = FLASH_IMAGE_SIZE % FLASH_ONE_WRITE_SIZE;

        qDebug() << "writing last packet with size: " << writeSize;
    }

    quint8 buf[64] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_WRITE_FLASH_DATA;
    buf[2] =  static_cast<quint8> ((longWriteAddr >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((longWriteAddr >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((longWriteAddr >> 0) & 0xff);

    buf[5] =  static_cast<quint8> ((writeSize >> 8) & 0xff);
    buf[6] =  static_cast<quint8> ((writeSize >> 0) & 0xff);

    // fill data
    for (int i = 0; i < writeSize; i++) {
        char c = bin[curSeq * FLASH_ONE_WRITE_SIZE + i];
        buf[7 + i] = static_cast<quint8>(c);
    }

    responseTimer->start(RESPONSE_TIMEOUT_WRITE);

    QByteArray ba((char*)buf, 7 + writeSize);
    emit sgnSendFrame(ba);
}

void A7105Downloader::parseRxedRFFrame(const QByteArray &frame)
{
    quint8 rfcmdid = static_cast<quint8>(frame.at(1));
    switch (rfcmdid) {

    case RFCMD_FLASH_SECTOR_ERASE_RESP:
        parseSectorEraseResponse(frame);
        break;
    case RFCMD_WRITE_FLASH_DATA_RESP:
        parseWriteDataResponse(frame);
        break;
    }
}

void A7105Downloader::parseSectorEraseResponse(const QByteArray &frame)
{
    Q_UNUSED(frame)
    qDebug() << __PRETTY_FUNCTION__;

    responseTimer->stop();
    retryLeft = RETRY_MAX;

    sectorErased = true;
    sendWriteRequest();
}

void A7105Downloader::parseWriteDataResponse(const QByteArray &frame)
{
    Q_UNUSED(frame)

    responseTimer->stop();
    retryLeft = RETRY_MAX;

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

        sendWriteRequest();
    }
}
