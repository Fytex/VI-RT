//
//  AABB.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//


#include "AABB.hpp"

#include <algorithm>
#include <utility>
#include <limits>

using namespace std;


// Algorithm explained: https://www.youtube.com/watch?v=USjbg5QXk3g
bool BB::intersect (Ray ray) {
    float tmin = 0;
    float tmax = std::numeric_limits<float>::infinity();
    
    // X axis //
    float txmin = (this->min.X - ray.o.X) / ray.dir.X;
    float txmax = (this->max.X - ray.o.X) / ray.dir.X;
    if (txmin > txmax) std::swap(txmin, txmax);

    if ((tmin > txmax) || (txmin > tmax)) return false;

    if (txmin > tmin) tmin = txmin;
    if (txmax < tmax) tmax = txmax;

    // Y axis //
    float tymin = (this->min.Y - ray.o.Y) / ray.dir.Y;
    float tymax = (this->max.Y - ray.o.Y) / ray.dir.Y;
    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax)) return false;

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    // Z axis //
    float tzmin = (this->min.Z - ray.o.Z) / ray.dir.Z;
    float tzmax = (this->max.Z - ray.o.Z) / ray.dir.Z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    return true;
}