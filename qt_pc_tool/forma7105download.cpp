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
        QString strElapsed = QString("Finished in %1 ms").arg(pingTime.elapsed());
        ui->labelStatus->setText(strElapsed);
    } else if (progress < 5) {
        ui->labelStatus->setText("Downloading...");
        QString strElapsed = QString("Elapsed %1 ms").arg(pingTime.elapsed());
        qDebug() << strElapsed;
    }
}

void FormA7105Download::sltFailed()
{
    ui->labelStatus->setText("Download Failed!");
}

void FormA7105Download::on_pushButtonBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select bin File",
                                                    QString(), "Bin (*.bin);; All file (*)");
    qDebug() << __PRETTY_FUNCTION__ << fileName;

    if (fileName.isEmpty())
    {
        return;
    }

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        baData = f.readAll();
        qDebug() << "file loaded, size: " << baData.size();

        ui->pushButtonDownload->setEnabled(true);
        ui->labelPreview->setEpaperBinData(baData);
    }
}

void FormA7105Download::on_pushButtonDownload_clicked()
{
    Q_ASSERT(downloader);
    pingTime.start();
    downloader->start(ui->spinBoxImageIndex->value(), baData);

    ui->labelStatus->setText("Erasing Sector");
}

void FormA7105Download::on_pushButtonStop_clicked()
{
    Q_ASSERT(downloader);
    downloader->stop();

}
