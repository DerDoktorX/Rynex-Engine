#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

struct OutPut
{
	vec3 Position;	        // 12
	vec3 Normal;	        // 24
	vec2 Coords;	        // 32
    vec4 PositionLigth[8];
    // vec4 PositionDirectionelLigth[8];	
    // vec4 PositionSpotLigth[8];
    // vec4 PositionPointLigth[32];
};

struct DrirectionLigthData
{
    mat4 ShadowMatrix;
    vec4 ShadowMapPos;
	vec3 Color;
    float Intensitie; 
    vec3 Position;
    float ShadowMapSize;
};

struct SpotLigthData 
{
    mat4 ShadowMatrix;

    vec4 ShadowMapPos;

    vec3 Color;
    float Intensitie;

    vec3 Position;
    float Distence;

    vec3 Direction;
    float Inner;

    float Outer;
    vec3 Empty;
};


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

layout(shared, binding = 3) uniform DrirectionLigthDataVS 
{
	int Aktive;
	// ivec3 Empty;
    DrirectionLigthData Direction[8];
} DrirectionelL;

layout(shared, binding = 4) uniform SpotLigth 
{
    int Aktive;
	// ivec3 Empty;
    SpotLigthData Spot[32];
} SpotL;


layout(shared, binding = 8) uniform LigthsData 
{
    mat4 ShadowMatrix[8];
    ivec4 ShadowCameraID;
} Ligths;



void main()
{
    vec4 worldPostion = Model.ModelMatrix * vec4( a_Postion , 1.0 );
    int LigthsCount = SpotL.Aktive.x + DrirectionelL.Aktive.x;
    for(int i = 0; i < LigthsCount; i++)
    { 
#if 1
        vOutData.PositionLigth[i] =  Ligths.ShadowMatrix[i] * worldPostion;
#else
        vOutData.PositionDirectionelLigth[indexShadow] =  DrirectionelL.Direction[indexShadow].ShadowMatrix * worldPostion;
#endif
    }   
#if 0
    for(int i = 0; i < DrirectionelL.Aktive; i++, indexShadow++)
    { 
#if 1
        vOutData.PositionPointLigth[indexShadow] =  Ligths.ShadowMatrix[indexShadow] * worldPostion;
#else
        vOutData.PositionPointLigth[indexShadow] =  SpotL.Spot[indexShadow].ShadowMatrix * worldPostion;
#endif

    } 

#endif    
   

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
    vec4 PositionLigth[8];
    // vec4 PositionDirectionelLigth[8];	
    // vec4 PositionSpotLigth[8];
    // vec4 PositionPointLigth[32];
};


struct DrirectionLigthData
{
    mat4 ShadowMatrix;

    vec4 ShadowMapPos;

	vec3 Color;
    float Intensitie; 

    vec3 Position;
    float ShadowMapSize;
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
    mat4 ShadowMatrix;

    vec4 ShadowMapPos;

    vec3 Color;
    float Intensitie;

    vec3 Position;
    float Distence;

    vec3 Direction;
    float Inner;

    float Outer;
    vec3 Empty;
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


layout(shared, binding = 2) uniform AmbientLigth 
{
	vec3 Color;
    float Intensitie;
} AmbientL;

layout(shared, binding = 3) uniform DrirectionLigthFS 
{
	int Aktive;
	// ivec3 Empty;
    DrirectionLigthData Direction[8];
} DrirectionelL;



layout(shared, binding = 4) uniform SpotLigth 
{
    int Aktive;
	// ivec3 Empty;
    SpotLigthData Spot[32];
} SpotL;

layout(shared, binding = 5) uniform PointLigth 
{
	int Aktive;
	// ivec3 Empty;
	PointLigthData Point[32];
} PointL;

#define PERSPECTIV_CAMERA_SHADOW_MAX_BIAS 0.000002
#define PERSPECTIV_CAMERA_SHADOW_MIN_BIAS 0.000001

#define OTRHOGRAFIC_CAMERA_SHADOW_MAX_BIAS 0.0025
#define OTRHOGRAFIC_CAMERA_SHADOW_MIN_BIAS 0.0

layout(binding = 6) uniform sampler2D Texture_Diffuse;
// layout(sampler2D, binding = 1) uniform sampler2D AlbedoMap; 
// layout(sampler2D, binding = 2) uniform sampler2D NormalMap; 
// layout(sampler2D, binding = 3) uniform sampler2D RoughnessMap; 
// layout(sampler2D, binding = 4) uniform sampler2D RoughnessMap; 

layout(binding = 7) uniform sampler2DShadow ShadowMap; 

vec3 shadowSTCoord(vec4 ligthCoordsPos)
{
    vec3 ligthCoords = ligthCoordsPos.xyz / ligthCoordsPos.w;
    vec2 UVCoords = vec2(
        0.5f * ligthCoords.x + 0.5f,
        0.5f * ligthCoords.y + 0.5f
    );
    float z = 0.5f * ligthCoords.z + 0.5f;
    return vec3(UVCoords.xy, z);
}


float calcShodowFactor(vec2 attasSizeTex, vec2 attasOffsetTex, vec4 ligthCoordsPos, float defuseFactor, float maxBias, float minBias)
{
    vec3 ligthCoords = shadowSTCoord(ligthCoordsPos);
    float z = ligthCoords.z;
    float Depth = 1.0;
    // float bias =  // 0.0;// 0.00025;
    float bias =  mix(maxBias, minBias, defuseFactor);
    float distenze = (z - bias);

    float maxST = 1.0f, minST = 0.00f;
    if(ligthCoords.x < maxST && ligthCoords.x > minST && ligthCoords.y < maxST && ligthCoords.y > minST)
    {
        vec2 stSelectTexCoord = attasSizeTex.xy * ligthCoords.xy + attasOffsetTex.xy;
        Depth = texture(ShadowMap, vec3( stSelectTexCoord.xy, distenze)).x;
        return Depth;

        float depthBias = (Depth + bias);
        if(depthBias < z)
            return 0.0f;
        else
            return 1.0f;
    }
    else
    {
        return 1.0f;
    }
}

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

