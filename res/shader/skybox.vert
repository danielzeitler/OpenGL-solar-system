#version 410

in vec3 in_position;
in vec3 in_color;

uniform mat4 u_mvp;

out vec4 v_color;
out vec3 v_position;

void main() {


    // Just pass the color through directly.
    v_color = vec4(in_color, 1);

    // Just pass the vertices through directly as skybox uv
    v_position = in_position;

    gl_Position = u_mvp * vec4(in_position, 1);


    //vec4 pos = u_mvp * vec4(in_position, 1);
    //gl_Position = pos.xyww;


}