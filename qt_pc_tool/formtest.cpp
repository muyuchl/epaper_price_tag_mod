#include "formtest.h"
#include "ui_formtest.h"
#include <QDebug>
#include "command.h"
#include <QTimer>

const int SET_IMAGE_INDEX_RESP_TIME_OUT = 20*1000;

FormTest::FormTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTest),
    setImgIndexResponseTimer(new QTimer(this))
{        
    ui->setupUi(this);

    setImgIndexResponseTimer->setInterval(SET_IMAGE_INDEX_RESP_TIME_OUT);
    setImgIndexResponseTimer->setSingleShot(true);
    connect(setImgIndexResponseTimer, &QTimer::timeout,
            this, &FormTest::sltSetImgIndexRespTimeout);
}

FormTest::~FormTest()
{
    delete ui;
}

void FormTest::on_pushButtonShowOnDevice_clicked()
{
    int idx = ui->spinBoxImageIndex->value();

    quint32 addr = 4096 * idx;

    quint8 buf[7 ] = {0};
    buf[0] = CMD_RF_TXDATA;
    buf[1] = RFCMD_LOAD_IMAGE_FROM_FLASH;
    buf[2] =  static_cast<quint8> ((addr >> 16) & 0xff);
    buf[3] =  static_cast<quint8> ((addr >> 8) & 0xff);
    buf[4] =  static_cast<quint8> ((addr >> 0) & 0xff);

    QByteArray ba((char*)buf, 5);

    emit sgnSendFrame(ba);

    statTime.restart();
    ui->labelSetImgIndexResult->setText("please wait");
    setImgIndexResponseTimer->start();
}

void FormTest::sltSetImgIndexRespTimeout()
{
    ui->labelSetImgIndexResult->setText("time out");
}

void FormTest::sltFrameReceived(QByteArray frame)
{
    quint8 cmdid = static_cast<quint8>(frame.at(0));

    cmdid = cmdid & 0x7F;   // clear resp flag

    if (CMD_RF_DATARECEIVED == cmdid) {
        parseRxedRFFrame(frame);
    }
}

void FormTest::parseRxedRFFrame(const QByteArray &frame)
{
    quint8 rfcmdid = static_cast<quint8>(frame.at(1));
    switch (rfcmdid) {

    case RFCMD_LOAD_IMAGE_FROM_FLASH_RESP:
        parseLoadImageIndexRespFrame(frame);
        break;
    }
}

void FormTest::parseLoadImageIndexRespFrame(const QByteArray &frame)
{
    setImgIndexResponseTimer->stop();
    qDebug() << "got response, elapsed " << statTime.elapsed();
    ui->labelSetImgIndexResult->setText(QString("finished in %1 ms").arg(statTime.elapsed()));
}
