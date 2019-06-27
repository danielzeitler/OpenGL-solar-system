#version 410

in vec4 v_color;
in vec3 v_position;

uniform samplerCube skybox;

out vec4 out_color;


void main() {

    //out_color = vec4(1,0,0,1);

    //out_color = v_color;

    out_color = texture(skybox, v_position);

}