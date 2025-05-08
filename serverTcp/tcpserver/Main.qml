import QtQuick
import QtQuick.Controls
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ListModel {
        id: sliderModel
        ListElement { name: "1";min: 0; max: 100; val: 50 }
        ListElement { name: "2";min: 0; max: 100; val: 50 }
        ListElement { name: "3";min: 0; max: 100; val: 50 }
        ListElement { name: "4";min: 0; max: 100; val: 50 }
        ListElement { name: "5";min: 0; max: 100; val: 50 }
        ListElement { name: "6";min: 0; max: 100; val: 50 }
        // thêm phần tử nếu cần
    }
    Column {
        spacing: 10

        Repeater {
            model: sliderModel

            Column {
                spacing: 4

                Text {
                    text: model.name
                    font.bold: true
                }

                Slider {
                    from: model.min
                    to: model.max
                    value: model.val
                    width: 200
                }
            }
        }
    }
}
