#version 410

in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;

uniform mat4 u_mvp;
uniform mat4 u_modelMatrix;

uniform vec3 cameraPosition;

out vec3 out_positionW;

out vec3 out_normalW;

out vec3 out_color;
out vec2 out_uv;

out vec3 out_viewVector;

void main() {

    out_uv = in_uv;

    out_positionW = vec4(u_modelMatrix * vec4(in_position, 1.0)).xyz;

    // IMPORTANT: don't scale up with 1.0 as 4th component!!
    out_normalW = vec4(u_modelMatrix * vec4(in_normal, 0.0)).xyz;

    // viewVector
    out_viewVector = normalize(cameraPosition - out_positionW.xyz);

    // glPosition
    gl_Position = u_mvp * vec4(in_position, 1.0);

}
