
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QTimer>
#include <QElapsedTimer>
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_connect_clicked();

    void on_btn_refresh_clicked();

    void updateCom();

    void on_btn_disconect_clicked();

    void updateGraph();

    void on_btn_plot_clicked();

    void on_btn_send_clicked();

    void on_btn_send_2_clicked();
    void readDataFromSTM();


private:
    Ui::MainWindow *ui;
    QList<double> setpoint_data;
    QList<double> realdata_data;
    QString m_data;
    int count;
//    QTime time;
    QElapsedTimer time;
    QTimer *timer;
    QVector<double> SetPointsVector;
    QVector<double> dataPointsVector;
};

#endif // MAINWINDOW_H
