#include <QOpenGLFramebufferObjectFormat>
#include <QSGSimpleTextureNode>
#include "showtexturemapping.h"

#define TO_OFFSET(x) reinterpret_cast<const void*>(x)

ShowTextureMapping::ShowTextureMapping()
    : m_showMappedVertices(false)
    , m_contentScale(1)
    , m_sphereResolution(360)
    , m_cameraPosition(0, 0, 25)
{

}

ShowTextureMapping::~ShowTextureMapping()
{

}

FBO::Renderer *ShowTextureMapping::createRenderer() const
{
    return new ShowTextureMappingRenderer();
}

QSGNode *ShowTextureMapping::updatePaintNode(QSGNode *oldNode,
                                             UpdatePaintNodeData *nodeData)
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

void ShowTextureMapping::setShowMappedVertices(bool val)
{
    if (m_showMappedVertices == val) {
        return;
    }

    m_showMappedVertices = val;
    emit showMappedVerticesChanged();
    update();
}

void ShowTextureMapping::setContentScale(double scale)
{
    if (m_contentScale == scale) {
        return;
    }

    m_contentScale = scale;
    emit contentScaleChanged();
    update();
}

void ShowTextureMapping::setSphereResolution(int newResolution)
{
    if (m_sphereResolution == newResolution) {
        return;
    }
    m_sphereResolution = newResolution;
    emit sphereResolutionChanged();
    update();
}

void ShowTextureMapping::setCameraPosition(const QVector3D &pos)
{
    if (m_cameraPosition == pos) {
        return;
    }
    m_cameraPosition = pos;
    emit cameraPositionChanged();
    update();
}

ShowTextureMappingRenderer::ShowTextureMappingRenderer()
    : vbo_rect(), pTex_rect(nullptr)
    , vbo_mv(), ebo_mv(QOpenGLBuffer::IndexBuffer)
{
    showMappedVertices = false;
    scale = 1;
    resolution = 360;
    cameraPosition = QVector3D(0, 0, 25);
    initialize();
}

ShowTextureMappingRenderer::~ShowTextureMappingRenderer()
{
    if (pTex_rect) { delete pTex_rect; }
}

void ShowTextureMappingRenderer::initialize()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Simple program use solid color
    m_colorProg.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        QStringLiteral(":/shaders/coloring.vert"));
    m_colorProg.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        QStringLiteral(":/shaders/coloring.frag"));
    m_colorProg.link();

    vertex_loc_0 = m_colorProg.attributeLocation("vPosition");
    color_loc_0 = m_colorProg.attributeLocation("vColor");
    mv_matrix_loc_0 = m_colorProg.uniformLocation("vModelView");
    proj_matrix_loc_0 = m_colorProg.uniformLocation("vProjection");

    // Program with texture
    m_texProg.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                      QStringLiteral(":/shaders/texture.vert"));
    m_texProg.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                      QStringLiteral(":/shaders/texture.frag"));
    m_texProg.link();

    vertex_loc_1 = m_texProg.attributeLocation("vPosition");
    texcoord_loc_1 = m_texProg.attributeLocation("vTexCoord");
    mv_matrix_loc_1 = m_texProg.uniformLocation("vModelView");
    proj_matrix_loc_1 = m_texProg.uniformLocation("vProjection");

    createGeometry();
}

void ShowTextureMappingRenderer::createGeometry()
{
    createRect();
    // can safely comment out here.
    // at least one sync is done before any render
    // createMappedVertices is called at that time.
    // createMappedVertices();
}

void ShowTextureMappingRenderer::synchronize(QQuickFramebufferObject *item)
{
    auto stm = qobject_cast<ShowTextureMapping *>(item);

    // update projection matrix
    updateProjection(stm->width(), stm->height());

    // Synchronize state data
    cameraPosition = stm->cameraPosition();
    showMappedVertices = stm->showMappedVertices();
    scale = stm->contentScale();
    if (resolution != stm->sphereResolution()) {
        resolution = stm->sphereResolution();
        createMappedVertices();
    }
}

void ShowTextureMappingRenderer::updateProjection(int width, int height)
{
    QSize size(width, height);
    if (size != m_viewportSize) {
        m_viewportSize = size;
        auto h = size.height();
        h = h == 0 ? 1 : h;
        m_projMatrix.setToIdentity();
        m_projMatrix.perspective(60.0f,
                                 size.width() / float(h),
                                 20.0f, 2000.0f);
        glViewport(0, 0, size.width(), size.height());
    }
}

