
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QByteArray>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QVector>
#include <QMessageBox>
#include <QIODevice>
#include <QByteArray>
// #include "string.h"
#include <stdint.h>
#include <frame_uart.h>

QSerialPort *Serialll;
QSerialPortInfo info;
QList<QSerialPortInfo> list_COM;
QString select_com, select_baud;

QByteArray frame_data;
QByteArray data_read;
QByteArray all_data;

QByteArray a_setpoint;
QByteArray a_realdata;
uint32_t count = 0;
QString s_setpoint;
QString s_realdata;
QStringList l_realdata;
QStringList l_setpoint;

uint64_t check_len;

frame_uart_t check_true;
uint8_t rx_buff[FRAME_DATA];

bool draw = false;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    Serialll = new QSerialPort(this);
}

MainWindow::~MainWindow()
{
    Serialll->close();
    delete ui;
}

void MainWindow::on_btn_connect_clicked()
{
    select_com = ui->comboBox_com->currentText();
    if (select_com.isEmpty())
    {
        QMessageBox::warning(this, "Can't select COM", "Kiem tra lai COM di dm");
    }
    //   select_baud = ui->comboBox_baud->currentText();
    Serialll->setPortName(select_com);
    ui->comboBox_com->setEnabled(false);
    Serialll->setBaudRate(QSerialPort::Baud115200);
    // Serialll->setBaudRate(select_baud.toInt());
    ui->comboBox_baud->setEnabled(false);
    Serialll->setDataBits(QSerialPort::Data8);   // sua 8 bit 9 bit gi thi sua o day, cứng sẵn trong code cho nhanh
    Serialll->setParity(QSerialPort::NoParity);  // may cai nay co dinh lun, co the taoj them trong file ui de chinh cung duoc, giong hercules a'
    Serialll->setStopBits(QSerialPort::OneStop); // cai quan trong la baudrate vs cong com nao thui
    Serialll->open(QIODevice::ReadWrite);        // baudrate chon tuy chon duoc ma
    if (Serialll->isOpen())
    {
        //        ui->textBrowser_command->append("Connected COM");
        qDebug() << "COM selected!";
    }
    else
    {
        QMessageBox::warning(this, "Can't do it", "Check COM, it can be not open!");
    }
}

void MainWindow::on_btn_refresh_clicked()
{
    updateCom();
}

void MainWindow::updateCom()
{
    ui->comboBox_com->clear();
    ui->comboBox_baud->clear();
    list_COM = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &port, list_COM)
    {
        ui->comboBox_com->addItem(port.portName());
    }
    QList<qint32> baudRates = info.standardBaudRates();
    QList<QString> strBaudRates;
    for (int i = 0; i < baudRates.size(); i++)
    {
        strBaudRates.append(QString::number(baudRates.at(i)));
    }
    ui->comboBox_baud->addItems(strBaudRates);
}

void MainWindow::on_btn_disconect_clicked()
{
    if (Serialll->isOpen())
    {
        Serialll->close();
        ui->comboBox_baud->setEnabled(true);
        ui->comboBox_com->setEnabled(true);
        // ui->textBrowser_command->append("Disconnect COM");
        qDebug() << "Close port oke";
    }
    else
    {
        qDebug() << "Cong COM chua mo ma doi close??";
        QMessageBox::warning(this, "Can not close", "Cong COM chua mo ma close con di me may");
    }
}

void MainWindow::on_btn_plot_clicked()
{
    //    s_realdata.clear();
    //    s_setpoint.clear()
    if (Serialll->isOpen())
    {

        // Timer program
        time.start();
        // create graphs
        ui->customPlot1->addGraph();                          // tao them 1 line
        ui->customPlot1->graph(0)->setPen(QPen(Qt::blue, 2)); // Màu đường line vs độ dày 2 pixel
        ui->customPlot1->xAxis->setLabel("Time (s)");
        ui->customPlot1->yAxis->setLabel("Velocity (vong/phut)");

        // create x, y
        ui->customPlot1->xAxis->setRange(0, 200);
        ui->customPlot1->yAxis->setRange(500, 2000);
        ui->customPlot1->addGraph();
        ui->customPlot1->graph(1)->setPen(QPen(Qt::red, 2));

        // Timer to draw graphs
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateGraph()));
        timer->setTimerType(Qt::CoarseTimer); // lower priority
        timer->start(35);

        qDebug() << "DEBUG 2  ";
    }
}

