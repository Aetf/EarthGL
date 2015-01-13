#include <QSGSimpleTextureNode>
#include "earth3d.h"
#include "earth3drenderer.h"

Earth3D::Earth3D()
    : m_cameraXRotate(25)
    , m_cameraYRotate(-25)
    , m_cameraDistance(2.5)
    , m_camera2XRotate(35)
    , m_camera2YRotate(-35)
    , m_camera2Distance(5)
    , m_showCamera(false)
    , m_useCamera2(false)
    , m_showVertices(false)
    , m_sphereResolution(360)
{
}

Earth3D::~Earth3D()
{
}

FBO::Renderer *Earth3D::createRenderer() const
{
    return new Earth3DRenderer();
}

QSGNode *Earth3D::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData)
{
    if (!oldNode) {
        oldNode = QQuickFramebufferObject::updatePaintNode(oldNode, nodeData);
        QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode *>(oldNode);
        if (n) {
            n->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
        }
        return oldNode;
    }
    return QQuickFramebufferObject::updatePaintNode(oldNode, nodeData);
}

void Earth3D::setCameraXRotate(double xRotate)
{
    if (m_cameraXRotate == xRotate) {
        return;
    }

    m_cameraXRotate = xRotate;
    emit cameraXRotateChanged();
    update();
}

void Earth3D::setCameraYRotate(double yRotate)
{
    if (m_cameraYRotate == yRotate) {
        return;
    }

    m_cameraYRotate = yRotate;
    emit cameraYRotateChanged();
    update();
}

void Earth3D::setCameraDistance(double distance)
{
    if (m_cameraDistance == distance) {
        return;
    }

    m_cameraDistance = distance;
    emit cameraDistanceChanged();
    update();
}

void Earth3D::setCamera2XRotate(double xRotate)
{
    if (m_camera2XRotate == xRotate) {
        return;
    }

    m_camera2XRotate = xRotate;
    emit camera2XRotateChanged();
    update();
}

void Earth3D::setCamera2YRotate(double yRotate)
{
    if (m_camera2YRotate == yRotate) {
        return;
    }

    m_camera2YRotate = yRotate;
    emit camera2YRotateChanged();
    update();
}

void Earth3D::setCamera2Distance(double distance)
{
    if (m_camera2Distance == distance) {
        return;
    }

    m_camera2Distance = distance;
    emit camera2DistanceChanged();
    update();
}

void Earth3D::setShowCamera(bool val)
{
    if (m_showCamera == val) {
        return;
    }

    m_showCamera = val;
    emit showCameraChanged();
    update();
}

void Earth3D::setShowVertices(bool val)
{
    if (m_showVertices == val) {
        return;
    }

    m_showVertices = val;
    emit showVerticesChanged();
    update();
}

void Earth3D::setUseCamera2(bool val)
{
    if (m_useCamera2 == val) {
        return;
    }

    m_useCamera2 = val;
    emit useCamera2Changed();
    update();
}

void Earth3D::setSphereResolution(int newResolution)
{
    if (m_sphereResolution == newResolution) {
        return;
    }
    m_sphereResolution = newResolution;
    emit sphereResolutionChanged();
    update();
}
