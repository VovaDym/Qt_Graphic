#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(progress);

    ui->plot->xAxis->setLabel("X");
    ui->plot->yAxis->setLabel("Y");

    ui->plot->xAxis->setRange(0, 5000);
    ui->plot->yAxis->setRange(-500, 5000);

    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::quicksort(QVector<double>& v, int start, int end)
{
    if (start < end) {
        int pivot = end;
        int j = start;
        for (int i = start; i < end; ++i)
        {
            if (v[i] < v[pivot])
            {
                std::swap(v[i], v[j]);
                ++j;
            }
        }
        std::swap(v[j], v[pivot]);
        int p = j;
        quicksort(v, start, p - 1);
        quicksort(v, p + 1, end);
    }
}

void MainWindow::processPendingDatagrams()
{
    QDataStream in(&datagram, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_12);
    do
    {
        x.clear();
        y.clear();
        yMed.clear();
        xPoint.clear();
        yPoint.clear();
        datagram.resize(udpSocket.pendingDatagramSize());
        udpSocket.readDatagram(datagram.data(), datagram.size());

        qint16 volSample = 0;
        qint16 indexX = -1;
        while ( !in.atEnd() )
        {
            in >> volSample;
            y.append( (double)volSample );
            ++indexX;
            x.append((double)indexX);
        }
        progress += 1;
        if (progress == ui->progressBar->maximum()) {
            progress = 0;
        }

        ui->progressBar->setValue(progress);
        ui->plot->addGraph();
        ui->plot->graph(0)->setData(x, y);

        ui->plot->addGraph();
        ui->plot->graph(1)->setPen(QPen(QColor("black"), 2, Qt::DotLine));
        ui->plot->addGraph();
        ui->plot->graph(2)->setScatterStyle(QCPScatterStyle::ssCircle);
        ui->plot->graph(2)->setLineStyle(QCPGraph::lsNone);
        ui->plot->graph(2)->setPen(QPen(QColor("red"), 3));

        QVector<double>::iterator itV = std::max_element(y.begin(), y.end());
        yPoint.append(*itV);
        QVector<double>::iterator itr = std::find(y.begin(), y.end(), *itV);
        double itt = std::distance(y.begin(), itr);
        xPoint.append(itt);

        ui->plot->graph(2)->setData(xPoint, yPoint);

        QCPItemText *coords = new QCPItemText(ui->plot);
        coords->position->setCoords((int)std::distance(y.begin(), itr), (int)*itV);
        QString strCoords = "x = " + QString::number(itt) + "  y = " + QString::number(*itV);
        coords->setText(strCoords);
        coords->setFont(QFont(font().family(), 12));

        quicksort(y,0,y.size() - 1);
        yMed.append(y[y.size() / 2]);
        yMed.append(y[y.size() / 2]);

        QCPItemCurve *arrow = new QCPItemCurve(ui->plot);
        arrow->start->setCoords(1830, 470);
        arrow->startDir->setCoords(1830, 470);
        arrow->endDir->setCoords(1830, 470);
        arrow->end->setCoords(2000, (int)y[y.size() / 2]);
        arrow->setHead(QCPLineEnding::esSpikeArrow);

        QCPItemText *median = new QCPItemText(ui->plot);
        median->position->setCoords(1400, 500);
        QString strMedian = "median = " + QString::number(y[y.size() / 2]);
        median->setText(strMedian);
        median->setFont(QFont(font().family(), 12));

        ui->plot->graph(1)->setData(xMed, yMed);

        ui->plot->replot();

        coords->deleteLater();
        median->deleteLater();
        arrow->deleteLater();

    }while (udpSocket.hasPendingDatagrams());
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_waveViever_triggered()
{
    udpSocket.bind(QHostAddress("127.0.0.1"), 10002);
    connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void MainWindow::on_about_triggered()
{
    QMessageBox::information(this,"Аbout the program", "Version 1.0\nThis program is developed in Qt 5.12.12");
}

void MainWindow::on_startStop_clicked(bool checked)
{
    if(checked)
    {
        ui->startStop->setText("start");
        disconnect(&udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
        udpSocket.disconnectFromHost();
    }
    else
    {
        ui->startStop->setText("stop");
        udpSocket.bind(QHostAddress("127.0.0.1"), 10002);
        connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    }
}



