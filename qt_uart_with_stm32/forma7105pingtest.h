#ifndef FORMA7105PINGTEST_H
#define FORMA7105PINGTEST_H

#include <QWidget>
#include <QTime>

namespace Ui {
class FormA7105PingTest;
}

class A7105PingTest;


class FormA7105PingTest : public QWidget
{
    Q_OBJECT

public:
    explicit FormA7105PingTest(QWidget *parent = nullptr);
    ~FormA7105PingTest();

    void setA7105PingTest(A7105PingTest *pingTest);

private slots:
    void on_pushButtonStart_clicked();
    void sltProgressChange(int progress);

    void on_pushButtonStop_clicked();

private:
    Ui::FormA7105PingTest *ui;

    A7105PingTest *a7105PingTest;
    QTime pingTime;
};

#endif // FORMA7105PINGTEST_H
