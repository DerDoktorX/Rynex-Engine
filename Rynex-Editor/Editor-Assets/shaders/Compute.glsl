#type Compute
#version 450 core


layout (local_size_x = 40, local_size_y = 24) in;

layout (binding = 0, rgba32F) readonly uniform image2D input1_image;
layout (binding = 1, rgba32F) readonly uniform image2D input2_image;
layout (binding = 2, rgba32F) writeonly uniform image2D output_image;
uniform vec2 u_MauseInViewPixelPos;


ivec2 offsets[25] = ivec2[]
(
	ivec2(-2,  2), 		ivec2( -1,  2), 		ivec2(  0,  2),		ivec2(  1,  2),		ivec2( 2,  2),
	ivec2(-2,  1), 		ivec2( -1,  1), 		ivec2(  0,  1),		ivec2(  1,  1),		ivec2( 2,  1),
	ivec2(-2,  0), 		ivec2( -1,  0), 		ivec2(  0,  0),		ivec2(  1,  0),		ivec2( 2,  0),
	ivec2(-2, -1), 		ivec2( -1, -2), 		ivec2(  0, -1),		ivec2(  1, -1),		ivec2( 2, -1),
	ivec2(-2, -2), 		ivec2( -1, -2), 		ivec2(  0, -2),		ivec2(  1, -2),		ivec2( 2, -2)
);

float kernel[25] = float[]
(
	1,   1,      1,		 1,		1,
	1,   1, 	-6,   	 1,		1,
	1,  -6,	   -18,		-6,		1,
	1,	 1,	 	-6,		 1,    	1,
	1,	 1,		 1,		 1,		1
);

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	ivec2 testMouse = ivec2(u_MauseInViewPixelPos);
	int range = 2;
	if( ( ( testMouse.x + range > pixel.x && testMouse.x - range < pixel.x ) 
		&& ( testMouse.y + range > pixel.y && testMouse.y - range < pixel.y ) ))
	{
		imageStore( output_image, pixel, vec4(0.9, 0.05, 0.02, 1.0));
	}
	else
	{    
		
    	float color = 0.0f;
		vec4 image;
		for(int i = 0; i < 25; i++)
		{
			image = imageLoad( input1_image, pixel + offsets[i] );
			color += (image.r + image.g + image.b )* kernel[i];
		}
		if(color> 1.)
		{
			color = 1.0;
		}
		else if(color < 0.0)
		{
			color = 0.0;
		}
		vec4 colorFinale =  color * vec4(1.0, 0.25, 0.0, 1.0);

		image = imageLoad( input2_image, pixel );
    	imageStore( output_image, pixel, image + colorFinale);
	}
}