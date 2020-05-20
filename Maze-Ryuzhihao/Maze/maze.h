#ifndef MAZE_H
#define MAZE_H
#include <QStack>
#include <QList>

class point
{
public:
    int i;
    int j;
    int state;//代表是障碍还是通道
    //构造函数重载
    point();
    point(int i,int j, int state);
    //运算符重载
    bool operator==(const point& maze);
};

class Maze
{
private:
    point **recordMatrix;
    QStack<point> *MazeStack;
    point move[4]; //人移动的四个方向
public:
    point **matrix;

    int height;//迷宫矩阵的行
    int width;//迷宫矩阵的列

    int X;//人的位置
    int Y;
    QList<point> pathStack;  //路径栈，用来显示路径（绘制路径按钮）
    QList<point> autoPath;   //存放自动寻路的结果

    Maze();//对类中数据进行初始化

    void initMaze(int h,int w);//根据长、宽初始化迷宫矩阵
    void createMaze();
    void autoFindPath();

private:
    void setDirFalse(bool& up,bool& down,bool& right,bool& left);
};

#endif // MAZE_H
