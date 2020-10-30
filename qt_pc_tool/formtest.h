#ifndef FORMTEST_H
#define FORMTEST_H

#include <QWidget>

namespace Ui {
class FormTest;
}

class FormTest : public QWidget
{
    Q_OBJECT

public:
    explicit FormTest(QWidget *parent = nullptr);
    ~FormTest();

private slots:
    void on_pushButtonShowOnDevice_clicked();

signals:
    void sgnSendFrame(QByteArray frame);

private:
    Ui::FormTest *ui;
};

#endif // FORMTEST_H
