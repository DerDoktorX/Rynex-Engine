#type Vertex
#version 450 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;

out Data {
    vec3 normal;
    vec3 pos;
    vec2 uv;
} DataOut;



void main()
{
    DataOut.uv = a_UV;


	DataOut.normal = mat3(transpose(inverse(u_Model))) * a_Normals;
    //DataOut.normal = a_Normals;
    DataOut.pos  = vec3(u_Model * vec4( a_Postion , 1. ));
    gl_Position = u_ViewProj *  vec4( DataOut.pos , 1. );
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 Color;
layout(location = 1) out vec4 Normal;
layout(location = 2) out int  EntityID;

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



void main()
{
    EntityID = u_EntityID; 
    
    vec3 ambient = u_LigthColor * vec3(u_Ambient);

    vec3 normal = normalize(DataIn.normal);
    vec3 ligthDir = normalize(u_LigthPos - DataIn.pos );


    float diff = max( dot(normal, ligthDir), 0.0);
    vec3 diffuse =  u_LigthColor * vec3(diff);

    
    vec3 viewDir = normalize(u_CamerPos - DataIn.pos);
    vec3 reflectDir = reflect(-ligthDir, normal);
    float specAmount = pow( max(  dot( viewDir, reflectDir ), 0.0  ), u_Shinines );
    vec3 specular =  u_LigthColor * vec3(specAmount * u_Specular);

    vec3 colorLigth = vec3(ambient+diffuse+specular);
    vec3 result = colorLigth * u_Color.rgb; //vec3(0.15, 0.15, 0.75)


	Color =  texture2D(u_Texture_Diffuse1, vec2(DataIn.uv.x, -DataIn.uv.y)) + vec4(result,1.);
    Normal = vec4( normal, 1.0 );
    // Color = vec4( normal, 1.0 );
	
}