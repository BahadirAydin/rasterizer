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

#define TRANSLATION 't'
#define SCALING 's'
#define ROTATION 'r'

namespace MeshTransformations {
Matrix4 translateMesh(Translation *t, Matrix4 &m);
Matrix4 scaleMesh(Scaling *s, Matrix4 &m);
Matrix4 rotateMesh(Rotation *r, Matrix4 &m);

Matrix4 applyAllTransformations(int num_transformations,
                                std::vector<char> &transformation_types,
                                std::vector<int> &transformation_ids,
                                std::vector<Translation *> &translations,
                                std::vector<Scaling *> &scalings,
                                std::vector<Rotation *> &rotations);

double min_of_three(double a, double b, double c);
double degreesToRadians(double degrees);
Matrix4 transposeMatrix(Matrix4 &m);
Matrix4 rotationMatrixAlongX(double angle);
} // namespace MeshTransformations
