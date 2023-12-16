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

namespace TriangleTransformations {
Vec4 transformVertex(Vec4 *v, Matrix4 &m);
std::vector<Vec4> transformTriangle(const Triangle &t, const Matrix4 &m,
                                    const std::vector<Vec3 *> &vertices);
} // namespace TriangleTransformations
