#include "snakehandle.h"

#include <QRandomGenerator>

SnakeHandle::SnakeHandle(QObject *parent) : QObject(parent), m_xSnake(0), m_ySnake(0)
{

}

int SnakeHandle::xSnake() const
{
    return m_xSnake;
}

int SnakeHandle::ySnake() const
{
    return m_ySnake;
}

void SnakeHandle::randomizePosition(int maxWidth, int maxHeight)
{
    int gridSize = 10;

    int numCols = maxWidth / gridSize;
    int numRows = maxHeight / gridSize;

    int randomCol = QRandomGenerator::global()->bounded(numCols);
    int randomRow = QRandomGenerator::global()->bounded(numRows);

    int newX = randomCol * gridSize;
    int newY = randomRow * gridSize;


    setXSnake(newX);
    setYSnake(newY);

    emit snakeReset();
}

void SnakeHandle::setXSnake(int x)
{
    if (m_xSnake != x) {
        m_xSnake = x;
        emit snakeChanged();
    }
}

void SnakeHandle::setYSnake(int y)
{
    if (m_ySnake != y) {
        m_ySnake = y;
        emit snakeChanged();
    }
}

void SnakeHandle::up(){
    std::array<int, 4> compareArray = {1, 0, 0, 0};

    if(m_actionOld == compareArray){
        return;
    }
    m_actionOld = {0, 1, 0, 0};

    if(m_ySnake-10 < 0){
        setYSnake(m_maxHeight);
    }
    else{
        setYSnake(m_ySnake-10);
    }

}
void SnakeHandle::down(){

    std::array<int, 4> compareArray = {0, 1, 0, 0};

    if(m_actionOld == compareArray){
        return;
    }
    m_actionOld = {1, 0, 0, 0};

    if(m_ySnake+10 > m_maxHeight){
        setYSnake(0);
    }
    else{
        setYSnake(m_ySnake+10);
    }
}
void SnakeHandle::right(){
    std::array<int, 4> compareArray = {0, 0, 1, 0};

    if(m_actionOld == compareArray){
        return;
    }
    m_actionOld = {0, 0, 0, 1};

    if(m_xSnake + 10 > m_maxWidth){
        setXSnake(0);
    }
    else{
        setXSnake(m_xSnake+10);
    }
}
void SnakeHandle::left(){
    std::array<int, 4> compareArray = {0, 0, 0, 1};

    if(m_actionOld == compareArray){
        return;
    }
    m_actionOld = {0, 0, 1, 0};

    if(m_xSnake - 10 < 0){
        setXSnake(m_maxWidth);
    }
    else{
        setXSnake(m_xSnake-10);
    }
}
