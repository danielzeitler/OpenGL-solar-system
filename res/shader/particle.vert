#version 410

//--------------------------------------------------------------------------------------------

in vec3 in_position;
in vec2 in_uv;

uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;

uniform mat4 u_mvp;
uniform vec3 u_offset;

out vec2 v_uv;

//--------------------------------------------------------------------------------------------

void main() {

    // pass thru tex_coords
    v_uv = in_uv;

    //--------------------------------------------------------------------------------
    // remove rotation from modelView matrix (bill-boarding)
    mat4 modelViewMatrix = u_viewMatrix * u_modelMatrix;

    // First colunm.
    modelViewMatrix[0][0] = 1.0;
    modelViewMatrix[0][1] = 0.0;
    modelViewMatrix[0][2] = 0.0;

    // Second colunm.
    modelViewMatrix[1][0] = 0.0;
    modelViewMatrix[1][1] = 1.0;
    modelViewMatrix[1][2] = 0.0;

    // Thrid colunm.
    modelViewMatrix[2][0] = 0.0;
    modelViewMatrix[2][1] = 0.0;
    modelViewMatrix[2][2] = 1.0;

    mat4 mvp = u_projectionMatrix * modelViewMatrix ;

    //--------------------------------------------------------------------------------

    //TODO: remove this -> move to modelMatrix
    float scaleTODO = 0.1;

    // frag pos
    //without billboarding
    gl_Position = u_mvp * vec4((in_position * scaleTODO) + u_offset, 1.0);

    // with billboarding
    //gl_Position = mvp * vec4((in_position * scaleTODO) + u_offset, 1.0);


}
