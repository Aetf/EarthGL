#include <QImage>
#include <QtMath>
#include <QMatrix4x4>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include "earth3d.h"
#include "earth3drenderer.h"

#define TO_OFFSET(x) reinterpret_cast<const void*>(x)

Earth3DRenderer::Earth3DRenderer()
    : vbo_camera(), ebo_camera(QOpenGLBuffer::IndexBuffer)
    , vbo_axis(), ebo_axis(QOpenGLBuffer::IndexBuffer)
    , vbo_sphere(), ebo_sphere(QOpenGLBuffer::IndexBuffer), pTex_sphere(nullptr)
{
    showVertices = showCamera = useCamera2 = false;
    resolution = 360;
    initialize();
}

Earth3DRenderer::~Earth3DRenderer()
{
    if (pTex_sphere) { delete pTex_sphere; }
}

void Earth3DRenderer::initialize()
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

    // Program with texture and lighting (Gourand algorithm)
    m_texLightProg.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           QStringLiteral(":/shaders/texlighting.vert"));
    m_texLightProg.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           QStringLiteral(":/shaders/texlighting.frag"));
    m_texLightProg.link();

    vertex_loc_1 = m_texLightProg.attributeLocation("vPosition");
    texcoord_loc_1 = m_texLightProg.attributeLocation("vTexCoord");
    normal_loc_1 = m_texLightProg.attributeLocation("vNormal");
    mv_matrix_loc_1 = m_texLightProg.uniformLocation("vModelView");
    proj_matrix_loc_1 = m_texLightProg.uniformLocation("vProjection");
    nm_matrix_loc_1 = m_texLightProg.uniformLocation("vNormalMatrix");
    lpos_loc_1 = m_texLightProg.uniformLocation("vLightPosition");

    ami_color_loc_1 = m_texLightProg.uniformLocation("fAmbientColor");
    dif_color_loc_1 = m_texLightProg.uniformLocation("fDiffuseColor");
    spec_color_loc_1 = m_texLightProg.uniformLocation("fSpecularColor");
    ami_ref_loc_1 = m_texLightProg.uniformLocation("fAmbientReflection");
    dif_ref_loc_1 = m_texLightProg.uniformLocation("fDiffuseReflection");
    spec_ref_loc_1 = m_texLightProg.uniformLocation("fSpecularReflection");
    shininess_loc_1 = m_texLightProg.uniformLocation("fShininess");

    createGeometry();
}

void Earth3DRenderer::createGeometry()
{
    createAxis();
    createCamera();
    // can safely comment out here.
    // at least one sync is done before any render
    // createSphere is called at that time.
    //    createSphere();
}

QOpenGLFramebufferObject *Earth3DRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void Earth3DRenderer::synchronize(QQuickFramebufferObject *item)
{
    auto earth3d = qobject_cast<Earth3D *>(item);

    // update projection matrix
    updateProjection(earth3d->width(), earth3d->height());

    // Synchronize state data
    showCamera = earth3d->showCamera();
    useCamera2 = earth3d->useCamera2();
    showVertices = earth3d->showVertices();
    updateCamera(0, earth3d->cameraXRotate(), earth3d->cameraYRotate(),
                 earth3d->cameraDistance());
    if (useCamera2) {
        updateCamera(1, earth3d->camera2XRotate(), earth3d->camera2YRotate(),
                     earth3d->camera2Distance());
    }
    if (resolution != earth3d->sphereResolution()) {
        resolution = earth3d->sphereResolution();
        createSphere();
    }

    // update view matrix
    updateViewMatrix();
}

void Earth3DRenderer::updateProjection(int width, int height)
{
    QSize size(width, height);
    if (size != m_viewportSize) {
        m_viewportSize = size;
        auto h = size.height();
        h = h == 0 ? 1 : h;
        m_projMatrix.setToIdentity();
        m_projMatrix.perspective(60.0f,
                                 size.width() / float(h),
                                 0.001f, 1000.0f);
        glViewport(0, 0, size.width(), size.height());
    }
}

void Earth3DRenderer::updateCamera(int idx, double xrot, double yrot, double dist)
{
    m_cameraTransform[idx].setToIdentity();
    m_cameraTransform[idx].rotate(xrot, 0, 1, 0);
    m_cameraTransform[idx].rotate(yrot, 1, 0, 0);
    m_cameraPos[idx] = m_cameraTransform[idx] * QVector3D(0, 0, dist);
    m_cameraUp[idx] = m_cameraTransform[idx] * QVector3D(0, 1, 0);
    m_cameraDistance[idx] = dist;
}

