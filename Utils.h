#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

#include "Camera.h"
#include "Matrix4.h"

namespace Utils {

// CAMERA TRANSFORMATION MATRIX
Matrix4 worldToCameraTranslationMatrix(Camera *c);
Matrix4 worldToCameraRotationMatrix(Camera *c);
Matrix4 worldToCameraMatrix(Camera *c);

// PROJECTION MATRIX
Matrix4 orthographicProjectionMatrix(double l, double r, double b, double t,
                                     double n, double f);

Matrix4 perspectiveToOrthographicMatrix(double l, double r, double b, double t,
                                        double n, double f);

Matrix4 perspectiveProjectionMatrix(double l, double r, double b, double t,
                                    double n, double f);

// VIEWPORT MATRIX
Matrix4 viewportMatrix(double res_x, double res_y);

} // namespace Utils
