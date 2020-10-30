#ifndef A7105DOWNLOADER_H
#define A7105DOWNLOADER_H

#include <QObject>

class QTimer;

// download bin data to price tag via A7105
class A7105Downloader : public QObject
{
    Q_OBJECT
public:
    explicit A7105Downloader(QObject *parent = nullptr);

public slots:
    void start(int imageIndex, const QByteArray &data);
    void stop();

public slots:
    void sltFrameReceived(QByteArray frame);
    void sltTimeout();

signals:
    void sgnSendFrame(QByteArray frame);
    void sgnProgressChange(int progress);
    void sgnFailed();

private:
    void sendEraseSectorRequest();
    void sendWriteRequest();
    void parseRxedRFFrame(const QByteArray &frame);
    void parseSectorEraseResponse(const QByteArray &frame);
    void parseWriteDataResponse(const QByteArray &frame);

private:
    // 4096 Byte per sector, one picture size is 4000 Byte
    // picture is sector aligned, the last 96 can be used as notes
    quint32 sectorStartAddress;
    quint32 curSeq;  // packect sequence working on
    quint32 totalCount;  // total packets to read
    int progInPercent;
    bool sectorErased;

    QByteArray bin;

    QTimer *responseTimer;
    int retryLeft;
    // statistic timeout count
    int statTimeoutCount;
};

#endif // A7105DOWNLOADER_H
