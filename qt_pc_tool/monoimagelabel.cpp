#include "monoimagelabel.h"

// in portrait mode
const int IMG_WIDTH = 128;
const int IMG_HEIGHT = 250;


MonoImageLabel::MonoImageLabel(QWidget *parent) : QLabel(parent),
    viewMode(VIEW_LANDSCAPE)
{

}

void MonoImageLabel::setEpaperBinData(const QByteArray &data)
{
    binData = data;
    updateImageFromBin();
}

void MonoImageLabel::setViewMode(int newMode)
{
    if (newMode != viewMode) {
        viewMode = newMode;
        updateImageFromBin();
    }
}

void MonoImageLabel::updateImageFromBin()
{
    binData.resize(IMG_WIDTH * IMG_HEIGHT /8);

    previewImage = QImage(IMG_WIDTH, IMG_HEIGHT, QImage::Format_Mono);

    for (int i = 0; i < binData.size(); i++) {
        quint8 byteData = static_cast<quint8>(binData.at(i));

        // pixel index is i * 8
        int row = i * 8 / IMG_WIDTH;
        int colStart = (i * 8) % IMG_WIDTH;

        for (int j = 0; j < 8; j++) {
            if (byteData & (1 << (7-j))) {
                previewImage.setPixel(colStart + j, row, 1);
            } else {
                previewImage.setPixel(colStart + j, row, 0);
            }
        }

    }



   // QImage mirroredImage = previewImage.mirrored(true, false);


    QPixmap pix;
    pix.convertFromImage(previewImage);

    if (VIEW_LANDSCAPE == viewMode) {
        QMatrix matrix;
        matrix.rotate(-90);
        pix = pix.transformed(matrix, Qt::SmoothTransformation);
    }

    setPixmap(pix);
}
