#include "dialogmergebins.h"
#include "ui_dialogmergebins.h"
#include <QFileDialog>
#include <QDebug>

DialogMergeBins::DialogMergeBins(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMergeBins)
{
    ui->setupUi(this);
}

DialogMergeBins::~DialogMergeBins()
{
    delete ui;
}

void DialogMergeBins::on_pushButtonBrowse_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                             this,
                             "Select bin files to open",
                             "",
                             "Bin Files (*.bin)");

    ui->listWidget->clear();

    for (QString &f : files) {
        ui->listWidget->addItem(f);
    }

    qDebug() << "bin file count " << files.size();
}

void DialogMergeBins::on_pushButtonSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "save file",
                                                 "", "Bin (*.bin)"   );

    qDebug() << "save merged bin file " << fileName;
    mergeAndSave(fileName);
}

void DialogMergeBins::mergeAndSave(QString &binFile)
{
    QByteArray ba;

    for (int i = 0; i < ui->listWidget->count(); i++) {
        QString oneFileName = ui->listWidget->item(i)->text();
        QFile oneFile(oneFileName);
        if (oneFile.open(QIODevice::ReadOnly)) {
            QByteArray baOne = oneFile.readAll();
            ba.append(baOne);
            oneFile.close();
        } else {
            qDebug() << "error opening " << oneFileName;
        }
    }

    QFile outFile(binFile);
    if (outFile.open(QIODevice::WriteOnly)) {
        outFile.write(ba);
        outFile.close();

        if (ba.size() == 64 * 4 * 1000) {
            qDebug() << "output fits flash size";
        } else {
            qDebug() << "output size: " << ba.size() ;
        }

    } else {
        qDebug() << "error open file for write";
    }
}
