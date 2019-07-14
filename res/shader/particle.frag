#version 410

in vec2 v_uv;

uniform sampler2D u_texture;
uniform vec4 u_color;

out vec4 fragColor;

//--------------------------------------------------------------------------------------------

void main() {

    vec4 texel = texture(u_texture, v_uv);

    fragColor = texel * u_color;

}
