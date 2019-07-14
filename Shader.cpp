#include "SolarSystem.h"

#include "Shader.h"


// Loads a shader and returns the compiled shader object.
// If the shader source file could not be opened or compiling the
// shader fails, then this function returns 0.
GLuint Shader::LoadShader( GLenum shaderType, const std::string& shaderFile )
{
    std::ifstream ifs;

    // Load the shader.
    ifs.open(shaderFile.c_str());

    if ( !ifs )
    {
        std::cerr << "Can not open shader file: \"" << shaderFile << "\"" << std::endl;
        return 0;
    }

    std::string source( std::istreambuf_iterator<char>(ifs), (std::istreambuf_iterator<char>()) );
    ifs.close();

    // Create a shader object.
    GLuint shader = glCreateShader( shaderType );

    // Load the shader source for each shader object.
    const GLchar* sources[] = { source.c_str() };
    glShaderSource( shader, 1, sources, NULL );

    // Compile the shader.
    glCompileShader( shader );

    // Check for errors
    GLint compileStatus;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );

    if ( compileStatus != GL_TRUE )
    {
        GLint logLength;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );
        GLchar* infoLog = new GLchar[logLength];
        glGetShaderInfoLog( shader, logLength, NULL, infoLog );

        std::cerr << infoLog << std::endl;

        delete infoLog;
        return 0;
    }

    // return the compiled shader
    return shader;

}


// Create a shader program from a set of compiled shader objects.
GLuint Shader::CreateShaderProgram( std::vector<GLuint> shaders )
{
    // Create a shader program.
    GLuint program = glCreateProgram();

    // Attach the appropriate shader objects.
    for( GLuint shader: shaders ) {
        glAttachShader( program, shader );
    }

    // Link the program
    glLinkProgram(program);

    // Check the link status.
    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus );
    if ( linkStatus != GL_TRUE )
    {
        GLint logLength;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );
        GLchar* infoLog = new GLchar[logLength];

        glGetProgramInfoLog( program, logLength, NULL, infoLog );

        std::cerr << infoLog << std::endl;

        delete infoLog;
        return 0;
    }

    // Return the compiled shader-program (vertex & fragment)
    return program;
}

