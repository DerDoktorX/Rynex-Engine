#type Vertex
#version 460 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

out vec4 ligthView_Postion;

uniform mat4 mvp;
uniform mat4 matrixShadow;


void main()
{
    ligthView_Postion = matrixShadow * vec4(a_Postion, 1.0f);
    gl_Position = mvp * vec4(a_Postion.x, a_Postion.y , a_Postion.z, 1.0f);
}

#type Fragment
#version 460 core

layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;


layout(binding = 0) uniform sampler2DShadow ShadowMap; 
uniform vec4 uColor;

void main()
{
 
	Color = vec4(1.0);
}