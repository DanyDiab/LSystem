#pragma once

#include <vector>

namespace LSystem {
    class Geometry {
    public:
        static std::vector<float> generateCylinder(int numSegments, float height, float radius);
    };
}
