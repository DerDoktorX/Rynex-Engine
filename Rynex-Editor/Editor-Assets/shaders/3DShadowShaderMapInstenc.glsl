#type Vertex
#version 460 core

layout(location = 0) in vec3 a_Postion;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normals;


layout(shared, binding = 8) uniform LigthsData
{
    mat4 ViewProjectionMatrixLigth[8];
	ivec4 ShadowCameraID;
} Ligths; 

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
	mat4 projetion = Ligths.ViewProjectionMatrixLigth[Ligths.ShadowCameraID.x];
	vec4 projetedWorldPostion = projetion * Model.ModelMatrix * vec4( a_Postion , 1.0 );
    gl_Position = projetedWorldPostion;
}

#type Fragment
#version 460 core

void main()
{
}