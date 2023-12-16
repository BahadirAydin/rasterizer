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

typedef std::vector<std::vector<Color>> Image;
typedef std::vector<std::vector<double>> Depth;

namespace TriangleTransformations {
Vec4 transformVertex(Vec4 *v, Matrix4 &m);
std::vector<Vec4> transformTriangle(const Triangle &t, const Matrix4 &m,
                                    const std::vector<Vec3 *> &vertices);

void rasterize(Image &image, const std::vector<Vec4> &triangle_vertices,
               const std::vector<Color> &colors, int res_x, int res_y, Depth &depth);
} // namespace TriangleTransformations
