//
//  Scene.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include "scene.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "primitive.hpp"
#include "mesh.hpp"
#include "Phong.hpp"
#include "AreaLight.hpp"

#include <iostream>
#include <set>
#include <vector>

using namespace tinyobj;



static void PrintInfo (const ObjReader myObj) {

    const tinyobj::attrib_t attrib = myObj.GetAttrib();
    const std::vector<tinyobj::shape_t> shapes = myObj.GetShapes();
    const std::vector<tinyobj::material_t> materials = myObj.GetMaterials();

    std::cout << std::endl;
    std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
    for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
        printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
    }

    std::cout << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
    std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2) << std::endl;
    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].mesh.indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));
    }
    // iterate shapes
    auto it_shape = shapes.begin();
    for (; it_shape != shapes.end(); it_shape++) {
        // assume each face has 3 vertices
        std::cout << "Processing shape " << it_shape->name << std::endl;
        
        // iterate faces
        auto it_vertex = it_shape->mesh.indices.begin();
        
        for (; it_vertex != it_shape->mesh.indices.end();) {
            // process 3 vertices
            for (int v=0 ; v<3 ; v++) {
                std::cout << it_vertex->vertex_index << " ";
                it_vertex++;
            }
            std::cout << std::endl;
        }
        std::cout << "There are " << it_shape->mesh.material_ids.size() << " material indexes\n" << std::endl;
    }
}



/*
 Use tiny load to load .obj scene descriptions
 https://github.com/tinyobjloader/tinyobjloader
*/

bool Scene::Load (const std::string &fname) {
    ObjReader myObjReader;

    if (!myObjReader.ParseFromFile(fname)) {
        return false;
    }
    
    // PrintInfo(myObjReader);
    const tinyobj::attrib_t attrib = myObjReader.GetAttrib();
    const std::vector<shape_t> shapes = myObjReader.GetShapes();
    const std::vector<material_t> materials = myObjReader.GetMaterials();


    for (size_t i = 0; i < attrib.vertices.size() / 3; ++i){
        this->vertices.push_back(Point(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]));
    }
    for (size_t i = 0; i < attrib.normals.size() / 3; ++i){
        this->normals.push_back(Vector(attrib.normals[3 * i], attrib.normals[3 * i + 1], attrib.normals[3 * i + 2]));
    }

    this->numPrimitives = shapes.size();
    for (size_t s = 0; s < this->numPrimitives; s++){
        size_t index_offset = 0;        
        int numFaces = 0;
        int numVerticesMesh = static_cast<int>(shapes[s].mesh.indices.size()) / 3;
        int numNormals = 0;
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(numFaces, numVerticesMesh, numNormals);
        Primitive primitive;
        int material_id;

        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++){
            numFaces++;
            Face face;
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            material_id = shapes[s].mesh.material_ids[f];
            
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                face.vert_ndx.push_back(idx.vertex_index);
                
                if (idx.normal_index >= 0) {
                    numNormals++;
                    face.vert_normals_ndx.push_back(idx.normal_index);
                }
            }
                  
            Point p0 = vertices.at(face.vert_ndx[0]);
            Point p1 = vertices.at(face.vert_ndx[1]);
            Point p2 = vertices.at(face.vert_ndx[2]);
            Vector e0 = p0.vec2point(p1);
            Vector e1 = p0.vec2point(p2);
            Vector gn = e0.cross(e1);

            gn.normalize();
            face.geoNormal = gn;
            face.hasShadingNormals = numNormals != 0 ? true : false;
            mesh->faces.push_back(face);
            index_offset += fv;
        }

        mesh->numFaces = numFaces;
        mesh->updateBBMesh(this->vertices); 

        primitive.g = mesh;
        primitive.material_ndx = material_id;
        this->prims.push_back(primitive);
    }


    for(auto& material : materials) {
        OurRGB Ka = OurRGB(material.ambient[0], material.ambient[1], material.ambient[2]);
        OurRGB Kd = OurRGB(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        OurRGB Ks = OurRGB(material.specular[0], material.specular[1], material.specular[2]);
        OurRGB Kt = OurRGB(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
        float Ns = material.shininess;

        auto phong = std::make_shared<Phong>(Ka, Kd, Ks, Kt, Ns);
        this->BRDFs.push_back(phong);
        this->numBRDFs++;
    }

    return true;
}



void Scene::print(){
    std::cout << "#primitives = " << numPrimitives << " ; ";
    std::cout << "#primitives Array = " << prims.size() << " ; ";
    for(auto i=0; i<prims.size(); ++i){
        auto mesh = dynamic_cast<Mesh *>(prims[i].g.get());
        std::cout << "#NUMFACES  = " << mesh->numFaces << " ; ";
        std::cout << "#NUMFACESVEC  = " << mesh->faces.size() << " ; ";
        for(auto j =0; j < mesh->faces.size(); ++j){
            for(int z = 0; z < 3; z++){
                Point p =  this->vertices.at(mesh->faces[j].vert_ndx[z]);
                std::cout << "#VERT X  = " << p.X << " ; ";
            }
        }
    }
}



bool Scene::trace (Ray r, Intersection *isect) {
    Intersection curr_isect;
    bool intersection = false;    
    
    if (numPrimitives==0) return false;
    
    // iterate over all primitives
    for (auto prim_itr = 0 ; prim_itr < prims.size() ; prim_itr++) {
        auto mesh = static_cast<Mesh *>(prims[prim_itr].g.get());
        
        if (mesh->intersect(this->vertices, r, &curr_isect)) {
            if (!intersection) { // first intersection
                intersection = true;
                *isect = curr_isect;
                isect->f = BRDFs[prims[prim_itr].material_ndx].get();
            }
            else if (curr_isect.depth < isect->depth) {
                *isect = curr_isect;
                isect->f = BRDFs[prims[prim_itr].material_ndx].get();
            }
        }
    }

    isect->isLight = false; // download new intersection.hpp
    // now iterate over light sources and intersect with those that have geometry
    for (auto l = lights.begin(); l != lights.end(); l++) {
        if ((*l)->type == AREA_LIGHT) {
            AreaLight *al = (AreaLight*) *l;
            
            if (al->gem->intersect(r, &curr_isect)) {
                if (!intersection) { // first intersection
                    intersection = true;
                    *isect = curr_isect;
                    isect->isLight = true;
                    isect->Le = al->L();
                    isect->power = al->power;
                }
                else if (curr_isect.depth < isect->depth) {
                    *isect = curr_isect;
                    isect->isLight = true;
                    isect->Le = al->L();
                    isect->power = al->power;
                }
            }
        }
    }

    return intersection;
}



// checks whether a point on a light source (distance maxL) is visible
bool Scene::visibility (Ray s, const float maxL) {
    bool visible = true;
    Intersection curr_isect;
    
    if (numPrimitives==0) return true;
    
    // iterate over all primitives while visible
    for (auto prim_itr = 0 ; prim_itr < prims.size() && visible ; prim_itr++) {
        auto mesh = static_cast<Mesh *>(prims[prim_itr].g.get());

        if (mesh->intersect(this->vertices, s, &curr_isect))
            if (curr_isect.depth < maxL)
                visible = false;
    }
    return visible;
}