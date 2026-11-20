#type Vertex
#version 460 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

layout(std140, binding = 0) uniform TransformObjectData
{
    mat4 Matrix;
    mat4 NormleMatrix;

    int EntityID;
	int Empty0;
	int Empty1;
	int Empty2;
} u_Model;


struct OutPut
{
	vec3 Position;	        // 12
	vec3 Normal;	        // 24
	vec2 Coords;	        // 32

	vec4 ShadowPosition;
};

layout(location = 0) out OutPut v_OutData;
layout(location = 5) out flat int v_EntityIndex;

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

const float g_Time = 1.16f * 0 + 1.16f * 0 + 1.16f * 0;
#define TAU 6.28318530718

void main()
{
    mat4 modelMatrix = u_Model.Matrix;
	mat4 normleMatrix = u_Model.NormleMatrix;

    vec2 coords = a_UV;
    vec4 pos = vec4( a_Postion.xyz , 1.0 );
    vec4 worldPostion = modelMatrix * pos;
   

	mat4 cameraViewProjtionMatric = u_Camera.ViewProjectionMatrix;
	mat4 ligthViewProjtionMatric = u_Shadow.ViewProjectionMatrix;

	vec4 modelCamerClipSpace = cameraViewProjtionMatric * worldPostion;
	vec4 ligthViewPos = ligthViewProjtionMatric * worldPostion;

	v_EntityIndex = u_Model.EntityID;
    v_OutData.Coords = coords;
	v_OutData.ShadowPosition = ligthViewPos;
	v_OutData.Position = vec3( worldPostion.xyz );
	v_OutData.Normal =  mat3( normleMatrix ) * a_Normals;

    
    
    gl_Position = modelCamerClipSpace;
}


#type Fragment
#version 460 core

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

layout(std140, binding = 3) uniform Materiel
{
	vec3 Color;
  	float Alpha;
  	float Shinines;
  	float Specular;
  	float AmbientLigthe;
  	int Empty;
} u_Materiel; 

layout(binding = 0) uniform sampler2D t_AlbedoTexture;
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
	bias = max(v * (1.0 - dot(normal, lightDir)), v * 0.85);
	return bias;
}

float calcShodowFactor(vec4 ligthCoordsPos, sampler2DShadow shadowMap, float bias)
{
	
	vec3 ligthCoords = shadowSTCoord(ligthCoordsPos);
	float z = ligthCoords.z;
	float Depth = 1.0f;

	float distenze = (z - bias);
	vec3 shadowCoords = vec3( ligthCoords.xy , distenze);


	Depth *= texture(shadowMap, shadowCoords).x;
	

	if(ligthCoords.y < 1.0 &&  ligthCoords.x < 1.0 && ligthCoords.x > 0.0 && ligthCoords.y > 0.0  && ligthCoords.z < 1.0 && ligthCoords.z > 0.0 )
	{
		return Depth;
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
	float gloss = 0.0f;
    vec3 V = cameraDir;
#if 0
        vec3 R = reflect( -L, N );
        float specularAmount = saturate( dot( V, R ) );
#else
        vec3 H = normalize( L + V );
        float specularAmount = saturate( dot( H, N ) );
#endif

#if 0
    float specularMultyplyer = min(1.0, defuseLambert);
	float specularExpoenent = specularMultyplyer * exp2(gloss * 6.0f) + 1.0f;
    vec3 specularLigth = vec3( pow( specularAmount , specularExpoenent ) );
	
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

#define ENABEL_SHADING_MIN 0
void main()
{
    EntityID = v_EntityIndex; 
	
    vec2 coords = v_Input.Coords;
	vec3 normale = normalize(v_Input.Normal);
	vec3 postion = v_Input.Position;
	Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    Color.rgba *= texture2D( t_AlbedoTexture, coords ).xyzw;
    Color.rgba *= vec4(u_Materiel.Color.rgb, u_Materiel.Alpha);

    vec3 ligthLocaleDir = normalize(u_Shadow.CamerPosition - postion);
	vec3 ligthDir;
	ligthDir = u_Shadow.ViewDirection;
	ligthDir = normalize(u_Shadow.CamerPosition);
	
	vec3 cameraDir; 
	cameraDir = u_Camera.ViewDirection;
	cameraDir = normalize(u_Shadow.CamerPosition - postion);
#if 0
	float shadowBias = calceBias(normale , ligthLocaleDir );
	float shadowValue = calcShodowFactor(v_Input.ShadowPosition, t_ShadowMap, shadowBias);
#if 0
	shadowValue += u_Materiel.AmbientLigthe + 0.1;
#endif
	Color.rgb *= (shadowValue+ 0.1);

#endif
#if 1
#if ENABEL_SHADING_MIN
	float colorSum = Color.r + Color.g + Color.b;
	if(colorSum > 0.1)
	{
#endif
		float ligthIntensitie = 0.75f;
		vec4 defiuseLigth_Lambert = defuseLigthing(normale, ligthDir, ligthIntensitie);
		vec3 defiuseaLigth = defiuseLigth_Lambert.xyz;
		float defuseLambert = defiuseLigth_Lambert.w;
		vec3 specularLigth = specularLigthing(normale, ligthDir, defuseLambert, ligthIntensitie, cameraDir);

		vec3 ligthColor = vec3(0.78f, 0.1f, 0.78f);
#if 0
		ligthColor.rgb = defiuseaLigth.rgb;
#elif 0
		ligthColor.rgb = specularLigth.rgb;
#elif 0
		ligthColor.rgb = u_Materiel.AmbientLigthe;
#elif 1
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
		YCbCr.x *= ligthColor.r;
		vec3 RGB  = ConvertYCbCrinRGB(YCbCr);
		
		Color.rgb = RGB;
#endif
#if ENABEL_SHADING_MIN
	}
	
#endif
#endif
    float gammaCorection = u_Camera.Gamma;
	Color.rgb = pow(Color.rgb, vec3(1.0f / gammaCorection));
}