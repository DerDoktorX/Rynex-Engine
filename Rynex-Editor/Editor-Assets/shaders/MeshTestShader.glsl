#type Vertex
#version 460 core

#define RY_GENRATE_MODEL_TEXTURE 0

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_Coords;
layout(location = 2) in vec3 a_Normals;


struct OutPut
{
	vec3 Position;	  
	vec3 Normal;	
    vec2 Coords; 
	

	vec4 ShadowPosition;

};

struct DrawObjectData
{
	
	// float Empty1;
	// float Empty2;
	// float Empty3;
	mat4 Matrix;
	mat4 NormleMatrix;
	int EnitityID;
	int MaterielID;
	// 
	int Empty0;
	int Empty1;
};


layout(location = 0) out OutPut v_OutData;
layout(location = 5) out flat int  v_outObjectIndex;
layout(location = 6) out flat int  v_EntityID;
layout(location = 7) out flat int  v_MaterilIndex;
// layout(location = 6) out vec4 v_ShadowPosition;

layout(std140, binding = 0) uniform CameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;				// 
} u_Camera; 




layout(std140, binding = 1) uniform ShadowCameraData 
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;				// 
} u_Shadow; 

layout(std430, binding = 0) buffer ModelData
{
	DrawObjectData Data[];
} Model; 

layout(std430, binding = 1) buffer OffsetData
{
	int Offset[];
} Object; 

const float g_Time = 1.16f * 0 + 1.16f * 0 + 1.16f * 0;
#define TAU 6.28318530718

#define SWAP_SHADOW_MAIN_CAMERA 0


vec2 GetCoordsCenter(vec2 coords)
{
	vec2 center = coords.xy * 2.0;
	center.xy -= vec2(1.0f);
	return center;
}


float GetCenterWave(vec2 coord)
{
	vec2 coordsCenter = GetCoordsCenter(coord);
	float timeScale = g_Time * 0.1f;

	float radialDistanc = length( coordsCenter );

	float wave = cos(radialDistanc - timeScale) * TAU * 0.5 + 0.5;
	wave *= 1.0f - radialDistanc;
	return wave;
}


float GetWaveC(float coordValue)
{
	float timeScale = g_Time * 0.1f;
	float timeCoordValue = coordValue - timeScale;
	float timeCoordTauValue = timeCoordValue * TAU;
	float cosInputValue=timeCoordTauValue * 5.0f;

	float wave = cos( cosInputValue );

	// wave *= 1-coordValue;
	return wave;
}

float GetWaveS(float coordValue)
{
	float wave = sin( ( coordValue - g_Time * 0.1) * TAU * 5.0 );
	// wave *= 1-coordValue;
	return wave;
}

vec2 CoordInUV(vec2 coord)
{
	// coord.xy = vec2(
	// 					0.0f <x> 1.0f, 
	//					0.0f <y> 1.0f
	//				);
	//
	// uv.xy = vec2(
	//				-1.0f <x> 1.0f, 
	//				-1.0f <y> 1.0f
	//			);
	float pointV = 2.0f;
	float lineV = pointV / 2.0f;
	vec2 uv1 = vec2(coord.x * pointV, coord.y * pointV );
	vec2 uv2 = vec2(uv1.x - lineV, uv1.y - lineV);
	
	return uv2;
	// return uv2;
	// return uv1;
}

