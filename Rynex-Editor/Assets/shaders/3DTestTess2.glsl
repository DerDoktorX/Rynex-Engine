// SrcShader

#type vertex

#version 460 core
layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

out vec2 v_Normals;

void main()
{
	v_Normals = a_UV;

#if 1
    gl_Position = u_ViewProj * u_Model * vec4( a_Postion , 1. );
#else
    gl_Position =  vec4( a_Postion , 1. );
#endif
}


#type fragment
#version 460 core

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;
layout(location = 2) out int  EntityID;

in vec2	v_Normals;

uniform vec2 u_Color;

void main()
{
	Color =  vec4( 1.0, 0.0, 1.0, 1.0);
	//Color =  vec4( u_Color.rgb  , 1.0 );
	Normal = vec4( v_Normals,0.0, 1.0 );
	EntityID = int(0);
}


#type tessControl
#version 460 core

layout(vertices = 4) out;


in vec2	v_Normals[];
out vec2 vControl_Normals[];

void main()
{
	float facktor = 10;

	vControl_Normals[gl_InvocationID] = v_Normals[gl_InvocationID];
#if 1
    gl_TessLevelOuter[0] = facktor; 
    gl_TessLevelOuter[1] = facktor; 
    gl_TessLevelOuter[2] = facktor; 
    gl_TessLevelOuter[3] = facktor; 
    
    gl_TessLevelInner[0] = facktor; 
    gl_TessLevelInner[1] = facktor;
	
#else
    gl_TessLevelOuter[0] = 2; 
    gl_TessLevelOuter[1] = 4; 
    gl_TessLevelOuter[2] = 6; 
    gl_TessLevelOuter[3] = 8; 
    
    gl_TessLevelInner[0] = 8; 
    gl_TessLevelInner[1] = 8;
#endif
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; 
    
}


#type tessEvalution

#version 460 core
layout(quads, equal_spacing, ccw) in;


in vec2	vControl_Normals[];

out vec2 v_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

vec4 interpolater( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
    vec4 a = mix( v0, v1, gl_TessCoord.x );
    vec4 b = mix( v2, v3, gl_TessCoord.x );
    return mix( a, b, gl_TessCoord.y );
}

void main()
{
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 uv0 = vControl_Normals[0];
    vec2 uv1 = vControl_Normals[1];
    vec2 uv2 = vControl_Normals[2];
    vec2 uv3 = vControl_Normals[3];

    vec2 leftUV = uv0 + v * (uv3 - uv0);
    vec2 rightUV = uv1 + v * (uv2 - uv1);
    vec2 texCoord = leftUV + u * (rightUV - leftUV);
	v_Normals = texCoord;



    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 pos = leftPos + u * (rightPos - leftPos);
#if 1
    gl_Position = pos;
#else
    
    gl_Position =  u_ViewProj * u_Model * interpolater(
        gl_in[0].gl_Position,
        gl_in[1].gl_Position,
        gl_in[2].gl_Position,
        gl_in[3].gl_Position
    );
#endif
}

