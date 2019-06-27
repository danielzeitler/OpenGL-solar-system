#ifndef __SPHERE_H__
#define __SPHERE_H__

using namespace glm;
using namespace std;

class Sphere {

public:

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> textureCoords;

    vector<GLuint> indices;

    const float PI = 3.1415926535897932384626433832795f;
    const float _2PI = 2.0f * PI;

    Sphere( float radius, int slices, int stacks );

};


#endif //__SPHERE_H__
