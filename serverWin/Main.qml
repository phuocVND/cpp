import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import com.company.tcp 1.0

ApplicationWindow {
    id: root
    width: 450
    height: 500
    visible: true
    title: qsTr("TCP Server Control")

    TcpServer {
        id: myServer
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        Label {
            text: qsTr("Trạng thái Server")
            font.bold: true
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            id: statusLabel
            text: qsTr("Server chưa khởi động.")
            font.pixelSize: 16
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            id: startButton
            text: qsTr("Khởi động Server trên Port 8888")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                myServer.startServer(8888)
                statusLabel.text = "Server đang lắng nghe trên port 8888.\nĐang chờ client kết nối..."
                this.enabled = false
                sendButton.enabled = true
            }
        }

        Item { Layout.preferredHeight: 20 }

        Label {
            text: qsTr("Nhập các tham số (0 - 255)")
            font.bold: true
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }

        GridLayout {
            columns: 2
            rowSpacing: 10
            columnSpacing: 15
            Layout.alignment: Qt.AlignHCenter

            Label { text: "Tham số 1:" }
            TextField {
                id: tf1
                text: "10"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 0; top: 255 }
            }

            Label { text: "Tham số 2:" }
            TextField {
                id: tf2
                text: "20"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 0; top: 255 }
            }

            Label { text: "Tham số 3:" }
            TextField {
                id: tf3
                text: "30"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 0; top: 255 }
            }

            Label { text: "Tham số 4:" }
            TextField {
                id: tf4
                text: "40"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 0; top: 255 }
            }

            Label { text: "Tham số 5:" }
            TextField {
                id: tf5
                text: "50"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 0; top: 255 }
            }

            Label { text: "Tham số 6:" }
            TextField {
                id: tf6
                text: "60"
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 0; top: 255 }
            }
        }

        Item { Layout.preferredHeight: 20 }

        Button {
            id: sendButton
            text: qsTr("Gửi Dữ Liệu")
            enabled: false
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                // Chuyển đổi và gửi dữ liệu
                myServer.sendParameters(
                    parseInt(tf1.text),
                    parseInt(tf2.text),
                    parseInt(tf3.text),
                    parseInt(tf4.text),
                    parseInt(tf5.text),
                    parseInt(tf6.text)
                )
                statusLabel.text = "Đã gửi dữ liệu tới client!"
            }
        }
    }
}
