#ifndef FORMTEST_H
#define FORMTEST_H

#include <QWidget>
#include <QTime>

namespace Ui {
class FormTest;
}

class QTimer;

class FormTest : public QWidget
{
    Q_OBJECT

public:
    explicit FormTest(QWidget *parent = nullptr);
    ~FormTest();

private slots:
    void on_pushButtonShowOnDevice_clicked();
    void sltSetImgIndexRespTimeout();

public slots:
    void sltFrameReceived(QByteArray frame);

signals:
    void sgnSendFrame(QByteArray frame);

private:
    void parseRxedRFFrame(const QByteArray &frame);
    void parseLoadImageIndexRespFrame(const QByteArray &frame);

private:
    Ui::FormTest *ui;
    QTimer *setImgIndexResponseTimer;
    QTime statTime;
};

#endif // FORMTEST_H
