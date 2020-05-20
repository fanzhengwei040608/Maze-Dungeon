#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStyleFactory>
#include <QMessageBox>
#include <QStack>
#include <QVector>
#include <QQueue>
#include <QTime>
#include <QTimer>
#include <QKeyEvent>
#include <QMovie>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

#define BASIC_LENGTH 40

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFocusPolicy(Qt::StrongFocus);
    initCharacter();
    initWidget();

}

//初始化ui的widgets
void MainWindow::initWidget()
{

    //初始化界面控件
    ui->spinBox->setStyle(QStyleFactory::create("Macintosh"));//高度控制
    ui->spinBox_2->setStyle(QStyleFactory::create("Macintosh"));//宽度控制
    ui->buttonGenerate->setStyle(QStyleFactory::create("fusion"));//生成迷宫按钮
    ui->buttonPlay->setStyle(QStyleFactory::create("fusion"));
    ui->buttonPath->setStyle(QStyleFactory::create("fusion"));
    connect(ui->buttonGenerate,SIGNAL(clicked()),this,SLOT(createMazeBtn()));//生成迷宫按钮
    connect(ui->buttonPath,SIGNAL(clicked()),this,SLOT(showAnimation()));//圆形按钮
    connect(ui->buttonPlay,SIGNAL(clicked()),this,SLOT(playMode()));//玩家模式按钮
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(showPath()));//pushButton 绘制路径按钮
    ui->buttonPath->setStyleSheet("QPushButton{border-image: url(:/new/image/button1.png);}"
                                  "QPushButton:hover{border-image: url(:/new/image/button4.png);}"
                                  "QPushButton:pressed{border-image: url(:/new/image/button2.png);}");

    ui->buttonPlay->setEnabled(false);
    ui->buttonPath->setEnabled(false);

    //游戏的状态
    isPlayMode = false;
    isShowPath = false;

    //创建好迷宫砖块
    for(int i=0; i <31 ;i++)
    {
        for(int j=0; j<31; j++)
        {
            mazeWidgets[i][j]=new QWidget(this);
            mazeWidgets[i][j]->hide();
        }
    }

    //人物
    _pMouse = new QWidget(this);
    _pMouse->setStyleSheet(character[0]);
    _pMouse->hide();

    //动画和时间相关的初始化
    group = new QSequentialAnimationGroup();//动画组
    isShowPath = false;

    timer = new QTimer(this);//计时器
    connect(timer,SIGNAL(timeout()),this,SLOT(moveCharacter()));

    //初始化开始界面
    surface = new interface(this);
}

bool MainWindow::checkIsOdd()
{
    if(ui->spinBox->value()%2 == 0)
        return false;
    if(ui->spinBox_2->value()%2 == 0)
        return false;

    return true;
}

void MainWindow::createMazeBtn()
{
    //停止之前可能进行的所有工作
    timer->stop();
    group->stop();
    time = 1;
    step = 0;
    isShowPath = false;
    isPlayMode = false;
    isPaintPath = false;

    if(!checkIsOdd())
    {
         QMessageBox::information(this,"警告","输入的数必须是奇数",QMessageBox::Yes);
         return;
    }

    //窗口形状变形
    maze.height = ui->spinBox->value();//SpinBox允许用户通过单击向上或向下指示器按钮，
                                       //或通过向上或向下按键盘来选择整数值。可选地，SpinBox也可以编辑，因此用户可以在输入字段中输入文本值。
    maze.width  = ui->spinBox_2->value();
    //setGeometry是相对于父窗体来说的一种对子窗体进行位置设置的方法,setGeometry (9, 9, 50, 25) 从屏幕上（9，9）位置开始
    //（即为最左上角的点）,显示一个50*25的界面（宽50，高25）
    if(maze.height*30+80 <=320)//#define BASIC_LENGTH 40
        this->setGeometry(40,40,240+maze.width*BASIC_LENGTH,320);//在MainWindow中设置迷宫窗口的位置
    else
        this->setGeometry(40,40,240+maze.width*BASIC_LENGTH,maze.height*BASIC_LENGTH+80);
    ui->groupBox->move(width()-180,10);//groupBox 主窗口右边的控制界面组

    //重新初始化mazeWidgets和_pMouse
    _pMouse->hide();
    for(int i=0;i<31;i++)
    {
        for(int j=0 ; j<31;j++)
        {
            mazeWidgets[i][j]->setStyleSheet("");
            mazeWidgets[i][j]->hide();
        }
    }

    //创建新的迷宫
    maze.initMaze(ui->spinBox->value(),ui->spinBox_2->value());//
    maze.createMaze();
    maze.autoFindPath();
    //角色位置更新（相对于主窗口的位置）
    xPos = 40 + maze.X*BASIC_LENGTH;
    yPos = 40 + maze.Y*BASIC_LENGTH;

    showMaze();
}

