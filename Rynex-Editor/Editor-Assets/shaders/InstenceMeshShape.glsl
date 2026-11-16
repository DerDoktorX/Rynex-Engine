#type Vertex
#version 460 core
#define RY_ENABLE_SSOB_MODEL_BUFFER 0

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;



#if !RY_ENABLE_SSOB_MODEL_BUFFER
layout(location = 3) in mat4 a_ModelMatrix;
#else

struct TransformObjectData{
	mat4 Matrix;
};

layout(std430, binding = 0) buffer ModelData
{
	TransformObjectData Object[];
} u_Model;
#endif


struct OutPut
{
	vec2 Coords;
};

layout(location = 0) out OutPut v_OutData;
layout(location = 5) out flat int v_EntityIndex;

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

const float g_Time = 1.16f * 0 + 1.16f * 0 + 1.16f * 0;
#define TAU 6.28318530718

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

    vec2 coords = a_UV;
    vec4 pos = vec4( a_Postion.xyz , 1.0 );
    vec4 worldPostion = modelMatrix * pos;
   

	mat4 cameraViewProjtionMatric = u_Camera.ViewProjectionMatrix;
	vec4 modelCamerClipSpace = cameraViewProjtionMatric * worldPostion;

    v_OutData.Coords = coords;

    
    
    gl_Position = modelCamerClipSpace;
}


#type Fragment
#version 460 core

struct InPut
{
	vec2 Coords;
};

layout(location = 0) out vec4 Color;

layout(location = 0) in InPut v_Input;

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



layout(binding = 0) uniform sampler2D t_AlbedoTexture;


#define ENABEL_SHADING_MIN 0
void main()
{
	
    vec2 coords = v_Input.Coords;
	Color = vec4(1.0f, 1.0f, 1.0f, 0.0f);
    Color.a = texture2D( t_AlbedoTexture, coords ).a;
    
}