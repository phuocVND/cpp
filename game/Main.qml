import QtQuick
import QtQuick.Controls
Window {
    id: root
    width: 200 + 10
    height: 200 + 10
    visible: true
    Rectangle {
        id: foodRect
        width: 10
        height: 10
        color: "red"
        radius: width / 2
        x: Food.xFood
        y: Food.yFood
    }

    Rectangle{
        id: snakeHead
        property int xback: snakeHead.x-10
        property int yback: snakeHead.y
        width: 10
        height: 10
        color: "blue"
        radius: width / 2
        x: Snake.xSnake
        y: Snake.ySnake
    }
    Rectangle{
        id: snake2
        property int xback: snake2.x-10
        property int yback: snake2.y
        width: 10
        height: 10
        color: "blue"
        radius: width / 5
        x: snakeHead.xback
        y: snakeHead.yback
    }
    Rectangle{
        id: snake3
        width: 10
        height: 10
        color: "blue"
        radius: width / 5
        x: snake2.xback
        y: snake2.yback
    }


    Connections {
        target: Snake
        onSnakeChanged: {

            snake2.xback = snake2.x
            snake2.yback = snake2.y

            snakeHead.xback = snakeHead.x
            snakeHead.yback = snakeHead.y

            snakeHead.x = Snake.xSnake
            snakeHead.y = Snake.ySnake
        }
        onSnakeReset: {
            snakeHead.x = Snake.xSnake
            snakeHead.y = Snake.ySnake

            snakeHead.xback = snakeHead.x-10
            snakeHead.yback = snakeHead.y

            snake2.xback = snake2.x-10
            snake2.yback = snake2.y

        }
    }


//     Item {
//        anchors.centerIn: parent
//        width: 200
//        height: 200

//        Button {
//            text: "↑"
//            anchors.top: parent.top
//            anchors.horizontalCenter: parent.horizontalCenter
//            onClicked: {
//                Snake.up()
//            }
//        }

//        Button {
//            text: "↓"
//            anchors.bottom: parent.bottom
//            anchors.horizontalCenter: parent.horizontalCenter
//            onClicked: {
//                Snake.down()
//            }
//        }

//        Button {
//            text: "←"
//            anchors.left: parent.left
//            anchors.verticalCenter: parent.verticalCenter
//            onClicked: {
//                Snake.left()
//            }
//        }

//        Button {
//            text: "→"
//            anchors.right: parent.right
//            anchors.verticalCenter: parent.verticalCenter
//            onClicked: {
//                Snake.right();
//            }
//        }
//    }
}
