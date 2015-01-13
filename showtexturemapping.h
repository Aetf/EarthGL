#ifndef SHOWTEXTUREMAPPING_H
#define SHOWTEXTUREMAPPING_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QQuickFramebufferObject>
using FBO = QQuickFramebufferObject;

class ShowTextureMapping : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(bool showMappedVertices
               READ showMappedVertices WRITE setShowMappedVertices
               NOTIFY showMappedVerticesChanged)
    Q_PROPERTY(double contentScale
               READ contentScale WRITE setContentScale
               NOTIFY contentScaleChanged)
    Q_PROPERTY(int sphereResolution
               READ sphereResolution WRITE setSphereResolution
               NOTIFY sphereResolutionChanged)
    Q_PROPERTY(QVector3D cameraPosition
               READ cameraPosition WRITE setCameraPosition
               NOTIFY cameraPositionChanged)
public:
    ShowTextureMapping();
    ~ShowTextureMapping();

    Renderer *createRenderer() const override;
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData) override;

    bool showMappedVertices() const { return m_showMappedVertices; }
    void setShowMappedVertices(bool val);

    double contentScale() const { return m_contentScale; }
    void setContentScale(double scale);

    int sphereResolution() const { return m_sphereResolution; }
    void setSphereResolution(int newResolution);

    QVector3D cameraPosition() const { return m_cameraPosition; }
    void setCameraPosition(const QVector3D &pos);

//    Q_INVOKABLE
//    QVector2D screenToWorld(const QVector2D &xy);

signals:
    void showMappedVerticesChanged();
    void contentScaleChanged();
    void sphereResolutionChanged();
    void cameraPositionChanged();

private:
    bool m_showMappedVertices;
    double m_contentScale;
    int m_sphereResolution;
    QVector3D m_cameraPosition;
};

class ShowTextureMappingRenderer : public FBO::Renderer, protected QOpenGLFunctions
{
public:
    ShowTextureMappingRenderer();
    ~ShowTextureMappingRenderer();

    void render() override;
    void synchronize(QQuickFramebufferObject * item) override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

protected:
    void initialize();
    void createGeometry();

    void updateProjection(int width, int height);
    QVector2D uvCoordInWorld(const QVector2D &uv, const QRectF &world);
    QVector2D scrCoordToModel(const QVector2D &xy);

    void createRect();
    void createMappedVertices();

    void paintRect();
    void paintMappedVertices();

private:
    // outside state
    bool showMappedVertices;
    int resolution;
    double scale;
    QVector3D cameraPosition;
    QSize m_viewportSize;

    // projection and view matrix and camera
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projMatrix;

    // total scale
    QRectF world;
    // rectangle
    QOpenGLVertexArrayObject vao_rect;
    QOpenGLBuffer vbo_rect;
    QOpenGLTexture *pTex_rect;
    // mapped vertices
    QOpenGLVertexArrayObject vao_mv;
    QOpenGLBuffer vbo_mv;
    QOpenGLBuffer ebo_mv;
    QVector<int> restartPoints;

    // shaders and attributes locations
    QOpenGLShaderProgram m_colorProg;
    QOpenGLShaderProgram m_texProg;
    int vertex_loc_0;
    int color_loc_0;
    int mv_matrix_loc_0;
    int proj_matrix_loc_0;

    int vertex_loc_1;
    int texcoord_loc_1;
    int mv_matrix_loc_1;
    int proj_matrix_loc_1;
};


#endif // SHOWTEXTUREMAPPING_H
