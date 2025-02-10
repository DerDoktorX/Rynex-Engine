// PBR-Material.glsl
#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

struct OutPut
{
	vec3 Position;	// 12
	vec3 Normal;	// 24
	vec2 Coords;	// 32
};

layout(location = 0) out OutPut vOutData;

layout(shared, binding = 0) uniform CameraData 
{
	mat4 ViewProjectionMatrix;
	mat4 ViewMatrix;		
	mat4 ProjectionMatrix;	
	vec3 CamerPosition;		
	int Empty;				
} Camera; 

layout(shared, binding = 1) uniform ModelDataVS
{
	mat4 ModelMatrix;	
	vec3 Color;			
	float Alpha;		
	float Emission;		
	float Matalness;	
	float Roughness;	
	float Shinines;
	float Specular;
	
	int EntityID;		
	ivec2 Empty;
	
} Model; 



void main()
{
	vec4 worldPostion = Model.ModelMatrix * vec4( a_Postion , 1.0 );

	vOutData.Position = vec3(worldPostion.xyz);
	vOutData.Normal =  mat3( transpose( inverse( Model.ModelMatrix ) ) ) * a_Normals;
	gl_Position = Camera.ViewProjectionMatrix * worldPostion;
}

#type Fragment
#version 450 core


struct InPut
{
	vec3 Position;	// 12
	vec3 Normal;	// 24
	vec2 Coords;	// 32
};


struct DrirectionLigthData
{
	vec4 Color;
    vec3 Position;
    float Intensitie; 
};

struct PointLigthData
{
    vec3 Color;
    float Distence;
    vec3 Position;
    float Intensitie;
};

struct SpotLigthData 
{
    vec3 Color;
    float Intensitie;
    vec3 Position;
    float Distence;
    vec3 Direction;
    float Inner;
    float Outer;
};


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
	float Emission;		
	float Matalness;	
	float Roughness;	
	float Shinines;
	float Specular;
	
	int EntityID;		
	ivec2 Empty;
	
} Model; 


layout(shared, binding = 2) uniform AmbientLigth 
{
	vec3 Color;
    float Intensitie;
} AmbientL;

layout(shared, binding = 3) uniform DrirectionLigth 
{
	int Aktive;
	// ivec3 Empty;
    DrirectionLigthData Direction[8];
} DrirectionelL;

layout(shared, binding = 4) uniform PointLigth 
{
	int Aktive;
	// ivec3 Empty;
	PointLigthData Point[32];
} PointL;

layout(shared, binding = 5) uniform SpotLigth 
{
    int Aktive;
	// ivec3 Empty;
    SpotLigthData Spot[32];
} SpotL;

// layout(sampler2D, binding = 1) uniform sampler2D AlbedoMap; 
// layout(sampler2D, binding = 2) uniform sampler2D NormalMap; 
// layout(sampler2D, binding = 3) uniform sampler2D RoughnessMap; 
// layout(sampler2D, binding = 4) uniform sampler2D RoughnessMap; 
// layout(sampler2D, binding = 5) uniform sampler2D ShadowMap; 
// layout(sampler2D, binding = 6) uniform sampler2D LigthMap; 

layout(location = 0) in InPut vData;

layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;

const float PI = 3.14159265359;

float distrbutionGXX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float demom = NdotH * NdotH * (a2 - 1.0);
	demom = PI * demom * demom; 
	return a2 / max( demom, 0.00000001 );
}

float geomtrySmith(float NdotV, float NdotL, float roughness)
{
	float r = roughness - 1.0;
	float k = ( r * r ) / 8.0;
	float ggx1 = NdotV * ( NdotV * ( 1.0 - k ) + k );
	float ggx2 = NdotL * ( NdotL * ( 1.0 - k ) + k );
	return ggx1 * ggx2;
}

vec3 frenselSchlick(float HdotV, vec3 baseReflectivity)
{
	return baseReflectivity + ( 1.0  - baseReflectivity ) * pow( 1.0 - HdotV, 5.0 );
}

vec3 drirectionelLigthing(vec3 N, vec3 V, vec3 baseReflectivity, float metalic, vec3 albedo)
{
	float roughness = Model.Roughness;
	vec3 result = vec3( 0.0 );
	int Aktive = DrirectionelL.Aktive;
	vec3 worldPostion = vData.Position;
	for( int i = 0; i < Aktive ; i++ )
	{
		DrirectionLigthData ligthData = DrirectionelL.Direction[i];
		
		vec3 ligtheOffset = ligthData.Position - worldPostion;
		vec3 L = normalize( ligtheOffset );
		vec3 H = normalize( V + L );
		float distence = length( ligtheOffset );
		float attenuation = 1.0 / ( distence * distence );
		vec3 radiance = ( ligthData.Color.rgb * ligthData.Intensitie ) * attenuation;

		float NdotV = max( dot( N , V ) , 0.0000001 );
		float NdotL = max( dot( N , L ) , 0.0000001 );
		float HdotV = max( dot( H , V ) , 0.0 );
		float NdotH = max( dot( N , H ) , 0.0 );

		float D = distrbutionGXX( NdotH , roughness );
		float G = geomtrySmith( NdotV , NdotL , roughness );
		vec3 F = frenselSchlick( HdotV , baseReflectivity );

		vec3 specular = D * G * F;
		specular /= 4.0 * NdotV * NdotL;

		vec3 kD = vec3( 1.0 ) - F;
		kD *= 1.0 - metalic;
		result += ( kD * albedo / PI + specular ) * radiance * NdotL;
		//result += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	return result;
}

vec3 ambientLigthing( vec3 albedo )
{
	return (AmbientL.Color * AmbientL.Intensitie) * albedo;
}

vec3 tonemapping( vec3 color )
{
	return color / ( color + vec3( 1.0 ) );
}

vec3 gamarCorrect( vec3 color )
{
	return pow( color , vec3( 1.0 / 2.2 ) );
}

vec3 Ligth()
{
	vec3 N = normalize( vData.Normal );
	vec3 V = normalize( Camera.CamerPosition - vData.Position );
	vec3 albedo = Model.Color;
	float metalic = Model.Matalness;
	vec3 baseReflectivity  = mix( vec3( 0.04 ) , albedo, metalic );
	vec3 Lo = vec3( 0.0 );
	Lo += drirectionelLigthing( N , V , baseReflectivity , metalic , albedo );
	Lo += ambientLigthing( albedo );

	return Lo;
}

void main()
{
	EntityID = Model.EntityID;
	vec3 color = vec3( 0.0 );
	color += Ligth();
	color = tonemapping( color );
    color = gamarCorrect( color );
	Color = vec4( color , Model.Alpha);

}
