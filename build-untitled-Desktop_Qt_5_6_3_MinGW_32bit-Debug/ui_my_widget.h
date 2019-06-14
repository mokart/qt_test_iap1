/********************************************************************************
** Form generated from reading UI file 'my_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MY_WIDGET_H
#define UI_MY_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_my_Widget
{
public:
    QComboBox *cmbPortName;
    QPushButton *btnOpen;
    QLineEdit *lineEdit_serial;
    QTextEdit *textEdit;
    QPushButton *pushButton_sent;
    QLineEdit *file_lineEdit;
    QPushButton *pushButton_openfile;
    QPushButton *pushButton_upgrade;
    QLabel *label_checksum;
    QProgressBar *progressBar;

    void setupUi(QWidget *my_Widget)
    {
        if (my_Widget->objectName().isEmpty())
            my_Widget->setObjectName(QStringLiteral("my_Widget"));
        my_Widget->resize(738, 554);
        cmbPortName = new QComboBox(my_Widget);
        cmbPortName->setObjectName(QStringLiteral("cmbPortName"));
        cmbPortName->setGeometry(QRect(30, 50, 69, 22));
        btnOpen = new QPushButton(my_Widget);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));
        btnOpen->setGeometry(QRect(30, 150, 75, 23));
        lineEdit_serial = new QLineEdit(my_Widget);
        lineEdit_serial->setObjectName(QStringLiteral("lineEdit_serial"));
        lineEdit_serial->setGeometry(QRect(40, 250, 113, 20));
        textEdit = new QTextEdit(my_Widget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(160, 30, 491, 191));
        pushButton_sent = new QPushButton(my_Widget);
        pushButton_sent->setObjectName(QStringLiteral("pushButton_sent"));
        pushButton_sent->setGeometry(QRect(220, 250, 75, 23));
        file_lineEdit = new QLineEdit(my_Widget);
        file_lineEdit->setObjectName(QStringLiteral("file_lineEdit"));
        file_lineEdit->setGeometry(QRect(40, 340, 621, 31));
        pushButton_openfile = new QPushButton(my_Widget);
        pushButton_openfile->setObjectName(QStringLiteral("pushButton_openfile"));
        pushButton_openfile->setGeometry(QRect(120, 450, 75, 23));
        pushButton_upgrade = new QPushButton(my_Widget);
        pushButton_upgrade->setObjectName(QStringLiteral("pushButton_upgrade"));
        pushButton_upgrade->setGeometry(QRect(400, 450, 75, 23));
        label_checksum = new QLabel(my_Widget);
        label_checksum->setObjectName(QStringLiteral("label_checksum"));
        label_checksum->setGeometry(QRect(270, 440, 61, 31));
        progressBar = new QProgressBar(my_Widget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(240, 390, 118, 23));
        progressBar->setValue(24);

        retranslateUi(my_Widget);

        QMetaObject::connectSlotsByName(my_Widget);
    } // setupUi

    void retranslateUi(QWidget *my_Widget)
    {
        my_Widget->setWindowTitle(QApplication::translate("my_Widget", "my_Widget", Q_NULLPTR));
        btnOpen->setText(QApplication::translate("my_Widget", "\346\211\223\345\274\200\344\270\262\345\217\243", Q_NULLPTR));
        pushButton_sent->setText(QApplication::translate("my_Widget", "\345\217\221\351\200\201", Q_NULLPTR));
        pushButton_openfile->setText(QApplication::translate("my_Widget", "\351\200\211\346\213\251\346\226\207\344\273\266", Q_NULLPTR));
        pushButton_upgrade->setText(QApplication::translate("my_Widget", "\345\215\207\347\272\247", Q_NULLPTR));
        label_checksum->setText(QApplication::translate("my_Widget", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class my_Widget: public Ui_my_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MY_WIDGET_H
