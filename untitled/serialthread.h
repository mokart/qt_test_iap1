#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include <QThread>
#include <QObject>

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QSemaphore>


class SerialThread : public QThread
{
    Q_OBJECT

public:
    SerialThread(QObject *parent);
    void opencom(QString strcom);
    void closecom();
    void send_com(QString str);
    void send_com(const char *data,int length);

signals:
    recved_package(QByteArray bta);

protected:
    void run();

private:
    volatile bool stopped;
    QSerialPort *my_serialport;
    QTimer *timer;
    QByteArray qba_rcv;
private slots:

};

#endif // SERIALTHREAD_H
