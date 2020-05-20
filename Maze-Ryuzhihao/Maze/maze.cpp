#include "maze.h"
#include <QTime>
#include <QDebug>
point::point(){}//默认构造函数为空

point::point(int i,int j, int state):i(i),j(j),state(state){}//根据参数构造

bool point::operator==(const point& t)//重载==运算符，可以比较两个point 对象是否完全一样
{
    if(this->i == t.i&&j == t.j && t.state == state)
        return true;
    else return false;
}

Maze::Maze()
{
    matrix = NULL;//二级point指针初始化为NULL
    recordMatrix = NULL;//二级point指针初始化为NULL

    height = width = 0;//高度、宽度初始化为NULL
    X = Y = 1;//角色的位置初始化在坐标（1,1）

    //初始化随机数种子
    QTime time=QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);

    //创建两个栈
    MazeStack= new QStack<point>;//给point栈分配堆上内存

    //初始化方向Move[4]  move[4] point数组 二维矩阵的坐标变化
    move[0].i = -1; move[0].j=0;//上
    move[1].i = 0 ; move[1].j=1;//右
    move[2].i = 1 ; move[2].j=0;//下
    move[3].i = 0 ; move[3].j=-1;//左
}

void Maze::initMaze(int h,int w) //创建一个大小为h*w的空白迷宫和record矩阵
{
    //让使用到的两个栈清空
    MazeStack->clear();
    autoPath.clear();

    //获取迷宫边界大小和人物的初始位置
    height = h;
    width  = w;
    //初始化角色位置在地图中心
    X=height/2;
    Y=width/2;
    if(X%2==0)
        X+=1;
    if(Y%2==0)
        Y+=1;

    //收回之前分配的两个矩阵的空间
    if(matrix != NULL)
    {
        //二级指针回收
        for(int i=0;i<height;i++)
            delete [] matrix[i];
        delete [] matrix;

        for(int i=0; i<height;i++)
            delete [] recordMatrix[i];
        delete [] recordMatrix;
    }
    //重新创建matrix数组和recordMaze数组, 因为height和width发生了改变
    matrix=new point*[height];
    recordMatrix = new point*[height];

    for(int i=0;i<height;i++)
    {
        matrix[i]=new point[width];
        recordMatrix[i] = new point[width];
    }
    //初始化matrix(全0)和recordMatrix的值(全1)
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            matrix[i][j].state=0;//所有节点设置为0 状态0 代表 墙 障碍
            recordMatrix[i][j].state= 1 ;//状态1 代表 草地 非障碍
            recordMatrix[i][j].i = matrix[i][j].i = i;//坐标分配
            recordMatrix[i][j].j = matrix[i][j].j = j;
        }
    }
}

void Maze::setDirFalse(bool& up,bool& down,bool& right,bool& left)
{
    up = down = right = left = false;
}

void Maze::createMaze()//先 init 再 create
{
    int i=3,j=3;
    matrix[i][j].state=1;

    point temp;
    temp.i=i;
    temp.j=j;
    temp.state=1;
    bool up=false, down=false, right=false, left=false;

    while(true)//从（3,3）的位置开始构建迷宫，每次都将当前的位置压入到栈MazeStack中
    {
        temp.i=i;
        temp.j=j;
        int randNum=qrand()%4;//randNum的取值在0-3之间
        switch(randNum)
        {
        case 0://上
            if(!up&&i>2&&matrix[i-2][j].state==0)//满足条件，代表可以铺通道
            {
                MazeStack->push(temp);
                matrix[i-2][j].state=1;
                matrix[i-1][j].state=1;
                i=i-2;
                setDirFalse(up, down, right, left);//每次都要将up,down,right,false的置为0，方便判断
            }
            else
                up=true;
            break;//保证每次在一个方向上铺路之后，跳出switch，从下一个位置继续判断各个方向能否铺路
        case 1://下
            if(!down&&i<height-3&&matrix[i+2][j].state==0)
            {
                MazeStack->push(temp);
                matrix[i+2][j].state=1;
                matrix[i+1][j].state=1;
                i=i+2;
                setDirFalse(up, down, right, left);
            }
            else
                down=true;
            break;
        case 2://左
            if(!left&&j>2&&matrix[i][j-2].state==0)
            {
                MazeStack->push(temp);
                matrix[i][j-2].state=1;
                matrix[i][j-1].state=1;
                j=j-2;
                setDirFalse(up, down, right, left);
            }
            else
                left=true;
            break;
        case 3://右
            if(!right&&j<width-3&&matrix[i][j+2].state==0)
            {
                MazeStack->push(temp);
                matrix[i][j+2].state=1;
                matrix[i][j+1].state=1;
                j=j+2;
                setDirFalse(up, down, right, left);
            }
            else
                right=true;
            break;
        }
        if(up&&down&&right&&left)//如果当前访问节点四个方向都没有可拆的节点，回溯
        {
            if(!MazeStack->empty())
            {
                i=MazeStack->top().i;
                j=MazeStack->top().j;
                MazeStack->pop();
                setDirFalse(up, down, right, left);
            }
            else//如果栈为空的话就返回，此时迷宫矩阵已经创建完毕
            {
                return;
            }
        }
    }
}

void Maze::autoFindPath()
{
    //初始化迷宫的时候，X,Y 被初始化在迷宫的中心点，所以绘制路径总是从中心点开始的
    pathStack.clear();
    recordMatrix[X][Y].state = 0;  //出口位置标记为已经访问过,0为墙
    point temp(X,Y,0);
    pathStack.push_back(temp);

    int i , j , d ;
    while(!pathStack.isEmpty())
    {
        //pathStack 是QList 所以back()就是栈顶元素
        point top = pathStack.back();
        pathStack.pop_back();

        if(!autoPath.isEmpty()&& !(top==autoPath.back()))
            autoPath.push_back(top);
        if(autoPath.isEmpty())
            autoPath.push_back(top);

        i = top.i;j=top.j;d=top.state;//初始时d=0

        while(d < 4)
        {
            //每次都是在i,j的基础上变化，所以是从一个点出发的不断尝试
            temp.i = i+move[d].i;
            temp.j = j+move[d].j;

            if(temp.i == height-2 &&temp.j ==width-2)//到达出口
            {
                pathStack.push_back(top);
                pathStack.push_back(temp);
                autoPath.push_back(temp);

                for(int i=0 ; i<pathStack.size();i++)
                {
                    qDebug()<<"("<<pathStack[i].i<<","<<pathStack[i].j<<")";
                }
                return;
            }
            //满足条件，代表移动成功
            if(matrix[temp.i][temp.j].state == 1 && recordMatrix[temp.i][temp.j].state == 1)
            {

                recordMatrix[temp.i][temp.j].state = 0;
                top.state=d;//d代表向哪个方向移动才到的这个位置，存储在移动前位置状态中
                pathStack.push_back(top);//压入移动前的位置
                temp.state=0;//移动后的位置状态标记为0，代表障碍，不走回头路
                pathStack.push_back(temp);//压入移动后的位置
                autoPath.push_back(temp);
                break;
            }
            d++;
        }
        if(d==4)
            autoPath.push_back(pathStack.back());
    }
}
