#type Vertex
#version 460 core
// #define RY_OLD_CAMER_PACKEGE
#define RY_ENABLE_SSOB_MODEL_BUFFER 0
#define RY_ENABLE_SINGLE_ENEITY_ID_INT

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

#if !RY_ENABLE_SSOB_MODEL_BUFFER
	struct EntityIDData {
	int EntityID;
	int Empty0;
	int Empty1;
	int Empty2;
	};


	layout(location = 3) in mat4 a_ModelMatrix;

	layout(location = 7) in mat4 a_NormleMatrix;
#ifdef RY_ENABLE_SINGLE_ENEITY_ID_INT
	layout(location = 11) in ivec4 a_EntityID;
#else
	layout(location = 11) in int a_EntityID;
#endif
#endif
#if RY_ENABLE_SSOB_MODEL_BUFFER

struct TransformObjectData{
	mat4 Matrix;
    mat4 NormleMatrix;

    int EntityID;
	int Empty0;
	int Empty1;
	int Empty2;
};

layout(std430, binding = 0) buffer ModelData
{
	TransformObjectData Object[];
} u_Model;

#endif

struct OutPut
{
	vec3 Position;	        // 12
	vec3 Normal;	        // 24
	vec2 Coords;	        // 32

	vec4 ShadowPosition;
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

layout(std140, binding = 2) uniform ShadowCameraData 
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;
} u_Shadow; 


#else

layout(std140, binding = 1) uniform CameraData
{	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	mat4 ViewProjectionMatrix;	
	vec4 CamerPosition;			
	vec4 ViewDirection;			
} u_Camera;

layout(std140, binding = 2) uniform ShadowCameraData
{	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	mat4 ViewProjectionMatrix;	
	vec4 CamerPosition;			
	vec4 ViewDirection;			
} u_Shadow;

#endif

const float g_Time = 1.16f * 0 + 1.16f * 0 + 1.16f * 0;
#define TAU 6.28318530718

void main()
{
#if RY_ENABLE_SSOB_MODEL_BUFFER
	TransformObjectData model = u_Model.Object[gl_InstanceID];


    mat4 modelMatrix = model.Matrix;
	mat4 normleMatrix = model.NormleMatrix;
#else
    mat4 modelMatrix = a_ModelMatrix;
	mat4 normleMatrix  = a_NormleMatrix;
#endif

    vec2 coords = a_UV;
    vec4 pos = vec4( a_Postion.xyz , 1.0 );
    vec4 worldPostion = modelMatrix * pos;
   
	// mat4 cameraViewProjtionMatric = u_Camera.ViewMatrix * u_Camera.ProjectionMatrix;
	// mat4 cameraViewProjtionMatric = u_Camera.ProjectionMatrix * u_Camera.ViewMatrix;

	mat4 cameraViewProjtionMatric = u_Camera.ViewProjectionMatrix;
	mat4 ligthViewProjtionMatric = u_Shadow.ViewProjectionMatrix;

	vec4 modelCamerClipSpace = cameraViewProjtionMatric * worldPostion;
	vec4 ligthViewPos = ligthViewProjtionMatric * worldPostion;

#if RY_ENABLE_SSOB_MODEL_BUFFER
	v_EntityIndex = model.EntityID;
#elif defined(RY_ENABLE_SINGLE_ENEITY_ID_INT)

	v_EntityIndex = a_EntityID.x;
#endif
    v_OutData.Coords = coords;
	v_OutData.ShadowPosition = ligthViewPos;
	v_OutData.Position = vec3( worldPostion.xyz );
	v_OutData.Normal =  mat3( normleMatrix ) * a_Normals;

    
    
    gl_Position = modelCamerClipSpace;
}


#type Fragment
#version 460 core
// #define RY_OLD_CAMER_PACKEGE
// #define RY_ALBEDO_TEXTURE_ARRAY
struct InPut
{
	vec3 Position;	        // 12
	vec3 Normal;	        // 24
	vec2 Coords;	        // 32

	vec4 ShadowPosition;
};

layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;

layout(location = 0) in InPut v_Input;
layout(location = 5) in flat int v_EntityIndex;

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

layout(std140, binding = 2) uniform ShadowCameraData 
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	float Gamma;				// 64
	vec3 ViewDirection;			// 
	float Empty;
} u_Shadow; 


#else

layout(std140, binding = 1) uniform CameraData
{	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	mat4 ViewProjectionMatrix;	
	vec4 CamerPosition;			
	vec4 ViewDirection;			
} u_Camera;

