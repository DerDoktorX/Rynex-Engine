// Ligth Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

out Data {
    vec3 normal;
    vec4 pos;
    vec2 uv;
} DataOut;



void main()
{
    DataOut.uv = a_UV;
	DataOut.normal = a_Normals;
    DataOut.pos = u_Model * vec4( a_Postion , 1. );

	gl_Position = u_ViewProj * DataOut.pos;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;
layout(location = 2) out int  EntityID;

uniform vec3 u_Color;
uniform vec3 u_CamerPos;

in Data {
    vec3 normal;
    vec4 pos;
    vec2 uv;
} DataIn;



void main()
{
    EntityID = int(0);

    ////////////////////////////////////

    vec3 ligthPos = vec3(7., 5., 4.);
    vec4 ligthColor = vec4( vec3(0.8) , 1.0 );
    //--------------------------
    
    float ambient = 0.20f;

    vec3 normal = normalize(DataIn.normal);
    vec3 ligthDir = normalize(ligthPos *  DataIn.pos.xyz );
    float difuse = max( dot(normal, ligthDir), 0.0f );

    float specuLigth = 0.50f;
    vec3 viewDir = normalize(u_CamerPos - DataIn.pos.xyz);
    vec3 reflectDir = reflect( -ligthDir, DataIn.normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0f  ), 8 );
    float specular = specAmount * specuLigth;

    vec4 colorOut = vec4(ligthColor.rgb * (difuse + ambient + specular), 1.0);

    ////////////////////////////////////

	Color =  colorOut * vec4(0.15, 0.15, 0.75, 1.);
    Color =  colorOut * vec4( u_Color.rgb  , 1.0 );
    Normal = vec4( DataIn.normal.xyz , 1.0 );

	
}