#include "Clipping.h"
#include <iostream>

bool is_visible(double den, double num, double &te, double &tl) {
    // from slides
    if (den > 0) {
        // potentially entering
        double t = num / den;
        if (t > tl) {
            return false;
        }
        if (t > te) {
            te = t;
        }
    } else if (den < 0) {
        // potentially leaving
        double t = num / den;
        if (t < te) {
            return false;
        }
        if (t < tl) {
            tl = t;
        }
    } else if (num > 0) {
        // line parallel to edge
        return false;
    }
    return true;
}

bool Clipping::clipLine(Line &line) {
    // liang barsky
    double dx = line.b.x - line.a.x;
    double dy = line.b.y - line.a.y;
    double dz = line.b.z - line.a.z;

    Color dc =
        Color(line.bColor.r - line.aColor.r, line.bColor.g - line.aColor.g,
              line.bColor.b - line.aColor.b);
    double te = 0;
    double tl = 1;
    if (is_visible(dx, -1 - line.a.x, te, tl)) {
        if (is_visible(-dx, line.a.x - 1, te, tl)) {
            if (is_visible(dy, -1 - line.a.y, te, tl)) {
                if (is_visible(-dy, line.a.y - 1, te, tl)) {
                    if (is_visible(dz, -1 - line.a.z, te, tl)) {
                        if (is_visible(-dz, line.a.z - 1, te, tl)) {
                            if (te > 0) {
                                line.a.x = line.a.x + te * dx;
                                line.a.y = line.a.y + te * dy;
                                line.a.z = line.a.z + te * dz;
                                line.aColor.r = line.aColor.r + te * dc.r;
                                line.aColor.g = line.aColor.g + te * dc.g;
                                line.aColor.b = line.aColor.b + te * dc.b;
                            }
                            if (tl < 1) {
                                line.b.x = line.a.x + tl * dx;
                                line.b.y = line.a.y + tl * dy;
                                line.b.z = line.a.z + tl * dz;
                                line.bColor.r = line.aColor.r + tl * dc.r;
                                line.bColor.g = line.aColor.g + tl * dc.g;
                                line.bColor.b = line.aColor.b + tl * dc.b;
                            }
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void swap(Line &line) {
    double temp = line.a.x;
    line.a.x = line.b.x;
    line.b.x = temp;
    temp = line.a.y;
    line.a.y = line.b.y;
    line.b.y = temp;
    temp = line.a.z;
    line.a.z = line.b.z;
    line.b.z = temp;
    Color temp2 = line.aColor;
    line.aColor = line.bColor;
    line.bColor = temp2;
}

void Clipping::rasterize(Image &image, Line &line, Depth &depth) {
    double slope = std::abs((line.b.y - line.a.y) / (line.b.x - line.a.x));
    int i = 1;
    if (slope > 0 && slope <= 1) {
        if (line.a.x > line.b.x) {
            swap(line);
        }
        if (line.a.y > line.b.y) {
            i = -1;
        }
        Color c = line.aColor;
        int y = line.a.y;
        int d = 2 * (line.a.y - line.b.y) + i * (line.b.x - line.a.x);
        Color c_change =
            Color((line.bColor.r - line.aColor.r) / (line.b.x - line.a.x),
                  (line.bColor.g - line.aColor.g) / (line.b.x - line.a.x),
                  (line.bColor.b - line.aColor.b) / (line.b.x - line.a.x));
        for (int x = line.a.x; x <= line.b.x; x++) {
            if (depth[x][y] > line.a.z) {
                depth[x][y] = line.a.z;
                image[x][y] = c;
            }
            c.r += c_change.r;
            c.g += c_change.g;
            c.b += c_change.b;
            if (i * d < 0) {
                y = y + i;
                d += 2 * ((line.a.y - line.b.y) + (line.b.x - line.a.x) * i);
            } else {
                d += 2 * (line.a.y - line.b.y);
            }
        }
    } else if (slope > 1) {
        if (line.a.y > line.b.y) {
            swap(line);
        }
        if (line.a.x > line.b.x) {
            i = -1;
        }
        Color c = line.aColor;
        int x = line.a.x;
        int d = 2 * (line.a.x - line.b.x) + i * (line.a.y - line.b.y);
        Color c_change =
            Color((line.bColor.r - line.aColor.r) / (line.b.y - line.a.y),
                  (line.bColor.g - line.aColor.g) / (line.b.y - line.a.y),
                  (line.bColor.b - line.aColor.b) / (line.b.y - line.a.y));
        for (int y = line.a.y; y <= line.b.y; y++) {
            if (depth[x][y] > line.a.z) {
                depth[x][y] = line.a.z;
                image[x][y] = c;
            }
            c.r += c_change.r;
            c.g += c_change.g;
            c.b += c_change.b;
            if (-i * d < 0) {
                x = x + i;
                d += 2 * ((line.a.x - line.b.x) + (line.a.y - line.b.y) * i);
            } else {
                d += 2 * (line.a.x - line.b.x);
            }
        }
    }
}