layout(std140, binding = 2) uniform ShadowCameraData
{	
	mat4 ViewMatrix;			
	mat4 ProjectionMatrix;		
	mat4 ViewProjectionMatrix;	
	vec4 CamerPosition;			
	vec4 ViewDirection;			
} u_Shadow;

#endif

layout(std140, binding = 3) uniform Materiel
{
	vec3 Color;
  	float Alpha;
  	float Shinines;
  	float Specular;
  	float AmbientLigthe;
  	int Empty;
} u_Materiel; 

#ifdef RY_ALBEDO_TEXTURE_ARRAY
	layout(binding = 0) uniform sampler2DArray t_AlbedoTextureArray;
#else
	layout(binding = 0) uniform sampler2D t_AlbedoTexture;
#endif

layout(binding = 1) uniform sampler2DShadow t_ShadowMap;

vec3 shadowSTCoord(vec4 ligthCoordsPos)
{
    vec3 UVCoordsAndz = ligthCoordsPos.xyz / ligthCoordsPos.w;
	UVCoordsAndz.rgb = vec3(0.5f * UVCoordsAndz.rgb + 0.5f);
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
	return shadow;
}

float shadowDefault(sampler2DShadow shadowMap, vec3 shadowCoords)
{
	float shadow = 0.0;
    shadow += texture(shadowMap,  shadowCoords).r; 
	return shadow;
}

float calceBias(vec3 normal, vec3 lightDir)
{
	float bias = 0.0;
	float v = 0.00125;// 125f;
	bias = max(v * (1.0 - dot(normal, lightDir)), v);
	return bias;
}
bool isInProjtionSpace(vec3 ligthCoords)
{
	return 0.0 < ligthCoords.x  && ligthCoords.x < 1.0  
	&& 0.0 < ligthCoords.y && ligthCoords.y < 1.0  
	&& 0.0 < ligthCoords.z && ligthCoords.z < 1.0;
}

float calcShodowFactor(vec4 ligthCoordsPos, sampler2DShadow shadowMap, float bias)
{
	
	vec3 ligthCoords = shadowSTCoord(ligthCoordsPos);
	float z = ligthCoords.z;
	float Depth = 1.0f;

	float distenze = (z - bias);
	vec3 shadowCoords = vec3( ligthCoords.xy , distenze);

	
	Depth *= texture(shadowMap, shadowCoords).x;
	// Depth *= textureProj(shadowMap, vec4(ligthCoords, 1.0)).x;

	if(isInProjtionSpace(ligthCoords))
	{
		if(Depth < shadowCoords.z)
		{
			return 0.0;
		}
		return 1.0;
	}
#if 1
	return 1.0;
#else
	return Depth;
#endif
}

float saturate(float value)
{
    return clamp(value, 0.0, 1.0);
}

vec4 defuseLigthing(vec3 normale, vec3 ligtheDir, float ligthIntensitie)
{
	vec3 ligthColor = vec3(1.0);
    vec3 N = normale;
    vec3 L = ligtheDir;
    float lambert = saturate( dot( N, L ) );
    vec3 defuseLigth = vec3(lambert);
    defuseLigth *= ligthColor;
    defuseLigth *= ligthIntensitie;
    
    // return vec3(0.);
    // return vec3( lambert );
    return vec4(defuseLigth.xyz, lambert);
}

vec3 specularLigthing(vec3 normale, vec3 ligtheDir, float defuseLambert, float ligthIntensitie, vec3 cameraDir)
{
	vec3 ligthColor = vec3(1.0);
    vec3 N = normale;
    vec3 L = ligtheDir;
	float gloss = 0.294f;
	// gloss = 0.0;
	// gloss = 0.492f;
	// gloss = 0.125f;
	// gloss = 0.25f;
	// gloss = 0.325f;
	// gloss = 0.5f;
	gloss = 1.0f;
	// gloss = 2.0f;
	// gloss = 3.95f;
	// gloss = 7.91f;
    vec3 V = cameraDir;
	float lambert = defuseLambert;
#if 0
        vec3 R = reflect( -L, N );
        float specularAmount = saturate( dot( V, R ) );
#else
        vec3 H = normalize( L + V );
        float specularAmount = saturate( dot( H, N ) );
#endif

#if 0

    float specularMultyplyer = min(1.0, defuseLambert) ;
	if(defuseLambert < 0)
		specularMultyplyer = 1.0f;
	else
		specularMultyplyer = 0.0f;
		
	float specularExpoenent = specularMultyplyer * exp2(gloss * 6.0f);
    vec3 specularLigth = vec3( pow( specularAmount , specularExpoenent ) );
	
    specularLigth *= ligthColor;
    specularLigth *= ligthIntensitie;

#elif 1
	
 	vec3 specularLigth;
	// if(lambert > 0)
	if(0 < lambert)
	{	
		specularLigth = vec3(specularAmount * 1.0); 
	}
	else
	{
		specularLigth = vec3(specularAmount * 0.0);
	}
	
	float specularExpoenentAdd = 2.0f;
	float glossMult = 11.0f;
		
	float specularExpoenent = exp2(gloss * glossMult) + specularExpoenentAdd;
   	specularLigth = vec3( pow( specularAmount , specularExpoenent ) );
	
    specularLigth *= ligthColor;
    specularLigth *= ligthIntensitie;
#else

	float specularMultyplyer = min(1.0, specularAmount);
    vec3 specularLigth = vec3( pow( specularMultyplyer , gloss ) );
    specularLigth *= ligthColor;
    specularLigth *= ligthIntensitie;

#endif
    // return vec3( V );
    // return vec3( R );
    // return vec3(0.);
    // return vec3( specularAmount );
    return specularLigth;
}

