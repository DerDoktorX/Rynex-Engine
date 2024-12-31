#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProj * u_Model * vec4( a_Postion , 1. );
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 Color;

uniform vec3 u_Color;

void main()
{
    Color =  vec4(u_Color, 1.0);
}