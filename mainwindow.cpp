#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include "customhelperfunctions.h"
#include <vector>
#include <string>
#include <QComboBox>
#include <termios.h>

extern "C"{
#include "obd_pids.h"
#include "obd_helpers.h"
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::MakePlot();
    //MainWindow::EstablishOBDIIConnection();
    ui->connectButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    outputfile.close();
}

void MainWindow::EstablishOBDIIConnection(char *device_name){

    size_t bytes_read = 0;
    QString ELM327_answer;
    /*----------------------- Configure serial port ------------------*/
    // open serial port file
    get_port(&fd, &device_name);

    // set up serial port
    serial_setup(&fd, vmin, vtime, baud_rate);
    /*----------------------------------------------------------------*/

    /*----------------------- Talking to the device ------------------*/
    set_elm(&fd, SET_NO_SPACES, buff_size); // turn off spaces in response messages

    answer = (char*)malloc(buff_size);

    std::cout << "INIT" << std::endl;
    // elm hard reset
    bytes_read = elm_talk(&fd, answer, buff_size, DEVICE_INFO, 3);
    if(!answer_check(answer, "ELM327", 6)){
        //std::cout << "\nReady to talk!" << std::endl;
        //std::cout << "Answer: " << answer << std::endl;
        ELM327_answer = QString("Connected to: ") + QString(answer);
        successful_connection = true;
        ui->disconnectButton->setEnabled(true);
        ui->connectButton->setEnabled(false);
        ui->FindDevices->setEnabled(false);
    }
    else{
        ELM327_answer = QString("Connection failed!");
        successful_connection = false;
        ui->connectButton->setEnabled(true);
        ui->FindDevices->setEnabled(true);
    }
    ui->ConnectionStatusLbl->setText(ELM327_answer);
    /*----------------------------------------------------------------*/

    if (1 < 0) {
        ui->statusbar->showMessage(
              QString("Error opening socket:"));
        //printf("Error opening socket: %s\n", strerror(errno));
        //exit(EXIT_FAILURE);
        successful_connection = false;
    }
    else {
        successful_connection = true;
        outputfile.open("~/OBDII_Response.txt");
        //supportedCommands = OBDIIGetSupportedCommands(&s);
        //std::cout << supportedCommands.numCommands << std::endl;
        //std::cout << supportedCommands.commands << std::endl;
    }


}
void MainWindow::MakePlot(){
    ui->customPlot->addGraph(); // blue line
    ui->customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    //ui->customPlot->addGraph(); // red line
    //ui->customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->customPlot->xAxis->setTicker(timeTicker);
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->yAxis->setRange(-5, 5);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::realtimeDataSlot(){
    static QTime timeStart = QTime::currentTime();
    size_t bytes_read = 0;
    int16_t pressure;
    //Read sensor Data:
    //char *answer = (char*)malloc(buff_size);
    // calculate two new data points:
    double key = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.5) // at most add point every 500 ms
    {

      // add data to lines:
      // Hier muessen die Werte von OBDII gelesen und geschrieben werden.
        if(successful_connection) {
           bytes_read = elm_talk(&fd, answer, buff_size, PID_MAP, 0);
           pressure = get_vehicle_speed(answer);
            //std::cout << "Pressure: " << answer << std::endl;
           ui->customPlot->graph(0)->addData(key,pressure);
            //outputfile << response.numericValue << std::endl;

        }
        else{
            ui->customPlot->graph(0)->addData(key,-0.96);
        }

      // rescale value (vertical) axis to fit the current data:
      ui->customPlot->graph(0)->rescaleValueAxis(true);
      //ui->customPlot->graph(1)->rescaleValueAxis(true);
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key, 300, Qt::AlignRight);
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2 && successful_connection) // average fps over 2 seconds
    {
      ui->statusbar->showMessage(
            QString("%1 FPS, Total Data points: %2")
            .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
            .arg(ui->customPlot->graph(0)->data()->size())
            , 0);
      lastFpsKey = key;
      frameCount = 0;
    }
}

void MainWindow::on_connectButton_clicked()
{
    // get the selected Device:
    //int dev_num = devices(ui->deviceSelect->currentIndex());
    std::string selected_device = "/dev/" + devices[ui->deviceSelect->currentIndex()];
    //char * char_selected_device = selected_device.c_str();
    MainWindow::EstablishOBDIIConnection(const_cast<char*> (selected_device.c_str()));

}


void MainWindow::on_BaudRateSelect_activated(int index)
{
    switch(index){
        case 0: baud_rate = 9600;
                break;
        case 1: baud_rate = 38400;
                break;
        case 2: baud_rate = 115200;
                break;
    }
    ui->statusbar->showMessage(QString("Baud Rate: %1").arg(baud_rate,0,'i',0));
}


void MainWindow::on_FindDevices_clicked()
{
    devices.clear();
    ui->deviceSelect->clear();
    number_of_devices = get_usb_devices(devices);

    if(number_of_devices > 0){
        ui->deviceSelect->clear();
        for(std::vector<std::string>::iterator it = devices.begin(); it != devices.end(); ++it){
            ui->statusbar->showMessage(QString::fromStdString(*it));
            //std::cout << *it << std::endl;
            ui->deviceSelect->addItem(QString::fromStdString(*it));
            ui->connectButton->setEnabled(true);
        }
    }
    else{
        ui->deviceSelect->addItem(QString("No Device found"));
        ui->connectButton->setEnabled(false);
    }
}




void MainWindow::on_disconnectButton_clicked()
{
    close_port(fd);

    ui->ConnectionStatusLbl->setText(QString("Disconnected"));
    successful_connection = false;
    ui->disconnectButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->FindDevices->setEnabled(true);
}

