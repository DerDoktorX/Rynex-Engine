// TextureTransform

#type Vertex
#version 450 core

layout(location = 0) in vec2	a_Position;
layout(location = 1) in vec2	a_TexCoord;

uniform mat4 u_ViewProjection;

layout(shared, binding = 0) uniform CameraData
{
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	mat4 ViewProjectionMatrix;	
	vec3 CamerPosition;			
	float Gamma;				
	vec3 ViewDirection;			
	float Empty;
} Camera; 

struct ObjectStruct {
    mat4 ModelMatrix;
    vec4 Color;
    int TexIndex;
    int EntityID;
};

layout(std430, binding = 1) buffer ObjectData
{
	ObjectStruct ObjectD[];
} Object;

layout(location = 0)      out vec2	v_TexCoord;
layout(location = 1) flat out uint	v_ObjectIndex;

void main()
{

    uint quadIndex = uint(float(gl_VertexID) / 4.f);
    ObjectStruct obj = Object.ObjectD[quadIndex];
    
	v_TexCoord = a_TexCoord;
	v_ObjectIndex = quadIndex;
	mat4 model = obj.ModelMatrix;
 	gl_Position = Camera.ViewProjectionMatrix * model * vec4(a_Position, 0.0 ,1.);
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int  EntityID;


layout(location = 0)      in vec2	v_TexCoord;
layout(location = 1) flat in uint	v_ObjectIndex;

uniform sampler2D u_Textures[32];

struct ObjectStruct {
    mat4 ModelMatrix;
    vec4 Color;
    int TexIndex;
    int EntityID;
};

layout(std430, binding = 1) buffer ObjectData
{
	ObjectStruct ObjectD[];
} Object;


void main()
{
	ObjectStruct obj = Object.ObjectD[uint(v_ObjectIndex)];

	color = texture2D(u_Textures[obj.TexIndex], v_TexCoord.xy) * obj.Color;
	EntityID = int(obj.EntityID);
}
