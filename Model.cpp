#include "SolarSystem.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Shader.h"

#include "Texture.h"

#include "Model.h"

//----------------------------------------------------------------------------------------------------------------------

Model::Model(std::string filename, std::string filepath, ModelOptions options) {
    g_filename = filename;
    g_filepath = filepath;

    g_scale = options.model_scale;
    g_position = options.model_position;
    g_rotation = options.model_rotation;

    g_light_scale = options.light_scale;
    g_ambient_scale = options.ambient_scale;
    g_emmisive_scale = options.emissive_scale;
    g_diffuse_scale = options.diffuse_scale;
    g_specular_scale = options.specular_scale;
    g_shininess_scale = options.shininess_scale;

    g_normalmap_unsigned = options.normalmap_unsigned;
    g_normalmap_2channel = options.normalmap_2channel;
    g_normalmap_greenup = options.normalmap_greenup;

    //------------------------------------------------------

    initAll();
}

//----------------------------------------------------------------------------------------------------------------------

static glm::vec3 genNormal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {

    return glm::normalize(glm::cross(v1 - v0, v2 - v0));

}

bool hasSmoothingGroup(const tinyobj::shape_t& shape) {
    for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) {
        if (shape.mesh.smoothing_group_ids[i] > 0) {
            return true;
        }
    }
    return false;
}

// TODO: this algo seems to be wrong!!!
// see: https://stackoverflow.com/questions/45477806/general-method-for-calculating-smooth-vertex-normals-with-100-smoothness


void computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, std::map<int, glm::vec3>& smoothVertexNormals) {
    smoothVertexNormals.clear();

    std::map<int, glm::vec3>::iterator iter;

    for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
        // Get the three indexes of the face (all faces are triangular)
        tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
        tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
        tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

        // Get the three vertex indexes and coordinates
        int vi[3];      // indexes
        float v[3][3];  // coordinates

        for (int k = 0; k < 3; k++) {
            vi[0] = idx0.vertex_index;
            vi[1] = idx1.vertex_index;
            vi[2] = idx2.vertex_index;
            assert(vi[0] >= 0);
            assert(vi[1] >= 0);
            assert(vi[2] >= 0);

            v[0][k] = attrib.vertices[3 * vi[0] + k];
            v[1][k] = attrib.vertices[3 * vi[1] + k];
            v[2][k] = attrib.vertices[3 * vi[2] + k];
        }

//--------------------------------------------------------------------------------------
        // Compute the normal of the face
        float normal[3];

        //CalcNormal(normal, v[0], v[1], v[2]);

        glm::vec3 v0 = glm::vec3(v[0][0], v[0][1], v[0][2]);
        glm::vec3 v1 = glm::vec3(v[1][0], v[1][1], v[1][2]);
        glm::vec3 v2 = glm::vec3(v[2][0], v[2][1], v[2][2]);

        glm::vec3 norm = genNormal(v0, v1, v2);

        // TODO: remove this
        normal[0] = norm[0];
        normal[1] = norm[1];
        normal[2] = norm[2];

//--------------------------------------------------------------------------------------

        // Add the normal to the three vertexes
        for (size_t i = 0; i < 3; ++i) {
            iter = smoothVertexNormals.find(vi[i]);
            if (iter != smoothVertexNormals.end()) {
                // add

               iter->second[0] += normal[0];
                iter->second[1] += normal[1];
                iter->second[2] += normal[2];

            } else {

                smoothVertexNormals[vi[i]][0] = normal[0];
                smoothVertexNormals[vi[i]][1] = normal[1];
                smoothVertexNormals[vi[i]][2] = normal[2];
            }
        }

    }  // f

    // Normalize the normals, that is, make them unit vectors
    for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end(); iter++) {

        iter->second = glm::normalize(iter->second);

    }

}  // computeSmoothingNormals

//----------------------------------------------------------------------------------------------------------------------

