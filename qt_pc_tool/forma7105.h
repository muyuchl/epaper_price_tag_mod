#ifndef FORMA7105_H
#define FORMA7105_H

#include <QWidget>

namespace Ui {
class FormA7105;
}

class FormA7105 : public QWidget
{
    Q_OBJECT

public:
    explicit FormA7105(QWidget *parent = nullptr);
    ~FormA7105();
public slots:
    void sltFrameReceived(QByteArray frame);

signals:
    void sgnSendFrame(QByteArray frame);

private slots:
    void on_pushButtonPing_clicked();

    void on_pushButtonReadFlashJedecId_clicked();

    void on_pushButtonReadData_clicked();

    void on_pushButtonSectorErase_clicked();

    void on_pushButtonWriteData_clicked();

    void on_pushButtonRead4KB_clicked();

    void on_pushButtonWrite4KB_clicked();

    void on_pushButtonChipErase_clicked();

    void on_pushButtonWriteBin_clicked();

private:
    void parseRxedRFFrame(const QByteArray &frame);
    void parsePingResponse(const QByteArray &frame);
    void parseJedecIdResponse(const QByteArray &frame);
    void parseReadDataResponse(const QByteArray &frame);

    void parseSectorEraseResponse(const QByteArray &frame);
    void parseWriteDataResponse(const QByteArray &frame);

    void parseChipEraseResponse(const QByteArray &frame);

    void sendLongReadRequest();

    void sendLongWriteRequest();

    void saveLongReadToFile();

private:
    Ui::FormA7105 *ui;

    QByteArray baLongRead;
    quint32 longReadAddr;
    quint32 longReadAddrEnd;    // not including

    QByteArray baLongWriting;
    int writeIndex;
    quint32 longWriteStartAddr;


};

#endif // FORMA7105_H
