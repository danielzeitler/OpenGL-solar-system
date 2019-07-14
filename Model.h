#ifndef _MODEL_H_
#define _MODEL_H_

class Model {

public:
    struct ModelOptions {
        float light_scale = 1.0f;
        float ambient_scale = 1.0f;
        float emissive_scale = 1.0f;
        float diffuse_scale = 1.0f;
        float specular_scale = 1.0f;
        float shininess_scale = 1.0f;

        glm::vec3 model_scale = glm::vec3(1.0);
        glm::vec3 model_position = glm::vec3(0.0);
        glm::quat model_rotation = QUAT_IDENTITY;

        bool normalmap_unsigned = true;
        bool normalmap_2channel = false;
        bool normalmap_greenup = false;
    };

    //----------------------------------------------------------------------

    Model(std::string filename, std::string filepath, ModelOptions options);

    //----------------------------------------------------------------------

    void draw();

    //----------------------------------------------------------------------

    void setGViewMatrix(const glm::mat4 &gViewMatrix);
    void setGProjectionMatrix(const glm::mat4 &gProjectionMatrix);
    void setGRotation(const glm::quat &gRotation);
    void setGScale(const glm::vec3 &gScale);
    void setGPosition(const glm::vec3 &gPosition);
    void setGSunPosition(const glm::vec3 &gSunPosition);
    void setGCameraPosition(const glm::vec3 &gCameraPosition);
    void setGLightColor(const glm::vec3 &gLightColor);

    const glm::quat &getGRotation() const;

private:

    std::string g_filename;
    std::string g_filepath;

    glm::vec3 g_scale;
    glm::vec3 g_position;
    glm::quat g_rotation;

    float g_light_scale;
    float g_ambient_scale;
    float g_emmisive_scale;
    float g_diffuse_scale;
    float g_specular_scale;
    float g_shininess_scale;

    bool g_normalmap_unsigned;
    bool g_normalmap_2channel;
    bool g_normalmap_greenup;

    //----------------------------------------------------------------------

    void loadModel();

    void initVAO();

    void initShader();

    void initTextures();

    void initAll();

    void initUniforms();

    //----------------------------------------------------------------------

    //TODO: add default values for material!!

    struct ShapeMaterial {
        std::string name; // material name [newmtl]
        glm::vec3 ambient; // ambient reflectivity [Ka]
        glm::vec3 emissive; // emmisive reflectivity [Ke]
        glm::vec3 diffuse; // diffuse reflectivity [Kd]
        glm::vec3 specular; // specular reflectivity [Ks]

        float refraction; // optical_density (index of refraction) [Ni]
        float shininess; // specular exponent (shininess) [Ns]
        float dissolve; // material dissolve [d]
        int illum; // illumination model (0..10) (see http://www.fileformat.info/format/material/)

        std::string mapDiffuseFile; // diffuse texture g_filename [map_Kd]
        std::string mapBumpFile; // normal texture g_filename [map_Bump, map_bump, bump]
        std::string mapNormalFile; // normal texture g_filename [norm]
        std::string mapSpecularFile; // specular texture g_filename [map_Ks]

        // TODO: other textures & options

        // TODO: PBR extension
        // http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr

    };

    struct ShapeObject {
        std::string name;
        GLuint vao;
        std::vector<float> buffer; // pos(3*float), normal(3*float), uv(2*float)
        int vertexCount;
        int materialId;
    };

    //----------------------------------------------------------------------

    std::vector<ShapeMaterial> materials;
    std::vector<ShapeObject> shapes;
    std::map<std::string, GLuint> textures;

    //-------------------------------------------------------

    int g_numVertices = 0;
    int g_numTriangles = 0;

    glm::vec3 g_minVertex = glm::vec3(0);
    glm::vec3 g_maxVertex = glm::vec3(0);

    glm::vec3 g_realScale;
    float g_normScaleFactor;
    glm::vec3 g_realPosition;

    //-----------------------------------------------------------------------

    glm::vec3 g_sunPosition;
    glm::vec3 g_cameraPosition;

    glm::vec3 g_lightColor;

    //-----------------------------------------------------------------------

    GLuint g_shader;

    glm::mat4 g_viewMatrix, g_projectionMatrix, g_modelMatrix;

    //--------------------------------------------------------------------------------

    GLuint g_uniformMVP;
    GLuint g_uniformModelMatrix;

    GLuint g_uniformDiffuseMap;
    GLuint g_uniformSpecularMap;
    GLuint g_uniformNormalMap;

    GLuint  g_uniformSunPosition;
    GLuint  g_uniformCameraPosition;

    GLuint  g_uniformLightColor;

    GLuint  g_uniformAmbientColor;
    GLuint  g_uniformEmissiveColor;
    GLuint  g_uniformDiffuseColor;
    GLuint  g_uniformSpecularColor;
    GLuint  g_uniformShininess;

    GLuint g_uniformTextureFlags;
    GLuint g_uniformNormalmapFlags;

    //------------------------------------------

};


#endif //_MODEL_H_