void Model::loadModel() {

    tinyobj::attrib_t obj_attrib;
    std::vector<tinyobj::shape_t> obj_shapes;
    std::vector<tinyobj::material_t> obj_materials;

    //----------------------------------------------------------------------------------------------------------------------

    std::string fullname = this->g_filepath + this->g_filename;

    std::string warn;
    std::string err;

    bool obj_triangulate = true;

    bool ret = tinyobj::LoadObj(&obj_attrib, &obj_shapes, &obj_materials, &warn, &err, fullname.c_str(), this->g_filepath.c_str(), obj_triangulate);

    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cout << "OBJ-LOADER: ERROR while loading Model-File " << this->g_filename << std::endl;
        exit(1);
        //TODO: add a READY-FLAG to class
    }

    // Append `default` material
    obj_materials.push_back(tinyobj::material_t());

    for (size_t i = 0; i < obj_materials.size(); i++) {

        ShapeMaterial m;
        m.name = obj_materials[i].name;
        m.ambient = glm::vec3(obj_materials[i].ambient[0], obj_materials[i].ambient[1], obj_materials[i].ambient[2]);
        m.emissive = glm::vec3(obj_materials[i].emission[0], obj_materials[i].emission[1], obj_materials[i].emission[2]);
        m.diffuse = glm::vec3(obj_materials[i].diffuse[0], obj_materials[i].diffuse[1], obj_materials[i].diffuse[2]);
        m.specular = glm::vec3(obj_materials[i].specular[0], obj_materials[i].specular[1], obj_materials[i].specular[2]);

        m.refraction = obj_materials[i].ior;
        m.shininess = obj_materials[i].shininess;
        m.illum = obj_materials[i].illum;
        m.dissolve = obj_materials[i].dissolve;

        m.mapDiffuseFile = obj_materials[i].diffuse_texname;
        // BUMP as NORMAL
        m.mapBumpFile = obj_materials[i].normal_texname;
        m.mapNormalFile = obj_materials[i].bump_texname;

        m.mapSpecularFile = obj_materials[i].specular_texname;

        materials.push_back(m);

    }

