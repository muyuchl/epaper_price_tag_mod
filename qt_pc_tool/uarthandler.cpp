#include "uarthandler.h"
#include <QDebug>
#include <QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

const quint8 START_DELIMITER = 0x7E;
const quint16 MAX_FRAME_LEN = 1024;

const int RESPONSE_TIMEOUT = 3000;

UartHandler::UartHandler(QObject *parent) : QObject(parent),
    serialPort(nullptr),
    resTimer(new QTimer(this)),
    txFrameCount(0),
    rxFrameCount(0)
{
    serialPort = new QSerialPort(this);
    connect(serialPort, SIGNAL(	readyRead() ), this,
            SLOT(sltParseFrame()));

    resTimer->setSingleShot(true);
    connect(resTimer, &QTimer::timeout,
            this, &UartHandler::sltResTimeout);
}

bool UartHandler::init(const QString &port)
{
    bool ret = true;

    if (serialPort->isOpen()) {
        serialPort->close();
    }

    serialPort->setPortName(port);
    if (serialPort->open(QIODevice::ReadWrite))
    {
        if (serialPort->setBaudRate(QSerialPort::Baud115200)
                && serialPort->setDataBits(QSerialPort::Data8)
                && serialPort->setParity(QSerialPort::NoParity)
                && serialPort->setStopBits(QSerialPort::OneStop)
                && serialPort->setFlowControl(QSerialPort::NoFlowControl))
        {
            serialPort->clear();
            qDebug( "open serial port successful");
        }
        else
        {
            serialPort->close();
            qWarning( "set serial port parameters failed!");
            ret = false;
        }
    }
    else
    {        
        qWarning( "open serial port  failed!");
        ret = false;
    }

    txFrameCount = 0;
    rxFrameCount = 0;
    emit sgnStatisticChanged(txFrameCount, rxFrameCount);

    return ret;
}

void UartHandler::unInit()
{
    if (serialPort) {
        serialPort->close();
    }
}

// frame is api specific data
// one byte command id and following command data
void UartHandler::sltSendFrame(const QByteArray &frame)
{
    qDebug() << __FUNCTION__;

    Q_ASSERT(frame.size() <= MAX_FRAME_LEN);

    quint8 buf[8];
    quint16 len = static_cast<quint16>(frame.size()) ;

    buf[0] = START_DELIMITER;
    buf[1] = len >> 8;    // MSB
    buf[2] = len & 0xFF;    // LSB

    quint8 cksum = 0;

    for (int i = 0; i < frame.size(); i++)
    {
        char b = frame.at(i);
        cksum += static_cast<quint8>(b);
    }
    cksum = 0xFF - cksum;

    QByteArray baSerial( reinterpret_cast<const char *>(buf), 3 );
    baSerial.append(frame);
    baSerial.append(static_cast<char>(cksum));

    resTimer->start(RESPONSE_TIMEOUT);
    qint64 write_ret =  serialPort->write(baSerial);
    // qDebug() << "write_ret: " << write_ret;
    if (write_ret > 0) {
        txFrameCount++;
        emit sgnStatisticChanged(txFrameCount, rxFrameCount);

        sgnFrameTransmitted(frame);
    }

}

void UartHandler::sltParseFrame()
{
    qDebug() << __FUNCTION__;
    if (serialPort->bytesAvailable() >= 5)
    {
        char buf[3];
        if (serialPort->peek(buf, sizeof(buf)) == sizeof(buf))
        {
            quint32 frameLen = static_cast<quint32>(buf[1]) << 8 | static_cast<quint32>(buf[2]);
            if (serialPort->bytesAvailable() < 4 + frameLen)
            {
                // not enough
                return;
            }
            else
            {
                if (serialPort->bytesAvailable() > 4 + frameLen)
                {
                    qWarning("serial port has more data than expected");
                }

                // now parse
                QByteArray ba = serialPort->readAll();

                if (static_cast<quint8>(ba.at(0)) != START_DELIMITER)
                {
                    qWarning("rx invalid start delimiter");
                    return;
                }

                quint8 cksum = 0;
                for (int i = 0; i < static_cast<int>(frameLen) + 1 ; i++)
                {
                    cksum += static_cast<quint8>(ba.at(3 + i));
                }

                if (cksum != 0xFF)
                {
                    qWarning("rx cksum invalid");
                    return;
                }

                resTimer->stop();
                QByteArray baCmd = ba.mid(3, static_cast<int>(frameLen));
                emit sgnFrameReceived(baCmd);

                rxFrameCount++;
                emit sgnStatisticChanged(txFrameCount, rxFrameCount);
            }

        }

    }
}

void UartHandler::sltResTimeout()
{        
    qWarning("UartHandler::sltResTimeout");
    serialPort->clear();
}