vec3 ConvertRGBinYCbCr(vec3 rgb)
{
		float R = rgb.r;
		float G = rgb.g;
		float B = rgb.b;

		vec3 YValue = vec3(0.299, 0.587,0.144);
		float Y = 0.299*R+0.587*G+0.144*B;
		vec3 YMul = vec3( 0.299*R, 0.587*G, 0.144*B);
		// vec3 YMul =  rgb * YValue;
		// Y = YMul.x + YMul.y + YMul.z;

		vec3 CbValue = vec3(-0.168736, -0.331264,0.5);
		float Cb= -0.168736*R-0.331264*G+0.5*B;
		vec3 CbMul = rgb * CbValue;
		Cb = CbMul.x + CbMul.y + CbMul.z;

		vec3 CrValue = vec3(0.5, -0.418688,-0.081312);
		float Cr = 0.5*R-0.418688*G-0.081312*B;
		vec3 CrMul = rgb * CrValue;
		Cr = CrMul.x + CrMul.y + CrMul.z;

		vec3 YCbCr = vec3(Y, Cb, Cr);
		return YCbCr;
}

vec3 ConvertYCbCrinRGB(vec3 yCbCr)
{
		float Y = yCbCr.r;
		float Cb = yCbCr.g;
		float Cr = yCbCr.b;

		vec3 YValue = vec3(0.299, 0.587,0.144);
		vec3 CrValue = vec3(0.5, -0.418688,-0.081312);
		vec3 CbValue = vec3(-0.168736, -0.331264,0.5);
		YValue = YValue;
		CrValue = CrValue;
		CbValue = CbValue;

		
#if 0
		float R = YValue.x * Y 
		+ CbValue.x * Cb 
		+ CrValue.x * Cr;


		float G = YValue.y * Y 
		+ CbValue.y * Cb 
		+ CrValue.y * Cr;

		float B = YValue.z * Y
		+ CbValue.z * Cb 
		+ CrValue.z * Cr;
#else
		float R = Y + 1.402 * Cr;
		float G = Y - 0.344136 * Cb - 0.714136 * Cb;
		float B = Y + 1.772 * Cb;
		
#endif

		vec3 RGB = vec3(R, G, B);
		return RGB;
}

