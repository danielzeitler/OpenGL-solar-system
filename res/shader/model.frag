#version 410

in vec3 out_positionW;
in vec3 out_normalW;
in vec2 out_uv;

//TODO: add out_lightVector to vertexShader
in vec3 out_viewVector;


uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec3 lightColor;

uniform vec3 materialAmbientColor;
uniform vec3 materialEmissiveColor;
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialShininess;


uniform bvec3 textureFlags;
uniform bvec3 normalmapFlags;


out vec4 fragColor;

//--------------------------------------------------------------------------------------------

#define DIFFUSE_MAP 0
#define SPECULAR_MAP 1
#define NORMAL_MAP 2

#define NORMALMAP_UNSIGNED 0
#define NORMALMAP_2CHANNEL 1
#define NORMALMAP_GREENUP 2

//--------------------------------------------------------------------------------------------

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv ) {
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

//--------------------------------------------------------------------------------------------

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord ) {
    // assume N, the interpolated vertex normal and
    // V, the view vector (vertex to eye)
    vec3 map = texture( normalMap, texcoord ).xyz;
    if (normalmapFlags[NORMALMAP_UNSIGNED]) map = map * 255./127. - 128./127.;
    if (normalmapFlags[NORMALMAP_2CHANNEL]) map.z = sqrt( 1. - dot( map.xy, map.xy ) );
    if (normalmapFlags[NORMALMAP_GREENUP])  map.y = -map.y;
    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
}

//-------------------------------------------------------------------------------------------

void main() {

    vec3 N = normalize(out_normalW);

    N = (textureFlags[NORMAL_MAP]) ? perturb_normal(N, out_viewVector, out_uv) : N;

// -------------------------------------------------------------------------------------------------

    // ambient
    vec3 ambientColor = materialAmbientColor;

    // emissive
    vec3 emissiveColor = materialEmissiveColor;

    // diffuse
    vec3 L  = normalize(lightPosition - out_positionW); // lightDir //TODO: do this in vertexshader
    float diff = clamp(dot(N, L), 0.0, 1.0);
    vec3 diffuseMapColor = (textureFlags[DIFFUSE_MAP]) ? texture( diffuseMap, out_uv ).rgb : vec3(1.0);
    vec3 diffuseColor = diff * materialDiffuseColor * lightColor * diffuseMapColor;

    // specular
    vec3 V = normalize(cameraPosition - out_positionW); // viewDir //TODO: do this in vertexshader
    vec3 R = normalize( reflect(-L, N) );
    float spec = pow(max(dot(R,V), 0.0), materialShininess);
    vec3 specularMapColor = (textureFlags[SPECULAR_MAP]) ? texture( specularMap, out_uv ).rgb : vec3(1.0);
    vec3 specularColor = spec * materialSpecularColor * lightColor * specularMapColor;

//---------------------------------------------------------------------------------

    vec4 finalColor = vec4(ambientColor + emissiveColor + diffuseColor + specularColor, 1.0);

//---------------------------------------------------------------------------------

    fragColor = finalColor;

}
