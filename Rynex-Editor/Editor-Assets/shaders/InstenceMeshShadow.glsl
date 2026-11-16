#type Vertex
#version 460 core
#define RY_ENABLE_SSOB_MODEL_BUFFER 0
// #define RY_OLD_CAMER_PACKEGE

layout(location = 0) in vec3 a_Postion;


#if !RY_ENABLE_SSOB_MODEL_BUFFER

layout(location = 3) in mat4 a_ModelMatrix;
// layout(location = 3) in vec4 a_ModelMatrix0;
// layout(location = 4) in vec4 a_ModelMatrix1;
// layout(location = 5) in vec4 a_ModelMatrix2;
// layout(location = 6) in vec4 a_ModelMatrix3;

layout(std140, binding = 3) uniform CameraData1
{
	mat4 ViewProjectionMatrix;	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	vec3 CamerPosition;			
	float Gamma;				
	vec3 ViewDirection;			
	float Empty;
} u_MainCamera;

#else

struct TransformObjectData{
	mat4 Matrix;
};

layout(std430, binding = 0) buffer ModelData
{
	TransformObjectData Object[];
} u_Model;

#endif

#ifdef RY_OLD_CAMER_PACKEGE
layout(std140, binding = 1) uniform CameraData
{
	mat4 ViewProjectionMatrix;	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	vec3 CamerPosition;			
	float Gamma;				
	vec3 ViewDirection;			
	float Empty;
} u_Camera;

#else

layout(std140, binding = 1) uniform CameraData
{	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	mat4 ViewProjectionMatrix;	
	vec4 CamerPosition;			
	vec4 ViewDirection;			
} u_Camera;

#endif



void main()
{
#if RY_ENABLE_SSOB_MODEL_BUFFER
	TransformObjectData model = u_Model.Object[gl_InstanceID];
    mat4 modelMatrix = model.Matrix;
#else
	// mat4 modelMatrix = mat4(
	// 	a_ModelMatrix0,
	// 	a_ModelMatrix1,
	// 	a_ModelMatrix2,
	// 	a_ModelMatrix3
	// );
	mat4 modelMatrix = a_ModelMatrix;
	// vec4 a_Postion = vec4(0.0, 0.0, 0.0, 1.0);
#endif

    vec4 pos = vec4( a_Postion.xyz , 1.0 );
    vec4 worldPostion = modelMatrix * pos;
   

	mat4 cameraViewProjtionMatric = u_Camera.ProjectionMatrix * u_Camera.ViewMatrix;
	vec4 modelCamerClipSpace = cameraViewProjtionMatric * worldPostion;


    gl_Position = modelCamerClipSpace;
}


#type Fragment
#version 460 core

void main()
{
}