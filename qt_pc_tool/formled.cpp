#include "formled.h"
#include "ui_formled.h"

#include <QDebug>
#include "uarthandler.h"
#include "command.h"

FormLed::FormLed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLed)
{
    ui->setupUi(this);
}

FormLed::~FormLed()
{
    delete ui;
}

void FormLed::sltFrameReceived(QByteArray frame)
{
    // led don't care response, you can see LED

    //        quint8 cmdid = static_cast<quint8>(frame.at(0));

    //        cmdid = cmdid & 0x7F;   // clear resp flag

}

void FormLed::on_pushButtonLedOn_clicked()
{
    char buf[1];
    buf[0] = static_cast<char>(CMD_LED_ON);
    QByteArray ba(buf, 1);

    emit sgnSendFrame(ba);

}

void FormLed::on_pushButtonLedOff_clicked()
{
    char buf[1];
    buf[0] = static_cast<char>(CMD_LED_OFF);
    QByteArray ba(buf, 1);

    emit sgnSendFrame(ba);
}
