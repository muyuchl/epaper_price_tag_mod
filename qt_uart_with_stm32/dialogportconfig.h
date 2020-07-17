#ifndef DIALOGPORTCONFIG_H
#define DIALOGPORTCONFIG_H

#include <QDialog>
#include <QSerialPortInfo>


namespace Ui {
class DialogPortConfig;
}

class DialogPortConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPortConfig(QWidget *parent = nullptr);
    ~DialogPortConfig();

  //  void setPortName(QString &name);
    QString getPortName();

private slots:
    void on_pushButtonRefresh_clicked();

    void on_comboBoxPorts_currentIndexChanged(int index);

    void accept() override;
    void reject() override;

private:
    void refreshPortList();
    void updateComboPortSelection(QString &portName);

private:
    Ui::DialogPortConfig *ui;
    QString selectedPortName;

    QList<QSerialPortInfo> portInfos;
};

#endif // DIALOGPORTCONFIG_H
