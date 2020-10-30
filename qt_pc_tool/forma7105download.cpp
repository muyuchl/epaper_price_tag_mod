#include "forma7105download.h"
#include "ui_forma7105download.h"

#include <QDebug>
#include <QFileDialog>
#include "a7105downloader.h"

FormA7105Download::FormA7105Download(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormA7105Download),
    downloader(nullptr)
{
    ui->setupUi(this);
}

FormA7105Download::~FormA7105Download()
{
    delete ui;
}

void FormA7105Download::setDownloader(A7105Downloader *a7105Downloader)
{
    downloader = a7105Downloader;
    connect(downloader, &A7105Downloader::sgnProgressChange,
            this, &FormA7105Download::sltProgressChange);

    connect(downloader, &A7105Downloader::sgnFailed,
            this, &FormA7105Download::sltFailed);
}

void FormA7105Download::sltProgressChange(int progress)
{
    ui->progressBar->setValue(progress);

    if (100 == progress) {
        QString strElapsed = QString("elapsed %1 ms").arg(pingTime.elapsed());
        ui->labelPingTestStatus->setText(strElapsed);
    }
}

void FormA7105Download::sltFailed()
{
    ui->labelPingTestStatus->setText("Upload Failed!");
}

void FormA7105Download::on_pushButtonBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select bin File",
                                                    QString(), "Bin (*.bin)");
    qDebug() << __PRETTY_FUNCTION__ << fileName;

    if (fileName.isEmpty())
    {
        return;
    }

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        baData = f.readAll();
        qDebug() << "file loaded, size: " << baData.size();
    }
}

void FormA7105Download::on_pushButtonDownload_clicked()
{
    Q_ASSERT(downloader);
    pingTime.start();
    downloader->start(ui->spinBoxImageIndex->value(), baData);
}

void FormA7105Download::on_pushButtonStop_clicked()
{
    Q_ASSERT(downloader);
    downloader->stop();

}
