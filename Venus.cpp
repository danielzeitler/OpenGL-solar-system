#include "SolarSystem.h"

#include "Shader.h"

#include "Texture.h"

#include "Sphere.h"

#include "Venus.h"

Venus::Venus() {
    // init rotation
    g_rotation = glm::angleAxis(-glm::half_pi<float>(), vec3(1,0,0));

    initAll();
}

void Venus::initAll() {
    initTextures();
    initShader();
    initUniforms();
    initVAO();
}

void Venus::initUniforms() {

    g_uniformMVP = glGetUniformLocation(g_shader, "u_mvp");
    g_uniformModelMatrix = glGetUniformLocation(g_shader, "u_modelMatrix");

    g_uniformLightPosition = glGetUniformLocation(g_shader, "u_lightPosition");
    g_uniformCameraPosition = glGetUniformLocation(g_shader, "u_cameraPosition");

    g_uniformMaterialEmissive = glGetUniformLocation(g_shader, "u_materialEmissive");
    g_uniformMaterialDiffuse = glGetUniformLocation(g_shader, "u_materialDiffuse");
    g_uniformMaterialSpecular = glGetUniformLocation(g_shader, "u_materialSpecular");
    g_uniformMaterialShininess = glGetUniformLocation(g_shader, "u_materialShininess");

    g_uniformAmbient = glGetUniformLocation(g_shader, "u_ambient");

    g_uniformLightColor = glGetUniformLocation(g_shader, "u_lightColor");

    g_uniformUTime = glGetUniformLocation(g_shader, "u_time");

    g_uniformDiffuseMap = glGetUniformLocation(g_shader, "mapSurface");
    g_uniformNormalMap = glGetUniformLocation(g_shader, "mapNormal");
}

void Venus::initVAO() {

    // get shader inputs
    GLuint positionAttribID = (GLuint) glGetAttribLocation(g_shader, "in_position");
    GLuint textcoordAttribID = (GLuint) glGetAttribLocation(g_shader, "in_texcoord");
    GLuint normalAttribID = (GLuint) glGetAttribLocation(g_shader, "in_normal");

    // Create a VAO for the sphere
    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);

    // Gen buffers
    GLuint vbos[4];
    glGenBuffers(4, vbos);

    // bind and config buffers for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, g_sphere->positions.size() * sizeof(glm::vec3), g_sphere->positions.data(), GL_STATIC_DRAW );
    glVertexAttribPointer(positionAttribID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(positionAttribID);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, g_sphere->normals.size() * sizeof(glm::vec3), g_sphere->normals.data(), GL_STATIC_DRAW );
    glVertexAttribPointer(normalAttribID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normalAttribID);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
    glBufferData(GL_ARRAY_BUFFER, g_sphere->textureCoords.size() * sizeof(glm::vec2), g_sphere->textureCoords.data(), GL_STATIC_DRAW );
    glVertexAttribPointer(textcoordAttribID, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(textcoordAttribID);

    // bind buffers for index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_sphere->indices.size() * sizeof(GLuint), g_sphere->indices.data(), GL_STATIC_DRAW );


    // Clean up
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(positionAttribID);
    glDisableVertexAttribArray(textcoordAttribID);
    glDisableVertexAttribArray(normalAttribID);

    // cout
    std::cout << planetName << ": VAO + VBO initialized.\n";
}

void Venus::initShader() {

    // Load simple shaders.
    GLuint vertexShader = Shader::LoadShader( GL_VERTEX_SHADER, vertexShaderSource );
    GLuint fragmentShader = Shader::LoadShader( GL_FRAGMENT_SHADER, fragmentShaderSource );

    // Vector for shaders
    std::vector<GLuint> shaders;
    shaders.push_back(vertexShader);
    shaders.push_back(fragmentShader);

    // Create the shader program.
    g_shader = Shader::CreateShaderProgram(shaders);
    assert( g_shader != 0 );

    std::cout << planetName << ": Shader initalized.\n";
}

void Venus::initTextures() {

    g_diffuseMap = Texture::LoadTexture(diffuseMap, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

    std::cout << planetName << ": Textures initialized.\n";
}

void Venus::update(float tpf) {

    glm::vec3 axis_y = glm::normalize(glm::vec3(0,1,0) * glm::toMat3(g_rotation));
    glm::quat rot_y = glm::angleAxis<float>(0.0003f * tpf, axis_y);

    g_rotation = g_rotation * rot_y;
    // g_rotation = g_rotation;
}

void Venus::draw(float tpf, float time) {

    // bind vao
    glBindVertexArray( g_vao );

    // bind shaderprogram
    glUseProgram( g_shader );

    // Map Texture-Units
    glUniform1i(g_uniformDiffuseMap, 0);
    glUniform1i(g_uniformNormalMap, 1);

    // Activate and bind TU
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_normalMap);

//---------------------------------------------------

    glm::mat4 model_translation = glm::translate(this->g_position);
    glm::mat4 model_scale = glm::scale(this->g_scale);

    // glm::mat4 model_rotation = glm::toMat4(QUAT_IDENTITY);
    glm::mat4 model_rotation = glm::toMat4(g_rotation);
    glm::mat4 g_modelMatrix = model_translation * model_rotation * model_scale;

    glm::mat4 mvp = g_projectionMatrix * g_viewMatrix * g_modelMatrix;

//----------------------------------------------

    glUniformMatrix4fv( g_uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv( g_uniformModelMatrix, 1, GL_FALSE, glm::value_ptr(g_modelMatrix));
    glUniform3fv( g_uniformCameraPosition, 1, glm::value_ptr(g_cameraPosition));
    glUniform3fv( g_uniformLightPosition, 1, glm::value_ptr(g_sunPosition));


    glUniform4fv( g_uniformLightColor, 1, glm::value_ptr(g_lightColor));
    glUniform4fv( g_uniformAmbient, 1, glm::value_ptr(g_ambientColor));
    glUniform4fv( g_uniformMaterialEmissive, 1, glm::value_ptr(g_emissiveColor));
    glUniform4fv( g_uniformMaterialDiffuse, 1, glm::value_ptr(g_diffuseColor));
    glUniform4fv( g_uniformMaterialSpecular, 1, glm::value_ptr(g_specularColor));
    glUniform1f( g_uniformMaterialShininess, g_shininess);

//----------------------------------------------

    glUniform1f( g_uniformUTime, time);

//----------------------------------------------

    glDrawElements(GL_TRIANGLES, g_sphere->indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

//----------------------------------------------

    // clean up
    glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Venus::setGViewMatrix(const mat4 &gViewMatrix) {
    g_viewMatrix = gViewMatrix;
}

void Venus::setGProjectionMatrix(const mat4 &gProjectionMatrix) {
    g_projectionMatrix = gProjectionMatrix;
}

void Venus::setGSunPosition(const vec3 &gSunPosition) {
    g_sunPosition = gSunPosition;
}

void Venus::setGCameraPosition(const vec3 &gCameraPosition) {
    g_cameraPosition = gCameraPosition;
}

void Venus::setGLightColor(const vec4 &gLightColor) {
    g_lightColor = gLightColor;
}
