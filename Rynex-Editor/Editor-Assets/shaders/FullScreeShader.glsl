#type Vertex
#version 450 core

layout(location = 0) in int a_Vertex;

layout(location = 0) out vec2 v_TexCoord;

#define BIT(x) (1 << x)
#define BIT_AND(x, y) (x & y)
#define BIT_NUM_EQUAL(x, y) BIT_AND(BIT(x), y)

void main()
{
	
	gl_Position = vec4(0.0, 0.0, 0.5, 1.0);
	int bitIndex = 0;
	v_TexCoord = vec2(0.0);
	if(bool(BIT_NUM_EQUAL(bitIndex, a_Vertex )))
	{
		v_TexCoord.x = 1.0;
		gl_Position.x =  1.0;
	}
	else
	{
		v_TexCoord.x = 0.0;
		gl_Position.x = -1.0;
	}
	bitIndex = 1;
	if(  bool( BIT_NUM_EQUAL(bitIndex, a_Vertex) )  )
	{
		v_TexCoord.y = 1.0;
		gl_Position.y = 1.0;
	}
	else
	{
		v_TexCoord.y =  0.0;
		gl_Position.y = -1.0;
	}
	
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 color;

layout(location = 0) in vec2 v_TexCoord;

uniform sampler2D u_Texture;


void main()
{
	color = texture2D(u_Texture, v_TexCoord);
	// color = vec4(v_TexCoord, 0.0, 1.0);
}
