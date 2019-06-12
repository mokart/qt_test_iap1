#ifndef MY_WIDGET_H
#define MY_WIDGET_H
#include "serialthread.h"
#include <QWidget>
#include <QFile>
#include <QTimer>

namespace Ui {
class my_Widget;
}

struct Send_Info
{
    int upfilesize;
    int package_cnt;
    int sent_package;
    int sent_ok_package;
    unsigned char crc16_H;
    unsigned char crc16_L;
    int send_retry_cnt;
};


class my_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit my_Widget(QWidget *parent = 0);
    ~my_Widget();

private slots:

    void on_btnOpen_clicked();

    void on_pushButton_sent_clicked();

    void receivd_package(QByteArray qba);
    void on_pushButton_openfile_clicked();

    void on_pushButton_upgrade_clicked();

    void sendfile_timeout();
private:
    Ui::my_Widget *ui;
    SerialThread *serialthread;
    void init_com_info();

    QFile openfile;
    QString openfileName;
    QTimer *timeer;
    struct Send_Info sendinfo;
};



#endif // MY_WIDGET_H
