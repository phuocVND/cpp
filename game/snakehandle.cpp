#include "snakehandle.h"

#include <QRandomGenerator>
#include <iostream>

void SnakeHandle::handleDirection(char direction)
{
    switch (direction) {
    case 'u': up(); break;
    case 'd': down(); break;
    case 'l': left(); break;
    case 'r': right(); break;
    default: break;
    }
    // std::cout << "x: " << xSnake() << "\t" << "y: " << ySnake() << std::endl;
}

SnakeHandle::SnakeHandle(QObject *parent) : QObject(parent), m_xSnake(0), m_ySnake(0)
{
    last_action = 3;
    m_checkDead = 0;
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
    // std::cout << "x: " << xSnake() << "\t" << "y: " << ySnake() << std::endl;
}

void SnakeHandle::setYSnake(int y)
{
    if (m_ySnake != y) {
        m_ySnake = y;
        emit snakeChanged();
    }
    // std::cout << "x: " << xSnake() << "\t" << "y: " << ySnake() << std::endl;
}

void SnakeHandle::up(){
    std::array<int, 4> compareArray = {1, 0, 0, 0};

    if(m_actionOld == compareArray){
        m_checkDead = 1;
        return;
    }
    m_actionOld = {0, 1, 0, 0};

    if(m_ySnake - 10 < 0){
        m_checkDead = 1;
    }
    else{
        setYSnake(m_ySnake - 10);
    }
    last_action = 0;
}
void SnakeHandle::down(){

    std::array<int, 4> compareArray = {0, 1, 0, 0};

    if(m_actionOld == compareArray){
        m_checkDead = 1;
        return;
    }
    m_actionOld = {1, 0, 0, 0};

    if(m_ySnake + 10 > m_maxHeight){
        m_checkDead = 1;
    }
    else{
        setYSnake(m_ySnake + 10);
    }
    last_action = 1;
}
void SnakeHandle::right(){
    std::array<int, 4> compareArray = {0, 0, 1, 0};

    if(m_actionOld == compareArray){
        m_checkDead = 1;
        return;
    }
    m_actionOld = {0, 0, 0, 1};

    if(m_xSnake + 10 > m_maxWidth){
        m_checkDead = 1;
    }
    else{
        setXSnake(m_xSnake+10);
    }
    last_action = 2;
}
void SnakeHandle::left(){
    std::array<int, 4> compareArray = {0, 0, 0, 1};

    if(m_actionOld == compareArray){
        m_checkDead = 1;
        return;
    }
    m_actionOld = {0, 0, 1, 0};

    if(m_xSnake - 10 < 0){
        m_checkDead = 1;
    }
    else{
        setXSnake(m_xSnake-10);
    }
    last_action = 3;
}
