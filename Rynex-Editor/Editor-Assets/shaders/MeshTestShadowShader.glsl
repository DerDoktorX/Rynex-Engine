#type Vertex
#version 460 core

#if 1

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

struct DrawObjectData
{
	mat4 Matrix;
};
#elif 0

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

struct DrawObjectData
{
	mat4 Matrix;
	mat4 NormleMatrix;
};

#else

layout(location = 0) in vec3 a_Postion;

struct DrawObjectData
{
    mat4 Matrix;
    mat4 NormleMatrix;
    int EnitityID;
    int MaterielID;
    int Empty0;
    int Empty1;
};

#endif


layout(std140, binding = 0) uniform ShadowCameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	int Empty;					// 64
	vec3 ViewDirection;			// 
	float Empty2;				// 
} u_ShadowCamera; 

layout(std140, binding = 1) uniform MainCameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	int Empty;					// 64
	vec3 ViewDirection;			// 
	float Empty2;				// 
} u_MainCamera; 

layout(std430, binding = 0) buffer ModelData
{
	DrawObjectData Data[];
} Model; 

layout(std430, binding = 1) buffer OffsetData
{
	int Offset[];
} Object; 

#define PSM_SHADOW 0

void main()
{
	int objectIndex = Object.Offset[gl_DrawID] + gl_InstanceID;
	DrawObjectData modelData = Model.Data[objectIndex];
	mat4 modelMatrix = modelData.Matrix;


	mat4 viewShadow = u_ShadowCamera.ViewMatrix;
	mat4 projtionShadow = u_ShadowCamera.ProjectionMatrix;
	mat4 modelShadow = inverse(viewShadow);

	mat4 viewProjtionShadow = u_ShadowCamera.ViewProjectionMatrix;
	// viewProjtionShadow = viewShadow * projtionShadow;
	mat4 inverseViewProjtionShadow = inverse(viewProjtionShadow);

	

	mat4 viewMain = u_MainCamera.ViewMatrix;
	mat4 projtionMain  = u_MainCamera.ProjectionMatrix;

	mat4 viewProjtionMain = u_MainCamera.ViewProjectionMatrix;
	// viewProjtionMain = projtionMain * viewMain;
	mat4 inverseViewProjtionMain = inverse(viewProjtionMain);
	

	mat4 viewProjtion = viewProjtionShadow;
#if PSM_SHADOW

	vec3 lightDir = normalize( vec3(modelShadow[3].xyz) );

	vec3 viewLightDir = mat3(viewMain) * lightDir;
	vec4 projLightDir = projtionMain * vec4(viewLightDir, 0.0f);

	float z_n = projtionMain[3].z / (projtionMain[2].z - 1.0f);
    float z_f = projtionMain[3].z / (projtionMain[2].z + 1.0f);
    float scale = 2.0f / (z_f - z_n);

	mat4 T = mat4(1.0f);
    T[3].y = -projLightDir.y / projLightDir.w * scale;
    T[3].z = -projLightDir.z / projLightDir.w * scale;

	mat4 computePSMMatrix = T * inverseViewProjtionMain;

	viewProjtion = computePSMMatrix;
#endif
    vec4 worldPostion = modelMatrix * vec4( a_Postion , 1.0 );
	
	gl_Position = viewProjtion * worldPostion;

	

}

#type Fragment
#version 460 core

#define PSM_SHADOW 0


void main()
{
#if PSM_SHADOW
	vec3 projCoords = gl_FragCoord.xyz / gl_FragCoord.w;
	gl_FragDepth = projCoords.z;
#endif
}