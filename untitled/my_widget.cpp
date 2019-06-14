#include "my_widget.h"
#include "ui_my_widget.h"
#include <QDebug>
#include <QFileDialog>
#include "crc16.h"
#include <QMessageBox>

#define DELAY_TIME  500
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
    ui->progressBar->setMaximum(0);
    ui->progressBar->setMinimum(0);
    ui->pushButton_upgrade->setEnabled(false);

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
           ui->pushButton_upgrade->setEnabled(true);
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


//获取从单片机接收到的数据
//比如 dd 99 04 01 7b ed

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
    sendinfo.sent_ok_package = (unsigned char)qba.at(2)<<8;
    sendinfo.sent_ok_package |= (unsigned char)qba.at(3);
    ui->progressBar->setValue( sendinfo.sent_ok_package );
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
    if(ui->pushButton_upgrade->text() == "升级")
    {
        openfile.setFileName(openfileName);
        openfile.open(QIODevice::ReadOnly);
        QDataStream in(&openfile);
        //qDebug("fs:%d", file.size());
        unsigned char bf[200];

        USHORT rescrc=0xffff;
        int res_len=200;
        //int packge=0;


        sendinfo.send_retry_cnt = 0;
        sendinfo.sent_ok_package = 0;
        sendinfo.sent_package = 0;
        sendinfo.upfilesize = openfile.size();
        if(sendinfo.upfilesize == 0)
        {
            QMessageBox::warning(NULL, "文件错误", "文件长度是0", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }



        sendinfo.package_cnt = sendinfo.upfilesize/200+1;
        if(sendinfo.upfilesize%200 == 0)
        {
            sendinfo.package_cnt+=1;
        }

        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum( sendinfo.package_cnt );

        //qDebug("sendinfo.upfilesize : %d",sendinfo.upfilesize);
        //qDebug("sendinfo.package_cnt : %d",sendinfo.package_cnt);

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

        timeer->start(DELAY_TIME);
        ui->pushButton_upgrade->setText("停止升级");
    }
    else
    {
        ui->pushButton_upgrade->setText("升级");
        ui->progressBar->setMaximum(0);
        ui->progressBar->setMinimum(0);
        timeer->stop();

    }

}


void my_Widget::sendfile_timeout()
{
    unsigned char  senddata[250];
    unsigned char  checksum=0;
    unsigned char  bf[200];
    int i ;
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

            //qDebug("res_len :%d",res_len);
            senddata[0] = 0xdd;
            senddata[1] = 0x99;
            senddata[2] = res_len+5;
            senddata[3] = sendinfo.sent_package>>8;
            senddata[4] = sendinfo.sent_package;

            for(i=0;i<res_len;i++)
                senddata[5+i] = bf[i];
            checksum=0;
            for(i=0;i<senddata[2];i++)
                checksum+=senddata[i];

            senddata[res_len+5] = checksum;
            senddata[res_len+6] = 0xed;
            serialthread->send_com((const char *)senddata,res_len+7);
            timeer->start(DELAY_TIME);
            qDebug()<<"here";
            sendinfo.send_retry_cnt=0;
        }
        else
        {
            if( (sendinfo.sent_package == sendinfo.sent_ok_package) &&  (sendinfo.sent_ok_package == sendinfo.package_cnt) )
            {
                unsigned char  senddata[11];
                senddata[0] = 0xdd;
                senddata[1] = 0x98;
                senddata[2] = 0x9;
                senddata[3] = sendinfo.upfilesize>>24;
                senddata[4] = sendinfo.upfilesize>>16;
                senddata[5] = sendinfo.upfilesize>>8;
                senddata[6] = sendinfo.upfilesize;
                senddata[7] = sendinfo.crc16_H;
                senddata[8] = sendinfo.crc16_L;
                unsigned char sum=0;
                for(int i=0;i<senddata[2];i++)
                {
                    sum+=senddata[i];
                }
                senddata[9] = sum;
                senddata[10] = 0xed;
                serialthread->send_com((const char *)senddata,11);

                for(i=0;i<11;i++)
                {
                     qDebug("sd:%x",senddata[i]);
                }

                timeer->stop();
                sendinfo.send_retry_cnt = 0;
                sendinfo.sent_ok_package = 0;
                sendinfo.sent_package = 0;
                qDebug()<<"updata_ok";

            }
        }
    }
    else if( (sendinfo.sent_ok_package != sendinfo.sent_package) && (sendinfo.send_retry_cnt<3) )
    {
        sendinfo.send_retry_cnt++;
        if(res_len!=0)
        {
            sendinfo.sent_package = sendinfo.sent_ok_package;
            sendinfo.sent_package++;
            //qDebug("res_len :%d",res_len);
            //qDebug("res_len :%d",res_len);
            senddata[0] = 0xdd;
            senddata[1] = 0x99;
            senddata[2] = res_len+5;
            senddata[3] = sendinfo.sent_package>>8;
            senddata[4] = sendinfo.sent_package;
            for(i=0;i<res_len;i++)
                senddata[5+i] = bf[i];

            checksum = 0;
            for(i=0;i<senddata[2];i++)
                checksum+=senddata[i];

            senddata[res_len+5] = checksum;
            senddata[res_len+6] = 0xed;

            serialthread->send_com((const char *)senddata,res_len+7);
            timeer->start(DELAY_TIME);
            qDebug()<<"here1";
        }
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
