#include "MeshTransformations.h"
#include "Helpers.h"
#include "Vec3.h"
#include <cmath>

double MeshTransformations::min_of_three(double a, double b, double c) {
    return std::min(a, std::min(b, c));
}

double MeshTransformations::max_of_three(double a, double b, double c) {
    return std::max(a, std::max(b, c));
}

double MeshTransformations::degreesToRadians(double degrees) {
    return degrees * M_PI / 180;
}

Matrix4 MeshTransformations::transposeMatrix(Matrix4 &m) {
    double val[4][4] = {
        {m.values[0][0], m.values[1][0], m.values[2][0], m.values[3][0]},
        {m.values[0][1], m.values[1][1], m.values[2][1], m.values[3][1]},
        {m.values[0][2], m.values[1][2], m.values[2][2], m.values[3][2]},
        {m.values[0][3], m.values[1][3], m.values[2][3], m.values[3][3]}};
    Matrix4 result(val);
    return result;
}
Matrix4 MeshTransformations::rotationMatrixAlongX(double angle){
    double r = degreesToRadians(angle);
    double val[4][4] = {
        {1,0,0,0},
        {0,cos(r),-sin(r),0},
        {0,sin(r),cos(r),0},
        {0,0,0,1}
    };
    Matrix4 result(val);
    return result;
}

Matrix4 MeshTransformations::translateMesh(Translation *t, Matrix4 &m) {
    double val[4][4] = {
        {1, 0, 0, t->tx},
        {0, 1, 0, t->ty},
        {0, 0, 1, t->tz},
        {0, 0, 0, 1}};
    Matrix4 result(val);
    return multiplyMatrixWithMatrix(result, m);
}
Matrix4 MeshTransformations::scaleMesh(Scaling *s, Matrix4 &m) {
    double val[4][4] = {
        {s->sx, 0, 0, 0},
        {0, s->sy, 0, 0},
        {0, 0, s->sz, 0},
        {0, 0, 0, 1}};
    Matrix4 result(val);
    return multiplyMatrixWithMatrix(result, m);
}
Matrix4 MeshTransformations::rotateMesh(Rotation *r, Matrix4 &matrix) {
    // using the orthonormal method (slide modelling transformations starting
    // from page 41)
    Vec3 u(r->ux, r->uy, r->uz);
    Vec3 v;
    double min = min_of_three(u.x, u.y, u.z);
    if (min == u.x) {
        v = normalizeVec3(Vec3(0, -u.z, u.y));
    } else if (min == u.y) {
        v = normalizeVec3(Vec3(-u.z, 0, u.x));
    } else {
        v = normalizeVec3(Vec3(-u.y, u.x, 0));
    }
    Vec3 w = normalizeVec3(crossProductVec3(u, v));

    double m_inverse_val[4][4] = {
        {u.x, v.x, w.x, 0},
        {u.y, v.y, w.y, 0},
        {u.z, v.z, w.z, 0},
        {0, 0, 0, 1}

    };
    Matrix4 m_inverse(m_inverse_val);
    Matrix4 m = transposeMatrix(m_inverse);
    Matrix4 m_inverse_mult_rotatation = multiplyMatrixWithMatrix(m_inverse, rotationMatrixAlongX(r->angle));
    Matrix4 result = multiplyMatrixWithMatrix(m_inverse_mult_rotatation, m);
    return multiplyMatrixWithMatrix(result, matrix);
} 

Matrix4 MeshTransformations::applyAllTransformations(
    int num_transformations, std::vector<char> &transformation_types,
    std::vector<int> &transformation_ids,
    std::vector<Translation*> &translations, std::vector<Scaling*> &scalings,
    std::vector<Rotation*> &rotations) {
    Matrix4 result = getIdentityMatrix();
    for (int i = 0; i < num_transformations; i++) {
        if (transformation_types[i] == TRANSLATION) {
            result = translateMesh(translations[transformation_ids[i] - 1],
                                   result);
        } else if (transformation_types[i] == SCALING) {
            result = scaleMesh(scalings[transformation_ids[i] - 1], result);
        } else if (transformation_types[i] == ROTATION) {
            result = rotateMesh(rotations[transformation_ids[i] - 1], result);
        }
    }
    return result;
}
