#ifndef FORMTRAFFIC_H
#define FORMTRAFFIC_H

#include <QWidget>

namespace Ui {
class FormTraffic;
}

class FormTraffic : public QWidget
{
    Q_OBJECT

public:
    explicit FormTraffic(QWidget *parent = nullptr);
    ~FormTraffic();

public slots:
    void sltFrameTransmitted(QByteArray frame);
    void sltFrameReceived(QByteArray frame);

private slots:
    void on_pushButtonClear_clicked();

private:
    Ui::FormTraffic *ui;
};

#endif // FORMTRAFFIC_H