void Earth3DRenderer::updateViewMatrix()
{
    int idx = useCamera2 ? 1 : 0;
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt(m_cameraPos[idx], QVector3D(0, 0, 0), m_cameraUp[idx]);
}

void Earth3DRenderer::render()
{
    glDepthMask(true);
    glClearColor(0.5f, 0.5f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // actual paint here
    //    paintAxis();
    paintSphere();
    if (showCamera) {
        paintCamera();
    }
    if (showVertices) {
        paintSphereVertices();
    }

    update();
}

void Earth3DRenderer::paintAxis()
{
    QMatrix4x4 m;
    // Model transform
    m.scale(0.5);

    m_colorProg.bind();
    m_colorProg.setUniformValue(proj_matrix_loc_0, m_projMatrix);
    m_colorProg.setUniformValue(mv_matrix_loc_0, m_viewMatrix * m);

    vao_axis.bind();
    //    glEnable(GL_LINE_SMOOTH);
    glLineWidth(3.0f);
    glDrawElements(GL_LINES, 6, GL_UNSIGNED_SHORT, TO_OFFSET(0));
    //    glDisable(GL_LINE_SMOOTH);

    vao_axis.release();
    m_colorProg.release();
}

void Earth3DRenderer::paintCamera()
{
    QMatrix4x4 m;
    // Model transform
    m.translate(0, 0, m_cameraDistance[0]);
    m.rotate(180, 0, 1, 0);
    m.scale(0.1);
    m = m_cameraTransform[0] * m;

    m_colorProg.bind();
    m_colorProg.setUniformValue(proj_matrix_loc_0, m_projMatrix);
    m_colorProg.setUniformValue(mv_matrix_loc_0, m_viewMatrix * m);

    vao_camera.bind();
    // draw box and cylinder with strip
    //    glEnable(GL_PRIMITIVE_RESTART);
    //    glPrimitiveRestartIndex(0xFFFFFFFF);
    //    glDrawElements(GL_TRIANGLE_STRIP, stripCount, GL_UNSIGNED_INT, TO_OFFSET(0));
    //    glDisable(GL_PRIMITIVE_RESTART);

    int lastIdx = 0;
    for (auto idx : camera_restartPoints) {
        int count = idx - lastIdx;
        glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT,
                       TO_OFFSET(lastIdx * sizeof(GLuint)));
        lastIdx = idx + 1;
    }

    // draw cycle
    glDrawElements(GL_TRIANGLE_FAN, fanCount, GL_UNSIGNED_INT,
                   TO_OFFSET(stripCount * sizeof(GLuint)));

    vao_camera.release();
    m_colorProg.release();
}

void Earth3DRenderer::paintSphere()
{
    QMatrix4x4 m;
    // Model transform
    //    m.scale(0.5);

    // Lighting position
    QMatrix4x4 lightTransform;
    //    lightTransform.rotate(0, 0, 1, 0);
    QVector3D lightPos = lightTransform * QVector3D(-3, 3, 2);

    m_texLightProg.bind();
    m_texLightProg.setUniformValue(proj_matrix_loc_1, m_projMatrix);
    m_texLightProg.setUniformValue(mv_matrix_loc_1, m_viewMatrix * m);
    m_texLightProg.setUniformValue(nm_matrix_loc_1, (m_viewMatrix * m).normalMatrix());
    m_texLightProg.setUniformValue(lpos_loc_1, m_viewMatrix * lightPos);

    m_texLightProg.setUniformValue(ami_color_loc_1, QColor(100, 100, 100));
    m_texLightProg.setUniformValue(dif_color_loc_1, QColor(128, 128, 128));
    m_texLightProg.setUniformValue(spec_color_loc_1, QColor(255, 255, 255));
    m_texLightProg.setUniformValue(ami_ref_loc_1, 1.0f);
    m_texLightProg.setUniformValue(dif_ref_loc_1, 1.0f);
    m_texLightProg.setUniformValue(spec_ref_loc_1, 1.0f);
    m_texLightProg.setUniformValue(shininess_loc_1, 100.0f);

    vao_sphere.bind();
    pTex_sphere->bind();
    // draw
    //    glEnable(GL_PRIMITIVE_RESTART);
    //    glPrimitiveRestartIndex(0xFFFFFFFF);
    //    glDrawElements(GL_TRIANGLE_STRIP, sphere.indices().size(), GL_UNSIGNED_INT, TO_OFFSET(0));
    //    glDisable(GL_PRIMITIVE_RESTART);
    int lastIdx = 0;
    for (auto idx : sphere.restartPoints()) {
        int count = idx - lastIdx;
        glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT,
                       TO_OFFSET(lastIdx * sizeof(GLuint)));
        lastIdx = idx + 1;
    }

    pTex_sphere->release();
    vao_sphere.release();
    m_texLightProg.release();
}

