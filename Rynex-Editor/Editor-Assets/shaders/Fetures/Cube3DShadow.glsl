#type Vertex
#version 460 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

out vec4 ligthView_Postion;

uniform mat4 uModel;

uniform mat4 uCamerView;
uniform mat4 uCamerProje;

uniform mat4 uLigthview;
uniform mat4 uLigthProje;

uniform mat4 mvp;
uniform mat4 shadowMatrix;
void main()
{
    mat4 scaleMat = mat4( 0.5, 0.0, 0.0, 0.0
                        , 0.0, 0.5, 0.0, 0.0
                        , 0.0, 0.0, 0.5, 0.0
                        , 0.0, 0.0, 0.0, 1.0     );

    mat4 translation = mat4( 1.0, 0.0, 0.0, 0.0
                            , 0.0, 1.0, 0.0, 0.0
                            , 0.0, 0.0, 1.0, 0.0
                            , 0.5, 0.5, 0.5, 1.0    );

    mat4 lightSpaceMatrix = uLigthProje * (uLigthview); 
    ligthView_Postion = lightSpaceMatrix * uModel * vec4(a_Postion, 1.0f);
    // * scaleMat * translation 
    
    gl_Position = uCamerProje * uCamerView * uModel * vec4(a_Postion, 1.0f);
}

#type Fragment
#version 460 core

layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;

#define TEXTURE_SHADOW_SAMPLER 0

#if TEXTURE_SHADOW_SAMPLER
    layout(binding = 0) uniform sampler2DShadow ShadowMap; 
#else
    layout(binding = 0) uniform sampler2D ShadowMap; 
#endif

uniform vec4 uColor;

in vec4 ligthView_Postion;

void main()
{
	Color = vec4(uColor);
#if TEXTURE_SHADOW_SAMPLER

    vec3 p = ligthView_Postion.xyz / ligthView_Postion.w;
    p = p * 0.5 + 0.5;
    float multyplyer = textureProj(ShadowMap, ligthView_Postion);
    float shadow = p.z -bias > multyplyer  ? 1.0 : 0.0;
    Color.xyz *= shadow;
    // if(multyplyer > 0.0)
    //     Color.xyz *= multyplyer - (p.z - bias);
    // else
    //     Color.xyz *= 1.0f;

#else

   vec3 p = ligthView_Postion.xyz / ligthView_Postion.w;
   p.xyz = p.xyz * 0.5 + 0.5;
   float multyplyer = texture(ShadowMap, p.xy).r;
   float bias = 0.000009f;
   float shadow = p.z - bias > multyplyer ? 1.0 : multyplyer -  p.z - bias ;
   // Color.xyz *= multyplyer;

    // if(multyplyer > 0.0)
    Color.xyz *= (1.0 - shadow);
    //else
    //    Color.xyz *= 1.0;

#endif
    
}