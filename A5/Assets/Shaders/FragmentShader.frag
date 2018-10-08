#version 450

#define TEX_COLOUR              1 << 0
#define TEX_NORMAL              1 << 1
#define TEX_DISPLACE            1 << 2
#define TEX_SPECULAR            1 << 3
#define TEX_AMBIENT_OCCLUSION   1 << 4

#define MAX_LIGHTS 4

struct Light {
    vec3 colour;
    vec4 position;
    double falloff[3];
    float coneAngle;
    vec3 coneDirection;
};

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
    float alpha;
    vec4 dparams;
    float texScaling;
};

in vec3 geomNormal_WS;
in vec3 geomPosition_WS;
in vec2 geomTexCoord;
in vec3 geomTangent_WS;
in vec4 geomShadowCoord[MAX_LIGHTS];
in vec3 geomReflectVector_WS;

uniform bool enableTransparency;
uniform bool enableNormalShading;
uniform bool enableNormalMapping;
uniform bool enableSpecularMapping;
uniform bool enableAmbientOcclusionMapping;
uniform bool enableUVShading;
uniform bool enableTangentShading;
uniform bool enableDepthMapShading;
uniform bool enableTextureMapping;
uniform bool enableShadow;
uniform Light lights[MAX_LIGHTS];
uniform int num_lights;
layout (location = 0) uniform sampler2D texColour;
layout (location = 1) uniform sampler2D texNormal;
layout (location = 3) uniform sampler2D texSpecular;
layout (location = 4) uniform sampler2D texAmbientOcclusion;
uniform sampler2DShadow shadowMap[MAX_LIGHTS];
uniform samplerCube environMap;
uniform samplerCube environMapNight;
uniform int texture_flags;
uniform mat4 Model;
uniform mat4 View;
uniform vec3 eye;
uniform float blendFactor;

out vec4 fragColour;

uniform Material material;
uniform vec3 ambientIntensity;

vec2 PoissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);


vec2 getTexCoords()
{
    return material.texScaling * geomTexCoord;
}


float attenuation(Light light, float dist)
{
    return float(1 / (light.falloff[0] + light.falloff[1] * dist + light.falloff[2] * dist ));
}

float getShininess()
{
    if (enableSpecularMapping && (texture_flags & TEX_SPECULAR) != 0)
    {
        return max(material.shininess * (texture(texSpecular, getTexCoords()).r), 1.0);
    }
    return max(material.shininess, 1.0);
}

vec3 getAmbient(Light light)
{
    vec3 ret = ambientIntensity;
    if (enableAmbientOcclusionMapping && (texture_flags & TEX_AMBIENT_OCCLUSION) != 0)
    {
        ret *= texture( texAmbientOcclusion, getTexCoords() ).rgb;
    }
    vec3 groundNormal = vec3(0, 1, 0);
    ret *= clamp(6*dot(-normalize(light.position.xyz), groundNormal), 0.2, 1.0);
    return ret;
}

vec3 getDiffuse(Light light, float NdotL)
{
    if (enableTextureMapping && (texture_flags & TEX_COLOUR) != 0)
    {
        return (getAmbient(light) + max(vec3(NdotL), 0)) * material.kd * texture( texColour, getTexCoords() ).rgb;
    }
    return material.kd;
}

vec4 getBlendedColor(samplerCube c1, samplerCube c2, vec3 tc, float bf)
{
    vec4 texture1 = texture(c1, tc);
    vec4 texture2 = texture(c2, tc);
    return mix(texture1, texture2, bf);
}

