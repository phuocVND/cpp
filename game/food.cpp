#include "food.h"
#include <QRandomGenerator>

Food::Food(QObject *parent) : QObject(parent), m_xFood(0), m_yFood(0)
{

}

int Food::xFood() const
{
    return m_xFood;
}

int Food::yFood() const
{
    return m_yFood;
}

void Food::randomizePosition(int maxWidth, int maxHeight)
{
    int gridSize = 10;

    int numCols = maxWidth / gridSize;
    int numRows = maxHeight / gridSize;

    int randomCol = QRandomGenerator::global()->bounded(numCols);
    int randomRow = QRandomGenerator::global()->bounded(numRows);

    int newX = randomCol * gridSize;
    int newY = randomRow * gridSize;

    setXFood(newX);
    setYFood(newY);
}

void Food::setXFood(int x)
{
    if (m_xFood != x) {
        m_xFood = x;
        emit xFoodChanged();
    }
}

void Food::setYFood(int y)
{
    if (m_yFood != y) {
        m_yFood = y;
        emit yFoodChanged();
    }
}
