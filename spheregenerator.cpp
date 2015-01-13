#include <QtMath>
#include <QDebug>
#include "spheregenerator.h"

/*!
 * \brief SphereGenerator::fromPoleCoord
 * \param alpha: angle against Y axis, in radians
 * \param beta: angle against XZ plane, in radians
 * \param r: radius
 * \return
 */
QVector3D SphereGenerator::fromPoleCoord(double alpha, double beta, double r)
{
    double currR = r * qCos(beta);
    return QVector3D(currR * qCos(alpha), r * qSin(beta), currR * qSin(alpha));
}

QVector2D SphereGenerator::uvCoord(QVector3D xyz, double radius)
{
    xyz /= radius;
    return QVector2D(0.5 + qAtan2(xyz.z(), xyz.x()) / (2 * M_PI),
                     0.5 - qAsin(xyz.y()) / M_PI);
}

QVector2D SphereGenerator::uvCoordNew(int i, int j, int resolution)
{
    double u = 1 - i / (double)(2 * resolution);
    double v = j / (double) resolution;
    return QVector2D(u, v);
}

void SphereGenerator::generate(double radius, int resolution)
{
    m_vertices.clear();
    m_normals.clear();
    m_indices.clear();
    m_texcoords.clear();
    int count = 0;
    /*
     * 0 <= alpha <= 2*pi
     * -pi/2 <= beta <= pi/2
     */
    for (int j = 0; j <= resolution; j++) {
        double beta = j / (double) resolution * M_PI - M_PI_2;
        double betaN = (j + 1) / (double) resolution * M_PI - M_PI_2;
        for (int i = 0; i <= 2 * resolution; i++) {
            double alpha = i / (double) resolution * M_PI;
            auto p0 = fromPoleCoord(alpha, beta, radius);
            auto p1 = fromPoleCoord(alpha, betaN, radius);

            m_indices << count++ << count++;
            m_vertices << p0 << p1;
            m_normals << p0 << p1;
            m_texcoords << uvCoord(p0, radius)
                        << uvCoord(p1, radius);
//            m_texcoords << uvCoordNew(i, j, resolution)
//                        << uvCoordNew(i, j + 1, resolution);
        }
        m_indices << restartIndex();
    }
}
