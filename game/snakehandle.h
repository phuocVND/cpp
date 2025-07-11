#pragma once

#include <QtCore/qobject.h>
class SnakeHandle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int xSnake READ xSnake NOTIFY snakeChanged)
    Q_PROPERTY(int ySnake READ ySnake NOTIFY snakeChanged)

public:

    explicit SnakeHandle(QObject *parent = nullptr);

    int xSnake() const;
    int ySnake() const;
    Q_INVOKABLE void up();
    Q_INVOKABLE void down();
    Q_INVOKABLE void right();
    Q_INVOKABLE void left();

    Q_INVOKABLE void randomizePosition(int maxWidth = 200, int maxHeight = 200);
    Q_INVOKABLE void handleDirection(char direction);
    int last_action = 3;
    bool m_checkDead;
signals:
    void snakeChanged();
    void snakeReset();
private:
    std::array<int, 4 > m_actionOld = {0, 0, 0, 1};

    void setXSnake(int x);
    void setYSnake(int y);

    int m_maxWidth = 200;

    int m_maxHeight = 200;

    int m_xSnake;
    int m_ySnake;
};
