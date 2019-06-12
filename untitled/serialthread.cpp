#include "serialthread.h"
#include <QDebug>
#include <QMessageBox>


//int timerid;
SerialThread::SerialThread(QObject *parent): QThread(parent)
{
   stopped = false;
   my_serialport = new QSerialPort(this);


   //qDebug()<<"SerialThread constructure";
}

void SerialThread::opencom(QString strcom)
{
    my_serialport->setPortName(strcom);
    //以读写方式打开串口
    if(my_serialport->open(QIODevice::ReadWrite))
    {
        //设置波特率
        my_serialport->setBaudRate(9600);
        //设置数据位
        my_serialport->setDataBits(QSerialPort::Data8);
        //设置校验位
        my_serialport->setParity(QSerialPort::NoParity);
        //设置流控制
        my_serialport->setFlowControl(QSerialPort::NoFlowControl);
        //设置停止位
        my_serialport->setStopBits(QSerialPort::OneStop);
        //每秒读一次
        //connect(timer, SIGNAL(timeout()), this, SLOT(readComDataSlot()));
        //timer->start(1000);
        this->start();      //开始读线程
    }
    else
    {
        QMessageBox::about(NULL, "提示", "串口没有打开！");
        return;
    }
}

void SerialThread::closecom()
{
    stopped = true;
    this->wait(500);
    my_serialport->close();

}

void SerialThread::send_com(QString str )
{
    QByteArray bta(str.toLatin1());
    my_serialport->write(bta);
}

void SerialThread::send_com(const char *data, int length)
{
    //qDebug("length :%d",length);
    //for(int i=0;i<length;i++)
    //    qDebug("%d=,%x",i,(unsigned char)data[i]);
    my_serialport->write(data,length);
    my_serialport->waitForBytesWritten(2000);

}

void SerialThread::run()
{
   int i=0;
   unsigned char checksum=0;
   QByteArray tmp_data;
   tmp_data.resize(1);
   qba_rcv.resize(20);
   qba_rcv.clear();


    while(false == stopped)
    {
        my_serialport->waitForReadyRead(500);
        tmp_data = my_serialport->read(1);
        if(tmp_data.size() != 0  )
        {
            //qDebug("%x",(unsigned char)tmp_data.at(0));
            if(qba_rcv.size()<20)
            {
                qba_rcv.append(tmp_data);
                //timer->start(10);
                //qDebug("recvd: %d",qba_rcv.size());
                if( (qba_rcv.size()==1) && ((unsigned char)qba_rcv[0] != 0xdd) )
                {
                   qba_rcv.clear();
                   checksum=0;
                }
            }
            else
            {
                qba_rcv.clear();
                checksum=0;
            }

            //如果正确接受到了数据
            if((unsigned char)qba_rcv.data()[0] == 0xdd && (unsigned char)qba_rcv.at(qba_rcv.size()-1)==0xed && qba_rcv.size()>3)
            {
                for(i=0;i<(unsigned char)qba_rcv.at(2);i++)
                    checksum += qba_rcv.at(i);

                if(checksum != (unsigned char)qba_rcv.at(qba_rcv.at(2)))
                {
                   qba_rcv.clear();
                   checksum=0;
                   continue;
                }
                else
                {
                    //校验也通过了,告知主线程
                    emit recved_package(qba_rcv);
                    qba_rcv.clear();
                    checksum = 0;
                }
            }
        }
    }
    stopped = false;
}












