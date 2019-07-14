#ifndef _PARTICLEEMITTER_H
#define _PARTICLEEMITTER_H


// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    float Life;
    // init
    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};



class ParticleEmitter {


public:

    ParticleEmitter(int amount, int newParticles);

    void updateMatrices(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);

    void update(float tpf);

    void draw(float tpf);

    //----------------------------

    const glm::quat &getRotation() const;
    void setRotation(const glm::quat &rotation);

private:

    int amount;
    int newParticles;

    std::vector<Particle> particles;

    glm::mat4 projection_matrix, view_matrix, model_matrix;
    glm::mat4 mvp;

    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 scale;

    //------------------------------------

    void init();

    void initShader();

    void initVao();

    void initUniforms();

    void initTexture();

    void initParticles();

    //------------------------------------

    int firstUnusedParticle();

    void respawnParticle(int index);

    //------------------------------------

    GLuint vao;

    GLuint shader;

    GLuint texture;

    //------------------------------------

    GLuint uniform_projectionMatrix;
    GLuint uniform_viewMatrix;
    GLuint uniform_modelMatrix;
    GLuint uniform_mvp;

    GLuint uniform_offset;

    GLuint uniform_texture;

    GLuint uniform_color;

};


#endif //_PARTICLEEMITTER_H
