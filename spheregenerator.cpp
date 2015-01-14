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

QVector<int> SphereGenerator::restartPoints(bool drawWireframe) const
{
#if defined(Q_OS_ANDROID) || defined(TEST_ANDROID_LOCAL)
    if (drawWireframe) {
        return m_restartPoints;
    } else {
        return m_restartPoints.mid(0, m_maxRestartPointsForNonWireframe);
    }
#else
    Q_UNUSED(drawWireframe);
    return m_restartPoints;
#endif
}

void SphereGenerator::generate(double radius, int resolution)
{
    m_vertices.clear();
    m_normals.clear();
    m_indices.clear();
    m_texcoords.clear();
    m_restartPoints.clear();
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

            m_indices.append(count++);
            m_indices.append(count++);
            m_vertices << p0 << p1;
            m_normals << p0 << p1;
            //            m_texcoords << uvCoord(p0, radius)
            //                        << uvCoord(p1, radius);
            m_texcoords << uvCoordNew(i, j, resolution)
                        << uvCoordNew(i, j + 1, resolution);
        }
        m_restartPoints << m_indices.size();
        m_indices << restartIndex();
    }
#if defined(Q_OS_ANDROID) || defined(TEST_ANDROID_LOCAL)
    m_maxRestartPointsForNonWireframe = m_restartPoints.size();
    count = 0;
    for (int j = 0; j <= resolution; j++) {
        int temp = count;
        for (int i = 0; i <= 2 * resolution; i++) {
            m_indices << temp;
            temp += 2;
        }
        m_restartPoints << m_indices.size();
        m_indices << restartIndex();
        temp = count + 1;
        for (int i = 0; i <= 2 * resolution; i++) {
            m_indices << temp;
            temp += 2;
        }
        m_restartPoints << m_indices.size();
        m_indices << restartIndex();

        count += 2 * (2 * resolution + 1);
    }
#endif
}
