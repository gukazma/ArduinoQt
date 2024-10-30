#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QApplication>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    qDebug() << "Available ports:";
    for (const QSerialPortInfo& info : serialPortInfos) {
        ui->comboBox->addItem(info.portName());
        qDebug() << "Name:" << info.portName();
        qDebug() << "Description:" << info.description();
        qDebug() << "Manufacturer:" << info.manufacturer();
    }
    connect(ui->pushButton, &QPushButton::clicked, [&]() {
        serial.close();
        QString  port = ui->comboBox->currentText();
        int frequency = ui->spinBox->value();
        serial.setPortName(port); // 替换为你的串口号
        serial.setBaudRate(frequency);
        if (!serial.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open serial port";
        }
      });
    connect(&timer, &QTimer::timeout, [&]() {
        qDebug() << "1111";

        if (serial.waitForReadyRead(100)) {
            QByteArray data = serial.readAll();
            ui->plainTextEdit->setPlainText(data);
            qDebug() << "Received data:" << data;
            // 在这里处理接收到的数据，可以将其显示在界面上
        }
        });
    timer.start(1000); // 1秒钟读取一次串口数据
    /*connect(ui->pushButton, &QPushButton::clicked, [&]() { 
        ui->label->setText("2");
    });
    connect(ui->pushButton_2, &QPushButton::clicked, [&]() { ui->label->setText("4"); });*/
}

MainWindow::~MainWindow()
{
    delete ui;
}
