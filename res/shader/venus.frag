#version 410

in vec2 g_texcoord;
in vec3 g_vertexNormal;
in vec3 g_vertexPosition;
in vec3 g_viewVector;

uniform sampler2D mapSurface;
uniform sampler2D mapNormal;

uniform vec3 u_cameraPosition;
uniform vec3 u_lightPosition;

uniform vec4 u_lightColor;

uniform vec4 u_ambient;
uniform vec4 u_materialEmissive;
uniform vec4 u_materialDiffuse;
uniform vec4 u_materialSpecular;

uniform float u_materialShininess;

uniform vec2 u_resolution;
uniform float u_time;

out vec4 fragColor;

#define PI 3.14159265358979323846


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

    #define WITH_NORMALMAP_UNSIGNED

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord ) {

    // assume N, the interpolated vertex normal and
    // V, the view vector (vertex to eye)
    vec3 map = texture( mapNormal, texcoord ).xyz;

    #ifdef WITH_NORMALMAP_UNSIGNED
    map = map * 255./127. - 128./127.;
    #endif
    #ifdef WITH_NORMALMAP_2CHANNEL
    map.z = sqrt( 1. - dot( map.xy, map.xy ) );
    #endif
    #ifdef WITH_NORMALMAP_GREEN_UP
    map.y = -map.y;
    #endif

    mat3 TBN = cotangent_frame( N, -V, texcoord );

    return normalize( TBN * map );
}

    #define WITH_NORMALMAP

void main() {

    vec3 N = normalize(g_vertexNormal);

    #ifdef WITH_NORMALMAP
    N = perturb_normal(N, g_viewVector, g_texcoord);
    #endif

    // .......................................................................
    // ambient
    vec3 ambientColor = u_ambient.rgb * u_ambient.a;

    // emissive
    vec3 emissiveColor = u_materialEmissive.rgb * u_materialEmissive.a;

    // diffuse
    vec3 L  = normalize(u_lightPosition - g_vertexPosition); // lightDir
    float diff = clamp(dot(N, L), 0.0, 1.0);
    vec3 diffuseColor = diff * u_materialDiffuse.rgb * u_materialDiffuse.w * u_lightColor.rgb * u_lightColor.w;

    // specular
    vec3 V = normalize(u_cameraPosition - g_vertexPosition); // viewDir
    vec3 R = normalize( reflect(-L, N) );
    float spec = pow(max(dot(R,V), 0.0), u_materialShininess);
    vec3 specularColor = spec * u_materialSpecular.rgb * u_materialSpecular.w * u_lightColor.rgb * u_lightColor.w;

    vec4 whiteColor = vec4(1.0);
    vec4 surfaceColor = texture( mapSurface, g_texcoord );


    vec4 lightTerm = vec4(ambientColor + emissiveColor + diffuseColor + specularColor, 1.0);


    fragColor = surfaceColor * lightTerm;
}
