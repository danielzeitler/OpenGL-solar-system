#ifndef _MOON_H_
#define _MOON_H_


#include "Sphere.h"

class Moon {

public:

    Moon();

    void draw(float tpf);

    void update(float tpf);

    void initAll();

    void setGViewMatrix(const glm::mat4 &gViewMatrix);
    void setGProjectionMatrix(const glm::mat4 &gProjectionMatrix);
    void setGSunPosition(const vec3 &gSunPosition);
    void setGCameraPosition(const vec3 &gCameraPosition);

    void setGLightColor(const vec4 &gLightColor);



protected:
    // TODO: make static (const)
    Sphere* g_sphere = new Sphere(1.0f, 32, 32);

private:

    glm::mat4 g_viewMatrix, g_projectionMatrix, g_modelMatrix;

    GLuint g_shader;

    GLuint g_vao;

    void initUniforms();

    void initVAO();

    void initShader();

    void initTextures();

    //-----------------------------------------------------------

    std::string planetName = "Moon";

    std::string vertexShaderSource = "../../res/shader/moon.vert";
    std::string fragmentShaderSource = "../../res/shader/moon.frag";

    //-----------------------------------------------------------

    std::string diffuseMap = "../../res/textures/moon/8k_moon.jpg";
    std::string normalMap = "../../res/textures/moon/8k_moon_normal_map.jpg";

    GLuint g_diffuseMap;
    GLuint g_normalMap;

    //-----------------------------------------------------------

    glm::vec3 g_position = glm::vec3(-1.0, 0.0, 0.0);

    glm::vec3 g_scale = glm::vec3(1.0);

    glm::quat g_rotation = QUAT_IDENTITY;

    //-----------------------------------------------------------

    //TODO: uniforms

    GLuint g_uniformMVP;
    GLuint g_uniformModelMatrix;

    GLuint g_uniformLightPosition;
    GLuint g_uniformCameraPosition;

    GLuint g_uniformMaterialEmissive;
    GLuint g_uniformMaterialDiffuse;
    GLuint g_uniformMaterialSpecular;
    GLuint g_uniformMaterialShininess;

    GLuint g_uniformAmbient;

    GLuint g_uniformLightColor;

    GLuint g_uniformDiffuseMap;
    GLuint g_uniformNormalMap;


    //-----------------------------------------------------------------------------

    glm::vec3 g_sunPosition = glm::vec3(10,10,10);

    glm::vec3 g_cameraPosition;

    glm::vec4 g_lightColor;

    glm::vec4 g_ambientColor = glm::vec4(0.1, 0.1, 0.1, 1.0);
    glm::vec4 g_emissiveColor = glm::vec4(0.0, 0.0, 0.0, 0.0);
    glm::vec4 g_diffuseColor = glm::vec4(1.0, 1.0, 1.0, 1.0);
    glm::vec4 g_specularColor = glm::vec4(1, 1, 1, 0.5);

    float g_shininess = 50.0f;

};


#endif //_MOON_H_
