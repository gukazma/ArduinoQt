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
    // 创建Qt3D视图
    view = new Qt3DExtras::Qt3DWindow();
    view->defaultFrameGraph()->setClearColor(Qt::black);
    QWidget* container = QWidget::createWindowContainer(view);
    container->setMinimumSize(1024, 600); // 设置最小大小
    // 创建一个3D场景
    Qt3DCore::QEntity* rootEntity = new Qt3DCore::QEntity();

    // 创建一个球体
    Qt3DExtras::QSphereMesh* sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRadius(1);

    // 创建一个材质
    material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0xbeb32b)));
    //material->setAmbient(QColor(QRgb(0xbeb32b)));
    material->setAmbient(QColor(QRgb(0xbeb32b)));
    material->setSpecular(Qt::white);
    material->setShininess(50);

    // 创建一个变换组件
    Qt3DCore::QTransform* transform = new Qt3DCore::QTransform();
    transform->setTranslation(QVector3D(0, 0, 0));

    // 将球体、材质和变换组件添加到一个实体中
    Qt3DCore::QEntity* sphereEntity = new Qt3DCore::QEntity(rootEntity);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(material);
    sphereEntity->addComponent(transform);

    // 创建一个光源并添加到根实体中
    light = new Qt3DRender::QPointLight();
    light->setIntensity(0.1f);  // 设置光源的强度
    Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DCore::QTransform* lighttransform = new Qt3DCore::QTransform();
    lighttransform->setTranslation(QVector3D(0, 0, 10));
    lightEntity->addComponent(lighttransform);
    lightEntity->addComponent(light);

    // 创建实体
    Qt3DCore::QEntity* modelEntity = new Qt3DCore::QEntity(rootEntity);

    // 创建一个变换组件
    modeltransform = new Qt3DCore::QTransform();
    modeltransform->setTranslation(QVector3D(8, 0, 3));
    modeltransform->setRotationY(-90);
    // 加载模型
    Qt3DRender::QMesh* modelMesh = new Qt3DRender::QMesh();
    QString exeFullPath = QCoreApplication::applicationDirPath();
    QString modelPath = exeFullPath + QString::fromLocal8Bit("\\model\\BodyMesh.obj");
    modelMesh->setSource(QUrl::fromLocalFile(modelPath)); // 替换为你的模型路径
    modelEntity->addComponent(modelMesh);

    // 创建材质
    Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial();
    modelEntity->addComponent(material);
    modelEntity->addComponent(modeltransform);

    // 创建一个摄像机
    Qt3DRender::QCamera* camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 10));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // 将场景设置为视图的根实体
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
        serial.setPortName(port); // 替换为你的串口号
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
            // 在这里处理接收到的数据，可以将其显示在界面上
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
    timer.start(1000); // 1秒钟读取一次串口数据
    modeltimer.start(50); // 1秒钟读取一次串口数据
    /*connect(ui->pushButton, &QPushButton::clicked, [&]() { 
        ui->label->setText("2");
    });
    connect(ui->pushButton_2, &QPushButton::clicked, [&]() { ui->label->setText("4"); });*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 重写事件过滤器以捕获键盘事件
void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    auto intensity = light->intensity();
    // 按下键盘上的 "+" 键增加灯亮度
    if (event->key() == Qt::Key_A)
    {
        intensity+=0.1;
        light->setIntensity(0.1);
    }

    // 按下键盘上的 "-" 键减小灯亮度
    if (event->key() == Qt::Key_S)
    {
        intensity -= 0.1;
        isStop = false;
        light->setIntensity(1.0);
    }

    QMainWindow::keyReleaseEvent(event);
}
