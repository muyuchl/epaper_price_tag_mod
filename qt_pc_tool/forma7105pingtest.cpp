#include "forma7105pingtest.h"
#include "ui_forma7105pingtest.h"
#include "a7105pingtest.h"

#include <QDebug>

FormA7105PingTest::FormA7105PingTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormA7105PingTest),
    a7105PingTest(nullptr)
{
    ui->setupUi(this);
}

FormA7105PingTest::~FormA7105PingTest()
{
    delete ui;
}

void FormA7105PingTest::setA7105PingTest(A7105PingTest *pingTest)
{
    a7105PingTest = pingTest;
    connect(a7105PingTest, &A7105PingTest::sgnProgressChange,
            this, &FormA7105PingTest::sltProgressChange);
}

void FormA7105PingTest::on_pushButtonStart_clicked()
{
    Q_ASSERT(a7105PingTest);

    int count = ui->spinBoxCount->value();

    a7105PingTest->start(count);
    pingTime.start();
}

void FormA7105PingTest::sltProgressChange(int progress)
{
    ui->progressBar->setValue(progress);

    if (100 == progress) {
        QString strElapsed = QString("elapsed %1 ms").arg(pingTime.elapsed());
        ui->labelPingTestStatus->setText(strElapsed);
    }
}

void FormA7105PingTest::on_pushButtonStop_clicked()
{
    a7105PingTest->stop();
    ui->labelPingTestStatus->setText("");
}
