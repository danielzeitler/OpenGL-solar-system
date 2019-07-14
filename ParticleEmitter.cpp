
#include "SolarSystem.h"

#include "Shader.h"

#include "Texture.h"

#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(int amount, int newParticles) {
    this->amount = amount;
    this->newParticles = newParticles;

    //TODO: getter/setter etc.
    this->rotation = QUAT_IDENTITY;
    this->translation = glm::vec3(5.0, 0.0, 0.0);
    this->scale = glm::vec3(1.0);


    this->init();
}

void ParticleEmitter::initShader() {

    std::string vertexShaderSource = "../../res/shader/particle.vert";
    std::string fragmentShaderSource = "../../res/shader/particle.frag";


    // Load simple shaders.
    GLuint vertexShader = Shader::LoadShader( GL_VERTEX_SHADER, vertexShaderSource );
    GLuint fragmentShader = Shader::LoadShader( GL_FRAGMENT_SHADER, fragmentShaderSource );

    // Vector for shaders
    std::vector<GLuint> shaders;
    shaders.push_back(vertexShader);
    shaders.push_back(fragmentShader);

    // Create the shader program.
    shader = Shader::CreateShaderProgram(shaders);
    assert( shader != 0 );

    std::cout << "Particle-Shader initalized.\n";
}

void ParticleEmitter::initUniforms() {

    uniform_mvp = glGetUniformLocation(this->shader, "u_mvp");

    uniform_offset = glGetUniformLocation(this->shader, "u_offset");

    uniform_texture = glGetUniformLocation(this->shader, "u_texture");

    uniform_color = glGetUniformLocation(this->shader, "u_color");

    uniform_modelMatrix = glGetUniformLocation(this->shader, "u_modelMatrix");
    uniform_viewMatrix = glGetUniformLocation(this->shader, "u_viewMatrix");
    uniform_projectionMatrix = glGetUniformLocation(this->shader, "u_projectionMatrix");

}

void ParticleEmitter::initTexture() {
    std::string textureFile = "../../res/textures/fire-particles.png";

    texture = Texture::LoadTexture(textureFile);

    std::cout << "Particle-Texture initialized.\n";
}


void ParticleEmitter::initVao() {

    struct VertexXYZ {
        glm::vec3 m_Pos;
        glm::vec2 m_UV;
    };

    VertexXYZ g_Vertices[4] = {
            {glm::vec3( -1, -1, 0), glm::vec2(0,1)},
            {glm::vec3(  1, -1, 0), glm::vec2(1,1)},
            {glm::vec3( -1,  1, 0), glm::vec2(0,0)},
            {glm::vec3(  1,  1, 0), glm::vec2(1,0)},
    };

    GLuint g_Indices[6] = {
            0,1,2,
            2,1,3
    };

    //------------------------------------------------------------------------------------------------------------------

    // get shader inputs
    GLuint positionAttribID = (GLuint) glGetAttribLocation(this->shader, "in_position");
    GLuint textureAttribID = (GLuint) glGetAttribLocation(this->shader, "in_uv");


    // Create a VAO for the cube
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // Create buffer
    GLuint vertexBuffer, indexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &indexBuffer);

    // Bind buffer for vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_Vertices), g_Vertices, GL_STATIC_DRAW);

    // Bind buffer for indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_Indices), g_Indices, GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer(positionAttribID, 3, GL_FLOAT, GL_FALSE, sizeof(VertexXYZ), MEMBER_OFFSET(VertexXYZ,m_Pos));
    glEnableVertexAttribArray(positionAttribID);

    // UV
    glVertexAttribPointer(textureAttribID, 2, GL_FLOAT, GL_FALSE, sizeof(VertexXYZ), MEMBER_OFFSET(VertexXYZ,m_UV));
    glEnableVertexAttribArray(textureAttribID);

    //------------------------------------------------------------------------------------------------------------------

    // Clean up
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(positionAttribID);
    glDisableVertexAttribArray(textureAttribID);

    // cout
    std::cout << "Particle-VAO initialized.\n";

}




