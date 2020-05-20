#include "interface.h"
#include <QPalette>
#include <QPixmap>
#include <QFont>
#include <QThread>
int time;
QString msg[] = {"加载中..","加载中...","加载中...."};
interface::interface(QWidget *parent) :
    QWidget(parent)
{

    setGeometry(0,0,720,456);

    initWidgets();

    label = new QLabel(this);//转圈动画
    label->setGeometry(200,80,250,200);
    label->setContentsMargins(0,0,0,0);
    movie=new QMovie(":/new/image/loading.gif");
    label->setMovie(movie);
    label->hide();

    msgLabel = new QLabel(this);//提示信息
    msgLabel->setGeometry(320,250,100,75);
    msgLabel->setFont(QFont("宋体",13));
    msgLabel->hide();
}

void interface::paintEvent(QPaintEvent*)
{

    QPainter* painter = new QPainter(this);
    QImage pixmap(":/new/image/startPage.jpg");//开始页
    painter->drawImage(geometry(),pixmap);
    painter->end();//停止绘图，释放绘图设备QPaintDevice
}

void interface::initWidgets()
{
    probar = new QProgressBar(this);//进度条
    probar->setGeometry(120,300,480,10);
    probar->setTextVisible(true);
    probar->hide();

    //this->thread = new QThread(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(loading()));

    button= new QPushButton(this);//开始界面上的play按钮
    button->setGeometry(400,200,200,100);
    button->setStyleSheet("QPushButton{border-image: url(:/new/image/playbutton.png);}"
                          "QPushButton:hover{border-image: url(:/new/image/playbutton1.png);}");
    connect(button,SIGNAL(clicked()),this,SLOT(startButton()));
}

void interface::startButton()
{
    button->hide();//play按钮隐藏

    label->show();//转圈标签显示
    msgLabel->show();//提示消息显示
    movie->start();//转圈动画开始

    timer->start(100);//启动计数器
    probar->show();//进度前显示
}

void interface::loading()
{
    msgLabel->setText(msg[((time++)/2)%3]);//提示信息的变化
    probar->setValue(probar->value()+2);
    if(probar->value() >= 99)
    {
        delete this;
    }

}
