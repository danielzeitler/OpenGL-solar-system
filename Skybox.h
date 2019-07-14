#ifndef _SKYBOX_H_
#define _SKYBOX_H_


class Skybox {


public:

    Skybox();

    void draw();

    void initAll();

    //-----------------------------------------------------
    // Getter + Setter

    void setGViewMatrix(const glm::mat4 &gViewMatrix);

    void setGProjectionMatrix(const glm::mat4 &gProjectionMatrix);

private:

    glm::mat4 g_viewMatrix, g_projectionMatrix, g_modelMatrix;

    //-----------------------------------------------------

    GLuint g_ShaderProgram;

    GLuint g_Texture;

    GLuint g_uniformMVP;

    GLuint g_vaoSkybox;

    //-----------------------------------------------------

    void initVAO();

    void initShader();

    void initTexture();

// Our Vertex-Struct (pos and color)
    struct VertexXYZColor {
        glm::vec3 m_Pos;
        glm::vec3 m_Color;
    };

// Define the 8 vertices of a unit cube
    VertexXYZColor g_Vertices[8] = {
            { glm::vec3(  1,  1,  1 ), glm::vec3( 1, 1, 1 ) }, // 0
            { glm::vec3( -1,  1,  1 ), glm::vec3( 0, 1, 1 ) }, // 1
            { glm::vec3( -1, -1,  1 ), glm::vec3( 0, 0, 1 ) }, // 2
            { glm::vec3(  1, -1,  1 ), glm::vec3( 1, 0, 1 ) }, // 3
            { glm::vec3(  1, -1, -1 ), glm::vec3( 1, 0, 0 ) }, // 4
            { glm::vec3( -1, -1, -1 ), glm::vec3( 0, 0, 0 ) }, // 5
            { glm::vec3( -1,  1, -1 ), glm::vec3( 0, 1, 0 ) }, // 6
            { glm::vec3(  1,  1, -1 ), glm::vec3( 1, 1, 0 ) }, // 7
    };

// Define the vertex indices for the cube.
// Each set of 6 vertices represents a set of triangles in
// counter-clockwise winding order.
    GLuint g_Indices[36] = {
            0, 1, 2, 2, 3, 0,           // Front face
            7, 4, 5, 5, 6, 7,           // Back face
            6, 5, 2, 2, 1, 6,           // Left face
            7, 0, 3, 3, 4, 7,           // Right face
            7, 6, 1, 1, 0, 7,           // Top face
            3, 2, 5, 5, 4, 3            // Bottom face
    };
};


#endif //_SKYBOX_H_
