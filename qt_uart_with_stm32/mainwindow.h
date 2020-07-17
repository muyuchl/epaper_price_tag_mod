#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogportconfig.h"
#include "formled.h"
#include "formspiflash.h"
#include "formtraffic.h"

#include "forma7105.h"
#include "formepaperimage.h"

namespace Ui {
class MainWindow;
}

class UartHandler;

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sltStatisticChanged(int tx, int rx);

//    void sltFrameReceived(QByteArray frame);
//    void sltStopRTTTest();
//    void on_pushButtonInit_clicked();
//    void on_pushButtonUnInit_clicked();
//    void on_pushButtonPing_clicked();
//    void on_pushButtonRTTTest_clicked();


//    void on_pushButtonLedOn_clicked();

//    void on_pushButtonLedOff_clicked();

//    void on_pushButtonWrite_clicked();

//    void on_pushButtonRead_clicked();

//    void on_pushButtonRFTest_clicked();

//    void on_pushButtonRfTx_clicked();

    void on_actionPortConfig_triggered();

private:
    void init();
//    void sendPing(int count);
//    void handleEEPROMRead(const QByteArray &frame);
//    void handleRxRFData(const QByteArray &frame);


private:
    Ui::MainWindow *ui;
    QLabel *labelStatusPortName;
    QLabel *labelStatusPortError;
    QLabel *labelStatusPortStatistic;

    FormTraffic *formTraffic;
    FormLed *formLed;
    FormSPIFlash *formSpiFlash;
    FormA7105 *formA7105;
    FormEpaperImage *formEpaper;

    DialogPortConfig *dialogPortConfig;

    UartHandler *uartHandler;
    QString portName;


//    bool rttTesting;
//    qint32 rttCount;
};

#endif // MAINWINDOW_H
