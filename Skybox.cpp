
#include "SolarSystem.h"

#include "Shader.h"

#include "Texture.h"

#include "Skybox.h"

Skybox::Skybox() {

}

void Skybox::initAll() {

    initTexture();
    initShader();
    initVAO();

}

void Skybox::initTexture() {

    // Load Cubemap
    g_Texture = Texture::LoadSkybox("../../res/textures/skybox/",".png");

    std::cout << "SKYBOX: Texture initialized.\n";

}

void Skybox::initShader() {

    // Load simple shaders.
    GLuint vertexShader = Shader::LoadShader( GL_VERTEX_SHADER, "../../res/shader/skybox.vert" );
    GLuint fragmentShader = Shader::LoadShader( GL_FRAGMENT_SHADER, "../../res/shader/skybox.frag" );

    // Vector for shaders
    std::vector<GLuint> shaders;
    shaders.push_back(vertexShader);
    shaders.push_back(fragmentShader);

    // Create the shader program.
    g_ShaderProgram = Shader::CreateShaderProgram(shaders);
    assert( g_ShaderProgram != 0 );

    std::cout << "SKYBOX: Shader initalized.\n";

}

void Skybox::initVAO() {

    // Get attrib and uniform locations from compiled shader
    GLuint positionAttribID = (GLuint) glGetAttribLocation( g_ShaderProgram, "in_position" );
    GLuint colorAttribID = (GLuint) glGetAttribLocation( g_ShaderProgram, "in_color" );

    g_uniformMVP = glGetUniformLocation( g_ShaderProgram, "u_mvp" );

    // Create a VAO for the cube.
    glGenVertexArrays( 1, &g_vaoSkybox );
    glBindVertexArray( g_vaoSkybox );

    // Create the Index buffer
    GLuint vertexBuffer, indexBuffer;
    glGenBuffers( 1, &vertexBuffer );
    glGenBuffers( 1, &indexBuffer );

    // Bind Vertex Buffer
    glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(g_Vertices), g_Vertices, GL_STATIC_DRAW );

    // Bind Index Buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(g_Indices), g_Indices, GL_STATIC_DRAW );

    // Init the running pointer for the vertex pos
    glVertexAttribPointer( positionAttribID, 3, GL_FLOAT, GL_FALSE, sizeof(VertexXYZColor), MEMBER_OFFSET(VertexXYZColor,m_Pos) );
    glEnableVertexAttribArray( positionAttribID );

    // Init the running pointer for the vertex color
    glVertexAttribPointer( colorAttribID, 3, GL_FLOAT, GL_FALSE, sizeof(VertexXYZColor), MEMBER_OFFSET(VertexXYZColor,m_Color) );
    glEnableVertexAttribArray( colorAttribID );

    // Make sure we disable and unbind everything to prevent rendering issues later.
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glDisableVertexAttribArray( positionAttribID );
    glDisableVertexAttribArray( colorAttribID );


    std::cout << "SKYBOX: VAO + VBO initialized.\n";

}

void Skybox::draw() {

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Clear bg
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind things
    glBindVertexArray( g_vaoSkybox );
    glUseProgram( g_ShaderProgram );


    // Activate texture for cubemap
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, g_Texture );


    g_modelMatrix = MAT4_IDENTITY;

    g_modelMatrix = glm::scale(glm::vec3(300.0));

    g_viewMatrix = glm::mat4(glm::mat3(g_viewMatrix));

    glm::mat4 mvp = g_projectionMatrix * g_viewMatrix * g_modelMatrix;

    glUniformMatrix4fv( g_uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp) );

    glDrawElements( GL_TRIANGLES, sizeof(g_Indices), GL_UNSIGNED_INT, BUFFER_OFFSET(0) );

    // Unbind shaders & VAOs & Textures
    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}



void Skybox::setGViewMatrix(const glm::mat4 &gViewMatrix) {
    g_viewMatrix = gViewMatrix;
}



void Skybox::setGProjectionMatrix(const glm::mat4 &gProjectionMatrix) {
    g_projectionMatrix = gProjectionMatrix;
}
