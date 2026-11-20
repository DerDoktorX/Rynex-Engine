#type Vertex
#version 460 core

layout(location = 0) in vec3 a_Postion;

layout(std140, binding = 0) uniform TransformObjectData
{
    mat4 Matrix;
} u_Model;





layout(std140, binding = 1) uniform CameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;
} u_Camera; 


void main()
{
    mat4 modelMatrix = u_Model.Matrix;

    vec4 pos = vec4( a_Postion.xyz , 1.0 );
    vec4 worldPostion = modelMatrix * pos;
	mat4 cameraViewProjtionMatric = u_Camera.ViewProjectionMatrix;
	vec4 modelCamerClipSpace = cameraViewProjtionMatric * worldPostion;
    
    gl_Position = modelCamerClipSpace;
}


#type Fragment
#version 460 core



void main()
{

}