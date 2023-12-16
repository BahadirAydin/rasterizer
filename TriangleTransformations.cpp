#include "TriangleTransformations.h"
#include "Helpers.h"
#include "MeshTransformations.h"

Vec4 TriangleTransformations::transformVertex(Vec4 *v, Matrix4 &m) {
    return multiplyMatrixWithVec4(m, *v);
}

Color multiplyColorWithScalar(const Color c, double scalar) {
    return Color(c.r * scalar, c.g * scalar, c.b * scalar);
}

Color addColors(const Color c1, const Color c2) {
    return Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
}

std::vector<Vec4> TriangleTransformations::transformTriangle(
    const Triangle &t, const Matrix4 &transformation_matrix,
    const std::vector<Vec3 *> &vertices) {
    int v0_id = t.vertexIds[0];
    int v1_id = t.vertexIds[1];
    int v2_id = t.vertexIds[2];
    Vec3 *v0 = vertices[v0_id];
    Vec3 *v1 = vertices[v1_id];
    Vec3 *v2 = vertices[v2_id];

    Vec4 v0_4(v0->x, v0->y, v0->z, 1);
    Vec4 v1_4(v1->x, v1->y, v1->z, 1);
    Vec4 v2_4(v2->x, v2->y, v2->z, 1);

    Vec4 v0_transformed = multiplyMatrixWithVec4(transformation_matrix, v0_4);
    Vec4 v1_transformed = multiplyMatrixWithVec4(transformation_matrix, v1_4);
    Vec4 v2_transformed = multiplyMatrixWithVec4(transformation_matrix, v2_4);

    return std::vector<Vec4>{v0_transformed, v1_transformed, v2_transformed};
}

double computeHelper(double x, double y, double x1, double y1, double x2,
                     double y2) {
    return x * (y1 - y2) + y * (x2 - x1) + x1 * y2 - y1 * x2;
}

void TriangleTransformations::rasterize(
    // from triangle rasterization slides
    Image &image, const std::vector<Vec4> &triangle_vertices,
    const std::vector<Color> &colors, int res_x, int res_y) {

    Vec4 v0 = triangle_vertices[0];
    Vec4 v1 = triangle_vertices[1];
    Vec4 v2 = triangle_vertices[2];

    int max_x = std::min(std::max(std::round(MeshTransformations::max_of_three(
                                      v0.y, v1.y, v2.y)),
                                  0.0),
                         (double)res_x - 1);
    int max_y = std::min(std::max(std::round(MeshTransformations::max_of_three(
                                      v0.x, v1.x, v2.x)),
                                  0.0),
                         (double)res_y - 1);
    int min_x = std::min(std::max(std::round(MeshTransformations::min_of_three(
                                      v0.y, v1.y, v2.y)),
                                  0.0),
                         (double)res_x - 1);
    int min_y = std::min(std::max(std::round(MeshTransformations::min_of_three(
                                      v0.x, v1.x, v2.x)),
                                  0.0),
                         (double)res_y - 1);

    for (int x = min_x; x <= max_x; x++) {
        for (int y = min_y; y <= max_y; y++) {
            // f12
            double alpha = computeHelper(x, y, v1.x, v1.y, v2.x, v2.y) /
                           computeHelper(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y);
            // f20
            double beta = computeHelper(x, y, v2.x, v2.y, v0.x, v0.y) /
                          computeHelper(v1.x, v1.y, v2.x, v2.y, v0.x, v0.y);
            // f01
            double gamma = computeHelper(x, y, v0.x, v0.y, v1.x, v1.y) /
                           computeHelper(v2.x, v2.y, v0.x, v0.y, v1.x, v1.y);
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                image[x][y] = multiplyColorWithScalar(colors[0], alpha);
                image[x][y] = addColors(
                    multiplyColorWithScalar(colors[1], beta), image[x][y]);
                image[x][y] = addColors(
                    multiplyColorWithScalar(colors[2], gamma), image[x][y]);
            }
        }
    }
}
