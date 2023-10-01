#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QUdpSocket>
#include <QLineSeries>
//#include <QMessageBox>
//#include <QProgressBar>


using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void quicksort(QVector<double>& v, int start, int end);

    ~MainWindow();
private slots:
    void processPendingDatagrams();

    void on_waveViever_triggered();

    void on_about_triggered();

    void on_startStop_clicked(bool checked);



private:
    Ui::MainWindow *ui;
    QByteArray datagram;
    QUdpSocket udpSocket;
    QVector<double> x, y, xPoint, yPoint;
    QVector<double> xMed{0,4500}, yMed;
    int progress = 0;

};
#endif // MAINWINDOW_H
