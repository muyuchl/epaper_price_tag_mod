#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogportconfig.h"
#include "formled.h"
#include "formtraffic.h"

#include "a7105pingtest.h"
#include "a7105uploader.h"

namespace Ui {
class MainWindow;
}

class UartHandler;

class QLabel;

class A7105PingTest;
class FormA7105PingTest;
class FormA7105Uploader;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sltStatisticChanged(int tx, int rx);


    void on_actionPortConfig_triggered();

private:
    void init();


private:
    Ui::MainWindow *ui;
    QLabel *labelStatusPortName;
    QLabel *labelStatusPortError;
    QLabel *labelStatusPortStatistic;

    FormTraffic *formTraffic;
    FormLed *formLed;

    FormA7105PingTest *formA7105PingTest;
    FormA7105Uploader *formA7105Upploader;

    DialogPortConfig *dialogPortConfig;

    UartHandler *uartHandler;
    QString portName;

    A7105PingTest *a7105PingTest;
    A7105Uploader *a7105Upploader;

};

#endif // MAINWINDOW_H
