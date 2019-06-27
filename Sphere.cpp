#include "SolarSystem.h"
#include "Sphere.h"


Sphere::Sphere( float radius, int slices, int stacks ) {

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float u, v;                                     // vertex texCoord

    float sectorStep = 2 * PI / slices;
    float stackStep = PI / stacks;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stacks; ++i) {

        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= slices; ++j) {

            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            positions.push_back(vec3(x,y,z));

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(vec3(nx,ny,nz));

            // vertex tex coord (u, v) range between [0, 1]
            u = (float)j / slices;
            v = (float)i / stacks;
            textureCoords.push_back(vec2(u,v));

        }
    }


    // indices
    int k1, k2;
    for(int i = 0; i < stacks; ++i) {

        k1 = i * (slices + 1);     // beginning of current stack
        k2 = k1 + slices + 1;      // beginning of next stack

        for(int j = 0; j < slices; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

            }

            // k1+1 => k2 => k2+1
            if(i != (stacks-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);

            }
        }
    }

}
