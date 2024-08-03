// SrcShader

#type vertex

#version 450 core
layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

out vec2	v_UV;

void main()
{
	v_UV = a_UV;
	gl_Position = u_ViewProj * u_Model * vec4( a_Postion , 1. );
}


#type fragment
#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;

in vec2	v_UV;

uniform vec3 u_Color;

void main()
{
	Color =  vec4( u_Color.rgb  , 1.0 );
	Normal = vec4( v_UV, 0.0, 1.0 );
}


#type tessControl
#version 450 core

layout(vertices = 4) out;


in vec2 v_UV[];

out vec2 vControl_UV[];

void main(void)
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	vControl_UV[gl_InvocationID] = v_UV[gl_InvocationID];

	gl_TessLevelOuter[0] = 2;
	gl_TessLevelOuter[1] = 2;
	gl_TessLevelOuter[2] = 2;
	gl_TessLevelOuter[3] = 2;

	gl_TessLevelInner[0] = 2;
	gl_TessLevelInner[1] = 2;
}


#type tessEvalution

#version 450 core
layout(quads, equal_spacing, ccw) in;


in vec2	vControl_UV[];

out vec2 v_UV;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	vec4 a = mix( v0, v1, gl_TessCoord.x);
	vec4 b = mix( v2, v3, gl_TessCoord.x);
	return mix( a, b , gl_TessCoord.y );
}

vec2 interpolate(vec2 v0, vec2 v1, vec2 v2, vec2 v3)
{
	vec2 a = mix( v0, v1, gl_TessCoord.x);
	vec2 b = mix( v2, v3, gl_TessCoord.x);
	return mix( a, b , gl_TessCoord.y );
}

void main()
{
	//vec2 uv0 = vControl_UV[0];
	//vec2 uv1 = vControl_UV[1];
	//vec2 uv2 = vControl_UV[2];
	//vec2 uv3 = vControl_UV[3];

	//vec2 leftUV = uV0 + v * (uv3 - uv0);
	//vec2 rigtheUV = uv1 + v* (uv2 - uv1);
	//vec2 textCoord = leftUV + u * (rigtheUV - leftUV);
	//v_UV = textCoord;

	v_UV = interpolate(	vControl_UV[0],
						vControl_UV[1],
						vControl_UV[2],
						vControl_UV[3] );

	gl_Position = interpolate( 	gl_in[0].gl_Position,
								gl_in[1].gl_Position,
								gl_in[2].gl_Position,
								gl_in[3].gl_Position );
}