void ShowTextureMappingRenderer::render()
{
    glDepthMask(true);
    glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // actual paint here
    // View transform: camera position
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(cameraPosition, QVector3D(cameraPosition.toVector2D(), 0),
                        QVector3D(0, 1, 0));

    paintRect();
    if (showMappedVertices) {
        paintMappedVertices();
    }

    update();
}

QOpenGLFramebufferObject *ShowTextureMappingRenderer::createFramebufferObject(
    const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void ShowTextureMappingRenderer::createMappedVertices()
{
    sphere.generate(1.0, resolution);

    if (!vao_mv.isCreated()) { vao_mv.create(); }
    vao_mv.bind();

    if (!ebo_mv.isCreated()) { ebo_mv.create();}
    ebo_mv.bind();
    ebo_mv.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo_mv.allocate(sphere.indices().constData(), sphere.indexDataLength());

    if (vbo_mv.isCreated()) { vbo_mv.destroy(); }
    vbo_mv.create();
    vbo_mv.bind();
    vbo_mv.setUsagePattern(QOpenGLBuffer::StaticDraw);
    // use texcoords here, and draw it with scale
    vbo_mv.allocate(sphere.texcoords().constData(), sphere.texcoordDataLength());
    glVertexAttribPointer(vertex_loc_0,
                          2, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(0) // offset
                         );
    m_colorProg.enableAttributeArray(vertex_loc_0);

    vao_mv.release();
}

void ShowTextureMappingRenderer::createRect()
{
    auto img = QImage(":/assets/land_shallow_topo_2048.tif").mirrored();
    pTex_rect = new QOpenGLTexture(img);

    GLfloat w_2 = img.width() / (GLfloat) 2;
    GLfloat h_2 = img.height() / (GLfloat) 2;
    world.setRect(-w_2, h_2, 2 * w_2, 2 * h_2);

    GLfloat vertices[] = {
        -w_2, -h_2, -2,
        w_2, -h_2, -2,
        w_2, h_2, -2,
        -w_2, h_2, -2,
    };
    static const GLfloat texcoords[] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
    };
    vao_rect.create();
    vao_rect.bind();

    vbo_rect.create();
    vbo_rect.bind();
    vbo_rect.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_rect.allocate(sizeof(vertices) + sizeof(texcoords));
    vbo_rect.write(0, vertices, sizeof(vertices));
    vbo_rect.write(sizeof(vertices), texcoords, sizeof(texcoords));
    glVertexAttribPointer(vertex_loc_1,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          reinterpret_cast<const void *>(0) // offset
                         );
    glVertexAttribPointer(texcoord_loc_1,
                          2, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          reinterpret_cast<const void *>(sizeof(vertices)) // offset
                         );
    m_texProg.enableAttributeArray(vertex_loc_1);
    m_texProg.enableAttributeArray(texcoord_loc_1);

    vao_rect.release();
}

void ShowTextureMappingRenderer::paintMappedVertices()
{
    QMatrix4x4 m;
    // Model transform
    m.scale(scale, scale, 0.99);
    m.translate(-world.width() / 2, -world.height() / 2, 0);
    m.scale(world.width(), world.height(), 1);
    m.translate(0, 0, -2);

    m_colorProg.bind();
    m_colorProg.setUniformValue(mv_matrix_loc_0, m_viewMatrix * m);
    m_colorProg.setUniformValue(proj_matrix_loc_0, m_projMatrix);
    m_colorProg.setAttributeValue(color_loc_0, QColor(255, 128, 0));

    vao_mv.bind();
    // draw
#if defined(Q_OS_ANDROID) || defined(TEST_ANDROID_LOCAL)
    int lastIdx = 0;
    for (auto idx : sphere.restartPoints(true)) {
        int count = idx - lastIdx;
        glDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_INT,
                       TO_OFFSET(lastIdx * sizeof(GLuint)));
        lastIdx = idx + 1;
    }
#else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int lastIdx = 0;
    for (auto idx : sphere.restartPoints(true)) {
        int count = idx - lastIdx;
        glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT,
                       TO_OFFSET(lastIdx * sizeof(GLuint)));
        lastIdx = idx + 1;
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    vao_mv.release();
    m_colorProg.release();
}

void ShowTextureMappingRenderer::paintRect()
{
    QMatrix4x4 m;
    // Model transform
    m.scale(scale, scale, 1);

    m_texProg.bind();
    m_colorProg.setUniformValue(mv_matrix_loc_1, m_viewMatrix * m);
    m_texProg.setUniformValue(proj_matrix_loc_1, m_projMatrix);

    vao_rect.bind();
    pTex_rect->bind();
    // draw
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    pTex_rect->release();
    vao_rect.release();
    m_texProg.release();
}