/*
    for (int i = 0; i < materials.size(); i++) {

        std::cout << "Material[" << i << "]\n";
        std::cout << "name: " << materials[i].name << std::endl;
        std::cout << "ambient: " << glm::to_string(materials[i].ambient) << std::endl;
        std::cout << "emissive: " << glm::to_string(materials[i].emissive) << std::endl;
        std::cout << "diffuse: " << glm::to_string(materials[i].diffuse) << std::endl;
        std::cout << "specular: " << glm::to_string(materials[i].specular) << std::endl;
        std::cout << "shininess: " << materials[i].shininess << std::endl;

        std::cout << "mapDiffuseFile: " << materials[i].mapDiffuseFile.c_str() << std::endl;
        std::cout << "mapBumpFile: " << materials[i].mapBumpFile.c_str() << std::endl;
        std::cout << "mapNormalFile: " << materials[i].mapNormalFile.c_str() << std::endl;
        std::cout << "mapSpecularFile: " << materials[i].mapSpecularFile.c_str() << std::endl;

        std::cout << "--------------------------------------------\n";

    }
*/

    // min & max (bounding box)
    g_maxVertex[0] = g_maxVertex[1] = g_maxVertex[2] = 0.0f;
    g_minVertex[0] = g_minVertex[1] = g_minVertex[2] = 0.0f;

    for (size_t s = 0; s < obj_shapes.size(); s++) {

        //std::cout << "Shape: " << s << " " << obj_shapes[s].name << std:: endl;

        ShapeObject shape;
        shape.name = obj_shapes[s].name;

        int current_material_id;

        // TODO: fix smoothing normals

        std::map<int, glm::vec3> smoothVertexNormals;
        if (hasSmoothingGroup(obj_shapes[s]) > 0) {
            std::cout << "Compute smoothingNormal for shape: " << obj_shapes[s].name << "[" << s << "]" << std::endl;
            computeSmoothingNormals(obj_attrib, obj_shapes[s], smoothVertexNormals);
        }


        for (size_t f = 0; f < obj_shapes[s].mesh.indices.size() / 3; f++) {

            tinyobj::index_t idx0 = obj_shapes[s].mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = obj_shapes[s].mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = obj_shapes[s].mesh.indices[3 * f + 2];

            //----------------------------------------------------------------------------------------------------------
            // get material_id for current mesh (for diffuse vertex color)

            current_material_id = obj_shapes[s].mesh.material_ids[f];
            if ((current_material_id < 0) || (current_material_id >= static_cast<int>(obj_materials.size()))) {
                // Invaid material ID. Use default material.
                current_material_id = obj_materials.size() - 1;  // Default material is added to the last item in `materials`.
            }


            //----------------------------------------------------------------------------------------------------------
            // texcoords

            float tc[3][2];
            if (obj_attrib.texcoords.size() > 0) {
                if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) || (idx2.texcoord_index < 0)) {
                    // face does not contain valid uv index.
                    tc[0][0] = 0.0f;
                    tc[0][1] = 0.0f;
                    tc[1][0] = 0.0f;
                    tc[1][1] = 0.0f;
                    tc[2][0] = 0.0f;
                    tc[2][1] = 0.0f;
                } else {
                    //TODO: check faulty tex-coords
                    //assert(obj_attrib.texcoords.size() > size_t(2 * idx0.texcoord_index + 1));
                    //assert(obj_attrib.texcoords.size() > size_t(2 * idx1.texcoord_index + 1));
                    //assert(obj_attrib.texcoords.size() > size_t(2 * idx2.texcoord_index + 1));

                    // Flip Y coord.
                    tc[0][0] = obj_attrib.texcoords[2 * idx0.texcoord_index];
                    tc[0][1] = 1.0f - obj_attrib.texcoords[2 * idx0.texcoord_index + 1];
                    tc[1][0] = obj_attrib.texcoords[2 * idx1.texcoord_index];
                    tc[1][1] = 1.0f - obj_attrib.texcoords[2 * idx1.texcoord_index + 1];
                    tc[2][0] = obj_attrib.texcoords[2 * idx2.texcoord_index];
                    tc[2][1] = 1.0f - obj_attrib.texcoords[2 * idx2.texcoord_index + 1];
                }
            } else {
                // no uvs given at all
                tc[0][0] = 0.0f;
                tc[0][1] = 0.0f;
                tc[1][0] = 0.0f;
                tc[1][1] = 0.0f;
                tc[2][0] = 0.0f;
                tc[2][1] = 0.0f;
            }

            //----------------------------------------------------------------------------------------------------------
            // vertices
            float v[3][3];
            for (int k = 0; k < 3; k++) {
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;

                // TODO: check if index negative
                //assert(f0 >= 0);
                //assert(f1 >= 0);
                //assert(f2 >= 0);

                v[0][k] = obj_attrib.vertices[3 * f0 + k];
                v[1][k] = obj_attrib.vertices[3 * f1 + k];
                v[2][k] = obj_attrib.vertices[3 * f2 + k];

                // check min and max coords of all vertices
                g_minVertex[k] = std::min(v[0][k], g_minVertex[k]);
                g_minVertex[k] = std::min(v[1][k], g_minVertex[k]);
                g_minVertex[k] = std::min(v[2][k], g_minVertex[k]);
                g_maxVertex[k] = std::max(v[0][k], g_maxVertex[k]);
                g_maxVertex[k] = std::max(v[1][k], g_maxVertex[k]);
                g_maxVertex[k] = std::max(v[2][k], g_maxVertex[k]);

            }

            //-----------------------------------------------------------------------------------
            // normals

            float n[3][3];
            // bool for invalid normals
            bool invalid_normal_index = false;
            if (obj_attrib.normals.size() > 0) {
                int nf0 = idx0.normal_index;
                int nf1 = idx1.normal_index;
                int nf2 = idx2.normal_index;

                if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) {
                    // normal index is missing from this face.
                    invalid_normal_index = true;
                } else {
                    for (int k = 0; k < 3; k++) {
                        // TODO: check if indices are wrong
                        //assert(size_t(3 * nf0 + k) < attrib.normals.size());
                        //assert(size_t(3 * nf1 + k) < attrib.normals.size());
                        //assert(size_t(3 * nf2 + k) < attrib.normals.size());
                        n[0][k] = obj_attrib.normals[3 * nf0 + k];
                        n[1][k] = obj_attrib.normals[3 * nf1 + k];
                        n[2][k] = obj_attrib.normals[3 * nf2 + k];

                        //std::cout << "peter....\n";
                    }
                }
            } else {
                invalid_normal_index = true;
            }

            // TODO: check for smoothing_groups and smooth normals
            if (invalid_normal_index && !smoothVertexNormals.empty()) {
           // if (!smoothVertexNormals.empty()) {

                // Use smoothing normals
                int f0 = idx0.vertex_index;
                int f1 = idx1.vertex_index;
                int f2 = idx2.vertex_index;

                if (f0 >= 0 && f1 >= 0 && f2 >= 0) {

                    //std::cout << glm::to_string(smoothVertexNormals[f0]) << glm::to_string(smoothVertexNormals[f1]) << glm::to_string(smoothVertexNormals[f2]) << std::endl;

                    n[0][0] = smoothVertexNormals[f0][0];
                    n[0][1] = smoothVertexNormals[f0][1];
                    n[0][2] = smoothVertexNormals[f0][2];

                    n[1][0] = smoothVertexNormals[f1][0];
                    n[1][1] = smoothVertexNormals[f1][1];
                    n[1][2] = smoothVertexNormals[f1][2];

                    n[2][0] = smoothVertexNormals[f2][0];
                    n[2][1] = smoothVertexNormals[f2][1];
                    n[2][2] = smoothVertexNormals[f2][2];

                    invalid_normal_index = false;
                }
            }



            if (invalid_normal_index) {
                // compute geometric normals

                glm::vec3 v0 = glm::vec3(v[0][0], v[0][1], v[0][2]);
                glm::vec3 v1 = glm::vec3(v[1][0], v[1][1], v[1][2]);
                glm::vec3 v2 = glm::vec3(v[2][0], v[2][1], v[2][2]);

                glm::vec3 norm = genNormal(v0, v1, v2);

                //std::cout << glm::to_string(v0) << glm::to_string(v1) << glm::to_string(v2) << glm::to_string(norm) << std::endl;

                n[0][0] = norm[0];
                n[0][1] = norm[1];
                n[0][2] = norm[2];
                n[1][0] = norm[0];
                n[1][1] = norm[1];
                n[1][2] = norm[2];
                n[2][0] = norm[0];
                n[2][1] = norm[1];
                n[2][2] = norm[2];

            }

            //-----------------------------------------------------------------------------------


            for (int k = 0; k < 3; k++) {

                //vertices --------------------------------------------------------------
                shape.buffer.push_back(v[k][0]);
                shape.buffer.push_back(v[k][1]);
                shape.buffer.push_back(v[k][2]);

                //normals ---------------------------------------------------------------
                shape.buffer.push_back(n[k][0]);
                shape.buffer.push_back(n[k][1]);
                shape.buffer.push_back(n[k][2]);

                //uvs ---------------------------------------------------------------
                shape.buffer.push_back(tc[k][0]);
                shape.buffer.push_back(tc[k][1]);

            }

        }

        shape.vertexCount = shape.buffer.size() / (3 + 3 + 2);

        shape.materialId = current_material_id;


        //TODO: this is faulty too (when coloring vertices)
        /*
        // We do not support texturing with per-face material.
        if (obj_shapes[s].mesh.material_ids.size() > 0 && obj_shapes[s].mesh.material_ids.size() > s) {
            shape.materialId = obj_shapes[s].mesh.material_ids[0];  // use the material_id of the first face.
        } else {
            shape.materialId = obj_materials.size() - 1;// = ID for default material.
        }
        */

        shapes.push_back(shape);

        g_numVertices += shape.vertexCount;
        g_numTriangles = g_numVertices / 3;

    }

    g_realScale = g_maxVertex - g_minVertex;
    g_normScaleFactor = 1.0f / std::max(std::max(g_realScale.x, g_realScale.y), g_realScale.z);
    g_realPosition = g_minVertex;

    //------------------------------------------------------------------------------------------------------------------

    std::cout << "-----------------------------------------------------------------------------\n";
    std::cout << "Vertices: " << g_numVertices << " Triangles: " << g_numTriangles << std::endl;
    std::cout << "Max: " << g_maxVertex.x << ", " << g_maxVertex.y << ", " << g_maxVertex.z << std::endl;
    std::cout << "Min: " << g_minVertex.x << ", " << g_minVertex.y << ", " << g_minVertex.z << std::endl;
    std::cout << "Scale: " << g_realScale.x << ", " << g_realScale.y << ", " << g_realScale.z << std::endl;
    std::cout << "Position: " << g_realPosition.x << ", " << g_realPosition.y << ", " << g_realPosition.z << std::endl;
    std::cout << "-----------------------------------------------------------------------------\n";
    printf("# of vertices  = %d\n", (int)(obj_attrib.vertices.size()) / 3);
    printf("# of normals   = %d\n", (int)(obj_attrib.normals.size()) / 3);
    printf("# of texcoords = %d\n", (int)(obj_attrib.texcoords.size()) / 2);
    printf("# of materials = %d\n", (int) obj_materials.size() - 1);
    printf("# of shapes    = %d\n", (int) obj_shapes.size());
    std::cout << "-----------------------------------------------------------------------------\n";

}