void Earth3DRenderer::paintSphereVertices()
{
    QMatrix4x4 m;
    // Model transform
    m.scale(1.001);

    m_colorProg.bind();
    m_colorProg.setUniformValue(proj_matrix_loc_0, m_projMatrix);
    m_colorProg.setUniformValue(mv_matrix_loc_0, m_viewMatrix * m);
    m_colorProg.setAttributeValue(color_loc_0, QColor(255, 128, 0));

    vao_sphere_fw.bind();
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
    for (auto idx : sphere.restartPoints()) {
        int count = idx - lastIdx;
        glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT,
                       TO_OFFSET(lastIdx * sizeof(GLuint)));
        lastIdx = idx + 1;
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    vao_sphere_fw.release();
    m_colorProg.release();
}

void Earth3DRenderer::createAxis()
{
    static const GLfloat vertices[] = {
        0.0, 0.0, 0.0,
        1.5, 0.0, 0.0,
        0.0, 1.5, 0.0,
        0.0, 0.0, 1.5,
        1.5, 1.5, 1.5,
    };
    static const GLfloat colors[] = {
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    };
    static const GLushort indices[] = {
        4, 0, 1, 0, 2, 0, 3,
    };
    vao_axis.create();
    vao_axis.bind();

    ebo_axis.create();
    ebo_axis.bind();
    ebo_axis.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo_axis.allocate(indices, sizeof(indices));

    vbo_axis.create();
    vbo_axis.bind();
    vbo_axis.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_axis.allocate(sizeof(vertices) + sizeof(colors));
    vbo_axis.write(0, vertices, sizeof(vertices));
    vbo_axis.write(sizeof(vertices), colors, sizeof(colors));
    glVertexAttribPointer(vertex_loc_0,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(0) // offset
                         );
    glVertexAttribPointer(color_loc_0,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(sizeof(vertices)) // offset
                         );
    m_colorProg.enableAttributeArray(vertex_loc_0);
    m_colorProg.enableAttributeArray(color_loc_0);

    vao_axis.release();
}

