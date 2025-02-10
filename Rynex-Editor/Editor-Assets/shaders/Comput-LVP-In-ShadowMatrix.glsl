#type Compute
#version 450 core


layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (binding = 0, std430) buffer LigtheViewProjtionMatricies {
	mat4 ViewProjtionMatririx[];
};

layout (binding = 1, std430) buffer ShadowMatricies {
	mat4 ShadowMatririx[];
};

layout (binding = 2, std140) uniform Size {
	int CountTex;
	ivec3 Empty;
};


const mat4 Scaled = mat4( 0.5, 0.0, 0.0, 0.0
						, 0.0, 0.5, 0.0, 0.0
						, 0.0, 0.0, 0.5, 0.0
						, 0.0, 0.0, 0.0, 1.0 );

const mat4 Trasletde = mat4(  1.0, 0.0, 0.0, 0.0
							, 0.0, 1.0, 0.0, 0.0
							, 0.0, 0.0, 1.0, 0.0
							, 0.5, 0.5, 0.5, 1.0 );

void main()
{
	uint idx = gl_GlobalInvocationID.x;
	if(int(idx) >= CountTex)
		return;

	mat4 Matrix = Trasletde * Scaled * ViewProjtionMatririx[idx];
	ShadowMatririx[idx] = Matrix;
	
}