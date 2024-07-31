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
	
	//gl_Position = vec4(0.0,0.0,0.0,1.0);
}

#type fragment
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


#type tessControl
#version 450 core

layout(vertices = 4) out;



in vec3 myInData[];

out vec3 myData[];



void main(void)
{
	gl_TessLevelOuter[0] = 2.0;
	gl_TessLevelOuter[1] = 4.0;
	gl_TessLevelOuter[2] = 6.0;
	gl_TessLevelOuter[3] = 8.0;

	gl_TessLevelOuter[0] = 8.0;
	gl_TessLevelOuter[1] = 8.0;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	myData[gl_InvocationID] = myInData[gl_InvocationID];
}

#type tessEvalution
#version 450 core

layout(quads, equal_spacing, ccw) in;

//layout(location = 0)      in vec2	vi_UV;
//layout(location = 1) flat in vec3	vi_Normals;
//
//layout(location = 0)      out vec2	vo_UV;
//layout(location = 1) flat out vec3	vo_Normals;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	vec4 a = mix( v0, v1, gl_TessCoord.x);
	vec4 b = mix( v2, v3, gl_TessCoord.x);
	return mix( a, b , gl_TessCoord.y );
}

void main()
{
	//gl_Position = vec4( gl_TessCoord.x,
	//					gl_TessCoord.y,
	//					0,
	//					1 );
	//vo_UV		= vi_UV;
	//vo_Normals	= vi_Normals;
	//gl_Position = gl_in[0].gl_Position;
	gl_Position = interpolate( 	gl_in[0].gl_Position,
								gl_in[1].gl_Position,
								gl_in[2].gl_Position,
								gl_in[3].gl_Position );
}

