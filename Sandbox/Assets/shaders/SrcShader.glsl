// SrcShader

#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexurCoord;

out vec3 v_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Tranform;

void main()
{
	v_Color = a_Color;
	gl_Position = u_ViewProjection * u_Tranform * vec4(a_Position,0.,1.);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Color;

void main()
{
	//color = vec4(0.8,0.2,0.3, 1.0);
	color = vec4(v_Color.rgb ,1.);
}