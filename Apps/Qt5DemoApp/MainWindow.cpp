#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QApplication>
#include <QDebug>
struct SensorData {
    int lightLevel;
    int ledBrightness;
    int pirState;
};
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*QString input = "Light Level: 599 | LED Brightness: 106 | PIR State: 1";

    QStringList numbers;
    QString number;

    for (int i = 0; i < input.length(); ++i) {
        if (input.at(i).isDigit()) {
            number.append(input.at(i));
        }
        else if (!number.isEmpty()) {
            numbers.append(number);
            number.clear();
        }
    }

    if (!number.isEmpty()) {
        numbers.append(number);
    }
    ui->plainTextEdit->appendPlainText(numbers[0]);
    ui->plainTextEdit->appendPlainText(numbers[1]);
    ui->plainTextEdit->appendPlainText(numbers[2]);*/

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
    light->setIntensity(0.1f);  // ���ù�Դ��ǿ��
    Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DCore::QTransform* lighttransform = new Qt3DCore::QTransform();
    lighttransform->setTranslation(QVector3D(0, 0, 10));
    lightEntity->addComponent(lighttransform);
    lightEntity->addComponent(light);

    // ����ʵ��
    Qt3DCore::QEntity* modelEntity = new Qt3DCore::QEntity(rootEntity);

    // ����һ���任���
    modeltransform = new Qt3DCore::QTransform();
    modeltransform->setTranslation(QVector3D(8, 0, 3));
    modeltransform->setRotationY(-90);
    // ����ģ��
    Qt3DRender::QMesh* modelMesh = new Qt3DRender::QMesh();
    QString exeFullPath = QCoreApplication::applicationDirPath();
    QString modelPath = exeFullPath + QString::fromLocal8Bit("\\model\\BodyMesh.obj");
    modelMesh->setSource(QUrl::fromLocalFile(modelPath)); // �滻Ϊ���ģ��·��
    modelEntity->addComponent(modelMesh);

    // ��������
    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    modelEntity->addComponent(material);
    modelEntity->addComponent(modeltransform);

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
            QString input = data;

            QStringList numbers;
            QString number;

            for (int i = 0; i < input.length(); ++i) {
                if (input.at(i).isDigit()) {
                    number.append(input.at(i));
                }
                else if (!number.isEmpty()) {
                    numbers.append(number);
                    number.clear();
                }
            }

            if (!number.isEmpty()) {
                numbers.append(number);
            }

            SensorData sensorData;
            sensorData.lightLevel = numbers[0].toInt();
            sensorData.ledBrightness = numbers[1].toInt();
            sensorData.pirState = numbers[2].toInt();
            double intensity = sensorData.ledBrightness;
            double lightlevel = sensorData.lightLevel;
            if (lightlevel > 650)
            {
                light->setIntensity(1.0);
            }
            else
            {
                light->setIntensity(0.1);
            }
            if (sensorData.pirState == 1)
            {
                isStop = false;
            }
            else
            {
                isStop = true;
            }
            view->defaultFrameGraph()->setClearColor(QColor());
            qDebug() << "Received data:" << data;
            // �����ﴦ����յ������ݣ����Խ�����ʾ�ڽ�����
        }
        });

    connect(&modeltimer, &QTimer::timeout, [&]() {
        if (isStop)
        {
            auto translation = modeltransform->translation();
            translation.setX(8);
            modeltransform->setTranslation(translation);
        }
        auto x = modeltransform->translation().x();
        x -= 20*0.05;
        auto translation = modeltransform->translation();
        translation.setX(x);
        modeltransform->setTranslation(translation);
        if (x < -5)
        {
            isStop = true;
        }
        if (isStop)
        {
            auto translation = modeltransform->translation();
            translation.setX(8);
            modeltransform->setTranslation(translation);
        }
        });
    timer.start(1000); // 1���Ӷ�ȡһ�δ�������
    modeltimer.start(50); // 1���Ӷ�ȡһ�δ�������
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
    auto intensity = light->intensity();
    // ���¼����ϵ� "+" �����ӵ�����
    if (event->key() == Qt::Key_A)
    {
        intensity+=0.1;
        light->setIntensity(0.1);
    }

    // ���¼����ϵ� "-" ����С������
    if (event->key() == Qt::Key_S)
    {
        intensity -= 0.1;
        isStop = false;
        light->setIntensity(1.0);
    }

    QMainWindow::keyReleaseEvent(event);
}
