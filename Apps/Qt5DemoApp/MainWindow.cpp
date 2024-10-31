#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    // ����Qt3D��ͼ
    view = new Qt3DExtras::Qt3DWindow();
    view->defaultFrameGraph()->setClearColor(Qt::black);
    QWidget* container = QWidget::createWindowContainer(view);
    container->setMinimumSize(1024, 600); // ������С��С
    // ����һ��3D����
    Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity();

    // ����һ������
    Qt3DExtras::QSphereMesh* sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRadius(1);

    // ����һ������
    material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0xbeb32b)));
    //material->setAmbient(QColor(QRgb(0xbeb32b)));
    material->setAmbient(QColor(QRgb(0xbeb32b)));
    material->setSpecular(Qt::white);
    material->setShininess(50);

    // ����һ���任���
    Qt3DCore::QTransform* transform = new Qt3DCore::QTransform();
    transform->setTranslation(QVector3D(0, 0, 0));

    // �����塢���ʺͱ任�����ӵ�һ��ʵ����
    Qt3DCore::QEntity* sphereEntity = new Qt3DCore::QEntity(rootEntity);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(material);
    sphereEntity->addComponent(transform);

    // ����һ����Դ����ӵ���ʵ����
    light = new Qt3DRender::QPointLight();
    light->setIntensity(0.8f);  // ���ù�Դ��ǿ��
    Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DCore::QTransform* lighttransform = new Qt3DCore::QTransform();
    lighttransform->setTranslation(QVector3D(0, 0, 10));
    lightEntity->addComponent(lighttransform);
    lightEntity->addComponent(light);

    // ����һ�������
    Qt3DRender::QCamera* camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 10));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // ����������Ϊ��ͼ�ĸ�ʵ��
    view->setRootEntity(rootEntity);
    ui->groupBox_view->layout()->addWidget(container);
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
        qDebug() << "frequency: " << frequency;
        qDebug() << "port: " << port;
        serial.setPortName(port); // �滻Ϊ��Ĵ��ں�
        serial.setBaudRate(frequency);
        if (!serial.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open serial port";
        }
        else
        {
            qDebug() << "Success to open serial port";
        }
      });
    connect(&timer, &QTimer::timeout, [&]() {
        qDebug() << "1111";

        if (serial.waitForReadyRead(100)) {
            QByteArray data = serial.readAll();
            ui->plainTextEdit->appendPlainText(data);
            qDebug() << "Received data:" << data;
            // �����ﴦ����յ������ݣ����Խ�����ʾ�ڽ�����
        }
        });
    timer.start(1000); // 1���Ӷ�ȡһ�δ�������
    /*connect(ui->pushButton, &QPushButton::clicked, [&]() { 
        ui->label->setText("2");
    });
    connect(ui->pushButton_2, &QPushButton::clicked, [&]() { ui->label->setText("4"); });*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ��д�¼��������Բ�������¼�
void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    qDebug() << "2222";
    auto intensity = light->intensity();
    // ���¼����ϵ� "+" �����ӵ�����
    if (event->key() == Qt::Key_A)
    {
        qDebug() << "2222";
        intensity+=0.1;
        ui->plainTextEdit->appendPlainText("light:0");
        light->setIntensity(0.1);
    }

    // ���¼����ϵ� "-" ����С������
    if (event->key() == Qt::Key_S)
    {
        qDebug() << "333";
        intensity -= 0.1;
        ui->plainTextEdit->appendPlainText("light:1");
        light->setIntensity(1.0);
    }

    QMainWindow::keyReleaseEvent(event);
}
