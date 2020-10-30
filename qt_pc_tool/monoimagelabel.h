#ifndef MONOIMAGELABEL_H
#define MONOIMAGELABEL_H

#include <QLabel>

class MonoImageLabel : public QLabel
{
    Q_OBJECT

public:
    enum {
        VIEW_PORTRAIT,
        VIEW_LANDSCAPE
    };

public:
    explicit MonoImageLabel(QWidget *parent = nullptr);

    void setEpaperBinData(const QByteArray &data);
    void setViewMode(int newMode);

signals:

public slots:

private:
    void updateImageFromBin();

private:
    int viewMode;
    QByteArray binData;
    QImage previewImage;    // portrait, mono
};

#endif // MONOIMAGELABEL_H
