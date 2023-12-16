#include "TriangleTransformations.h"
#include "Helpers.h"

Vec4 TriangleTransformations::transformVertex(Vec4 *v, Matrix4 &m) {
    return multiplyMatrixWithVec4(m, *v);
}

std::vector<Vec4>
TriangleTransformations::transformTriangle(const Triangle &t,
                                           const Matrix4 &transformation_matrix,
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