void main()
{
    // mat4 model = mat4(
    //     1,0,0,0, 
    //     0,1,0,0, 
    //     0,0,1,0, 
    //     -1.5 + 3 * gl_InstanceID,0,0,1
    // );
	// v_outObjectIndex= 0;
	v_outObjectIndex = Object.Offset[gl_DrawID] + gl_InstanceID;
	
	DrawObjectData objectData = Model.Data[v_outObjectIndex];
	mat4 modelMatrix = objectData.Matrix;
	mat4 normleMatrix = objectData.NormleMatrix;
	int materielIndex = objectData.MaterielID;
	int entityID = objectData.EnitityID;
	v_MaterilIndex = materielIndex;
	v_EntityID = entityID;
	vec4 pos = vec4( a_Postion.xyz , 1.0 );

	

	vec2 coords = a_Coords;
	
	// float trasfomCoords = 0.5;
	// coords *=  trasfomCoords;
	// pos.z += (GetWaveS(coords.x)  + GetWaveC(coords.y)) * 1.125;
	// coords /= trasfomCoords;
	// int scaleCoords = 3;
	// coords *= 4.0 * scaleCoords;
	vec4 worldPostion;
	// if(v_outObjectIndex == 23)
    	worldPostion = modelMatrix * pos;
	// else
	// 	worldPostion = pos;


#if SWAP_SHADOW_MAIN_CAMERA
	mat4 cameraViewProjtionMatric = u_Shadow.ViewProjectionMatrix;
#else
	mat4 cameraViewProjtionMatric = u_Camera.ViewProjectionMatrix;
	mat4 cameraInvertseViewProjtionMatric = inverse(cameraViewProjtionMatric);
	mat4 viewCamera = u_Camera.ViewMatrix;
	mat4 projtionCamera = u_Camera.ProjectionMatrix;
#endif	



#if SWAP_SHADOW_MAIN_CAMERA
	mat4 ligthViewProjtionMatric = u_Camera.ViewProjectionMatrix;
#else
	mat4 ligthViewProjtionMatric = u_Shadow.ViewProjectionMatrix;
	mat4 viewShadow = u_Shadow.ViewMatrix;
	mat4 projtionShadow = u_Shadow.ProjectionMatrix;
	// ligthViewProjtionMatric = (ligthViewProjtionMatric);
#endif


	// ligthViewPos *= 0.5f;
	// ligthViewPos += 0.5f;

	

    
	vec4 camerClipSpace = cameraViewProjtionMatric * worldPostion;
	vec4 ligthViewPos;
	// ligthViewPos = ligthViewProjtionMatric * camerClipSpace;
	ligthViewPos = ligthViewProjtionMatric * worldPostion;

	v_OutData.Coords = coords;
	v_OutData.ShadowPosition = ligthViewPos;
	v_OutData.Position = vec3( worldPostion.xyz );
	// v_OutData.Normal =  mat3( transpose( inverse( modelMatrix ) ) ) * a_Normals;
	v_OutData.Normal =  mat3( normleMatrix ) * a_Normals;

#if RY_GENRATE_MODEL_TEXTURE

	vec2 coordsUV = CoordInUV(coords);
	vec4 uvClipScreen;
	uvClipScreen = vec4(coordsUV.xy, 0.0f, 1.0f);
	uvClipScreen = vec4(coords.xy, 0.0f, 1.0f);
	uvClipScreen = vec4(coords.xy * 2.0 - 1.0, 0.0, 1.0f);
	if(v_outObjectIndex == 5)
	{		
		uvClipScreen.z = 1.1f;
	}

	gl_Position = uvClipScreen;
#else
	
    gl_Position = camerClipSpace;
#endif
	// gl_Position = u_Camera.ViewProjectionMatrix * model * vec4( a_Postion , 1.0 );

		
}

#type Fragment
#version 460 core

#define RY_BINDLES_DETH_TEX 1
#define RY_LIGTH_ONLY 0
#define RY_NORMALE_ONLY 0
#define RY_ST_COORDS_ONLY 0
#define RY_GAMME_COREKTION 1
#define RY_FRAGMENT_COORDS_ONLY 0

#if RY_BINDLES_DETH_TEX
	#extension GL_ARB_bindless_texture : require
#endif

struct InPut
{
	vec3 Position;	        // 12
	vec3 Normal;	        // 24
	vec2 Coords;	        // 32

	vec4 ShadowPosition;
};

struct MaterielShaderData 
{
  vec3 Color;
  float Alpha;
  float Shinines;
  float Specular;
  float AmbientLigthe;
  int UseTexure;
};


layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;

layout(location = 0) in InPut v_Input;
layout(location = 5) in flat int v_inObjectIndex;
layout(location = 6) in flat int v_EntityID;
layout(location = 7) in flat int v_MaterielIndex;
// layout(location = 6) out vec4 v_ShadowPosition;


layout(std140, binding = 1) uniform ShadowCameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;				// 
} u_Shadow;

layout(std140, binding = 0) uniform CameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;				// 
} u_Camera; 

#if RY_BINDLES_DETH_TEX
layout(binding = 2, std430) readonly buffer BindlesAlbedoTextureSSBO {
	sampler2D albedoTexturesBindless[];
};	

