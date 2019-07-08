#version 410

in vec2 uv;

uniform vec2 u_resolution;  // Canvas size (width,height)
uniform vec3 u_mouse;       // mouse position in screen pixels and click as z
uniform float u_wheel;      // mousewheel
uniform float u_time;       // Time in seconds since load


out vec4 fragColor;


#define PI 3.14159265358979323846


void main() {
    vec2 coord = 6.0 * uv.xy; // zoom out of the shader with 6.0 * uv

    for(int n = 1; n < 8; n++) {
        float i = float(n);
        coord += vec2(0.7 / i * sin(i * coord.y + u_time + 0.3 * i) + 0.8, 0.4 / i * sin(coord.x + u_time + 0.3 * i) + 1.6);
    }

    coord *= vec2(0.7 / sin(coord.y + u_time + 0.3) + 0.8, 0.4 / sin(coord.x + u_time + 0.3) + 1.6);

    vec3 color = vec3(0.5 * sin(coord.x) + 0.5, 0.5 * sin(coord.y) + 0.5, sin(coord.x + coord.y));

    fragColor = vec4(color, 1.0);
}
