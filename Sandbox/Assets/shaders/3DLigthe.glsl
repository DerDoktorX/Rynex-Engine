#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjTest; // Test if thsi compied! Result compied
} CameraData;


out Data {
    vec3 normal;
    vec3 pos;
    vec2 uv;
} DataOut;


void main()
{
    DataOut.uv = a_UV;


	DataOut.normal = mat3(transpose(inverse(u_Model))) * a_Normals;
    DataOut.pos  = vec3(u_Model * vec4( a_Postion , 1. ));
    gl_Position = CameraData.ViewProjTest *  vec4( DataOut.pos , 1. );
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out int  EntityID;

uniform vec3 u_Color;
uniform vec3 u_CamerPos;

uniform vec3 u_LigthPos;
uniform vec3 u_LigthColor;

uniform float u_Shinines;
uniform float u_Specular;
uniform float u_Ambient;

uniform int u_EntityID;
  
uniform sampler2D u_Texture_Diffuse1;

in Data {
    vec3 normal;
    vec3 pos;
    vec2 uv;
} DataIn;


layout(std140, binding = 2) uniform AmbientLigth
{
    vec3 Color;
    float Intensitie;
} AmbientData;

struct DrirectionLigtheData 
{
    vec4 Color;
    vec3 Position;
    float Intensitie; 
};

layout(std140, binding = 3) uniform DrirectionLigth
{
    int Aktive;
    DrirectionLigtheData Direction[8];
} DrirectionData;


struct PointLigtheData
{
    vec3 Color;
    float Distence;
    vec3 Position;
    float Intensitie;
    
};

layout(std140, binding = 4) uniform PointLigthe
{
    int Aktive;
    PointLigtheData Point[32];
} PointData;

struct SpotLigtheData 
{
    vec3 Color;
    float Intensitie;
    vec3 Position;
    float Distence;
    vec3 Direction;
    float Inner;
    float Outer;
    vec2 Empty;
};

layout(std140, binding = 5) uniform SpotLigthe
{
    int Aktive;
    
    SpotLigtheData Spot[32];
} SpotData;

vec3 PointLigte(int index)
{
    PointLigtheData ligth = PointData.Point[index];
    vec3 ligthpos = ligth.Position; //vec3(0., 7.97,23.51);
    vec3 ligthRelativ = ligthpos.xyz - DataIn.pos;

    float dist = length(ligthRelativ);
    float a = ligth.Distence; // 1.00;
    float b = ligth.Intensitie; //0.04;
    float inner = 1.0f / (a * (dist * dist) + b * dist + 1.0f);

    vec3 ambinet = AmbientData.Color.rgb * AmbientData.Intensitie;
    vec3 normal = normalize(DataIn.normal);

    vec3 ligthDir = normalize(ligthRelativ );


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(u_CamerPos - DataIn.pos);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), u_Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * u_Specular) ;

    return vec3(( (diffuse * inner) + (specular * inner)));
}

vec3 DrictionelLigte(int index)
{
    DrirectionLigtheData ligth = DrirectionData.Direction[index];


    vec3 normal = normalize(DataIn.normal);

    vec3 ligthDir = normalize(ligth.Position.xyz );


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(u_CamerPos - DataIn.pos);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), u_Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * u_Specular);
    
    return vec3(( diffuse + specular) * ligth.Intensitie);
}

vec3 DefaultDrictionelLigte(int index)
{
    vec3 ambinet = vec3(u_Ambient);
   
    vec3 normal = normalize(DataIn.normal);
    vec3 ligthDir = normalize(u_LigthPos);


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  u_LigthColor * vec3(diff);

    
    vec3 viewDir = normalize(u_CamerPos - DataIn.pos);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), u_Shinines );
    vec3 specular =  u_LigthColor * vec3(specAmount * u_Specular) ;

    return vec3( diffuse + specular);

}

vec3 SpotLigth(int index)
{
    SpotLigtheData ligth = SpotData.Spot[index];

    float outerCone = ligth.Outer;
    float innerCone = ligth.Inner;

    vec3 ambinet = AmbientData.Color.rgb * AmbientData.Intensitie;
    vec3 normal = normalize(DataIn.normal);

    vec3 ligthDir = normalize(ligth.Position.xyz - DataIn.pos);


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  ligth.Color.rgb * vec3(diff);

    
    vec3 viewDir = normalize(u_CamerPos - DataIn.pos);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), u_Shinines );
    vec3 specular =  ligth.Color.rgb * vec3(specAmount * u_Specular) ;

    vec3 directione = normalize(ligth.Direction);
    directione = vec3(0.0, -1.0, 0.0);
    float angle = dot(directione, -ligthDir);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);
    return vec3(((diffuse * inten) + (specular * inten)) * ligth.Intensitie);
}

void main()
{
    EntityID = u_EntityID; 
    vec3 ligths = vec3(0.0f,0.0,0.);
#if 1
    int aktiveL = DrirectionData.Aktive;
    if(aktiveL!=0)
    {
        for(int i = 0; i < aktiveL; i++)
        {
            ligths+=DrictionelLigte(i);
        }
    }
    aktiveL = PointData.Aktive; 
    if(aktiveL!=0)
    { 
        for(int i = 0; i < aktiveL; i++)
        {
            ligths+=PointLigte(i);
        }
    }    
    aktiveL = SpotData.Aktive;
    if(aktiveL!=0)
    {
        for(int i = 0; i < aktiveL; i++)
        {
            ligths+=SpotLigth(i);
        }
    }
#endif
    vec3 ambinet = AmbientData.Color.rgb * AmbientData.Intensitie;
	Color = texture2D(u_Texture_Diffuse1, vec2(DataIn.uv.x, -DataIn.uv.y)) * vec4( ligths+ambinet,1.0);
    //Color = vec4( , 1.0 );
	
}