    return vec3( (ambinet + diffuse * inner) + (specular * inner));
}




vec3 drictionelLigth(int index, int indexShadow, vec3 texColor)
{
    DrirectionLigthData ligth = DrirectionelL.Direction[index];

    vec3 ligthDir = normalize( ligth.Position.xyz);
    vec3 normal = normalize( vData.Normal );
    float diffuseFactor = dot(normal, -ligthDir);
    float shadow = calcShodowFactor(ligth.ShadowMapPos.xy, ligth.ShadowMapPos.zw, vData.PositionLigth[indexShadow], diffuseFactor, OTRHOGRAFIC_CAMERA_SHADOW_MAX_BIAS, OTRHOGRAFIC_CAMERA_SHADOW_MIN_BIAS);

    

    


    float diff = max( diffuseFactor, 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(Camera.CamerPosition - vData.Position);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), Model.Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * Model.Specular);

    return vec3(( diffuse + specular ) * ligth.Intensitie * shadow) ;
}


vec3 spotLigth(int index, int indexShadow)
{
    SpotLigthData ligth = SpotL.Spot[index];

    vec3 stCoord = shadowSTCoord(vData.PositionLigth[0]);
    // return vec3(stCoord.xy, 0.0);
    vec3 ligthpos = ligth.Position;
    vec3 normal = normalize(vData.Normal);
    vec3 ligthRelativ = ligthpos.xyz - vData.Position;
    vec3 ligthDir = normalize(ligthRelativ);
    float dist = length(ligthRelativ);
    float diffuseFactor = dot(normal, ligthDir);
    float shadow = calcShodowFactor(ligth.ShadowMapPos.xy, ligth.ShadowMapPos.zw, vData.PositionLigth[indexShadow], diffuseFactor, PERSPECTIV_CAMERA_SHADOW_MAX_BIAS, PERSPECTIV_CAMERA_SHADOW_MIN_BIAS);
    // return vec3(shadow);

    float outerCone = ligth.Outer;
    float innerCone = ligth.Inner;

    

     //vec3(0., 7.97,23.51);
   
    
    float a = ligth.Distence; 
    float b = ligth.Intensitie;
    float inner = 1.0f / (a * (dist * dist) + b * dist + 1.0f);

    


    float diff = max( dot(normal, ligthDir), 0.0f );
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(Camera.CamerPosition - vData.Position);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0f  ), Model.Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * Model.Specular) ;

    vec3 directione = normalize(ligth.Direction);

    float angle = dot(directione, ligthDir);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);
    // return vec3( (diffuse + specular) * shadow);
    // return vec3(((diffuse * inten ) + (specular * inner)) * shadow);
    // return vec3(((diffuse * inten ) + (specular * inner)) * ligth.Intensitie);
    return vec3(((diffuse * inten) + (specular * inner)) * ligth.Intensitie * shadow);
}

void main()
{
    EntityID = Model.EntityID; 

    vec3 texColor;
    switch(Model.UseTexture)
    {
        case 0: texColor = vec3(Model.Color); break;
        case 1: texColor = vec3(texture2D(Texture_Diffuse, vec2(vData.Coords.x, -vData.Coords.y))); break;
        default: Color = vec4(1.0f, 1.0f, 0.0f, 1.0f);  return;
    }
        

#if 1
    vec3 ligths = vec3(0.0f , 0.0f , 0.0f);

    int aktiveL = DrirectionelL.Aktive;
    int shadowIndex = 0;
    if(aktiveL!=0)
    {
        for(int i = 0; i < aktiveL; i++, shadowIndex++)
        {
            ligths+=drictionelLigth(i,shadowIndex,texColor);
            
        }
    }
#if 1
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
        for(int i = 0; i < aktiveL; i++, shadowIndex++)
        {
            ligths+=spotLigth(i,shadowIndex);
        }
    }
#endif
    vec3 ambinet = AmbientL.Color.rgb * AmbientL.Intensitie;
    ligths += ambinet;

    switch(Model.UseTexture)
    {
        case 0: Color = vec4(Model.Color * ligths, Model.Alpha);break;
        case 1: Color =  vec4(texColor *Model.Color * ligths, Model.Alpha);break;
        default: Color = vec4(1.0f, 1.0f, 0.0f, 1.0f);break;
    };

    // Color = vec4(normalize(vData.Normal), 1.0f);
#else
    Color = vec4(vData.Coords, 0.0f, 1.0f);
#endif 
   
	
}