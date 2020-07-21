#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPixmap>
#include <QFile>
#include <QTextStream>

#include "imagedata.h"
#include "dialogtexttoimage.h"

#include <QDebug>
#include <QColor>
#include <QFileDialog>
#include <QMessageBox>
#include "dialogmergebins.h"

const int IMG_WIDTH = 128;
const int IMG_HEIGHT = 250;

const QRgb TOOL_RGB_BACKGROUND = qRgb(250, 250, 250);
const QRgb TOOL_RGB_FORGROUND = qRgb(30, 200, 30);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    image(IMG_WIDTH, IMG_HEIGHT, QImage::Format_ARGB32)
{
    ui->setupUi(this);

    generateImage();
    showImage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 * 128x250
2.13寸
*/
void MainWindow::generateImage()
{  
    image.fill(Qt::red);
    genHorizontal(img3);
}

void MainWindow::showImage()
{
    QPixmap pix;
    pix.convertFromImage(image);
    ui->labelImage->setPixmap(pix);
}

// img1[0]  pixels:  y == 0, x = 0, 1, 2, 3, 4, 5, 6, 7
// img1[0] bit 7 is (0, 0), bit 6: (1, 0)
void MainWindow::genHorizontal(const unsigned char *img)
{
    int x = 0;
    int y = 0;

    for (int i = 0; i < IMG_WIDTH*IMG_HEIGHT/8; i++) {
        quint8 byte = img[i];

        for (int bit = 0; bit < 8; bit++) {
            bool bitValue = (byte & (1 << (7-bit))) != 0;

            if (bitValue) {
                image.setPixel((IMG_WIDTH-1-x), (IMG_HEIGHT-1-y), TOOL_RGB_BACKGROUND);
            } else {
                image.setPixel((IMG_WIDTH-1-x), (IMG_HEIGHT-1-y), TOOL_RGB_FORGROUND);
            }

            // update x, y
            x++;
            if (x == IMG_WIDTH) {
                // one line finish
                x = 0;
                y++;
            }

        }

    }
}

// img1[0]  pixels:  x == 0, y = 0, 1, 2, 3, 4, 5, 6, 7
//void MainWindow::genVertical()
//{
//    int x = 0;
//    int y = 0;

//    for (int i = 0; i < 250*128/8; i++) {
//        quint8 byte = img1[i];

//        y = 8 * (i / 250);

//        for (int bit = 0; bit < 8; bit++) {
//            bool bitValue = (byte & (1 << bit)) != 0;

//            if (bitValue) {
//                image.setPixel(x, y, qRgb(250, 250, 250));
//            } else {
//                image.setPixel(x, y, qRgb(30, 200, 30));
//            }

//            // update x, y
//            y++;
//            if ( (y % 8) == 0) {
//                // one vertical 8 pixel line finish
//                y = 8 * (i / 250);
//                x++;
//                if (x == 250) {
//                    x = 0;
//                }
//            }

//        }

//    }
//}


void MainWindow::on_actionLoadImage_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select image File", QString());
    qDebug() << __PRETTY_FUNCTION__ << fileName;

    if (fileName.isEmpty())
    {
        return;
    }

    QPixmap pix(fileName);

    qDebug() << "loaded pix, width:" << pix.width() << " height: " << pix.height();



    ui->labelImage->setPixmap(pix);

    image = pix.toImage();
}

