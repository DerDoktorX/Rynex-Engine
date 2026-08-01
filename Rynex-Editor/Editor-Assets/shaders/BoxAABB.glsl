#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;

uniform mat4 u_ViewProj;
uniform vec3 u_GloblePostion;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProj * u_Model * vec4(a_Postion, 1. );
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out int EntityID;

uniform int u_EntityID;
uniform vec3 u_Color;

void main()
{
    EntityID = u_EntityID;
    Color = vec4(u_Color.rgb, 1.0);
}