void Earth3DRenderer::createCamera()
{
    QVector<QVector3D> vertices;
    QVector<QVector3D> colors;
    QVector<GLuint> indices;
    camera_restartPoints.clear();

    // first the rectagular box
    vertices << QVector3D(1, 0.5, 0) << QVector3D(1, 0.5, -0.5)
             << QVector3D(-1, 0.5, -0.5) << QVector3D(-1, 0.5, 0)
             << QVector3D(-1, -0.5, -0.5) << QVector3D(-1, -0.5, 0)
             << QVector3D(1, -0.5, 0) << QVector3D(1, -0.5, -0.5);
    colors << QVector3D(0.8, 0.8, 0.8) << QVector3D(0.8, 0.8, 0.8)
           << QVector3D(0.8, 0.8, 0.8) << QVector3D(0.8, 0.8, 0.8)
           << QVector3D(0.8, 0.8, 0.8) << QVector3D(0.8, 0.8, 0.8)
           << QVector3D(0.8, 0.8, 0.8) << QVector3D(0.8, 0.8, 0.8);
    indices << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7
            << 0xFFFFFFFF
            << 3 << 5 << 6 << 0 << 7 << 1 << 4 << 2
            << 0xFFFFFFFF;
    camera_restartPoints << 8 << 17;
    // then generate a cylinder
    int count = vertices.size();
    for (int i = 0; i <= 360; i++) {
        double alpha = (double) i / 360 * 2 * M_PI;
        double x = 0.4 * qCos(alpha);
        double y = 0.4 * qSin(alpha);
        vertices << QVector3D(x, y, 0) << QVector3D(x, y, 0.3);
        colors << QVector3D(1, 0.5, 0) << QVector3D(1, 0.5, 0);
        indices.append(count++);
        indices.append(count++);
    }
    camera_restartPoints << indices.size();
    indices << 0xFFFFFFFF;
    stripCount = indices.size();
    // then a cycle
    vertices << QVector3D(0, 0, 0.3);
    colors << QVector3D(1, 0.5, 0);
    indices << count++;
    for (int i = 0; i <= 360; i++) {
        double alpha = (double) i / 360 * 2 * M_PI;
        double x = 0.4 * qCos(alpha);
        double y = 0.4 * qSin(alpha);
        vertices << QVector3D(x, y, 0.3);
        colors << QVector3D(1, 0.5, 0);
        indices << count++;
    }
    fanCount = indices.size() - stripCount;

    if (vao_camera.isCreated()) { vao_camera.destroy(); }
    vao_camera.create();
    vao_camera.bind();

    if (ebo_camera.isCreated()) { ebo_camera.destroy(); }
    ebo_camera.create();
    ebo_camera.bind();
    ebo_camera.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo_camera.allocate(indices.constData(), indices.size() * sizeof(GLuint));

    vbo_camera.create();
    vbo_camera.bind();
    vbo_camera.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_camera.allocate((vertices.size() + colors.size()) * sizeof(QVector3D));
    vbo_camera.write(0, vertices.constData(), vertices.size() * sizeof(QVector3D));
    vbo_camera.write(vertices.size() * sizeof(QVector3D),
                     colors.constData(), colors.size() * sizeof(QVector3D));
    glVertexAttribPointer(vertex_loc_0,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(0) // offset
                         );
    glVertexAttribPointer(color_loc_0,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(vertices.size() * sizeof(QVector3D)) // offset
                         );
    m_colorProg.enableAttributeArray(vertex_loc_0);
    m_colorProg.enableAttributeArray(color_loc_0);

    vao_camera.release();
}

void Earth3DRenderer::createSphere()
{
    sphere.generate(1.0, resolution);

    if (!vao_sphere.isCreated()) { vao_sphere.create(); }
    vao_sphere.bind();

    if (!ebo_sphere.isCreated()) { ebo_sphere.create(); }
    ebo_sphere.bind();
    ebo_sphere.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ebo_sphere.allocate(sphere.indices().constData(), sphere.indexDataLength());

    if (!vbo_sphere.isCreated()) { vbo_sphere.create(); }
    vbo_sphere.bind();
    vbo_sphere.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_sphere.allocate(sphere.vertexDataLength()
                        + sphere.normalDataLength()
                        + sphere.texcoordDataLength());
    int offset = 0;
    vbo_sphere.write(offset, sphere.vertices().constData(), sphere.vertexDataLength());
    glVertexAttribPointer(vertex_loc_1,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(offset) // offset
                         );
    offset += sphere.vertexDataLength();
    vbo_sphere.write(offset, sphere.texcoords().constData(), sphere.texcoordDataLength());
    glVertexAttribPointer(texcoord_loc_1,
                          2, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(offset) // offset
                         );
    offset += sphere.texcoordDataLength();
    vbo_sphere.write(offset, sphere.normals().constData(), sphere.normalDataLength());
    glVertexAttribPointer(normal_loc_1,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(offset) // offset
                         );
    m_texLightProg.enableAttributeArray(vertex_loc_1);
    m_texLightProg.enableAttributeArray(texcoord_loc_1);
    m_texLightProg.enableAttributeArray(normal_loc_1);

    vao_sphere.release();

    if (pTex_sphere == nullptr)
        pTex_sphere = new QOpenGLTexture(
            QImage(":/assets/land_shallow_topo_2048.tif").mirrored());

    // A firmwire version
    if (!vao_sphere_fw.isCreated()) { vao_sphere_fw.create(); }
    vao_sphere_fw.bind();

    ebo_sphere.bind();
    vbo_sphere.bind();

    glVertexAttribPointer(vertex_loc_0,
                          3, GL_FLOAT, // tupleSize, type
                          GL_FALSE, 0, // normalize, stride
                          TO_OFFSET(0) // offset
                         );
    m_texLightProg.enableAttributeArray(vertex_loc_0);

    vao_sphere_fw.release();
}
