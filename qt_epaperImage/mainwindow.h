#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QImage>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void generateImage();
    void showImage();

private slots:
    void on_actionLoadImage_triggered();

    void on_actionGenCodeArray_triggered();

    void on_actionloadBinFile_triggered();

    void on_actionTextToImage_triggered();

    void on_actionmergeBins_triggered();

private:
    void genHorizontal(const unsigned char *img);

   // void genVertical();

    void genTest();

private:
    Ui::MainWindow *ui;

    QImage image;
};

#endif // MAINWINDOW_H