void Model::initVAO() {

    // get shader inputs
    GLuint positionAttribID = (GLuint) glGetAttribLocation(g_shader, "in_position");
    GLuint normalAttribID = (GLuint) glGetAttribLocation(g_shader, "in_normal");

//    GLuint colorAttribID = (GLuint) glGetAttribLocation(g_shader, "in_color");

    GLuint uvAttribID = (GLuint) glGetAttribLocation(g_shader, "in_uv");

    GLsizei stride = (3 + 3 + 2) * sizeof(float);

    for (int i=0; i < shapes.size(); i++) {

        // create vao for the shape
        glGenVertexArrays(1, &shapes[i].vao);
        glBindVertexArray(shapes[i].vao);

        // Gen buffers
        GLuint vbos[1];
        glGenBuffers(1, vbos);

        // bind and fill buffer for vertex normal color and uv data
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, shapes[i].buffer.size() * sizeof(float), &shapes[i].buffer.at(0), GL_STATIC_DRAW);

        // config attrib pointer
        glVertexAttribPointer(positionAttribID, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 0));
        glVertexAttribPointer(normalAttribID, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 3));
        glVertexAttribPointer(uvAttribID, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 6));

        // enable attrib pointer
        glEnableVertexAttribArray(positionAttribID);
        glEnableVertexAttribArray(normalAttribID);
        glEnableVertexAttribArray(uvAttribID);

        //--------------------------------------------------------------------------------------------------------
        // Clean up
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray(positionAttribID);
        glDisableVertexAttribArray(normalAttribID);
        glDisableVertexAttribArray(uvAttribID);

        //clear buffer from each shape! dont waste ram space!!!
        shapes[i].buffer.clear();

    }

    // cout
    std::cout << "MODEL: VAO + VBO initialized.\n";

}

