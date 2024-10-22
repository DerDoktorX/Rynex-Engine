// Icon Texture Shader

#type Vertex
#version 450 core

layout(location = 0) in vec4	a_Position;
layout(location = 1) in int		a_TexCoord;
layout(location = 2) in int		a_TexIndex;
layout(location = 3) in int		a_EntityID;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_ViewProjection;
uniform uvec2 u_ViewPortSize;


layout(location = 0)      out vec2	v_TexCoord;
layout(location = 1) flat out int	v_TexIndex;
layout(location = 2) flat out int	v_EntityID;
layout(location = 3) flat out vec4	v_ClipSpacePos;

#define BIT(x) (1 << x)
#define BIT_AND(x, y) (x & y)
#define BIT_NUM_EQUAL(x, y) BIT_AND(BIT(x), y)

vec2 texCoord[4] = vec2[]
(
	vec2( 0.0, 0.0 ),
	vec2( 1.0, 0.0 ),
	vec2( 1.0, 1.0 ),
	vec2( 0.0, 1.0 )
);

vec4 quadPosition[4] = vec4[]
(
	vec4( -1.0f, -1.0f, -1.0f, 0.0f ),
	vec4(  1.0f, -1.0f, -1.0f, 0.0f ),
	vec4(  1.0f,  1.0f, -1.0f, 0.0f ),
	vec4( -1.0f,  1.0f, -1.0f, 0.0f )
);

void main()
{
	
	v_ClipSpacePos = vec4(0.0, 0.0, 0.0, 1.0);
	v_ClipSpacePos = u_ViewProjection * vec4( a_Position );
	vec3 postion = vec3(v_ClipSpacePos.xyz/v_ClipSpacePos.w); 
	float sizeX = (2. / u_ViewPortSize.x)/1.;
	float sizeY = (2. / u_ViewPortSize.y)/1.;
	float sizeIcone = 15 + a_Position.z /10;
	for(int i = 0; i < 4; i++)
	{
		if( bool(BIT_NUM_EQUAL(i, a_TexCoord)) )
		{
			v_TexCoord = texCoord[i];
			postion = postion+vec3(quadPosition[i].x * sizeX * sizeIcone, quadPosition[i].y * sizeY * sizeIcone, quadPosition[i].z);
			 
		}
	}
	v_EntityID = a_EntityID;
	v_TexIndex = a_TexIndex;
	
	gl_Position = vec4(postion,1.);


}

#type Fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int  EntityID;


layout(location = 0)      in vec2	v_TexCoord;
layout(location = 1) flat in int	v_TexIndex;
layout(location = 2) flat in int	v_EntityID;
layout(location = 3) flat in vec4	v_ClipSpacePos;

uniform sampler2D u_Textures[8];
uniform uvec2 u_ViewPortSize;

void main()
{
	color = texture2D(u_Textures[int(v_TexIndex)], v_TexCoord.xy);
	//color = vec4(1.0);
	EntityID = int(v_EntityID);
}