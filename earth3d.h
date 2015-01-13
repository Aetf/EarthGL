#ifndef EARTH3D_H
#define EARTH3D_H

#include <QQuickFramebufferObject>

class Earth3D : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(double cameraXRotate
               READ cameraXRotate WRITE setCameraXRotate
               NOTIFY cameraXRotateChanged)
    Q_PROPERTY(double cameraYRotate
               READ cameraYRotate WRITE setCameraYRotate
               NOTIFY cameraYRotateChanged)
    Q_PROPERTY(double cameraDistance
               READ cameraDistance WRITE setCameraDistance
               NOTIFY cameraDistanceChanged)
    Q_PROPERTY(double camera2XRotate
               READ camera2XRotate WRITE setCamera2XRotate
               NOTIFY camera2XRotateChanged)
    Q_PROPERTY(double camera2YRotate
               READ camera2YRotate WRITE setCamera2YRotate
               NOTIFY camera2YRotateChanged)
    Q_PROPERTY(double camera2Distance
               READ camera2Distance WRITE setCamera2Distance
               NOTIFY camera2DistanceChanged)
    Q_PROPERTY(bool showCamera
               READ showCamera WRITE setShowCamera
               NOTIFY showCameraChanged)
    Q_PROPERTY(bool useCamera2
               READ useCamera2 WRITE setUseCamera2
               NOTIFY useCamera2Changed)
    Q_PROPERTY(bool showVertices
               READ showVertices WRITE setShowVertices
               NOTIFY showVerticesChanged)
    Q_PROPERTY(int sphereResolution
               READ sphereResolution WRITE setSphereResolution
               NOTIFY sphereResolutionChanged)
public:
    Earth3D();
    ~Earth3D();

    Renderer *createRenderer() const override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData) override;

    double cameraXRotate() const { return m_cameraXRotate; }
    double cameraYRotate() const { return m_cameraYRotate; }
    double cameraDistance() const { return m_cameraDistance; }
    void setCameraXRotate(double xRotate);
    void setCameraYRotate(double yRotate);
    void setCameraDistance(double distance);

    double camera2XRotate() const { return m_camera2XRotate; }
    double camera2YRotate() const { return m_camera2YRotate; }
    double camera2Distance() const { return m_camera2Distance; }
    void setCamera2XRotate(double xRotate);
    void setCamera2YRotate(double yRotate);
    void setCamera2Distance(double distance);

    bool showCamera() const { return m_showCamera; }
    void setShowCamera(bool val);

    bool useCamera2() const { return m_useCamera2; }
    void setUseCamera2(bool val);

    bool showVertices() const { return m_showVertices; }
    void setShowVertices(bool val);

    int sphereResolution() const { return m_sphereResolution; }
    void setSphereResolution(int newResolution);

signals:
    void cameraXRotateChanged();
    void cameraYRotateChanged();
    void cameraDistanceChanged();
    void camera2XRotateChanged();
    void camera2YRotateChanged();
    void camera2DistanceChanged();
    void useCamera2Changed();
    void showCameraChanged();
    void showVerticesChanged();
    void sphereResolutionChanged();

public slots:

private:
    double m_cameraXRotate;
    double m_cameraYRotate;
    double m_cameraDistance;

    double m_camera2XRotate;
    double m_camera2YRotate;
    double m_camera2Distance;

    bool m_showCamera;
    bool m_useCamera2;
    bool m_showVertices;

    int m_sphereResolution;
};

#endif // EARTH3D_H