void Model::initTextures() {

    for (int i=0; i < materials.size(); i++) {

        //TODO: load all material files
        std::string mapDiffuseFile = materials[i].mapDiffuseFile;
        std::string mapSpecularFile = materials[i].mapSpecularFile;
        std::string mapNormalFile = materials[i].mapNormalFile; // TODO: check bump == normal

        // check if texture loaded already
        if (mapDiffuseFile != "" && textures.find(mapDiffuseFile) == textures.end()) {
            std::string fullpath = g_filepath + mapDiffuseFile;
            GLuint texturePointer = Texture::LoadTexture(fullpath, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            if (texturePointer > 0) textures[mapDiffuseFile] = texturePointer;
        }

        if (mapSpecularFile != "" && textures.find(mapSpecularFile) == textures.end()) {
            std::string fullpath = g_filepath + mapSpecularFile;
            GLuint texturePointer = Texture::LoadTexture(fullpath, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            if (texturePointer > 0) textures[mapSpecularFile] = texturePointer;
        }

        if (mapNormalFile != "" && textures.find(mapNormalFile) == textures.end()) {
            std::string fullpath = g_filepath + mapNormalFile;
            GLuint texturePointer = Texture::LoadTexture(fullpath, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
            if (texturePointer > 0) textures[mapNormalFile] = texturePointer;
        }

    }

    // cout
    std::cout << "MODEL: Textures initialized.\n";

}

void Model::initShader() {
    // Load simple shaders.
    GLuint vertexShader = Shader::LoadShader( GL_VERTEX_SHADER, "../../res/shader/model.vert" );
    GLuint fragmentShader = Shader::LoadShader( GL_FRAGMENT_SHADER, "../../res/shader/model.frag" );

    // Vector for shaders
    std::vector<GLuint> shaders;
    shaders.push_back(vertexShader);
    shaders.push_back(fragmentShader);

    // Create the shader program.
    g_shader = Shader::CreateShaderProgram(shaders);
    assert( g_shader != 0 );

    std::cout << "MODEL: Shader initalized.\n";
}

void Model::initAll() {

    loadModel();

    initTextures();

    initShader();

    initUniforms();

    initVAO();
}

void Model::initUniforms() {

    g_uniformMVP = glGetUniformLocation(g_shader, "u_mvp");

    g_uniformModelMatrix = glGetUniformLocation(g_shader, "u_modelMatrix");

    g_uniformDiffuseMap = glGetUniformLocation(g_shader, "diffuseMap");
    g_uniformSpecularMap = glGetUniformLocation(g_shader, "specularMap");
    g_uniformNormalMap = glGetUniformLocation(g_shader, "normalMap");


    g_uniformSunPosition = glGetUniformLocation(g_shader, "lightPosition");
    g_uniformCameraPosition = glGetUniformLocation(g_shader, "cameraPosition");

    g_uniformLightColor = glGetUniformLocation(g_shader, "lightColor");

    g_uniformAmbientColor = glGetUniformLocation(g_shader, "materialAmbientColor");
    g_uniformEmissiveColor = glGetUniformLocation(g_shader, "materialEmissiveColor");
    g_uniformDiffuseColor = glGetUniformLocation(g_shader, "materialDiffuseColor");
    g_uniformSpecularColor = glGetUniformLocation(g_shader, "materialSpecularColor");
    g_uniformShininess = glGetUniformLocation(g_shader, "materialShininess");

    g_uniformTextureFlags = glGetUniformLocation(g_shader, "textureFlags");
    g_uniformNormalmapFlags = glGetUniformLocation(g_shader, "normalmapFlags");

}

void Model::draw() {

    // bind shaderprogram
    glUseProgram( g_shader );

    // draw wireframe?
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //enable polygon offset filling (reduce artifacts between shapes)
    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(1.0, 1.0);

    for (int i=0; i < shapes.size(); i++) {

        // bind vao for each shape
        glBindVertexArray( shapes[i].vao );

        //--------------------------------------------------------------------------------------------------------------
        //TEXTURES

        // Map Texture-Units
        glUniform1i(g_uniformDiffuseMap, 0);
        glUniform1i(g_uniformSpecularMap, 1);
        glUniform1i(g_uniformNormalMap, 2);

        // bind diffuse texture
        glActiveTexture(GL_TEXTURE0);
        GLuint diffuseMap = textures[materials[shapes[i].materialId].mapDiffuseFile];
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        // bind specular texture
        glActiveTexture(GL_TEXTURE1);
        GLuint specularMap = textures[materials[shapes[i].materialId].mapSpecularFile];
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // bind normal texture
        glActiveTexture(GL_TEXTURE2);
        GLuint normalMap = textures[materials[shapes[i].materialId].mapNormalFile];
        glBindTexture(GL_TEXTURE_2D, normalMap);

        //--------------------------------------------------------------------------------------------------------------

        glm::mat4 model_scale = glm::scale(g_normScaleFactor * g_scale);

        glm::mat4 model_rotation = glm::toMat4(g_rotation);

        glm::mat4 model_translation = glm::translate(g_position);

        glm::vec3 offset_delta = g_realPosition + (g_realScale * 0.5f);

        glm::mat4 offset_translation = glm::translate(-offset_delta);

        g_modelMatrix = model_translation * model_rotation * model_scale * offset_translation;

        glm::mat4 mvp = g_projectionMatrix * g_viewMatrix * g_modelMatrix;

        //----------------------------------------------

        glUniformMatrix4fv( g_uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));

        glUniformMatrix4fv( g_uniformModelMatrix, 1, GL_FALSE, glm::value_ptr(g_modelMatrix));

    //----------------------------------------------

        glUniform3fv( g_uniformSunPosition, 1, glm::value_ptr(g_sunPosition));
        glUniform3fv( g_uniformCameraPosition, 1, glm::value_ptr(g_cameraPosition));

    //----------------------------------------------

        // TODO: maybe move light_scaling to loadModel

        glUniform3fv( g_uniformLightColor, 1, glm::value_ptr(g_lightColor * g_light_scale));

        glUniform3fv( g_uniformAmbientColor, 1, glm::value_ptr(materials[shapes[i].materialId].ambient * g_ambient_scale));
        glUniform3fv( g_uniformEmissiveColor, 1, glm::value_ptr(materials[shapes[i].materialId].emissive *  g_emmisive_scale));
        glUniform3fv( g_uniformDiffuseColor, 1, glm::value_ptr(materials[shapes[i].materialId].diffuse * g_diffuse_scale));
        glUniform3fv( g_uniformSpecularColor, 1, glm::value_ptr(materials[shapes[i].materialId].specular * g_specular_scale));

        glUniform1f( g_uniformShininess, materials[shapes[i].materialId].shininess * g_shininess_scale);

    //----------------------------------------------

        glUniform3ui( g_uniformTextureFlags, diffuseMap, specularMap, normalMap);

        glUniform3ui( g_uniformNormalmapFlags, g_normalmap_unsigned, g_normalmap_2channel, g_normalmap_greenup);

    //----------------------------------------------

        glDrawArrays(GL_TRIANGLES, 0, shapes[i].vertexCount);

    //----------------------------------------------

        // clean up
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    // clean up
    glUseProgram(0);

    // turn off offset fill
    //glDisable(GL_POLYGON_OFFSET_FILL);

    // drawn as wireframe?
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void Model::setGViewMatrix(const glm::mat4 &gViewMatrix) {
    g_viewMatrix = gViewMatrix;
}

void Model::setGProjectionMatrix(const glm::mat4 &gProjectionMatrix) {
    g_projectionMatrix = gProjectionMatrix;
}

void Model::setGRotation(const glm::quat &gRotation) {
    g_rotation = gRotation;
}

void Model::setGScale(const glm::vec3 &gScale) {
    g_scale = gScale;
}

void Model::setGPosition(const glm::vec3 &gPosition) {
    g_position = gPosition;
}

void Model::setGSunPosition(const glm::vec3 &gSunPosition) {
    g_sunPosition = gSunPosition;
}

void Model::setGCameraPosition(const glm::vec3 &gCameraPosition) {
    g_cameraPosition = gCameraPosition;
}

void Model::setGLightColor(const glm::vec3 &gLightColor) {
    g_lightColor = gLightColor;
}

const glm::quat &Model::getGRotation() const {
    return g_rotation;
}
