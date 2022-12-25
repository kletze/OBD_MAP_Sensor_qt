#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>

//#include "customhelperfunctions.h"

#include <iostream>
#include <fstream>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void MakePlot();
    void realtimeDataSlot();
    //void EstablishOBDIIConnection();
    void on_connectButton_clicked();

    void on_BaudRateSelect_activated(int index);

    void on_FindDevices_clicked();

    void on_disconnectButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;

    //QComboBox *deviceSelect = new QComboBox;

    std::ofstream outputfile;
    bool successful_connection = false;
    int baud_rate = 9600;
    int number_of_devices = 0;
    std::vector<std::string> devices;

    int fd;                             //File Descriptor
    size_t vmin = 18;
    size_t buff_size = 21; // nbytes
    size_t vtime = 2; // blocking read time in deciseconds

    void EstablishOBDIIConnection(char  *device_name);
    //Read sensor Data:
    //char *answer = (char*)malloc(buff_size);
    char * answer;
};
#endif // MAINWINDOW_H
