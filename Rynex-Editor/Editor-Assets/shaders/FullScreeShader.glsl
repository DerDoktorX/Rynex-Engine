#type Vertex
#version 460 core

layout(location = 0) in vec2 a_Vertex;

layout(location = 0) out vec2 v_TexCoord;

uniform float u_Depth;

void main()
{
	
	gl_Position = vec4(a_Vertex.xy, u_Depth, 1.0);
	int bitIndex = 0;
	v_TexCoord = vec2(0.0);
	switch(gl_DrawID)
	{
		case 0:
			v_TexCoord = vec2(0.0, 0.0);
			break;
		case 1:
			v_TexCoord = vec2(1.0, 0.0);
			break;
		case 2:
			v_TexCoord = vec2(0.0, 1.0);
			break;
		case 3:
			v_TexCoord = vec2(1.0, 1.0);
			break;
		default:
			break;
	}

	
}

#type Fragment
#version 460 core

layout(location = 0) out vec4 color;

layout(location = 0) in vec2 v_TexCoord;

layout(binding = 0) uniform sampler u_Texture;

void main()
{
	color = texture2D(u_Texture, v_TexCoord);
	// color = vec4(v_TexCoord, 0.0, 1.0);
}
