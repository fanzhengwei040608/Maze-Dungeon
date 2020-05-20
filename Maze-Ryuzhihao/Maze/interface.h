#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QMovie>
#include <QPainter>
#include <QStyleOption>
#include <QThread>
//这个类设计游戏开始界面
class interface : public QWidget
{
    Q_OBJECT
public:
    explicit interface(QWidget *parent = 0);
private:
    QLabel* msgLabel;
    QPushButton* button;
    QProgressBar* probar;//进度条
    QTimer* timer;
    QLabel* label;
    QMovie* movie;//请稍后转圈动画
    //QThread* thread;
private:
    void paintEvent(QPaintEvent*);

private:
    void initWidgets();
signals:

public slots:
    void startButton();
    void loading();
};

#endif // INTERFACE_H