void MainWindow::on_actionGenCodeArray_triggered()
{
    qDebug() << __PRETTY_FUNCTION__;

    QImage imgTmp = image;


    if (imgTmp.width() != IMG_WIDTH
            || imgTmp.height() != IMG_HEIGHT) {
        QMessageBox::warning(this, "warning", "image size not match, will resize");
        imgTmp = imgTmp.scaled(IMG_WIDTH, IMG_HEIGHT);
    }

    QImage imgOut(imgTmp.width(), imgTmp.height(), imgTmp.format());

    // flip vertical and horizontally
    for (int x = 0; x < IMG_WIDTH; x++) {
        for (int y = 0; y < IMG_HEIGHT; y++) {
            QRgb pix = imgTmp.pixel(x, y);

            // to mono
            int diffBackground = abs(qRed(pix) - qRed(TOOL_RGB_BACKGROUND))
                    + abs(qGreen(pix) - qGreen(TOOL_RGB_BACKGROUND))
                    + abs(qBlue(pix) - qBlue(TOOL_RGB_BACKGROUND));

            int diffForeground = abs(qRed(pix) - qRed(TOOL_RGB_FORGROUND))
                    + abs(qGreen(pix) - qGreen(TOOL_RGB_FORGROUND))
                    + abs(qBlue(pix) - qBlue(TOOL_RGB_FORGROUND));

            if (diffBackground < diffForeground) {
                pix = TOOL_RGB_BACKGROUND;
            } else {
                pix = TOOL_RGB_FORGROUND;
            }

            imgOut.setPixel(IMG_WIDTH-1-x, IMG_HEIGHT-1-y, pix);
        }

    }

    QPixmap pix;
    pix.convertFromImage(imgOut);
    ui->labelTmp->setPixmap(pix);
    qDebug() << "tmp label " << ui->labelTmp->width() << " " << ui->labelTmp->height();

    QByteArray baOut;
    baOut.resize(IMG_WIDTH*IMG_HEIGHT/8);

    for (int i = 0; i < IMG_WIDTH*IMG_HEIGHT/8; i++) {
        int y = i * 8 / IMG_WIDTH;

        // reset to background
        quint8 byteValue = 0xFF; // value 1 is background

        int startX = (i*8) % IMG_WIDTH;
        for (int x = startX; x < startX + 8; x++) {
            QRgb rgb = imgOut.pixel(x, y);

            if (rgb != TOOL_RGB_BACKGROUND
                    && rgb != TOOL_RGB_FORGROUND) {
                qDebug() << "other color " << rgb;
            }

            if (TOOL_RGB_FORGROUND == rgb) {
                int bitIdx = 7 -  (x - startX);
                byteValue &= ~(1 << bitIdx);
            }

        }

        baOut[i] = (char) byteValue;
    }

    // reload from byte array
    const char * data = baOut.constData();
    genHorizontal( (const unsigned char *) data );

    showImage();

    QFile fout("output.txt");
      if (fout.open(QFile::WriteOnly | QFile::Truncate)) {
          QTextStream out(&fout);

          for (int i = 0; i < baOut.size(); i++) {
              if ((i % 16) == 0) {
                  out << "\n";
              }

              quint8 byteValue = (quint8) baOut[i];

              QString strValue = QString("%1").arg(byteValue, 2, 16, QChar('0'));
              out << "0x" << strValue << ", ";

          }

      }

      QFile binFile("output.bin");
      if (binFile.open(QIODevice::WriteOnly)) {
          binFile.write(baOut);
          binFile.close();

          qDebug() << "written to output.bin";

      }

}

void MainWindow::on_actionloadBinFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select bin File", QString());
    qDebug() << __PRETTY_FUNCTION__ << fileName;

    if (fileName.isEmpty())
    {
        return;
    }

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        quint8 buf[IMG_WIDTH * IMG_HEIGHT / 8] = {0};
        qint64 ret = f.read((char *)buf, IMG_WIDTH * IMG_HEIGHT / 8);
        if (ret != IMG_WIDTH * IMG_HEIGHT / 8) {
            qDebug() << "warning, not all data read, only" << ret;
        }

        genHorizontal(buf);
        showImage();
    }


}

void MainWindow::on_actionTextToImage_triggered()
{
    DialogTextToImage dlg;
    dlg.exec();
}

void MainWindow::on_actionmergeBins_triggered()
{
    DialogMergeBins dlg;
    dlg.exec();
}