void MainWindow::showMaze()
{
    //maze.X初始化为maze.height的一半  maze.Y初始化为maze.width的一半
    //setGeometry（x,y,w,h）中 x代表坐标点的横坐标，y代表坐标点的纵坐标
    //所以传递的横坐标为：40+maze.Y*BASIC_LENGTH 纵坐标为：40+maze.X*BASIC_LENGTH
    _pMouse->setGeometry(40+maze.Y*BASIC_LENGTH,40+maze.X*BASIC_LENGTH,BASIC_LENGTH,BASIC_LENGTH);
    _pMouse->show();
    for(int i=0 ; i<maze.height ; i++)
    {
        for(int j=0 ; j<maze.width; j++)
        {
            mazeWidgets[i][j]->show();
            mazeWidgets[i][j]->setGeometry(40+j*BASIC_LENGTH,40+i*BASIC_LENGTH,BASIC_LENGTH,BASIC_LENGTH);
            //将maze中生成的Point矩阵数组，与mainwindow中生成的QWidget数组一一对应，对mazeWidgets数组赋值
            if(maze.matrix[i][j].state == 0)//状态0 代表 墙 障碍
                mazeWidgets[i][j]->setStyleSheet("border-image: url(:/new/image/Wall.png);");

            if(maze.matrix[i][j].state == 1)//状态1 代表 草地 非障碍
                mazeWidgets[i][j]->setStyleSheet("border-image: url(:/new/image/Load.png);");
        }
    }
    //出口 箱子 位置固定
    mazeWidgets[maze.height-2][maze.width-2]->setStyleSheet("border-image: url(:/new/image/box.png);");

    //更新按钮状态
    ui->buttonPath->setEnabled(true);
    ui->buttonPlay->setEnabled(true);
}

void MainWindow::showAnimation()//显示动画
{
    if(isShowPath == true)
    {
        if(timer->isActive())
            timer->stop();
        group->stop();
        isShowPath = false;
        ui->buttonPlay->setEnabled(true);
        ui->buttonPath->setStyleSheet("QPushButton{border-image: url(:/new/image/button1.png);}"
                                      "QPushButton:hover{border-image: url(:/new/image/button4.png);}"
                                      "QPushButton:pressed{border-image: url(:/new/image/button2.png);}");
        return;
    }
    else
    {
        ui->buttonPath->setEnabled(true);
        ui->buttonPath->setStyleSheet("QPushButton{border-image: url(:/new/image/button3.png);}"
                                      "QPushButton:hover{border-image: url(:/new/image/button4.png);}"
                                      "QPushButton:pressed{border-image: url(:/new/image/button2.png);}");
        time = 1;
        ui->buttonPlay->setEnabled(false);
        isPlayMode = false;
        isShowPath =true;
        time = 1;//栈元素标志量
        group->clear();//动画组清空

        for(int i=0; i<maze.autoPath.size()-1;i++)
        {
            QPropertyAnimation* animation = new QPropertyAnimation(_pMouse, "pos");//动作初始化
            animation->setDuration(400);//设置动作间隔
            animation->setStartValue(QPoint(40+maze.autoPath[i].j*BASIC_LENGTH,40+maze.autoPath[i].i*BASIC_LENGTH));
            animation->setEndValue(QPoint(40+maze.autoPath[i+1].j*BASIC_LENGTH,40+maze.autoPath[i+1].i*BASIC_LENGTH));
            animation->setEasingCurve(QEasingCurve::Linear);
            group->addAnimation(animation);//向动画组中添加动作
        }
        timer->start(400);//设置计时间隔，必须与动作间隔大小一样，才会同步
        group->start();//动画组启用
    }
}
//键盘事件
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(isPlayMode == false)
        return;
    step++;
    switch(event->key())
    {
    case Qt::Key_W://上
        //上面为墙，则什么也不执行
        if(maze.matrix[(xPos-8-40)/40][maze.Y].state  == 0)
        {
            return;
        }
        xPos-=8;//每次减8
        maze.X=(xPos-40)/40;
        _pMouse->move(yPos,xPos);//角色窗口移动
        _pMouse->setStyleSheet(character[3+(step/5)%3]);//显示向上移动图片
        break;
    case Qt::Key_S://下
        if(maze.matrix[(xPos)/40][maze.Y].state  == 0)
        {
           return;
        }
        xPos+=8;
        maze.X=(xPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[0+(step/5)%3]);
        break;
    case Qt::Key_A://左
        if(maze.matrix[maze.X][(yPos-8-40)/40].state  == 0)
        {
            return;
        }
        yPos-=8;
        maze.Y=(yPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[6+(step/5)%3]);
        break;
    case Qt::Key_D://右
        if(maze.matrix[maze.X][(yPos)/40].state  == 0)
        {
            return;
        }
        yPos+=8;
        maze.Y=(yPos-40)/40;
        _pMouse->move(yPos,xPos);
        _pMouse->setStyleSheet(character[9+(step/5)%3]);
        break;
    }
}