void ParticleEmitter::init() {

    this->initShader();

    this->initTexture();

    this->initVao();

    this->initUniforms();

    this->initParticles();

}

void ParticleEmitter::draw(float tpf) {

    //TODO: remove this -> use billboarding
    glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);

    // Use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // bind shaderprogram
    glUseProgram( shader );

//----------------------------------------------------------------------------------------------------------------------

    for (Particle particle : this->particles) {

        if (particle.Life > 0.0f) {
            // bind vao
            glBindVertexArray( vao );

            //----------------------------------------

            // Map Texture-Units
            glUniform1i(uniform_texture, 0);
            // Activate and bind TU
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            //----------------------------------------
            glUniformMatrix4fv( uniform_modelMatrix, 1, GL_FALSE, glm::value_ptr(this->model_matrix));
            glUniformMatrix4fv( uniform_viewMatrix, 1, GL_FALSE, glm::value_ptr(this->view_matrix));
            glUniformMatrix4fv( uniform_projectionMatrix, 1, GL_FALSE, glm::value_ptr(this->projection_matrix));





            glUniformMatrix4fv( uniform_mvp, 1, GL_FALSE, glm::value_ptr(this->mvp));

            glUniform3fv( uniform_offset, 1, glm::value_ptr(particle.Position));

            glUniform4fv( uniform_color, 1, glm::value_ptr(particle.Color));

            //----------------------------------------

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

            //----------------------------------------

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    }



//----------------------------------------------------------------------------------------------------------------------

    // clean up
    glUseProgram(0);

    // Don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //TODO: remove this -> use billboarding
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

}

void ParticleEmitter::updateMatrices(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {

   // std::cout << glm::to_string(viewMatrix) << std::endl;

    glm::mat4 translation = glm::translate(this->translation);
    glm::mat4 scale = glm::scale(this->scale);
    glm::mat4 rotation = glm::toMat4(this->rotation);

    glm::mat4 modelMatrix = translation * rotation * scale;

    this->projection_matrix = projectionMatrix;
    this->view_matrix = viewMatrix;
    this->model_matrix = modelMatrix;

    this->mvp = projectionMatrix * viewMatrix * modelMatrix;
}

void ParticleEmitter::initParticles() {
    // Create this->amount default particle instances
    for (int i = 0; i < this->amount; i++)
        this->particles.push_back(Particle());
}

void ParticleEmitter::update(float tpf) {

    // Add new particles
    for (int i = 0; i < this->newParticles; i++) {
        int unusedParticle = this->firstUnusedParticle();
        if (unusedParticle >= 0)
            this->respawnParticle(unusedParticle);
    }

    // Update all particles
    float dt = tpf / 1000;
    for (Particle &p : this->particles) {

        // reduce life
        p.Life -= dt;

        // particle is alive, thus update
        if (p.Life > 0.0f) {
            p.Position -= p.Velocity * dt;
            p.Color.a -= dt;
        }

    }

}

int lastUsedParticle = 0;
int ParticleEmitter::firstUnusedParticle() {

    // First search from last used particle, this will usually return almost instantly
    for (int i = lastUsedParticle; i < this->amount; i++){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }

    // Otherwise, do a linear search
    for (int i = 0; i < lastUsedParticle; i++){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }

    lastUsedParticle = 0;
    return -1;

}

void ParticleEmitter::respawnParticle(int index) {

    // random position
    // glm::vec3 pos = glm::ballRand(5.0);
    glm::vec3 pos = glm::sphericalRand(1.0);

    // random velocity
    //glm::vec3 vel = glm::sphericalRand(1.0);
    glm::vec3 vel = glm::ballRand(2.0);

    //------------------------------------------------------------------------------

    this->particles[index].Position = pos;
    this->particles[index].Velocity = vel;

    this->particles[index].Life = 1.0f;

    //TODO: lil random color
    this->particles[index].Color = glm::vec4(1.0);

}


//-----------------------------------------------------------------------------------------------------

void ParticleEmitter::setRotation(const glm::quat &rotation) {
    ParticleEmitter::rotation = rotation;
}

const glm::quat &ParticleEmitter::getRotation() const {
    return rotation;
}