layout(binding = 3, std430) readonly buffer BindlesShadowTextureSSBO {
	sampler2DShadow shadowTexturesBindless[];
};
	
#else
	layout(binding = 0) uniform sampler2D t_AlbedoTexture;
	layout(binding = 1) uniform sampler2DShadow t_ShadowMap;
#endif

layout(binding = 4, std430) readonly buffer MaterielSSBO {
		MaterielShaderData materiels[];
	};
// layout(binding = 0) uniform sampler2D DefaultTex;

vec3 shadowSTCoord(vec4 ligthCoordsPos)
{
    // vec3 ligthCoords = ligthCoordsPos.xyz / ligthCoordsPos.w;

	// ligthCoordsPos.w = 100.0;
    vec3 UVCoordsAndz = ligthCoordsPos.xyz / ligthCoordsPos.w;
 	// UVCoordsAndz = ligthCoordsPos.xyw;
	// UVCoordsAndz.z = 0.5f;
	UVCoordsAndz.rgb = vec3(0.5f * UVCoordsAndz.rgb + 0.5f);
	// UVCoordsAndz *= 0.5f;
	// UVCoordsAndz +=  0.5f;
	// UVCoordsAndz = UVCoordsAndz.xyz / ligthCoordsPos.w;
	
	
   
    return UVCoordsAndz;
}

float shadowPCF(sampler2DShadow shadowMap, vec3 shadowCoords, int pcfRange)
{
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -pcfRange; x <= pcfRange; ++x) 
	{
    	for(int y = -pcfRange; y <= pcfRange; ++y) 
		{
			vec2 coordsOffset = vec2( shadowCoords.xy + vec2(x, y) * texelSize.xy);
			vec3 projCoords = vec3( coordsOffset.xy, shadowCoords.z );

        	float depth = texture(shadowMap,  projCoords).r; 
        	shadow += depth;
    	}    
	}
	float sumePCF = pow(2 * pcfRange + 1, 2);
	shadow /= sumePCF;
	if(shadow < 0.1)
		shadow = 0.0;
	return shadow;
}

float shadowDefault(sampler2DShadow shadowMap, vec3 shadowCoords)
{
	float shadow = 0.0;
    shadow += texture(shadowMap, shadowCoords).r; 

	return shadow;
}

float calceBias(vec3 normal, vec3 lightDir)
{
	float bias = 0.0;
	float v = 0.00125;// 125f;
	bias = max(v * (1.0 - dot(normal, lightDir)), v * 0.85);
	return bias;
}

float calcShodowFactor(vec4 ligthCoordsPos, sampler2DShadow shadowMap, float bias)
{
	
	vec3 ligthCoords = shadowSTCoord(ligthCoordsPos);
	float z = ligthCoords.z;
	float Depth = 1.0f;
	// bias = 0.00115f;
	// bias = 0.003f;
	// bias = 0.00005f;
 	// bias = 0.0f;
	// bias = calceBias();
	float distenze = (z - bias);

	vec3 shadowCoords = vec3( ligthCoords.xy , distenze);

#if RY_BINDLES_DETH_TEX
	// sampler2DShadow shadowMap = shadowTexturesBindless[shadowIndex];
	Depth *= texture(shadowMap, shadowCoords).x;
	// Depth *= shadowDefault(shadowMap, shadowCoords);
	// Depth *= shadowPCF(shadowMap, shadowCoords, 9);
#else
	// sampler2DShadow shadowMap = ShadowMap;
	Depth *= texture(shadowMap, shadowCoords).x;
	// Depth *= shadowDefault(shadowMap, shadowCoords);
	// Depth *= shadowPCF(shadowMap, shadowCoords);
#endif
	return Depth;
	if(ligthCoords.y < 1.0 &&  ligthCoords.x < 1.0 && ligthCoords.x > 0.0 && ligthCoords.y > 0.0  && ligthCoords.z < 1.0 && ligthCoords.z > 0.0 )
	{
		// return vec3(0.0,Depth, 0.0);
		// return vec3(ligthCoords.x, ligthCoords.y, Depth);
		return 0.0;
	}
	#if 1
		return Depth;
	#else
		return 1.0;
	#endif
	
}



