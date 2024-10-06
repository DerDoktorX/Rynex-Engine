// SrcShader

#type Vertex
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
	
	//gl_Position = vec4(0.0,0.0,0.0,1.0);
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;
layout(location = 2) out int  EntityID;


layout(location = 0)      in vec2	v_UV;
layout(location = 1) flat in vec3	v_Normals;

uniform vec3 u_Color;

#define SWITCH_PICTURES 1

void main()
{
#if SWITCH_PICTURES

	Normal = vec4( u_Color.rgb 	 , 1.0 );
	Color  = vec4( v_Normals.rgb , 1.0 );

#else

	Color =  vec4( u_Color.rgb  , 1.0 );
	Normal = vec4( v_Normals.rgb, 1.0 );
#endif
	EntityID = int(0);
}