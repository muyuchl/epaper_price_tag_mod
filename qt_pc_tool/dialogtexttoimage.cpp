#include "dialogtexttoimage.h"
#include "ui_dialogtexttoimage.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <map>
#include "texttobingenerator.h"

const int LANDSCAPE_IMAGE_WIDTH = 250;
const int LANDSCAPE_IMAGE_HEIGHT = 128;

const int IMG_WIDTH = 128;
const int IMG_HEIGHT = 250;

DialogTextToImage::DialogTextToImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTextToImage),
    preViewImage(LANDSCAPE_IMAGE_WIDTH, LANDSCAPE_IMAGE_HEIGHT, QImage::Format_Mono)
{
    ui->setupUi(this);
}

DialogTextToImage::~DialogTextToImage()
{
    delete ui;
}

void DialogTextToImage::on_pushButtonPreview_clicked()
{
    QString text = ui->plainTextEditInput->toPlainText().trimmed();

    QStringList lines = text.split(QChar('\n'));
    for (int i = 0; i < lines.size(); i++) {
        qDebug() << i << " : " << lines[i];
    }

    uint colorWhite = qRgb(255,255,255);


    preViewImage.fill(colorWhite);


    QPainter painter(&preViewImage);

    QFont font = ui->fontComboBox->currentFont();
    font.setPixelSize(ui->spinBoxFontSize->value());

    qDebug() << "font family" << font.family();
    painter.setFont(font);


    int lineHeight = LANDSCAPE_IMAGE_HEIGHT / lines.size();
    for (int i = 0; i < lines.size(); i++) {
        QRect rect(0, i * lineHeight, LANDSCAPE_IMAGE_WIDTH, lineHeight);
        painter.drawText(rect, Qt::AlignCenter, lines[i]);
    }

    QPixmap pix = QPixmap::fromImage(preViewImage);
    ui->labelPreview->setPixmap(pix);

}

void DialogTextToImage::on_pushButtonSaveBin_clicked()
{
    QString defaultName;
    QString text = ui->plainTextEditInput->toPlainText().trimmed();
    QStringList lines = text.split(QChar('\n'));
    if (lines.size()) {
        defaultName = lines[0] + ".bin";
    }

    QString fileName = QFileDialog::getSaveFileName(this, "save file",
                                                    defaultName, "Bin (*.bin)"   );


    QFont font = ui->fontComboBox->currentFont();

    TextToBinGenerator::generateBin(font.family(),ui->spinBoxFontSize->value(),
                                    lines, fileName );

}

void DialogTextToImage::on_pushButtonBatchGen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "open batch task file", QString(),
                                                    "CSV (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }

    qDebug() << "batch task file name:" << fileName;

    processBatchTaskFile(fileName);
}

void DialogTextToImage::processBatchTaskFile(const QString &taskFileName)
{
    QFile file(taskFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "can not open file for read " << taskFileName;
        return;
    }

    QDir dir = QFileInfo(taskFileName).absoluteDir();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();

        generateBinFromTaskLine(dir.absolutePath(), line);
        qDebug() << "-----------------------------------------";
    }

}

void DialogTextToImage::generateBinFromTaskLine(const QString &parentPath, const QString &line)
{
 //   qDebug() << "generating for " << parentPath << "  " << line;

    QStringList fields = line.split(',');
    if (fields.size() != 3) {
        qWarning() << "invalid field format for line: " << line;
        return;
    }

    QString fontFamily = fields[0];
    int pixSize = fields[1].toInt();

    QStringList textLines = fields[2].split("\\n");
    if (textLines.size() < 1) {
        return;
    }

    QDir dir(parentPath);
    QFileInfo fi(dir, QString("%1.bin").arg(textLines[0]));
    QString binFileName = fi.absoluteFilePath();
    qDebug() << "bin file path: " << binFileName;




    TextToBinGenerator::generateBin(fontFamily, pixSize,
                                    textLines, binFileName );
}
