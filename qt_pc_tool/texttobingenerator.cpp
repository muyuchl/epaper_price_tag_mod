#include "texttobingenerator.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QImage>

#include <QFile>

const int LANDSCAPE_IMAGE_WIDTH = 250;
const int LANDSCAPE_IMAGE_HEIGHT = 128;

// epaper pixel data is portrait
const int IMG_WIDTH = 128;
const int IMG_HEIGHT = 250;

TextToBinGenerator::TextToBinGenerator()
{

}

void TextToBinGenerator::generateBin(const QString &fontFamily, int pixelSize,
                                     const QStringList &lines, const QString &binFile)
{
    QImage landscapeImage(LANDSCAPE_IMAGE_WIDTH, LANDSCAPE_IMAGE_HEIGHT, QImage::Format_Mono);
    uint colorWhite = qRgb(255,255,255);
    landscapeImage.fill(colorWhite);

    QFont font(fontFamily) ;
    font.setPixelSize(pixelSize);

    qDebug() << "font family" << font.family();

    QPainter painter(&landscapeImage);
    painter.setFont(font);

    int lineHeight = LANDSCAPE_IMAGE_HEIGHT / lines.size();
    for (int i = 0; i < lines.size(); i++) {
        QRect rect(0, i * lineHeight, LANDSCAPE_IMAGE_WIDTH, lineHeight);
        painter.drawText(rect, Qt::AlignCenter, lines[i]);
    }

    // save to bin file
    saveToBin(landscapeImage, binFile);
}

void TextToBinGenerator::saveToBin(const QImage &imageLandscape, const QString &binFileName)
{
    QTransform transform;
    transform.rotate(90);

    QImage imgPortrait = imageLandscape.transformed(transform);

    QByteArray baOut;
    baOut.resize(IMG_WIDTH*IMG_HEIGHT/8);

    for (int i = 0; i < baOut.size(); i++) {
        quint8 byteData = 0;

        // pixel index is i * 8
        int row = i * 8 / IMG_WIDTH;
        int colStart = (i * 8) % IMG_WIDTH;

        for (int j = 0; j < 8; j++) {
            int x = colStart + j;
            int y = row;

            if (imgPortrait.pixelColor(x, y) == 0xffffffff) {
                // set bit
                byteData |=  (1 << (7-j));
            }

        }

        baOut[i] = static_cast<char>(byteData);

    }

    QFile binFile(binFileName);
    if (binFile.open(QIODevice::WriteOnly)) {
        binFile.write(baOut);
        binFile.close();

        qDebug() << "written to " << binFileName;

    } else {
        qWarning() << "unable to open bin file to write:" << binFileName;
    }
}
