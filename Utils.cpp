#include "Utils.h"
#include "Helpers.h"

// CAMERA TRANSFORMATION MATRIX
Matrix4 Utils::worldToCameraTranslationMatrix(Camera *c) {
    double val[4][4] = {{1, 0, 0, -c->position.x},
                        {0, 1, 0, -c->position.y},
                        {0, 0, 1, -c->position.z},
                        {0, 0, 0, 1}};
    Matrix4 result(val);
    return result;
}

Matrix4 Utils::worldToCameraRotationMatrix(Camera *c) {
    double val[4][4] = {{c->u.x, c->u.y, c->u.z, 0},
                        {c->v.x, c->v.y, c->v.z, 0},
                        {c->w.x, c->w.y, c->w.z, 0},
                        {0, 0, 0, 1}};
    Matrix4 result(val);
    return result;
}

Matrix4 Utils::worldToCameraMatrix(Camera *c) {
    Matrix4 translation = worldToCameraTranslationMatrix(c);
    Matrix4 rotation = worldToCameraRotationMatrix(c);
    // First translate, then rotate
    return multiplyMatrixWithMatrix(rotation, translation);
}

// PROJECTION MATRIX
Matrix4 Utils::orthographicProjectionMatrix(double l, double r, double b,
                                            double t, double n, double f) {
    double val[4][4] = {{2 / (r - l), 0, 0, -(r + l) / (r - l)},
                        {0, 2 / (t - b), 0, -(t + b) / t - b},
                        {0, 0, -2 / (f - n), -(f + n) / (f - n)},
                        {0, 0, 0, 1}};
    Matrix4 result(val);
    return result;
}

Matrix4 Utils::perspectiveToOrthographicMatrix(double l, double r, double b,
                                               double t, double n, double f) {
    double val[4][4] = {{n, 0, 0, 0},
                        {0, n, 0, 0},
                        {0, 0, f + n, f * n},
                        {0, 0, -1, 0}};
    Matrix4 result(val);
    return result;
}



Matrix4 Utils::perspectiveProjectionMatrix(double l, double r, double b,
                                           double t, double n, double f) {
    Matrix4 orth = orthographicProjectionMatrix(l, r, b, t, n, f);
    Matrix4 p2o = perspectiveToOrthographicMatrix(l, r, b, t, n, f);
    return multiplyMatrixWithMatrix(orth, p2o);
}

// VIEWPORT MATRIX

Matrix4 Utils::viewportMatrix(double res_x, double res_y) {
    double val[4][4] = {{res_x / 2, 0, 0, (res_x - 1) / 2},
                        {0, res_y / 2, 0, (res_y - 1) / 2},
                        {0, 0, 0.5, 0.5},
                        {0, 0, 0, 1}};
    Matrix4 result(val);
    return result;
}
