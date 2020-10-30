#ifndef FORMSPIFLASH_H
#define FORMSPIFLASH_H

#include <QWidget>

namespace Ui {
class FormSPIFlash;
}

class FormSPIFlash : public QWidget
{
    Q_OBJECT

public:
    explicit FormSPIFlash(QWidget *parent = nullptr);
    ~FormSPIFlash();

public slots:
    void sltFrameReceived(QByteArray frame);

signals:
    void sgnSendFrame(QByteArray frame);

private:
    void parseJedecId(const QByteArray &frame);
    void parseReadData(const QByteArray &frame);

    void sendReadChipCmd();
    void saveReadChipData();

private slots:
    void on_pushButtonJedecId_clicked();
    void on_pushButtonReadData_clicked();
    void on_pushButtonReadAll_clicked();

    void on_pushButtonWritePage_clicked();

    void on_pushButtonTest_clicked();

    void on_pushButtonWriteData_clicked();

    void on_pushButtonSectorErase_clicked();

private:
    Ui::FormSPIFlash *ui;

    bool readingChip;
    quint32 readingAddr;
    QByteArray baReadData;


    QByteArray baWritingData;
    quint32 writingAddr;
};

#endif // FORMSPIFLASH_H
