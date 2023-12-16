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
#include "Scene.h"

namespace MeshTransformations {
Matrix4 translateMesh(Translation &t, Matrix4 &m);
Matrix4 scaleMesh(Scaling &s, Matrix4 &m);
Matrix4 rotateMesh(Rotation &r, Matrix4 &m);

double min_of_three(double a, double b, double c);
double degreesToRadians(double degrees);
Matrix4 transposeMatrix(Matrix4 &m);
Matrix4 rotationMatrixAlongX(double angle);
} // namespace MeshTransformations