void main()
{
	

    EntityID = v_EntityID; 
	// if(v_inObjectIndex > 1 && v_inObjectIndex < 200)
	// 	Color = vec4( vec3(10.0f, 10.0f, 5.0f), 1.0f) * texture2D(DefaultTex, v_Input.Coords * 15.0).xyzw;
	// else
	vec2 coords = v_Input.Coords;
	vec3 normale = normalize(v_Input.Normal);
	vec3 postion = v_Input.Position;

	int materileIndex = -1; 
	materileIndex = v_MaterielIndex;
	// materileIndex = 1; 
	MaterielShaderData materiel;
	int materilSize = materiels.length();
	if(materileIndex != -1 && materileIndex < materilSize)
	{		
		materiel = materiels[materileIndex];
	}	
	else
	{
		materiel.Color = vec3(7.0f, 1.0f, 3.0f);
		materiel.Alpha = 0.5f;
		materiel.UseTexure = 0;
		materiel.Shinines = 0.0f;
		materiel.AmbientLigthe = 2.0f;
	}

	// Color = texture2D(DefaultTex, coords ).xyzw;
	int texIndex = -1;
	texIndex = materiel.UseTexure;
	// texIndex = 21;
	

#if RY_BINDLES_DETH_TEX
	int textureSize = albedoTexturesBindless.length();
	Color = vec4(1.0, 1.0, 1.0, 1.0);

	if(texIndex < -1)
	{
		texIndex = 0;
		materiel.Color = vec3(0.1f, 7.0f, 0.3f);
		materiel.Alpha = 0.5;
		materiel.Shinines = 0.0f;
		materiel.AmbientLigthe = 2.0f;
	}
	else if(texIndex == -1)
	{
		texIndex = 0;
		materiel.Color = vec3(1.0f, 3.0f, 7.0f);
		materiel.Alpha = 0.5;
		materiel.Shinines = 0.0f;
		materiel.AmbientLigthe = 2.0f;
	}
	else if(texIndex > textureSize)
	{
		texIndex = 0;
		materiel.Color = vec3(1.0f, 3.0f, 7.0f);
		materiel.Alpha = 1.;
		materiel.Shinines = 0.0f;
		materiel.AmbientLigthe = 5.0f;
	}

	sampler2D albedoTexture = albedoTexturesBindless[texIndex];
	Color = texture2D( albedoTexture, coords ).xyzw;
	// Color.rgba = vec4(coords.x, coords.y , 0.0, 1.0);

#else
	if(texIndex != -1 && texIndex < 27)
	{	
		Color = texture2D( t_AlbedoTexture, coords ).xyzw;
	}
	else
	{
		Color = vec4(0.1, 0.1, 0.1, 1.0);
		materiel.Color = vec3(1.0f, 1.0f, 70.0f);
		materiel.Alpha = 0.5;
	}
	coords.xy *= 10.0;
	Color = texture2D( t_AlbedoTexture, coords ).xyzw;
#endif

	Color.rgba *= vec4(materiel.Color.rgb, materiel.Alpha);

	// Color = vec4(1.0f);
	// Color.rgb *= vec3( coords, 0.0 );
	// Color.rgb *= vec3( normale ) * 1.0;
	// Color.rgb *= vec3(0.5f, 0.025f, 0.025f);

#if RY_LIGTH_ONLY
	Color.rgba = vec4(1.0, 1.0, 1.0, 1.0);
#endif


vec3 ligthDir = normalize(u_Shadow.CamerPosition - postion);

#if RY_BINDLES_DETH_TEX
	sampler2DShadow shadowMap = shadowTexturesBindless[0];
	float shadowValue = calcShodowFactor(v_Input.ShadowPosition, shadowMap, calceBias(normale , ligthDir ));
	shadowValue += materiel.AmbientLigthe;
	Color.rgb *= shadowValue;
#else
	Color.rgb *= calcShodowFactor(v_Input.ShadowPosition, t_ShadowMap, calceBias(normale , ligthDir ));
#endif

#if RY_NORMALE_ONLY

	Color.rgb = vec3( normale );
#endif

#if RY_ST_COORDS_ONLY
	vec2 uvCoords = vec2(coords.xy);
	Color.rgba = vec4( uvCoords.xy, 0.0f, 1.0f );
#endif

	
#if RY_GAMME_COREKTION
	float gammaCorection = u_Camera.Gamma;
	// gammaCorection = 2.2;

	Color.rgb = pow(Color.rgb, vec3(1.0f / gammaCorection));
#endif

}