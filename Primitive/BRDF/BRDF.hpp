//
//  BRDF.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//


#ifndef BRDF_hpp
#define BRDF_hpp

#include "vector.hpp"
#include "RGB.hpp"


typedef enum {
    SPECULAR_REF=1,
    DIFFUSE_REF=2,
    SPECULAR_TRANS=4,
    GLOSSY_REF=8,
    BRDF_ALL= SPECULAR_REF | DIFFUSE_REF | SPECULAR_TRANS | GLOSSY_REF
} BRDF_TYPES;

class BRDF {
public:
    BRDF () {}
    ~BRDF () {}
    // return the BRDF RGB value for a pair of (incident, scattering) directions : (wi,wo)
    virtual OurRGB f (Vector wi, Vector wo, const BRDF_TYPES = BRDF_ALL) {return OurRGB();}
    // return an outgoing direction wo and brdf RGB value for a given wi and probability pair prob[2]
    virtual OurRGB Sample_f (Vector wi, float *prob, Vector *wo, const BRDF_TYPES = BRDF_ALL) {return OurRGB();}
    // return the probability of sampling wo given wi
    virtual float pdf(Vector wi, Vector wo, const BRDF_TYPES = BRDF_ALL) {return 0.;}
};

#endif /* BRDF_hpp */
