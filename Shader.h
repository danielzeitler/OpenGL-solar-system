#ifndef __SHADER_H__
#define	__SHADER_H__



// Shader class for loading and compiling the shader program
class Shader {

public:
    // Static functions because we  need no Object (for now)
    static GLuint LoadShader( GLenum shaderType, const std::string& shaderFile );
    static GLuint CreateShaderProgram( std::vector<GLuint> shaders );


private:

};

#endif	/* __SHADER_H__ */

