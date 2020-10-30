#include "formtraffic.h"
#include "ui_formtraffic.h"

FormTraffic::FormTraffic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTraffic)
{
    ui->setupUi(this);
}

FormTraffic::~FormTraffic()
{
    delete ui;
}

void FormTraffic::sltFrameTransmitted(QByteArray frame)
{
    if (ui->checkBoxLogTx->isChecked()) {
        QString line = QString("Tx:%1").arg(QString(frame.toHex(' ')));
        ui->plainTextEdit->appendPlainText(line);
        ui->plainTextEdit->ensureCursorVisible();
    }
}

void FormTraffic::sltFrameReceived(QByteArray frame)
{
    if (ui->checkBoxLogRx->isChecked()) {
        QString line = QString("Rx:%1").arg(QString(frame.toHex(' ')));
        ui->plainTextEdit->appendPlainText(line);
        ui->plainTextEdit->ensureCursorVisible();
    }
}

void FormTraffic::on_pushButtonClear_clicked()
{
    ui->plainTextEdit->clear();
}
