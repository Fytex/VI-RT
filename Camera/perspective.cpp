//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//

#include "perspective.hpp"

bool Perspective::GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter) {
    float xs;
    float ys;

    if (cam_jitter == NULL) {
        xs = 2.f * ((float)x + .5f)/W - 1.f;
        ys = 2.f * ((float)(H-y-1) + .5f)/H - 1.f;
    } else {
        xs = 2.f * ((float)x + cam_jitter[0])/W - 1.f;
        ys = 2.f * ((float)(H-y-1) + cam_jitter[1])/H - 1.f;
    }

    float xc = xs * tanf(this->fovW / 2.0f);
    float yc = ys * tanf(this->fovH / 2.0f);

    r->o =  this->Eye;
    r->dir = Vector(this->c2w[0][0] * xc  + this->c2w[0][1] * yc + this->c2w[0][2] *  1 ,
                    this->c2w[1][0] * xc  + this->c2w[1][1] * yc + this->c2w[1][2] *  1 ,
                    this->c2w[2][0] * xc  + this->c2w[2][1] * yc + this->c2w[2][2] *  1);
    r->dir.normalize();

    return true;
}
