// Texture

#type vertex
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

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 colorTexCoord;
layout(location = 2) out int  EntityID;


layout(location = 0)      in vec2	v_TexCoord;
layout(location = 1)	  in vec4	v_Color;
layout(location = 2) flat in int	v_TexIndex;
layout(location = 3) flat in int	v_EntityID;

uniform sampler2D u_Textures[32];

void main()
{
	

	

	colorTexCoord = vec4(v_TexCoord.xy, 0.0, 1.0);
	color = texture(u_Textures[int(v_TexIndex)], colorTexCoord.xy ) * v_Color;
	// TODO: Why Shader reload broke Textures
	#if 0		
		vec3 ligthColor = vec3(1.0);
		vec3 ligthDirction = normalize(vec3(1.0, 2.0, 3.0));
		color += vec4( -1.5, 0.0, 1.0, 1.0);
	#endif

	EntityID = int(v_EntityID);
}
