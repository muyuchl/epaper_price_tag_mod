#ifndef UARTHANDLER_H
#define UARTHANDLER_H

#include <QObject>

class QSerialPort;
class QTimer;

class UartHandler : public QObject
{
    Q_OBJECT
public:
    explicit UartHandler(QObject *parent = nullptr);
    bool init(const QString &port);
    void unInit();

signals:
    void sgnFrameReceived(QByteArray frame);
    void sgnFrameTransmitted(QByteArray frame);

    void sgnStatisticChanged(int tx, int rx);

public slots:
    void sltSendFrame(const QByteArray &frame);
    void sltParseFrame();
    void sltResTimeout();

private:
    QSerialPort *serialPort;
    QTimer *resTimer;

    int txFrameCount;
    int rxFrameCount;
};

#endif // UARTHANDLER_H
