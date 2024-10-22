// Texture

#type Vertex
#version 450 core

layout(location = 0) in vec3	a_Position;
layout(location = 1) in vec4	a_Color;
layout(location = 2) in vec2	a_TexCoord;
layout(location = 3) in int		a_TexIndex;
layout(location = 4) in int		a_EntityID;

uniform mat4 u_ViewProjection;

layout(location = 0)      out vec2	v_TexCoord;
layout(location = 1)	  out vec4	v_Color;
layout(location = 2) flat out int	v_TexIndex;
layout(location = 3) flat out int	v_EntityID;

void main()
{

	v_EntityID = a_EntityID;
	v_TexIndex = a_TexIndex;
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	
	gl_Position = u_ViewProjection * vec4(a_Position,1.);
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int  EntityID;


layout(location = 0)      in vec2	v_TexCoord;
layout(location = 1)	  in vec4	v_Color;
layout(location = 2) flat in int	v_TexIndex;
layout(location = 3) flat in int	v_EntityID;

uniform sampler2D u_Textures[32];

void main()
{
	color = texture2D(u_Textures[int(v_TexIndex)], v_TexCoord.xy) * v_Color;
	EntityID = int(v_EntityID);
}
