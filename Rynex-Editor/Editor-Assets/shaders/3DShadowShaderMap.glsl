#type Vertex
#version 460 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 uModel;

uniform mat4 uLigthview;
uniform mat4 uLigthProje;

uniform mat4 mvp;
void main()
{
    mat4 lightSpaceMatrix = uLigthProje * (uLigthview); 
    mat4 mvpL = lightSpaceMatrix * uModel;
    gl_Position = mvpL * vec4( a_Postion , 1.0 );
}

#type Fragment
#version 460 core

void main()
{
}