#ifndef SPHEREGENERATOR_H
#define SPHEREGENERATOR_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>

class SphereGenerator
{
public:
    void generate(double radius, int resolution);

    const QVector<QVector3D> &vertices() const { return m_vertices; }
    int vertexDataLength() const { return m_vertices.size() * sizeof(QVector3D); }
    const QVector<QVector3D> &normals() const { return m_normals; }
    int normalDataLength() const { return m_normals.size() * sizeof(QVector3D); }
    const QVector<QVector2D> &texcoords() const { return m_texcoords; }
    int texcoordDataLength() const { return m_texcoords.size() * sizeof(QVector2D); }
    const QVector<unsigned int> &indices() const { return m_indices; }
    int indexDataLength() const { return m_indices.size() * sizeof(unsigned int); }

    unsigned int restartIndex() const { return 0xFFFFFFFF; }

protected:
    QVector3D fromPoleCoord(double alpha, double beta, double r);
    QVector2D uvCoord(QVector3D xyz, double radius = 1.0);
    QVector2D uvCoordNew(int i, int j, int resolution);

private:
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    QVector<QVector2D> m_texcoords;
    QVector<unsigned int> m_indices;
};

#endif // SPHEREGENERATOR_H
