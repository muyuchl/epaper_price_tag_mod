#ifndef DIALOGMERGEBINS_H
#define DIALOGMERGEBINS_H

#include <QDialog>

namespace Ui {
class DialogMergeBins;
}

class DialogMergeBins : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMergeBins(QWidget *parent = nullptr);
    ~DialogMergeBins();

private slots:
    void on_pushButtonBrowse_clicked();

    void on_pushButtonSave_clicked();

private:
    void mergeAndSave(QString &binFile);

private:
    Ui::DialogMergeBins *ui;
};

#endif // DIALOGMERGEBINS_H