#define ENABEL_SHADING_MIN 1
void main()
{
#if 0
	int modelPostionMatrixIndex = 3;
	int viewDirMatrixIndex = 2;
	float multyPlayViewDir = -1.0;
	mat4 shadowModel = inverse(u_Shadow.ViewMatrix);
	vec3 shadowPos = shadowModel[modelPostionMatrixIndex].xyz;
	vec3 shadowViewDir = u_Shadow.ViewMatrix[viewDirMatrixIndex].xyz;
	shadowViewDir.z *= -1.0;

	mat4 cameraModel = inverse(u_Camera.ViewMatrix);
	vec3 cameraPos = cameraModel[modelPostionMatrixIndex].xyz;
	vec3 cameraViewDir = vec3(
		u_Camera.ViewMatrix[0].z,
		u_Camera.ViewMatrix[1].z,
		u_Camera.ViewMatrix[2].z
	);
	// cameraViewDir.z *= -1.;
#endif
    EntityID = v_EntityIndex; 
	
    vec2 coords = v_Input.Coords;
	vec3 normale = normalize(v_Input.Normal);
	vec3 postion = v_Input.Position;
	Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef RY_ALBEDO_TEXTURE_ARRAY
	const int textureIndex = 0;
	const float textureIndexF = int(textureIndex);
    Color.rgba *= texture( t_AlbedoTextureArray, vec3(coords, textureIndexF) ).xyzw;
#else
    Color.rgba *= texture2D( t_AlbedoTexture, coords ).xyzw;
#endif

	// Color.rgba *= vec4(u_Materiel.Color.rgb, u_Materiel.Alpha);


	vec3 ligthLocaleDir;
    ligthLocaleDir = -normalize(u_Shadow.CamerPosition.xyz - postion);
	// ligthLocaleDir = -normalize(shadowPos.xyz - postion);
	vec3 ligthDir;
	ligthDir = u_Shadow.ViewDirection.xyz;
	// ligthDir = normalize(u_Shadow.CamerPosition.xyz);
	// ligthDir = normalize(shadowPos);
	// ligthDir = shadowViewDir;
	vec3 cameraDir; 
	cameraDir = u_Camera.ViewDirection.xyz;
	// cameraDir = normalize(u_Camera.CamerPosition.xyz - postion);
	// cameraDir = cameraViewDir;
	float shadowValue = 1.0;
	float finelShadowValue = shadowValue;
#if 1
	float shadowBias = calceBias(normale , ligthDir );
	// shadowBias = 0.0;
	shadowValue = calcShodowFactor(v_Input.ShadowPosition, t_ShadowMap, shadowBias);
	
	
	finelShadowValue = shadowValue;
#if 0
	finelShadowValue = shadowValue + u_Materiel.AmbientLigthe;
#endif

	Color.rgb *= finelShadowValue;

#endif

#if 1

#if ENABEL_SHADING_MIN
#if 0
	if(shadowValue != 0.0)
#elif 1
	if(0.5 < shadowValue)
#else
	if(shadowValue != 1.0)
#endif
	{
#endif
		float ligthIntensitie = 0.75f;
		// ligthIntensitie = 1.0f;

		vec3 ligthColor = vec3(0.78f, 0.1f, 0.78f);
		// ligthColor = vec3(1.0f, 1.0f, 1.0f);

		vec4 defiuseLigth_Lambert = defuseLigthing(normale, ligthDir, ligthIntensitie);
		vec3 defiuseaLigth = defiuseLigth_Lambert.xyz;
		float defuseLambert = defiuseLigth_Lambert.w;
		vec3 specularLigth = specularLigthing(normale, ligthDir, defuseLambert, ligthIntensitie, cameraDir);

		
#if 0
		ligthColor.rgb = defiuseaLigth.rgb;
#elif 0
		ligthColor.rgb = specularLigth.rgb;
#elif 0
		ligthColor.rgb = u_Materiel.AmbientLigthe;
#elif 0
		ligthColor.rgb = defiuseaLigth.rgb * specularLigth.rgb;
		
#elif 0
		ligthColor.rgb = defiuseaLigth.rgb + specularLigth.rgb;
#endif

#if 0
		Color.rgb *= ligthColor;
#elif 0
		Color.rgb += ligthColor;
#elif 0
		Color.rgb = ligthColor;
#elif 0
		vec3 YCbCr = ConvertRGBinYCbCr(Color.rgb);
		// YCbCr.x += u_Materiel.AmbientLigthe;
		vec3 RGB  = ConvertYCbCrinRGB(YCbCr);

		Color.rgb = YCbCr;
		Color.rgb = RGB;
#elif 0
		vec3 YCbCr = ConvertRGBinYCbCr(Color.rgb);
		YCbCr.rgb *= ligthColor.r;
		vec3 RGB  = ConvertYCbCrinRGB(YCbCr);
		
		Color.rgb = RGB;



#elif 0
		Color.rgb = defiuseaLigth.rgb * Color.rgb;
		Color.rgb += specularLigth.rgb;
#elif 1
		Color.rgb = defiuseaLigth.rgb * Color.rgb + specularLigth.rgb;
		// Color.g = 0.0;
#elif 0
		Color.rgb = defiuseaLigth.rgb * Color.rgb * specularLigth.rgb;
#elif 0
		Color.rgb = defiuseaLigth.rgb + Color.rgb + specularLigth.rgb;

#endif

#if ENABEL_SHADING_MIN
	}
#endif

#endif
	// Color.rgb = cameraDir.rgb;
	//Color.rg = coords;
	//Color.b = 0.0;
	//
	//Color.a = 1.0;
#ifdef RY_OLD_CAMER_PACKEGE
    float gammaCorection = u_Camera.Gamma;
#else
    float gammaCorection = 2.2;
#endif
	Color.rgb = pow(Color.rgb, vec3(1.0f / gammaCorection));
	
}