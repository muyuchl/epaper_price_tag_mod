#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "command.h"

#include "uarthandler.h"

#include <QDebug>
#include <QtGlobal>
#include <QTimer>
#include <QDateTime>
#include <QLabel>

#include "forma7105pingtest.h"
#include "forma7105uploader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    labelStatusPortName(new QLabel("")),
    labelStatusPortError(new QLabel("")),
    labelStatusPortStatistic(new QLabel("")),
    formTraffic(new FormTraffic()),
    formLed(new FormLed()),
    formSpiFlash(new FormSPIFlash()),
    formA7105(new FormA7105()),
    formEpaper(new FormEpaperImage()),
    formA7105PingTest(new FormA7105PingTest()),
    formA7105Upploader(new FormA7105Uploader()),
    dialogPortConfig(new DialogPortConfig(this)),
    a7105PingTest(new A7105PingTest()),
    a7105Upploader(new A7105Uploader())
{
    ui->setupUi(this);

    ui->tabWidget->addTab(formTraffic, "Traffic");
    ui->tabWidget->addTab(formLed, "Led");

    // ui->tabWidget->addTab(formSpiFlash, "SPI Flash");
   // ui->tabWidget->addTab(formA7105, "A7105");
   // ui->tabWidget->addTab(formEpaper, "Epaper");

    ui->tabWidget->addTab(formA7105PingTest, "A7105 Ping Test");
    formA7105PingTest->setA7105PingTest(a7105PingTest);

    ui->tabWidget->addTab(formA7105Upploader, "A7105 Uploader");
    formA7105Upploader->setUploader(a7105Upploader);

    statusBar()->addWidget(labelStatusPortName);
    statusBar()->addWidget(labelStatusPortError);
    statusBar()->addWidget(labelStatusPortStatistic);

    uartHandler = new UartHandler(this);
    connect(uartHandler, &UartHandler::sgnStatisticChanged,
            this, &MainWindow::sltStatisticChanged);

    QTimer::singleShot(0, this, &MainWindow::on_actionPortConfig_triggered);

    init();
}

MainWindow::~MainWindow()
{
    uartHandler->unInit();
    delete ui;
}

void MainWindow::sltStatisticChanged(int tx, int rx)
{
    labelStatusPortStatistic->setText(QString("tx: %1  rx: %2")
                                      .arg(tx).arg(rx));
}


void MainWindow::on_actionPortConfig_triggered()
{
    int ret = dialogPortConfig->exec();
    if (QDialog::Accepted == ret) {
        // use new serial port
        portName = dialogPortConfig->getPortName();
        qDebug() << "got new port name: " << portName;

        labelStatusPortName->setText(portName);

        if (uartHandler->init(portName)) {
            labelStatusPortError->setText("");
        } else {
            labelStatusPortError->setText("error!");
        }

    }
}

void MainWindow::init()
{
    connect(uartHandler, &UartHandler::sgnFrameTransmitted,
            formTraffic, &FormTraffic::sltFrameTransmitted);
    connect(uartHandler, &UartHandler::sgnFrameReceived,
            formTraffic, &FormTraffic::sltFrameReceived);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            formLed, &FormLed::sltFrameReceived);
    connect(formLed, &FormLed::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            a7105PingTest, &A7105PingTest::sltFrameReceived);
    connect(a7105PingTest, &A7105PingTest::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

    connect(uartHandler, &UartHandler::sgnFrameReceived,
            a7105Upploader, &A7105Uploader::sltFrameReceived);
    connect(a7105Upploader, &A7105Uploader::sgnSendFrame,
            uartHandler, &UartHandler::sltSendFrame);

//    connect(uartHandler, &UartHandler::sgnFrameReceived,
//            formSpiFlash, &FormSPIFlash::sltFrameReceived);
//    connect(formSpiFlash, &FormSPIFlash::sgnSendFrame,
//            uartHandler, &UartHandler::sltSendFrame);

//    connect(uartHandler, &UartHandler::sgnFrameReceived,
//            formA7105, &FormA7105::sltFrameReceived);
//    connect(formA7105, &FormA7105::sgnSendFrame,
//            uartHandler, &UartHandler::sltSendFrame);

//    connect(uartHandler, &UartHandler::sgnFrameReceived,
//            formEpaper, &FormEpaperImage::sltFrameReceived);
//    connect(formEpaper, &FormEpaperImage::sgnSendFrame,
//            uartHandler, &UartHandler::sltSendFrame);
}
