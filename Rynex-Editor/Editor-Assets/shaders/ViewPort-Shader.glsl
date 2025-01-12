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
} Model; 


void main()
{
     gl_Position = Camera.ViewProjectionMatrix * Model.ModelMatrix * vec4( a_Postion , 1.0f );
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 Color;


void main()
{
    Color = vec4( vec3(1.0f,1.0f, 1.0f) * (gl_FragDepth), 1.0f);
}