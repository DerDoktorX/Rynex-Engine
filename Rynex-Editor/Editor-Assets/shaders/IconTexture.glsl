// Icon Texture Shader

#type Vertex
#version 450 core

// Icon Texture Shader

layout(location = 0) in vec4	a_Position;
layout(location = 1) in int		a_TexCoord;
layout(location = 2) in int		a_TexIndex;
layout(location = 3) in int		a_EntityID;

layout(shared, binding = 0) uniform CameraData
{
	mat4 ViewProjectionMatrix;	// 16
	mat4 ViewMatrix;			// 32
	mat4 ProjectionMatrix;		// 48
	vec3 CamerPosition;			// 60
	int Empty;					// 64
} Camera;

layout(shared, binding = 1) uniform DisplayData
{
	ivec4 ViewPortConf;
} Display;

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

vec2 quadPosition[4] = vec2[]
(
	vec2( -1.0f, -1.0f),
	vec2(  1.0f, -1.0f),
	vec2(  1.0f,  1.0f),
	vec2( -1.0f,  1.0f)
);

void main()
{
	
	v_ClipSpacePos = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 cameraPos = Camera.CamerPosition;
	float lentheObjectOrig = length(a_Position.xyz-cameraPos);
	v_ClipSpacePos = Camera.ViewProjectionMatrix * vec4(a_Position.xyz, 1.0 );
	vec2 viewPortConfOffset = Display.ViewPortConf.zw;
	vec2 viewPortConfSize = Display.ViewPortConf.xy;
	vec4 postion4 = vec4(v_ClipSpacePos.xyzw);
	postion4 /= postion4.w; 
	vec2 postion = vec2(postion4.xy); 
	float postionZ = postion4.z;
	float aspectY = viewPortConfSize.x/viewPortConfSize.y;
	float aspectX = viewPortConfSize.y/viewPortConfSize.x;
	vec2 size = vec2((2.0 / viewPortConfSize.xy) / 1.0);

	float sizeX = (2. / viewPortConfOffset.x) / 1.;
	float sizeY = (2. / viewPortConfOffset.y) / 1.;

	float sizeIcone = 10 + a_Position.z / 1;
	float sizeIconeFactor = (2.0 - postionZ) * 0.5f;
	// sizeIconeFactor = (1.0-postionZ) * 0.5f;
	// sizeIconeFactor	= 2.0f - sizeIconeFactor;
	// sizeIconeFactor = 2.0-v_ClipSpacePos.z;
	sizeIconeFactor = lentheObjectOrig;
	sizeIconeFactor = (postionZ);
	// sizeIconeFactor /= 2.0;
	// sizeIconeFactor *= aspectY - aspectX;
	float sizeIconeFactorBegin = 1.0-exp(sizeIconeFactor-2.5);
	float sizeIconeFactorEnd = 1.0-exp(-sizeIconeFactor);
	float curve = -0.9*pow(sizeIconeFactor-0, 2)+1.0;
	// sizeIconeFactor = min(max(sizeIconeFactorBegin, 0.1), sizeIconeFactor);
	// sizeIconeFactor = exp(sizeIconeFactor-2)-1;
	// sizeIconeFactor = sizeIconeFactorBegin;
	float minProzentOfImage = 0.001;
	float maxProzentOfImage = 1.0;
	sizeIconeFactor =  minProzentOfImage * curve + maxProzentOfImage;
	sizeIconeFactor *= 0.05; 
	// sizeIconeFactor *= 2;
	// sizeIconeFactor *= 0.15;
	// sizeIconeFactor = 1.0f;
	
	vec2 sizeIcone2 = vec2( sizeIconeFactor * 0.15); 
	sizeIcone2 = vec2( sizeIconeFactor); 
	sizeIcone2.x *= (aspectX);
	sizeIcone2 = vec2(0.0625,0.0625);
	for(int i = 0; i < 4; i++)
	{
		if( bool(BIT_NUM_EQUAL(i, a_TexCoord)) )
		{
			v_TexCoord = texCoord[i];
			postion4.xy = postion4.xy + vec2(sizeIcone2 * quadPosition[i]);
			
			postion4.xyzw /= postion4.w;
			postion4.z = -1.0;
		}
	}

	
	v_EntityID = a_EntityID;
	v_TexIndex = a_TexIndex;
	v_ClipSpacePos =postion4;
	gl_Position = vec4(postion4);


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

void main()
{
	color = texture2D(u_Textures[v_TexIndex], v_TexCoord.xy);
	//color = vec4(1.0);
	EntityID = int(v_EntityID);
}