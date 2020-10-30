#include "dialogportconfig.h"
#include "ui_dialogportconfig.h"

#include <QDebug>


DialogPortConfig::DialogPortConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPortConfig)
{
    ui->setupUi(this);
    refreshPortList();
}

DialogPortConfig::~DialogPortConfig()
{
    delete ui;
}

//void DialogPortConfig::setPortName(QString &name)
//{
//    qDebug() << __PRETTY_FUNCTION__ << " not implemented";
//}

QString DialogPortConfig::getPortName()
{
    return selectedPortName;
}

void DialogPortConfig::on_pushButtonRefresh_clicked()
{
    refreshPortList();

    updateComboPortSelection(selectedPortName);
}

void DialogPortConfig::refreshPortList()
{
    ui->comboBoxPorts->clear();
    ui->labelPortName->setText("");
    ui->labelPortManufact->setText("");
    ui->labelPortDesc->setText("");

    portInfos =  QSerialPortInfo::availablePorts();
    qDebug() << "available ports: " << portInfos.size();
    foreach(QSerialPortInfo portInfo, portInfos) {

        qDebug() << "portName:" << portInfo.portName();
        qDebug() << "manufacturer: " << portInfo.manufacturer();
        qDebug() << "desceiption: " << portInfo.description();
        qDebug() << "------------------------------------";

        ui->comboBoxPorts->addItem(portInfo.portName());
    }

}

void DialogPortConfig::updateComboPortSelection(QString &portName)
{
    for (int i = 0; i < ui->comboBoxPorts->count(); i++) {
        if (selectedPortName == ui->comboBoxPorts->itemText(i)) {
            ui->comboBoxPorts->setCurrentIndex(i);
            break;
        }
    }

}

void DialogPortConfig::on_comboBoxPorts_currentIndexChanged(int index)
{
    if (index < 0 || index >= portInfos.size()) {
        qDebug() << "invalid index, ignore";
        return;
    }

    QSerialPortInfo info = portInfos[index];

    ui->labelPortName->setText(info.portName());
    ui->labelPortManufact->setText(info.manufacturer());
    ui->labelPortDesc->setText(info.description());

}

void DialogPortConfig::accept()
{
    qDebug() << __PRETTY_FUNCTION__;

    if (ui->comboBoxPorts->count()) {
        selectedPortName =  ui->comboBoxPorts->currentText();
    } else {
        selectedPortName = "";
    }

    QDialog::accept();
}

void DialogPortConfig::reject()
{
    qDebug() << __PRETTY_FUNCTION__;
    // restore previous selected port name

    updateComboPortSelection(selectedPortName);

    QDialog::reject();
}
