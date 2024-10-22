// FlatColorShader


#type Vertex
#version 450 core

layout(location = 0) in vec2 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Tranform;

void main()
{
	gl_Position = u_ViewProjection * u_Tranform * vec4(a_Position,0.,1.);
}

#type Fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
	//color = vec4(0.2,0.3,0.8, 1.0);
	color = vec4(u_Color);
}