vec3 getSpecular(Light light, float NdotH)
{

    return getAmbient(light) + getBlendedColor(environMap, environMapNight, vec3(geomReflectVector_WS.x, -geomReflectVector_WS.yz), blendFactor).rgb * material.ks * pow(NdotH, getShininess());
}

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float getVisibility(float NdotL)
{
    if (enableShadow)
    {
        float visibility = 1.0;
        float bias = 0.0;

        for (int j = 0; j < num_lights; ++j)
        {

            if (lights[j].position.w == 0)
            {
                // directional light
                // for (int i = 0; i < 16; i++) {
                //     int index = int(16.0*random(floor(geomPosition_WS.xyz*1000.0), i)) % 16;            
                //     visibility -= 0.05*(1.0 - texture(
                //         shadowMap[j],
                //         vec3(geomShadowCoord[j].xy + PoissonDisk[index]/700.0,  geomShadowCoord[j].z-bias)
                //     ));
                // }
                if (texture( shadowMap[j], geomShadowCoord[j].xyz ) < geomShadowCoord[j].z-bias)
                {
                    visibility = 0.5;
                }
                if (NdotL < 0)
                {
                    visibility = 0.5;
                }
            } else {
                // for (int i = 0; i < 16; i++) {
                //     int index = int(16.0*random(floor(geomPosition_WS.xyz*1000.0), i)) % 16;            
                //     visibility -= 0.05*(1.0 - texture(
                //         shadowMap[j],
                //         vec3(geomShadowCoord[j].xy + PoissonDisk[index]/700.0,  geomShadowCoord[j].z-bias) / geomShadowCoord[j].w
                //     ));
                // }
            }
        }
        return clamp(visibility, 0, 1);
    } else {
        return 1.0;
    }
}

vec3 applyLight(Light light, vec3 N, vec3 surfacePos, vec3 V)
{
    vec3 L;
    float att = 0.0;
    if (light.position.w == 0.0) {
        //directional light
        L = -normalize(light.position.xyz);
        att = 1.0; //no attenuation for directional lights
    } else {
        L = (light.position).xyz - surfacePos.xyz;
        float dist = length(L);
        L /= dist;
        float lightToSurfaceAngle = degrees(acos(dot(-L, normalize(light.coneDirection))));
        if (lightToSurfaceAngle < light.coneAngle)
        {
            att = attenuation(light, dist);
        }
    }

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 retval;

    
    float NdotL, NdotH;
    vec3 H;
    NdotL = dot(N, L);
    
    if (light.position.w == 0)
    {
        // if it is directional light and is below the ground
        vec3 groundNormal = vec3(0, 1, 0);
        NdotL *= max(dot(-normalize(light.position.xyz), groundNormal), 0);
    }

    diffuse = getDiffuse(light, NdotL);    
 
    H = normalize(V + L);
    NdotH = max(dot(N, H), 0.0);
    specular = getSpecular(light, NdotH);

    float visibility = getVisibility(NdotL);

    retval = att * visibility * light.colour * (diffuse + specular);
    return retval;
}

vec3 phongModel(vec3 N, vec3 V, vec3 position) {

    vec3 retval = vec3(0.0); 
    Light light;
    for (int i = 0; i < num_lights; ++i)
    {
        light = lights[i];
        retval += applyLight(light, N, position, V);
    }
    return retval;
}

vec3 computePertubedNormal(vec3 T, vec3 B, vec3 N)
{
    vec3 bumpNormal = 2 * (texture(texNormal, getTexCoords()).rgb - 0.5); // [0, 1] => [-1, 1]
    mat3 rotation = mat3(T, B, N);
    return rotation * bumpNormal;
}

void main() {
    vec3 V = normalize(eye - geomPosition_WS);
    vec3 N = normalize(geomNormal_WS);
    vec3 T = geomTangent_WS;
    vec3 B = cross(geomTangent_WS, geomNormal_WS);
    T = normalize(T - dot(N, T)*N);
    B = normalize(B - dot(N, B)*N);
    if (enableNormalMapping && (texture_flags & TEX_NORMAL) != 0)
    {
        N = computePertubedNormal(T, B, N);
    }
    if (enableUVShading)
    {    
        fragColour = vec4(getTexCoords().x - floor(getTexCoords().x), 0, getTexCoords().y - floor(getTexCoords().y), 1.0);
    }
    else if (enableNormalShading)
    {
        fragColour = vec4(N, 1.0);
    } else if (enableTangentShading)
    {
        fragColour = vec4(T, 1.0);
    }
    else 
    {
        fragColour = vec4(phongModel(N, V, geomPosition_WS), enableTransparency ? material.alpha: 1.0);
    }
}
