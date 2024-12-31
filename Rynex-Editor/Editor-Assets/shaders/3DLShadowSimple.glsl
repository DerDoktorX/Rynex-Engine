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
    vec4 PositionLigthe;
};

struct DrirectionLigthData
{
    mat4 ShadowMatrix;
	vec4 Color;
    vec3 Position;
    float Intensitie; 
};
layout(std140, binding = 3) uniform DrirectionLigthVS 
{
	int Aktive;
	// ivec3 Empty;
    DrirectionLigthData Direction[8];
} DrirectionelL;


layout(location = 0) out OutPut vOutData;

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
    if(DrirectionelL.Aktive == 1)
    {
        vOutData.PositionLigthe = DrirectionelL.Direction[0].ShadowMatrix * vec4(worldPostion.xyz, 1.0f);
    }
    else
    {
        vOutData.PositionLigthe = vec4(-1.0, -2., -3.0, -4.0);
    }

    vOutData.Coords = a_UV;
	

	vOutData.Position = vec3(worldPostion.xyz);
	vOutData.Normal =  mat3( transpose( inverse( Model.ModelMatrix ) ) ) * a_Normals;
    gl_Position = Camera.ViewProjectionMatrix * worldPostion;
}

#type Fragment
#version 450 core



struct InPut
{
	vec3 Position;	        // 12
	vec3 Normal;	        // 24
	vec2 Coords;	        // 32
    vec4 PositionLigthe;	// 32
};


struct DrirectionLigthData
{
    mat4 ShadowMatrix;
	vec4 Color;
    vec3 Position;
    float Intensitie; 
};

layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;

layout(location = 0) in InPut vData;

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

layout(std140, binding = 2) uniform AmbientLigth 
{
	vec3 Color;
    float Intensitie;
} AmbientL;

layout(std140, binding = 3) uniform DrirectionLigthFS 
{
	int Aktive;
	// ivec3 Empty;
    DrirectionLigthData Direction[8];
} DrirectionelL;


layout(binding = 6) uniform sampler2D Texture_Diffuse;
// layout(sampler2D, binding = 1) uniform sampler2D AlbedoMap; 
// layout(sampler2D, binding = 2) uniform sampler2D NormalMap; 
// layout(sampler2D, binding = 3) uniform sampler2D RoughnessMap; 
// layout(sampler2D, binding = 4) uniform sampler2D RoughnessMap; 
layout(binding = 7) uniform sampler2DShadow ShadowMap; 
// layout(sampler2D, binding = 6) uniform sampler2D LigthMap;


vec3 drictionelLigth(int index)
{
    DrirectionLigthData ligth = DrirectionelL.Direction[index];
    float shadow  = 1.0f;
    if(index == 0)
    {
       shadow *= textureProj(ShadowMap, vData.PositionLigthe);
    }

    vec3 ambinet = AmbientL.Color.rgb * AmbientL.Intensitie;

    vec3 normal = normalize( vData.Normal );

    vec3 ligthDir = normalize( ligth.Position.xyz );


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(Camera.CamerPosition - vData.Position);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), Model.Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * Model.Specular);
    
    return vec3((ambinet + diffuse * (1.0f - shadow ) + specular ) * ligth.Intensitie * (1.0f - shadow ));
}


void main()
{
    EntityID = Model.EntityID; 

    Color = vec4(1,0,0,1);

    // vec4 fragPosLigth = vData.PositionLigthe;
    // float shadow = 0.0f;
    // vec3 ligthCoords = fragPosLigth.xyz / fragPosLigth.w;
    // if(ligthCoords.z <= 1.0f)
    // {
    //     ligthCoords = (ligthCoords + 1.0f) / 2.0f;
    // 
    //     float clostDepth = texture(ShadowMap, ligthCoords.xy).r;
    //     float currentDepth = ligthCoords.z;
    // 
    //     if(currentDepth >  clostDepth)
    //     {
    //         shadow = 1.0f;
    //     }
    // }
    // Color.xyz *= (1.0f - shadow);
    // Color.xyz *=  textureProj(ShadowMap, vData.PositionLigthe)-0.1f;
	Color.xyz *= drictionelLigth(0);
}