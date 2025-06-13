#pragma once

#include <QObject>

class Food : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int xFood READ xFood NOTIFY xFoodChanged)
    Q_PROPERTY(int yFood READ yFood NOTIFY yFoodChanged)

public:
    explicit Food(QObject *parent = nullptr);

    int xFood() const;
    int yFood() const;
    Q_INVOKABLE void randomizePosition(int maxWidth = 640, int maxHeight = 480);

signals:
    void xFoodChanged();
    void yFoodChanged();

private:
    void setXFood(int x);
    void setYFood(int y);

    int m_xFood;
    int m_yFood;
};

