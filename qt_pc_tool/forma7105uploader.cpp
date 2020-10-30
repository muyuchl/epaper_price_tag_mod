#include "forma7105uploader.h"
#include "ui_forma7105uploader.h"
#include "a7105uploader.h"
#include <QFileDialog>
#include <QFile>
#include <QDebug>

FormA7105Uploader::FormA7105Uploader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormA7105Uploader),
    uploader(nullptr)
{
    ui->setupUi(this);
    ui->labelPreview->hide();
}

FormA7105Uploader::~FormA7105Uploader()
{
    delete ui;
}

void FormA7105Uploader::setUploader(A7105Uploader *a7105Uploader)
{
    uploader = a7105Uploader;

    connect(uploader, &A7105Uploader::sgnProgressChange,
            this, &FormA7105Uploader::sltProgressChange);

    connect(uploader, &A7105Uploader::sgnFailed,
            this, &FormA7105Uploader::sltFailed);
}

void FormA7105Uploader::on_pushButtonUpload_clicked()
{
    Q_ASSERT(uploader);
    ui->labelPreview->hide();

    pingTime.start();
    uploader->start(ui->spinBoxImageIndex->value());
}

void FormA7105Uploader::on_pushButtonStop_clicked()
{
    Q_ASSERT(uploader);
    uploader->stop();
}

void FormA7105Uploader::sltProgressChange(int progress)
{
    ui->progressBar->setValue(progress);

    if (100 == progress) {
        QString strElapsed = QString("elapsed %1 ms").arg(pingTime.elapsed());
        ui->labelPingTestStatus->setText(strElapsed);
        ui->labelPreview->setEpaperBinData(uploader->getBinData());
        ui->labelPreview->show();
    }
}

void FormA7105Uploader::sltFailed()
{
    ui->labelPingTestStatus->setText("Upload Failed!");
}

void FormA7105Uploader::on_pushButtonSaveBin_clicked()
{
    QString defaultName = QString("upload_%1.bin").arg(ui->spinBoxImageIndex->value());

    QString fileName = QFileDialog::getSaveFileName(this, "save to file",
                                                    defaultName, "Bin (*.bin)"   );

    QFile binFile(fileName);
    if (binFile.open(QIODevice::WriteOnly)) {
        binFile.write(uploader->getBinData());
        binFile.close();

        qDebug() << "written to " << fileName
                 << " size: " << uploader->getBinData().size();

    }
}
