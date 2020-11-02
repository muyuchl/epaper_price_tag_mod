#ifndef TEXTTOBINGENERATOR_H
#define TEXTTOBINGENERATOR_H
#include <QString>
#include <QStringList>

class QImage;

class TextToBinGenerator
{
public:
    TextToBinGenerator();

    static void generateBin(const QString &fontFamily, int pixelSize,
                            const QStringList &lines, const QString &binFile);


private:
    static void saveToBin(const QImage &imageLandscape, const QString &binFileName);

};

#endif // TEXTTOBINGENERATOR_H
