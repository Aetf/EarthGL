#ifndef EARTH3DRENDERER_H
#define EARTH3DRENDERER_H

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QQuickFramebufferObject>
#include <QSize>
#include "spheregenerator.h"

using FBO = QQuickFramebufferObject;

class Earth3DRenderer : public FBO::Renderer, protected QOpenGLFunctions_4_3_Core
{
public:
    Earth3DRenderer();
    ~Earth3DRenderer();

    void render() override;
    void synchronize(QQuickFramebufferObject * item) override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

protected:
    void initialize();
    void createGeometry();

    void updateProjection(int width, int height);
    void updateCamera(int idx, double xrot, double yrot, double dist);
    void updateViewMatrix();

    void createAxis();
    void createCamera();
    void createSphere();

    void paintAxis();
    void paintCamera();
    void paintSphere();
    void paintSphereVertices();

private:
    // state copied from outside Item
    bool showVertices;
    bool showCamera;
    bool useCamera2;
    int resolution;
    QSize m_viewportSize;

    // projection and view matrix and camera
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projMatrix;
    QVector3D m_cameraPos[2];
    QVector3D m_cameraUp[2];
    QMatrix4x4 m_cameraTransform[2];
    double m_cameraDistance[2];

    // camera shape
    QOpenGLVertexArrayObject vao_camera;
    QOpenGLBuffer vbo_camera;
    QOpenGLBuffer ebo_camera;
    int stripCount;
    int fanCount;
    // axis
    QOpenGLVertexArrayObject vao_axis;
    QOpenGLBuffer vbo_axis;
    QOpenGLBuffer ebo_axis;
    // sphere
    SphereGenerator sphere;
    QOpenGLVertexArrayObject vao_sphere;
    QOpenGLBuffer vbo_sphere;
    QOpenGLBuffer ebo_sphere;
    QOpenGLTexture *pTex_sphere;
    // sphere vertices
    QOpenGLVertexArrayObject vao_sphere_fw;

    // shaders and attributes locations
    QOpenGLShaderProgram m_colorProg;
    QOpenGLShaderProgram m_texLightProg;
    int vertex_loc_0;
    int color_loc_0;
    int mv_matrix_loc_0;
    int proj_matrix_loc_0;

    int vertex_loc_1;
    int texcoord_loc_1;
    int normal_loc_1;
    int mv_matrix_loc_1;
    int proj_matrix_loc_1;
    int nm_matrix_loc_1;
    int lpos_loc_1;
    int ami_color_loc_1;
    int ami_ref_loc_1;
    int dif_color_loc_1;
    int dif_ref_loc_1;
    int spec_color_loc_1;
    int spec_ref_loc_1;
    int shininess_loc_1;
};

#endif // EARTH3DRENDERER_H