void MainWindow::playMode()
{
    if(isPlayMode == false)
    {
        isPlayMode =true;
        ui->buttonPath->setEnabled(false);

    }
    else
    {
        isPlayMode = false;
    }
}

void MainWindow::moveCharacter()//设置移动时的图片
{
     if(maze.autoPath[time+1].i < maze.autoPath[time].i)//up
     {
         _pMouse->setStyleSheet(character[3+time%3]);
     }
     if(maze.autoPath[time+1].i > maze.autoPath[time].i)//down
     {
         _pMouse->setStyleSheet(character[0+time%3]);
     }
     if(maze.autoPath[time+1].j > maze.autoPath[time].j)//right
     {
         _pMouse->setStyleSheet(character[9+time%3]);
     }
     if(maze.autoPath[time+1].j < maze.autoPath[time].j)//left
     {
         _pMouse->setStyleSheet(character[6+time%3]);
     }
     time++;
     if(time == maze.autoPath.size()-1)
     {
         timer->stop();
         time = 0;
     }
}
//角色在游戏中就是一系列的连贯图片
void MainWindow::initCharacter()
{
    character[0] = "border-image: url(:/new/image/down1.png);";
    character[1] = "border-image: url(:/new/image/down2.png);";
    character[2] = "border-image: url(:/new/image/down3.png);";
    character[3] = "border-image: url(:/new/image/up1.png);";
    character[4] = "border-image: url(:/new/image/up2.png);";
    character[5] = "border-image: url(:/new/image/up3.png);";
    character[6] = "border-image: url(:/new/image/left1.png);";
    character[7] = "border-image: url(:/new/image/left2.png);";
    character[8] = "border-image: url(:/new/image/left3.png);";
    character[9] = "border-image: url(:/new/image/right1.png);";
    character[10]= "border-image: url(:/new/image/right2.png);";
    character[11]= "border-image: url(:/new/image/right3.png);";
}

void MainWindow::showPath()
{
    for(int i=0; i<maze.pathStack.size();i++)
    {
        point temp = maze.pathStack[i];
        int dir = maze.pathStack[i].state;
        switch (dir) {
        //将生成pathStack时的路径状态数 d 与绘图时显示的图片对应（没有标准答案）
        case 1:
            dir = 3;
            break;
        case 2:
            dir =9;
            break;
        case 3:
            dir = 0;
            break;
        case 4:
            dir = 6;
            break;
        default:
            break;
        }
        if(!isPaintPath)
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet(character[dir+i%3]);
        }
        else
        {
            mazeWidgets[temp.i][temp.j]->setStyleSheet("border-image: url(:/new/image/Load.png);");
            mazeWidgets[maze.height-2][maze.width-2]->setStyleSheet("border-image: url(:/new/image/box.png);");

        }
    }
    isPaintPath = !isPaintPath;
}

MainWindow::~MainWindow()
{
    delete ui;
}
