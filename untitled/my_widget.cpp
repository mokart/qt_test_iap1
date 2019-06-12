#include "my_widget.h"
#include "ui_my_widget.h"
#include <QDebug>
#include <QFileDialog>
#include "crc16.h"
#include <QMessageBox>

my_Widget::my_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::my_Widget)
{
    ui->setupUi(this);
    serialthread = new SerialThread(this);
    connect(serialthread,SIGNAL(recved_package(QByteArray)),this,SLOT(receivd_package(QByteArray)));
    init_com_info();
    timeer = new QTimer(this);
    connect(timeer,SIGNAL(timeout()),this,SLOT(sendfile_timeout()));
    sendinfo.send_retry_cnt=0;
    sendinfo.sent_package = 0;
    sendinfo.sent_ok_package = 0;
}

my_Widget::~my_Widget()
{
    delete ui;
}


void my_Widget::on_btnOpen_clicked()
{
    if(ui->btnOpen->text() == "打开串口")
       {
           serialthread->opencom(ui->cmbPortName->currentText());
           ui->btnOpen->setText("关闭串口");
       }
       else
       {
           serialthread->closecom();
           ui->btnOpen->setText("打开串口");
    }
}

void my_Widget::init_com_info()
{
    //读取串口信息
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //qDebug()<<"Name:"<<info.portName();
        //qDebug()<<"Description:"<<info.description();
        //qDebug()<<"Manufacturer:"<<info.manufacturer();

        //这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            //将串口号添加到cmb
            ui->cmbPortName->addItem(info.portName());
            //关闭串口等待人为(打开串口按钮)打开
            serial.close();
        }
    }
}

void my_Widget::on_pushButton_sent_clicked()
{

    serialthread->send_com(ui->lineEdit_serial->text());
}

void my_Widget::receivd_package(QByteArray qba)
{
    QString str;

    for(int i=0;i<qba.size();i++)
    {
        str.append(QString::number((unsigned char)qba[i], 16).toUpper());
        str.append(" ");
    }
    ui->textEdit->append(str);
    //qDebug("qba %x",(unsigned char)qba.at(i));
    sendinfo.sent_ok_package++;
    qDebug("sendinfo.sent_package = %d",sendinfo.sent_package);
    qDebug("sendinfo.sent_ok_package = %d",sendinfo.sent_ok_package);

}

void my_Widget::on_pushButton_openfile_clicked()
{
    openfileName = QFileDialog::getOpenFileName(this,
                                                       tr("文件对话框！"),
                                                       "F:",
                                                       tr("固件文件(*.bin)"));
    ui->file_lineEdit->setText(openfileName);
}

void my_Widget::on_pushButton_upgrade_clicked()
{
    openfile.setFileName(openfileName);
    openfile.open(QIODevice::ReadOnly);
    QDataStream in(&openfile);
    //qDebug("fs:%d", file.size());
    unsigned char bf[200];

    USHORT rescrc=0xffff;
    int res_len=200;
    //int packge=0;

    sendinfo.upfilesize = openfile.size();
    if(sendinfo.upfilesize == 0)
    {
        QMessageBox::warning(NULL, "文件错误", "文件长度是0", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    sendinfo.upfilesize = sendinfo.upfilesize/200+1;
    if(sendinfo.upfilesize%200 == 0)
    {
        sendinfo.upfilesize+=1;
    }

    //进行获取文件的校验和操作
    do
    {
        res_len = in.readRawData((char *)&bf,res_len);
        if(res_len != 0)
        {
            //进行CRC16校验
            rescrc = fenduanCRC(bf,200,rescrc);
        }
    }while( res_len != 0 );

    ui->label_checksum->setText(QString("rescrc:%1").arg(rescrc,0,16));
    qDebug("rescrc:%x",rescrc);
    sendinfo.crc16_H = rescrc>>8;
    sendinfo.crc16_L = rescrc;


    openfile.close();

    timeer->start(1000);

}


void my_Widget::sendfile_timeout()
{
    unsigned char  senddata[250];
    unsigned char  checksum=0;
    unsigned char  bf[200];

    openfile.setFileName(openfileName);
    openfile.open(QIODevice::ReadOnly);
    QDataStream in(&openfile);

    int res_len = 200;

    //发送到串口进行升级
    if(sendinfo.sent_ok_package == sendinfo.sent_package)
    {
        openfile.seek(200*sendinfo.sent_ok_package);
        res_len = in.readRawData((char *)&bf,res_len);
        if(res_len!=0)
        {
            sendinfo.sent_package++;
            for(int i=0;i<res_len+3;i++)
                checksum+=bf[i];
            //qDebug("res_len :%d",res_len);
            senddata[0] = 0xdd;
            senddata[1] = 0x99;
            senddata[2] = sendinfo.sent_package;
            senddata[3] = res_len+3;
            senddata[res_len+4] = checksum;
            senddata[res_len+5] = 0xed;
            serialthread->send_com((const char *)senddata,res_len+6);
            timeer->start(1000);
            qDebug()<<"here";
            sendinfo.send_retry_cnt=0;
        }
    }
    else if( (sendinfo.sent_ok_package != sendinfo.sent_package) && (sendinfo.send_retry_cnt<3) )
    {
        sendinfo.send_retry_cnt++;
        if(res_len!=0)
        {
            for(int i=0;i<res_len+3;i++)
                checksum+=bf[i];
            //qDebug("res_len :%d",res_len);
            senddata[0] = 0xdd;
            senddata[1] = 0x99;
            senddata[2] = sendinfo.sent_package;
            senddata[3] = res_len+3;
            senddata[res_len+4] = checksum;
            senddata[res_len+5] = 0xed;
            serialthread->send_com((const char *)senddata,res_len+6);
            qDebug()<<"here1";
        }
        timeer->start(1000);
        openfile.close();
        return;

    }
    if(sendinfo.send_retry_cnt>=3)
    {
        timeer->stop();
        sendinfo.send_retry_cnt = 0;
        sendinfo.sent_ok_package = 0;
        sendinfo.sent_package = 0;
        qDebug()<<"here2";
    }

    openfile.close();
}

//dd 99 04 01 7b ed
