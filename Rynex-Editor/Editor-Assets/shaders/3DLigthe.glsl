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
	vec4 Color;
    vec3 Position;
    float Intensitie; 
    mat4 ShadowMatrix;
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
        vOutData.PositionLigthe = DrirectionelL.Direction[0].ShadowMatrix * worldPostion;
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



layout(std140, binding = 4) uniform SpotLigth 
{
    int Aktive;
	// ivec3 Empty;
    SpotLigthData Spot[32];
} SpotL;

layout(std140, binding = 5) uniform PointLigth 
{
	int Aktive;
	// ivec3 Empty;
	PointLigthData Point[32];
} PointL;

layout(binding = 6) uniform sampler2D Texture_Diffuse;
// layout(sampler2D, binding = 1) uniform sampler2D AlbedoMap; 
// layout(sampler2D, binding = 2) uniform sampler2D NormalMap; 
// layout(sampler2D, binding = 3) uniform sampler2D RoughnessMap; 
// layout(sampler2D, binding = 4) uniform sampler2D RoughnessMap; 
layout(binding = 7) uniform sampler2DShadow ShadowMap; 
// layout(sampler2D, binding = 6) uniform sampler2D LigthMap;


vec3 pointLigth(int index)
{
    PointLigthData ligth = PointL.Point[index];
    vec3 ambinet = AmbientL.Color.rgb * AmbientL.Intensitie;

    vec3 ligthpos = ligth.Position; //vec3(0., 7.97,23.51);
    vec3 ligthRelativ = ligthpos.xyz - vData.Position;

    float dist = length(ligthRelativ);
    float a = ligth.Distence; // 1.00;
    float b = ligth.Intensitie; //0.04;
    float inner = 1.0f / (a * (dist * dist) + b * dist + 1.0f);

    vec3 normal = normalize(vData.Normal);

    vec3 ligthDir = normalize(ligthRelativ );


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(Camera.CamerPosition - vData.Position);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), Model.Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * Model.Specular) ;

    return vec3(( (ambinet + diffuse * inner) + (specular * inner)));
}

vec3 drictionelLigth(int index)
{
    DrirectionLigthData ligth = DrirectionelL.Direction[index];
    // float shadow  = 0.0f;
    // if(index == 1)
    // {
    //    shadow = textureProj(ShadowMap, vData.PositionLigthe);
    // }

    vec3 ambinet = AmbientL.Color.rgb * AmbientL.Intensitie;

    vec3 normal = normalize( vData.Normal );

    vec3 ligthDir = normalize( ligth.Position.xyz );


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(Camera.CamerPosition - vData.Position);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), Model.Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * Model.Specular);

    return vec3((ambinet + diffuse + specular) * ligth.Intensitie );
    // return vec3((ambinet + diffuse * (1.0f - shadow ) + specular ) * ligth.Intensitie * (1.0f - shadow ));
}


vec3 spotLigth(int index)
{
    SpotLigthData ligth = SpotL.Spot[index];
    vec3 ambinet = AmbientL.Color.rgb * AmbientL.Intensitie;

    float outerCone = ligth.Outer;
    float innerCone = ligth.Inner;

    vec3 normal = normalize(vData.Normal);

    vec3 ligthDir = normalize(ligth.Position.xyz - vData.Position);


    float diff = max( dot(normal, ligthDir), 0.0f );
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(Camera.CamerPosition - vData.Position);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0f  ), Model.Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * Model.Specular) ;

    vec3 directione = normalize(ligth.Direction);
    directione = vec3(0.0, -1.0, 0.0);
    float angle = dot(directione, -ligthDir);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);
    return vec3(((diffuse * inten + ambinet) + (specular * inten)) * ligth.Intensitie);
}

void main()
{
    EntityID = Model.EntityID; 
    vec3 ligths = vec3(0.0f , 0.0f , 0.0f);
#if 1
    int aktiveL = DrirectionelL.Aktive;
    if(aktiveL!=0)
    {
        for(int i = 0; i < aktiveL; i++)
        {
            ligths+=drictionelLigth(i);
        }
    }
    aktiveL = PointL.Aktive; 
    if(aktiveL!=0)
    { 
        for(int i = 0; i < aktiveL; i++)
        {
            ligths+=pointLigth(i);
        }
    }    
    aktiveL = SpotL.Aktive;
    if(aktiveL!=0)
    {
        for(int i = 0; i < aktiveL; i++)
        {
            ligths+=spotLigth(i);
        }
    }
#endif
    
    
    switch(Model.UseTexture)
    {
        case 0: Color = vec4(Model.Color * ligths, Model.Alpha);break;
        case 1: Color = texture2D(Texture_Diffuse, vec2(vData.Coords.x, -vData.Coords.y)) * vec4(Model.Color * ligths, Model.Alpha);break;
        default: Color = vec4(1.0f, 1.0f, 0.0f, 1.0f);break;
    };
    
   
	
}