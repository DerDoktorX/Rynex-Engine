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
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position,1.);
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 colorOut;
layout(location = 1) out int  EntityID;


layout(location = 0)      in vec2	v_TexCoord;
layout(location = 1)	  in vec4	v_Color;
layout(location = 2) flat in int	v_TexIndex;
layout(location = 3) flat in int	v_EntityID;

uniform sampler2D u_FontAtlas[8];

const float pxRange = 2.0f;

float screenPxRange()
{
	vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlas[0], 0));
	vec2 screeTexSize = vec2(1.0) /fwidth(v_TexCoord);
	return max(0.5*dot(unitRange, screeTexSize), 1.0);
}

float median(vec3 color)
{
	return max(min(color.r, color.g), min( max(color.r, color.g), color.b));
}

// void main()
// {
// 	colorOut = texture2D(u_Textures[int(v_TexIndex)], v_TexCoord.xy) * v_Color;
// 	EntityID = int(v_EntityID);
// }

void main()
{


	vec3 msd = texture(u_FontAtlas[v_TexIndex],  v_TexCoord.xy).rgb;
	float sd = median(msd);
	float screenPxDistenc = screenPxRange() * (sd-0.5);
	float opacity = clamp(screenPxDistenc + 0.5, 0.0, 1.0);

	if(opacity == 0.0)
		discard;

	vec4 bgColor = vec4(vec3(0.0), 1.0);
	colorOut = mix(bgColor, v_Color, opacity);
	EntityID = int(v_EntityID);
}