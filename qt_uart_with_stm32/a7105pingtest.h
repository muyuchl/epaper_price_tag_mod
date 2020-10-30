#ifndef A7105PINGTEST_H
#define A7105PINGTEST_H

#include <QObject>

class QTimer;

class A7105PingTest : public QObject
{
    Q_OBJECT
public:
    explicit A7105PingTest(QObject *parent = nullptr);

    void start(int count);
    void stop();

public slots:
    void sltFrameReceived(QByteArray frame);


signals:
    void sgnSendFrame(QByteArray frame);

    void sgnProgressChange(int progress);

private:
    void sendPing();
    void parseRxedRFFrame(const QByteArray &frame);
    void parsePingResponse(const QByteArray &frame);


    quint32 curSeq; // 0 mean not testing

    int progInPercent;
    int totalCount;
};

#endif // A7105PINGTEST_H
