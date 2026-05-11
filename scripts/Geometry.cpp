#include "./headers/Geometry.hpp"
#include <cmath>
#include <numbers>

namespace LSystem {

    namespace {
        std::vector<float> generateCirclePoints(int numPoints, float zOffset, float radius) {
            std::vector<float> circlePoints;
            float stepSize = ((2.0f * M_PI) / numPoints);

            for(int i = 0; i < numPoints; i++) {
                float x = cos(i * stepSize) * radius;
                float y = sin(i * stepSize) * radius; 
                
                float z = zOffset;

                circlePoints.push_back(x);
                circlePoints.push_back(y);
                circlePoints.push_back(z);
            }

            if (!circlePoints.empty()) {
                circlePoints.push_back(circlePoints.at(0));
                circlePoints.push_back(circlePoints.at(1));
                circlePoints.push_back(circlePoints.at(2));
            }

            return circlePoints;
        }
    }

    std::vector<float> Geometry::generateCylinder(int numSegments, float length, float radius) {
        if (numSegments < 3) {
            return {};
        }

        std::vector<float> circle1 = generateCirclePoints(numSegments, 0.0f, radius);
        
        std::vector<float> circle2 = generateCirclePoints(numSegments, length, radius);

        std::vector<float> cylinderPoints;
        int p1 = 0;
        int p2 = 0;
        
        for(size_t i = 0; i < circle1.size() / 3; i++) {
            cylinderPoints.push_back(circle1.at(p1));
            cylinderPoints.push_back(circle1.at(p1 + 1));
            cylinderPoints.push_back(circle1.at(p1 + 2));

            cylinderPoints.push_back(circle2.at(p2));
            cylinderPoints.push_back(circle2.at(p2 + 1));
            cylinderPoints.push_back(circle2.at(p2 + 2));

            p1 += 3;
            p2 += 3;
        }

        return cylinderPoints;
    }
}