#include "formbatchdownloader.h"
#include "ui_formbatchdownloader.h"

#include <QFileDialog>
#include <QMessageBox>
#include "batchdownloader.h"
#include <QDebug>
#include <QFile>
#include "monoimagelabel.h"

const int MAX_PICTURES = 64;

FormBatchDownloader::FormBatchDownloader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBatchDownloader),
    batchDownloader(nullptr)
{
    ui->setupUi(this);
    initTableWidget();
}

FormBatchDownloader::~FormBatchDownloader()
{
    delete ui;
}

void FormBatchDownloader::setBatchDownloader(BatchDownloader *downloader)
{
    batchDownloader = downloader;
    connect(batchDownloader, &BatchDownloader::sgnFailed,
            this, &FormBatchDownloader::sltFailed);
    connect(batchDownloader, &BatchDownloader::sgnAllDone,
            this, &FormBatchDownloader::sltAllDone);
    connect(batchDownloader, &BatchDownloader::sgnProgressChange,
            this, &FormBatchDownloader::sltProgressChange);
}

void FormBatchDownloader::closeEvent(QCloseEvent *event)
{
    // alwary stop batchdownloader when closed
    batchDownloader->stop();
    QWidget::closeEvent(event);
}

void FormBatchDownloader::initTableWidget()
{
  //  ui->tableWidget->setRowCount(MAX_PICTURES);
    ui->tableWidget->setColumnCount(3);

    QStringList headerLabels;
    headerLabels << "Picture Index";
    headerLabels << "FileName";
    headerLabels << "Status";
    ui->tableWidget->setColumnWidth(COL_FILENAME, 200);

    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
}

void FormBatchDownloader::populateTableWidget()
{
    int firstIndex = ui->spinBoxFirstIndex->value();

    ui->tableWidget->setRowCount(filesToDownload.size());

    int row = 0;
    for (auto f : filesToDownload) {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(firstIndex + row));
        ui->tableWidget->setItem(row, COL_PICTURE_INDEX, newItem);

        QFileInfo fi(f);
        QString base = fi.completeBaseName();

        newItem = new QTableWidgetItem(tr("%1").arg(base));
        ui->tableWidget->setItem(row, COL_FILENAME, newItem);

        QString text = "0";
        newItem = new QTableWidgetItem(tr("%1").arg(text));
        ui->tableWidget->setItem(row, COL_PROGRESS, newItem);

        row++;
    }
}

void FormBatchDownloader::on_pushButtonBrowse_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Select one or more files to open",
                "",
                "Bin (*.bin);;All files (*.*)");

    int firstIndex = ui->spinBoxFirstIndex->value();

    int count = files.size();

    if (0 == count) {
        // canceled
        return;
    } else if (count + firstIndex > MAX_PICTURES) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Too many, trunct to 64 files\n"));


        files = files.mid(0, MAX_PICTURES - firstIndex);

    }

    filesToDownload = files;
    populateTableWidget();

}

void FormBatchDownloader::sltFailed()
{
    ui->labelStatus->setText("failed!");
}

void FormBatchDownloader::sltAllDone()
{
    ui->labelStatus->setText("all done");
}

void FormBatchDownloader::sltProgressChange(int fileIndex, int progress)
{
    if (!this->isVisible()) {
        return;
    }

    QTableWidgetItem *item = new QTableWidgetItem(QString("%1").arg(progress));
    ui->tableWidget->setItem(fileIndex, COL_PROGRESS, item);
}

void FormBatchDownloader::on_pushButtonStart_clicked()
{
    int firstIndex = ui->spinBoxFirstIndex->value();
    batchDownloader->start(firstIndex, filesToDownload);
}

void FormBatchDownloader::on_pushButtonStop_clicked()
{
    batchDownloader->stop();
}

void FormBatchDownloader::on_pushButtonShuffle_clicked()
{
    qsrand(::time(0));

    int count = filesToDownload.size();

    for (int i = 0; i < count; i++) {
        int indexB = qrand() % count;
        if (indexB != i) {
            // swap index i and indexB
            QString strI = filesToDownload.at(i);
            QString strB = filesToDownload.at(indexB);

            filesToDownload.replace(indexB, strI);
            filesToDownload.replace(i, strB);
        }
    }

    populateTableWidget();
}

void FormBatchDownloader::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    qDebug() << __PRETTY_FUNCTION__ << " currentRow:" << currentRow;
    if (currentRow >= 0
            && currentRow < filesToDownload.size()) {

        QFile binFile(filesToDownload[currentRow]);
                if (!binFile.open(QIODevice::ReadOnly)) {
                   ui->labelPreview->setText("File Open Failed");
                    return;
                } else {
                    QByteArray ba = binFile.readAll();
                    ui->labelPreview->setEpaperBinData(ba);
                }


    } else {
        ui->labelPreview->setText("Invalid Index");
    }
}
