import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import OpenGLUnderQML 1.0

ApplicationWindow {
    title: qsTr("EarthGL")
    width: 700
    height: 480
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: messageDialog.show(qsTr("Open action triggered"))
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit()
            }
        }
    }

    RowLayout {
        anchors.fill: parent

        Earth3D {
            id: earth
            Layout.fillWidth: true
            Layout.fillHeight: true

            cameraXRotate: 0
            cameraYRotate: 0
            cameraDistance: defaultCameraDistance / zoomFactor
            showCamera: false
            useCamera2: false

            property real defaultCameraDistance: 2.5;
            property real zoomFactor: 1;

            function zoomIn() {
                zoomFactor *= 1.1;
            }

            function zoom(factor) {
                if (factor === 0) factor = 0.001
                zoomFactor = factor;
            }

            function zoomOut() {
                zoomFactor *= 0.9;
            }

            PinchArea {
                property real capturedFactor;
                onPinchStarted: {
                    capturedFactor = earth.zoomFactor;
                }

                onPinchUpdated: {
                    earth.zoom(pinch.scale * capturedFactor);
                }
            }

            MouseArea {
                anchors.fill: parent

                property int lastX
                property int lastY

                function captureMouse(mouse) {
                    lastX = mouse.x
                    lastY = mouse.y
                }

                onWheel: {
                    if (wheel.angleDelta.y > 0) {
                        earth.zoomIn()
                    } else {
                        earth.zoomOut()
                    }
                }
                onPressed: {
                    captureMouse(mouse)
                }
                onPositionChanged: {
                    if (mouse.buttons & Qt.LeftButton != 0) {
                        var deltaX = mouse.x - lastX
                        var deltaY = mouse.y - lastY
                        var alpha = earth.cameraXRotate
                        var beta = earth.cameraYRotate

                        alpha -= deltaX
                        while (alpha < 0)
                            alpha += 360
                        while (alpha > 360)
                            alpha -= 360

                        beta -= deltaY
                        if (beta < -90)
                            beta = -90
                        if (beta > 90)
                            beta = 90

                        earth.cameraXRotate = alpha
                        earth.cameraYRotate = beta
                    }
                    captureMouse(mouse)
                }
            }

            Rectangle {
                anchors.fill: bottomRow
                anchors.margins: -10

                color: Qt.rgba(1, 1, 1, 0.7)
                radius: 10
                border.width: 1
                border.color: "white"
            }

            RowLayout {
                id: bottomRow
                anchors {
                    right: parent.right
                    left: parent.left
                    bottom: parent.bottom
                    margins: 15
                }

                Text {
                    text: "顶点个数"
                }
                Slider {
                    id: slider
                    Layout.fillWidth: true
                    maximumValue: 200
                    minimumValue: 10
                    stepSize: 1
                    value: 10

                    property real lazyValue;
                    Binding on lazyValue {
                        when: !slider.pressed
                        value: slider.value
                    }
                    Binding {
                        target: earth
                        property: "sphereResolution"
                        value: slider.lazyValue
                    }
                }
                Text {
                    Layout.preferredWidth: 40
                    Layout.alignment: Qt.AlignRight
                    text: slider.value * slider.value * 2
                    horizontalAlignment: Text.AlignRight
                }
            }
        }

        ColumnLayout {
            Earth3D {
                id: earthCamera
                Layout.fillWidth: true
                Layout.fillHeight: true

                cameraXRotate: earth.cameraXRotate
                cameraYRotate: earth.cameraYRotate
                cameraDistance: earth.cameraDistance
                sphereResolution: earth.sphereResolution
                showCamera: true
                useCamera2: true
                camera2XRotate: 60
                camera2YRotate: 0
                camera2Distance: 5
            }

            ShowTextureMapping {
                id: textureMapping
                Layout.fillWidth: true
                Layout.fillHeight: true

                sphereResolution: earth.sphereResolution
                contentScale: 1.0
                cameraPosition: "0, 0, 1000"

                function zoomIn() {
                    var z = cameraPosition.z * 0.9
                    if (z < 25)
                        z = 25
                    cameraPosition.z = z
                }

                function zoomOut() {
                    var z = cameraPosition.z * 1.1
                    if (z > 2000)
                        z = 1980
                    cameraPosition.z = z
                }

                MouseArea {
                    anchors.fill: parent

                    property int lastX
                    property int lastY

                    function captureMouse(mouse) {
                        lastX = mouse.x
                        lastY = mouse.y
                    }

                    onWheel: {
                        if (wheel.angleDelta.y > 0) {
                            textureMapping.zoomIn()
                        } else {
                            textureMapping.zoomOut()
                        }
                    }
                    onPressed: {
                        captureMouse(mouse)
                    }
                    onPositionChanged: {
                        if (mouse.buttons & Qt.LeftButton != 0) {
                            var deltaX = mouse.x - lastX
                            var deltaY = mouse.y - lastY

                            textureMapping.cameraPosition.x -= deltaX
                            textureMapping.cameraPosition.y += deltaY
                        }
                        captureMouse(mouse)
                    }
                }

                Rectangle {
                    anchors.fill: sideCol
                    anchors.margins: -10

                    color: Qt.rgba(1, 1, 1, 0.7)
                    radius: 10
                    border.width: 1
                    border.color: "white"
                }

                Column {
                    id: sideCol
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 15
                        right: parent.right
                        rightMargin: 15
                    }
                    CheckBox {
                        id: chk1
                        text: qsTr("显示映射的顶点")
                        Binding {
                            target: textureMapping
                            property: "showMappedVertices"
                            value: chk1.checked
                        }
                    }
                    CheckBox {
                        id: chk2
                        text: qsTr("显示Camera0视图中的顶点")
                        Binding {
                            target: earth
                            property: "showVertices"
                            value: chk2.checked
                        }
                    }
                }
            }
        }
    }
}