void MainWindow::updateGraph()
{
    QVector<double> x(dataPointsVector.size());
    for (uint32_t i = 0; i < dataPointsVector.size(); i++)
    {
        x[i] = i;
    }
    ui->customPlot1->graph(0)->addData(x, SetPointsVector);
    ui->customPlot1->graph(1)->setData(x, dataPointsVector);
    // Rescale and redraw graph
    ui->customPlot1->rescaleAxes();
    ui->customPlot1->replot();
    ui->customPlot1->update();
    ui->customPlot1->graph(0)->data()->clear();
    ui->customPlot1->graph(1)->data()->clear();
}
void MainWindow::on_btn_send_clicked()
{
    // Send pid
    float roll_Kp = ui->r_kp->text().toFloat();
    float roll_Ki = ui->r_ki->text().toFloat();
    float roll_Kd = ui->r_kd->text().toFloat();

    float ptich_Kp = ui->p_kp->text().toFloat();
    float ptich_Ki = ui->p_ki->text().toFloat();
    float ptich_Kd = ui->p_kd->text().toFloat();

    float yaz_Kp = ui->y_kp->text().toFloat();
    float yaz_Ki = ui->y_ki->text().toFloat();
    float yaz_Kd = ui->y_kd->text().toFloat();
    //  send offset
    int ser_off_r = ui->s_off_r->text().toInt();
    int ser_off_l = ui->s_off_l->text().toInt();

    int motor_off_r = ui->m_off_r->text().toInt();
    int motor_off_l = ui->m_off_r->text().toInt();

    // add to array
    frame_data.append(roll_Kp);
    frame_data.append(roll_Ki);
    frame_data.append(roll_Kd);
    frame_data.append(ptich_Kp);
    frame_data.append(ptich_Ki);
    frame_data.append(ptich_Kd);
    frame_data.append(yaz_Kp);
    frame_data.append(yaz_Ki);
    frame_data.append(yaz_Kd);
    frame_data.append(ptich_Kd);
    frame_data.append(ser_off_r);
    frame_data.append(ser_off_l);
    frame_data.append(motor_off_r);
    frame_data.append(motor_off_l);

    if (Serialll->isOpen())
    {
        Serialll->write(frame_data);
    }
    else
    {
        QMessageBox::warning(this, "Can not send the PID", "Check the connection");
    }
}

void MainWindow::on_btn_send_2_clicked()
{
    QTimer *timer_receive = new QTimer(this);
    timer_receive->setInterval(7);
    if (Serialll->isOpen())
    {
        connect(timer_receive, &QTimer::timeout, this, &MainWindow::readDataFromSTM);
        timer_receive->start();
    }
    else
    {
        QMessageBox::warning(this, "Can not send the PID", "Check the connection");
    }
}
void MainWindow::readDataFromSTM()
{
    FrameUart call_get_data;
    if (Serialll->bytesAvailable())
    {
        data_read.clear();
        data_read = Serialll->readAll();
        if (call_get_data.call_get_dataFromSTM() == FRAME_OK)
        {
            for (int i = 0; i < sizeof(rx_buff); i++)
            {
                if (i < 4)
                    s_setpoint.append((char)rx_buff[i]);

                else
                    s_realdata.append((char)rx_buff[i]);
            }
            qDebug() << "s_realdata.size()" << s_realdata.size();
            if (s_realdata.size() == 800)
                s_realdata.remove(0, 4);
            if (s_setpoint.size() == 800)
                s_setpoint.remove(0, 4);

            /// qDebug() << "Could not read data";
            l_setpoint.clear();
            l_realdata.clear();
            setpoint_data.clear();
            realdata_data.clear();
            for (int i = 0; i < s_setpoint.size(); i += 4)
            {
                QString str = s_setpoint.mid(i, 4);
                l_setpoint.append(str);
            }

            for (int i = 0; i < s_realdata.size(); i += 4)
            {
                QString str = s_realdata.mid(i, 4);
                l_realdata.append(str);
            }
            for (int i = 0; i < l_realdata.size(); i++)
            {
                double temp_setpoint = l_setpoint[i].toDouble();
                setpoint_data.append(temp_setpoint);
                double temp_realdata = l_realdata[i].toDouble();
                realdata_data.append(temp_realdata);
            }
            SetPointsVector = QVector<double>::fromList(setpoint_data);
            dataPointsVector = QVector<double>::fromList(realdata_data);
        }
    }
    else
    {
        qDebug() << "Could not read data";
    }
}
frame_uart_t FrameUart::call_get_dataFromSTM()
{
    FrameUart get_data;

    check_true = get_data.GetFrameData(reinterpret_cast<uint8_t *>(data_read.data()), data_read.size(), rx_buff);
    if (check_true == FRAME_OK)
        count++;
    qDebug() << "count:  " << count;
    // qDebug() << "sizeof: "<<data_read.size();
    return check_true;
}
