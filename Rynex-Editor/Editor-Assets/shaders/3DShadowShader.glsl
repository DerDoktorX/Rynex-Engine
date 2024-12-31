#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

layout(shared, binding = 0) uniform CameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	int Empty;					// 64
} Camera; 

layout(shared, binding = 1) uniform ModelData 
{
	mat4 ModelMatrix;	
	vec3 Color;			
	float Alpha;
    
	float Shinines;
	float Specular;
	int EntityID;	
    int UseTexture;
	
} Model; 




void main()
{
	vec4 worldPostion = Model.ModelMatrix * vec4( a_Postion , 1.0 );
    gl_Position = Camera.ViewProjectionMatrix * worldPostion;
}

#type Fragment
#version 450 core

void main()
{
    
	
}