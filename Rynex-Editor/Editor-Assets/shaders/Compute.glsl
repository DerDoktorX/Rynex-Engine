#type Compute
#version 450 core


layout (local_size_x = 8, local_size_y = 16, local_size_z = 1) in;

layout (binding = 0, rgba32F) readonly uniform image2D input1_image;
layout (binding = 1, rgba32F) readonly uniform image2D input2_image;
layout (binding = 2, rgba32F) writeonly uniform image2D output_image;
uniform vec2 u_MauseInViewPixelPos;

#if 1
	#define SAMPLE_COUNT 25
	ivec2 offsets[SAMPLE_COUNT] = ivec2[]
	(
		ivec2(-2,  2), 		ivec2( -1,  2), 		ivec2(  0,  2),		ivec2(  1,  2),		ivec2( 2,  2),
		ivec2(-2,  1), 		ivec2( -1,  1), 		ivec2(  0,  1),		ivec2(  1,  1),		ivec2( 2,  1),
		ivec2(-2,  0), 		ivec2( -1,  0), 		ivec2(  0,  0),		ivec2(  1,  0),		ivec2( 2,  0),
		ivec2(-2, -1), 		ivec2( -1, -2), 		ivec2(  0, -1),		ivec2(  1, -1),		ivec2( 2, -1),
		ivec2(-2, -2), 		ivec2( -1, -2), 		ivec2(  0, -2),		ivec2(  1, -2),		ivec2( 2, -2)
	);

	float kernel[SAMPLE_COUNT] = float[]
	(
		1,   1,      1,		 1,		1,
		1,   1, 	-6,   	 1,		1,
		1,  -6,	   -18,		-6,		1,
		1,	 1,	 	-6,		 1,    	1,
		1,	 1,		 1,		 1,		1
	);
#elif 0
	#define SAMPLE_COUNT 9
	ivec2 offsets[SAMPLE_COUNT] = ivec2[]
	(
		ivec2( -1,  1), 		ivec2(  0,  1),		ivec2(  1,  1),
		ivec2( -1,  0), 		ivec2(  0,  0),		ivec2(  1,  0),
		ivec2( -1, -2), 		ivec2(  0, -1),		ivec2(  1, -1)
	);

	float kernel[SAMPLE_COUNT] = float[]
	(
		 1, 	-3,   	 1,
		-3,	   -10,		-3,
		 1,	 	-3,		 1
	);
#else
	#define SAMPLE_COUNT 5
	ivec2 offsets[SAMPLE_COUNT] = ivec2[]
	(
		 				ivec2(  0,  1),
		ivec2( -1,  0), ivec2(  0,  0),		ivec2(  1,  0),
						ivec2(  0, -1)
	);

	float kernel[SAMPLE_COUNT] = float[]
	(
				 1,
		  1,	-4,		1,
				 1
	);
#endif
#define RY_GAMME_COREKTION 0

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
#if 1
	ivec2 testMouse = ivec2(u_MauseInViewPixelPos);
	int range = 3;
	if( ( ( testMouse.x + range > pixel.x && testMouse.x - range < pixel.x ) 
		&& ( testMouse.y + range > pixel.y && testMouse.y - range < pixel.y ) )
	)
	{
		imageStore( output_image, pixel, vec4(0.9, 0.05, 0.02, 1.0));
	}
	else
	{    
		
    	float color = 0.0f;
		vec4 image;

		for(int i = 0; i < SAMPLE_COUNT; i++)
		{
			image = imageLoad( input1_image, pixel + offsets[i] );
			color += (image.r + image.g + image.b) * kernel[i];
		}

		vec4 colorFinale;
		

		if(color < 1.)
		{
			// color = 1.0;
			// colorFinale = imageLoad( input2_image, pixel );
			// colorFinale = imageLoad( input1_image, pixel );
			colorFinale = vec4(0.0, 0.0, 0.0, 0.0);

		}
		else if(color > 0.0)
		{
			// color = 0.0;
			colorFinale = vec4(1.0, 0.25, 0.0, 1.0);
		}
		// colorFinale =  color * vec4(1.0, 0.25, 0.0, 1.0);
		// colorFinale += imageLoad( input1_image, pixel );
		image = imageLoad( input2_image, pixel );
#if RY_GAMME_COREKTION
		float gammaCorection;
		// gammaCorection = u_Camera.Gamma;
		gammaCorection = 2.2;

		image.rgb = pow(image.rgb, vec3(1.0f / gammaCorection));
#endif
		
    	imageStore( output_image, pixel, image + colorFinale);
	}
#elif 0
		vec4 image;
		image = imageLoad( input2_image, pixel );
		image.rgb -= 1.0;

#if RY_GAMME_COREKTION
	float gammaCorection;
	// gammaCorection = u_Camera.Gamma;
	gammaCorection = 2.2;

	image.rgb = pow(image.rgb, vec3(1.0f / gammaCorection));
#endif
    	imageStore( output_image, pixel, image);

#elif 0
		vec4 image;
		image = imageLoad( input1_image, pixel );
    	imageStore( output_image, pixel, image);

#else
		vec4 image;
		image = imageLoad( input2_image, pixel );
		image.xyz *= 1.0f;
		imageStore( output_image, pixel, image );

#endif
}