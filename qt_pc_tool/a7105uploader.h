#ifndef A7105UPLOADER_H
#define A7105UPLOADER_H

#include <QObject>
class QTimer;

// upload data from price tag to host PC via A7105
class A7105Uploader : public QObject
{
    Q_OBJECT
public:
    explicit A7105Uploader(QObject *parent = nullptr);

signals:

public slots:
    void start(int imageIndex);
    void stop();
    const QByteArray &getBinData() const;

public slots:
    void sltFrameReceived(QByteArray frame);
    void sltTimeout();


signals:
    void sgnSendFrame(QByteArray frame);
    void sgnProgressChange(int progress);
    void sgnFailed();

private:
    void sendReadRequest();
    void parseRxedRFFrame(const QByteArray &frame);
    void parseReadDataResponse(const QByteArray &frame);

private:

  //  void parseRxedRFFrame(const QByteArray &frame);

    // 4096 Byte per sector, one picture size is 4000 Byte
    // picture is sector aligned, the last 96 can be used as notes
    quint32 sectorStartAddress;
    quint32 curSeq;  // packect sequence working on
    quint32 totalCount;  // total packets to read
    int progInPercent;

    QByteArray bin;

    QTimer *responseTimer;
    int retryLeft;
    // statistic timeout count
    int statTimeoutCount;
};

#endif // A7105UPLOADER_H
