#include "a7105pingtest.h"

#include <QTimer>
#include "command.h"
#include <QDebug>


A7105PingTest::A7105PingTest(QObject *parent) : QObject(parent),
    curSeq(0),
    progInPercent(0),
    totalCount(0)
{

}

void A7105PingTest::start(int count)
{
    totalCount = count;
    curSeq = 1;
    progInPercent = 0;

    emit sgnProgressChange(progInPercent);

    sendPing();
}

void A7105PingTest::stop()
{
    totalCount = 0;
    curSeq = 1;
    progInPercent = 0;

    emit sgnProgressChange(progInPercent);
}

void A7105PingTest::sltFrameReceived(QByteArray frame)
{
    if (0 == curSeq) {
        return;
    }

    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_RF_DATARECEIVED == cmdid) {
        parseRxedRFFrame(frame);
    }

}

void A7105PingTest::sendPing()
{
    quint8 buf[6];
    buf[0] = CMD_RF_TXDATA;
    buf[1] =  static_cast<quint8> (RFCMD_PING);

    *reinterpret_cast<quint32 *>(buf + 2) = curSeq;


    QByteArray bauart((char*)buf, 6);

    emit sgnSendFrame(bauart);
}

void A7105PingTest::parseRxedRFFrame(const QByteArray &frame)
{
    quint8 rfcmdid = static_cast<quint8>(frame.at(1));
    switch (rfcmdid) {
    case RFCMD_PING_RESP:
        parsePingResponse(frame);
        break;
    }
}

void A7105PingTest::parsePingResponse(const QByteArray &frame)
{
    QByteArray ba = frame.mid(2, 4);
    quint32 seqResponse = *reinterpret_cast<const quint32 *>(ba.constData());

    if (seqResponse == curSeq) {
        if (curSeq == totalCount) {
            // finish
            curSeq = 0;
            progInPercent = 100;
            emit sgnProgressChange(progInPercent);
        } else {
            int newPercent = curSeq * 100 / totalCount;
            if (newPercent != progInPercent) {
                progInPercent = newPercent;
                emit sgnProgressChange(progInPercent);
            }

            curSeq++;
            sendPing();
        }

    } else {
        qWarning() << "expect seq " << curSeq << " but got " << seqResponse << ", abort.";
        curSeq = 0;
        progInPercent = 0;
        emit sgnProgressChange(progInPercent);
    }
}
