#ifndef FORMLED_H
#define FORMLED_H

#include <QWidget>

namespace Ui {
class FormLed;
}

class FormLed : public QWidget
{
    Q_OBJECT

public:
    explicit FormLed(QWidget *parent = nullptr);
    ~FormLed();

public slots:
    void sltFrameReceived(QByteArray frame);

signals:
    void sgnSendFrame(QByteArray frame);

private slots:
    void on_pushButtonLedOn_clicked();

    void on_pushButtonLedOff_clicked();

private:
    Ui::FormLed *ui;
};

#endif // FORMLED_H
