// SrcShader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

layout(location = 0)      out vec2	v_UV;
layout(location = 1) flat out vec3	v_Normals;

void main()
{
	
	v_Normals = a_Normals;
	v_UV = a_UV;
	gl_Position = u_ViewProj * u_Model * vec4( a_Postion , 1. );
	//gl_Position = vec4(0.5,0.5,0.0,1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 uv;
layout(location = 2) out int  EntityID;


layout(location = 0)      in vec2	v_UV;
layout(location = 1) flat in vec3	v_Normals;

uniform vec3 u_Color;

void main()
{
	
	color = vec4(u_Color.rgb, 1.);
	uv = vec4(v_Normals, 1.0);
	EntityID = int(0);
}