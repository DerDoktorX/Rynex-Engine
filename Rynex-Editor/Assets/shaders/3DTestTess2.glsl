// SrcShader

#type vertex

#version 460 core
layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

out Data {
    vec3 normal;
    vec4 eye;
    vec2 uv;
} DataOut;

void main()
{
    DataOut.uv = a_UV;
	DataOut.normal = a_Normals;
    DataOut.eye = vec4(0.0);
#if 0
    gl_Position = u_ViewProj * u_Model * vec4( a_Postion , 1. );
#else
    gl_Position =  vec4( a_Postion , 1. );
#endif
}


#type fragment
#version 460 core

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normals;
layout(location = 2) out int  EntityID;

in Data {
    vec3 normal;
    vec4 eye;
    vec2 uv;
} DataIn;

uniform vec4 u_Color;


void main()
{
    vec4 diffuse = vec4(vec3(0.65), 1.0);
    vec4 ambient = vec4(vec3(0.75), 1.0);
    vec4 specular = vec4(vec3(0.65), 1.0);
    float shininess = 8.0;
    vec3 l_dir = normalize(vec3(7., 5., 4.));
//////////////////////////////////
//////////////////////////////////
    vec4 spec = vec4(0.0);
    vec3 n = normalize(DataIn.normal);
    vec3 e = normalize(vec3(DataIn.eye));

    float intensity = max(dot(n,l_dir), 0.0);
    if (intensity > 0.0) {
        // compute the half vector
        vec3 h = normalize(l_dir - e);  
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = specular * pow(intSpec,shininess);
    }
    vec4 colorOut = max(intensity * diffuse + spec, ambient);

//////////////////////////////////////////////
//////////////////////////////////////////////
	Color =  colorOut * vec4(0.15, 0.15, 0.75, 1.);
	// Color =  colorOut * vec4( u_Color.rgb  , 1.0 );
	Normals = vec4( DataIn.normal.xyz , 1.0 );
	EntityID = int(0);
}


#type tessControl
#version 460 core

layout(vertices = 4) out;


in Data {
    vec3 normal;
    vec4 eye;
    vec2 uv;
} DataIn[];


out Data {
    vec3 normal;
    vec4 eye;
    vec2 uv;
} DataControle[];

void main()
{
	float facktor = 1;

	DataControle[gl_InvocationID].uv = DataIn[gl_InvocationID].uv;
    DataControle[gl_InvocationID].normal = DataIn[gl_InvocationID].normal;
    DataControle[gl_InvocationID].eye = DataIn[gl_InvocationID].eye;
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


in Data {
    vec3 normal;
    vec4 eye;
    vec2 uv;
} DataControle[];


out Data {
    vec3 normal;
    vec4 eye;
    vec2 uv;
} DataIn;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

vec4 interpolater( vec4 v0, vec4 v1, vec4 v2, vec4 v3 )
{
    vec4 a = mix( v0, v1, gl_TessCoord.x );
    vec4 b = mix( v2, v3, gl_TessCoord.x );
    return mix( a, b, gl_TessCoord.y );
}

vec3 interpolater( vec3 v0, vec3 v1, vec3 v2, vec3 v3 )
{
    vec3 a = mix( v0, v1, gl_TessCoord.x );
    vec3 b = mix( v2, v3, gl_TessCoord.x );
    return mix( a, b, gl_TessCoord.y );
}

void main()
{
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 uv0 = DataControle[0].uv;
    vec2 uv1 = DataControle[1].uv;
    vec2 uv2 = DataControle[2].uv;
    vec2 uv3 = DataControle[3].uv;

    vec2 leftUV = uv0 + v * (uv3 - uv0);
    vec2 rightUV = uv1 + v * (uv2 - uv1);
    vec2 texCoord = leftUV + u * (rightUV - leftUV);
	DataIn.uv = texCoord;



    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 pos = leftPos + u * (rightPos - leftPos);
#if 0
    
    


    gl_Position = u_ViewProj * u_Model * vec4(pos.xyz , 1.0);
#else
    pos = interpolater(
        gl_in[0].gl_Position,
        gl_in[1].gl_Position,
        gl_in[2].gl_Position,
        gl_in[3].gl_Position
    );
    vec3 normals = interpolater(
        DataControle[0].normal,
        DataControle[1].normal,
        DataControle[2].normal,
        DataControle[3].normal
    );
    
    #if 1
        DataIn.normal = normalize(normals);
        DataIn.eye = vec4(pos.xyz, 1.0);
        gl_Position =  u_ViewProj * u_Model * vec4(pos.xyz , 1.0);
    #else
        vec3 direction =  normalize(pos.xyz);
        DataIn.normal = direction;
        DataIn.eye = vec4(-direction.xyz, 1.0);
        gl_Position = u_ViewProj * u_Model * vec4(direction.xyz , 1.0);
    #endif
#endif
}


