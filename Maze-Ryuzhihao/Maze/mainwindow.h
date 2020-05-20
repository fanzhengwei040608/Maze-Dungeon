#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "maze.h"
#include "interface.h"
#include <QStack>
#include <QVector>
#include <QQueue>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QSequentialAnimationGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    interface* surface;//开始界面
    Maze maze;

    QTimer *timer;//计时器
    int time;

    QWidget* mazeWidgets[31][31];//迷宫地图窗口
    QWidget* _pMouse;
    QSequentialAnimationGroup* group;//动画组

    bool isPaintPath;//绘制路径按钮
    bool isShowPath;//
    bool isPlayMode;//玩家模式
    //角色位置
    int xPos;
    int yPos;
    int step;

    QString character[12];//角色动作图片字符串

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initWidget();
    bool checkIsOdd();//检查是否为奇数
    void showMaze();
    void initCharacter();

protected:

    void keyPressEvent(QKeyEvent* event);//键盘事件

private slots:

    void showAnimation();

    void createMazeBtn();

    void playMode();

    void moveCharacter();

    void showPath();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
