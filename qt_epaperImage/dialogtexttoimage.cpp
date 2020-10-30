#include "dialogtexttoimage.h"
#include "ui_dialogtexttoimage.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <map>

const int PREVIEW_IMAGE_WIDTH = 250;
const int PREVIEW_IMAGE_HEIGHT = 128;

const int IMG_WIDTH = 128;
const int IMG_HEIGHT = 250;

const QRgb TOOL_RGB_BACKGROUND = qRgb(250, 250, 250);
const QRgb TOOL_RGB_FORGROUND = qRgb(30, 200, 30);


DialogTextToImage::DialogTextToImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTextToImage),
    preViewImage(PREVIEW_IMAGE_WIDTH, PREVIEW_IMAGE_HEIGHT, QImage::Format_Mono)
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


    int lineHeight = PREVIEW_IMAGE_HEIGHT / lines.size();
    for (int i = 0; i < lines.size(); i++) {
        QRect rect(0, i * lineHeight, PREVIEW_IMAGE_WIDTH, lineHeight);
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

    saveBin(fileName);
}

void DialogTextToImage::saveBin(QString &fileName)
{
    QTransform transform;
    transform.rotate(90);

    QImage imgTmp = preViewImage.transformed(transform);

  //  imgTmp = imgTmp.mirrored(true, true);

//    QPixmap pix = QPixmap::fromImage(imgTmp);
//    ui->labelPreview->setPixmap(pix);

#if 1

    QImage imgOut = imgTmp.convertToFormat(QImage::Format_Mono, Qt::MonoOnly);


    QPixmap pix = QPixmap::fromImage(imgOut);
    ui->labelPreview->setPixmap(pix);


    QByteArray baOut;
    baOut.resize(IMG_WIDTH*IMG_HEIGHT/8);

    for (int i = 0; i < IMG_WIDTH*IMG_HEIGHT/8; i++) {
        int y = i * 8 / IMG_WIDTH;

        // reset to background
        quint8 byteValue = 0xFF; // value 1 is background

        int startX = (i*8) % IMG_WIDTH;
        for (int x = startX; x < startX + 8; x++) {
            QRgb rgb = imgOut.pixel(x, y);

//            if (rgb != TOOL_RGB_BACKGROUND
//                    && rgb != TOOL_RGB_FORGROUND) {
//                qDebug() << "other color " << rgb;
//            }

            if (0xffffffff != rgb) {
                // fore ground
                int bitIdx = 7 -  (x - startX);
                byteValue &= ~(1 << bitIdx);
            }

        }

        baOut[i] = (char) byteValue;
    }


      QFile binFile(fileName);
      if (binFile.open(QIODevice::WriteOnly)) {
          binFile.write(baOut);
          binFile.close();

          qDebug() << "written to " << fileName;

      }

#endif
}
