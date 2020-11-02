#include "formbatchuploader.h"
#include "ui_formbatchuploader.h"
#include "batchuploader.h"

#include <QFileDialog>
#include <QMessageBox>
const int MAX_PICTURES = 64;

FormBatchUploader::FormBatchUploader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBatchUploader),
    startIdx(0)
{
    ui->setupUi(this);
    initTableWidget();
}

FormBatchUploader::~FormBatchUploader()
{
    delete ui;
}

void FormBatchUploader::setBatchUploader(BatchUploader *uploader)
{
    batchUploader = uploader;
    connect(batchUploader, &BatchUploader::sgnFailed,
            this, &FormBatchUploader::sltFailed);
    connect(batchUploader, &BatchUploader::sgnAllDone,
            this, &FormBatchUploader::sltAllDone);
    connect(batchUploader, &BatchUploader::sgnProgressChange,
            this, &FormBatchUploader::sltProgressChange);
}

void FormBatchUploader::on_pushButtonStart_clicked()
{
    startIdx = ui->spinBoxStartIndex->value();
    int count = ui->spinBoxCount->value();

    if (startIdx + count > MAX_PICTURES) {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Out of range, trunct to 64 files\n"));
        count = MAX_PICTURES - startIdx;
        ui->spinBoxCount->setValue(count);
    }

    ui->tableWidget->setRowCount(count);
    for (int i = 0; i < count; i++) {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(startIdx + i));
        ui->tableWidget->setItem(i, COL_PICTURE_INDEX, newItem);

        QString text = "0";
        newItem = new QTableWidgetItem(tr("%1").arg(text));
        ui->tableWidget->setItem(i, COL_PROGRESS, newItem);

    }

    batchUploader->start(startIdx, count);
}

void FormBatchUploader::on_pushButtonStop_clicked()
{
    batchUploader->stop();
}

void FormBatchUploader::sltFailed()
{
    ui->labelStatus->setText("Failed!");
}

void FormBatchUploader::sltAllDone()
{
    ui->labelStatus->setText("All Done");
}

void FormBatchUploader::sltProgressChange(int picIndex, int progress)
{
    int row = picIndex - startIdx;
    Q_ASSERT(row >= 0 && row <= ui->tableWidget->rowCount());

    // update progress
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(progress));
    ui->tableWidget->setItem(row, COL_PROGRESS, newItem);
}

void FormBatchUploader::initTableWidget()
{
    ui->tableWidget->setColumnCount(2);

    QStringList headerLabels;
    headerLabels << "Picture Index";
    headerLabels << "Status";

    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
}
