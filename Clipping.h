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
#include "TriangleTransformations.h"
#include "Vec4.h"

struct Line {
    Vec4 a, b;
    Color aColor, bColor;
};

namespace Clipping {
bool clipLine(Line &line);
void rasterize(Image &image, Line &line);
} // namespace Clipping
