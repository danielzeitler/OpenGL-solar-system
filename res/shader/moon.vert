
#version 410

in vec3 in_position;
in vec3 in_normal;
in vec2 in_texcoord;

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;

uniform vec3 u_cameraPosition;


out vec2 g_texcoord;
out vec3 g_vertexNormal;
out vec3 g_vertexPosition;
out vec3 g_viewVector;



void main() {

    g_texcoord = in_texcoord;

    // normal in worldspace
    g_vertexNormal = vec3(u_modelMatrix * vec4(in_normal,0)  );

    // vertex in worldspace
    vec3 vertexPosition = vec3(u_modelMatrix * vec4(in_position,0)  );
    g_vertexPosition = vertexPosition;

    // viewVector
    g_viewVector = u_cameraPosition - vertexPosition;

    // fragüpos
    gl_Position = u_mvp * vec4(in_position, 1.0);

}
