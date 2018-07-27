import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import "../InputTypes"

GridLayout {
    id: vtkBoxWidget

    property alias translateX: translateXInput.value
    property alias translateY: translateYInput.value
    property alias translateZ: translateZInput.value

    property alias rotateX: rotateXInput.value
    property alias rotateY: rotateYInput.value
    property alias rotateZ: rotateZInput.value

    property alias scaleX: scaleXInput.value
    property alias scaleY: scaleYInput.value
    property alias scaleZ: scaleZInput.value

    readonly property real inputWidth: 75
    readonly property int decimals: 2

    columns: 4
    columnSpacing: 4
    rowSpacing: 4

    signal edited()

    function getTranslation()
    {
        var translation = [translateX, translateY, translateZ];
        return translation;
    }

    function getRotation()
    {
        var rotation = [rotateX, rotateY, rotateZ];
        return rotation;
    }

    function getScale()
    {
        var scale = [scaleX, scaleY, scaleZ];
        return scale;
    }

    function setTranslation(translation)
    {
        translateX = translation[0];
        translateY = translation[1];
        translateZ = translation[2];
    }

    function setRotation(rotation)
    {
        rotateX = rotation[0];
        rotateY = rotation[1];
        rotateZ = rotation[2];
    }

    function setScale(scale)
    {
        scaleX = scale[0];
        scaleY = scale[1];
        scaleZ = scale[2];
    }

    Label
    {
        id: normalLabel
        text: qsTr("Translation: ")
    }

    FloatField
    {
        id: translateXInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: translateYInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: translateZInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    Label
    {
        id: rotationLabel
        text: qsTr("Rotation: ")
    }

    FloatField
    {
        id: rotateXInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: rotateYInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: rotateZInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    Label
    {
        id: scaleLabel
        text: qsTr("Scale: ")
    }

    FloatField
    {
        id: scaleXInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: scaleYInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: scaleZInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    Component.onCompleted:
    {
        onTranslateXChanged.connect(edited)
        onTranslateYChanged.connect(edited)
        onTranslateZChanged.connect(edited)

        onRotateXChanged.connect(edited)
        onRotateYChanged.connect(edited)
        onRotateZChanged.connect(edited)

        onScaleXChanged.connect(edited)
        onScaleYChanged.connect(edited)
        onScaleZChanged.connect(edited)
    }
}