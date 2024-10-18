// Icon Texture Shader

#type Vertex
#version 450 core

layout(location = 0) in vec3	a_Position;
layout(location = 1) in vec2	a_TexCoord;
layout(location = 2) in int		a_TexIndex;
layout(location = 3) in int		a_EntityID;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_ViewProjection;
uniform uvec2 u_SreenSize;


layout(location = 0)      out vec2	v_TexCoord;
layout(location = 1) flat out int	v_TexIndex;
layout(location = 2) flat out int	v_EntityID;


void main()
{

	v_EntityID = a_EntityID;
	v_TexIndex = a_TexIndex;
	v_TexCoord = a_TexCoord;
	
	gl_Position = u_ViewProjection * vec4(a_Position,1.);
	float size = u_SreenSize.x / u_SreenSize.y;
#if 1
	if(a_TexCoord.x == 1.0)
	{
		gl_Position.x = 30/size;
	} 
	else
	{
		gl_Position.x = -30;
	}
	if(a_TexCoord.y == 1.0)
	{
		gl_Position.y = 30;
	}
	else
	{
		gl_Position.y = -30;
	}
	gl_Position.z = 30.0;
#endif
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int  EntityID;


layout(location = 0)      in vec2	v_TexCoord;
layout(location = 1) flat in int	v_TexIndex;
layout(location = 2) flat in int	v_EntityID;

uniform sampler2D u_Textures[8];

void main()
{
	color = texture2D(u_Textures[int(v_TexIndex)], v_TexCoord.xy) ;
	EntityID = int(v_EntityID);
}