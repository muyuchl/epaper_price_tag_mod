#include "formtest.h"
#include "ui_formtest.h"
#include <QDebug>
#include "command.h"

FormTest::FormTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTest)
{
    ui->setupUi(this);
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
}
