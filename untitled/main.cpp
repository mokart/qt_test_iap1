#include "my_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    my_Widget w;
    w.show();

    return a.exec();
}
