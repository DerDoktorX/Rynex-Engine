#include "rypch.h"
#include "RenderPipline.h"
#include <Rynex/Renderer/Rendering/RenderTarget.h>
#if 0
#undef far;
#undef near;

namespace ObjectNamesDefnitionCompile {
	using namespace glm;

	using uint = uint32_t;
	using ID = uint32_t;

	struct Prame
	{
		uint index;
		void* value;
	};
	
	struct Texture
	{
		uint index; // store index in buffer array (use index form path)
		union{ float size; vec2 size; vec3 size; };
	};

	struct Buffer 
	{
		uint index; // store index in buffer array (use index form path)
	};

	struct Sprite
	{
		struct Side
		{
			vec3 bottem;
			vec3 top;
		};


		Side left;
		Side rigth;

	};

	struct Entity
	{
		struct
		{
			struct
			{
				vec3 postion;
				vec3 rotation;
				vec3 scale;
				quat quantion;
				vec3 direction;
			} transformation;
			mat4 modelMatrix;
			mat4 normaleMatrix;
			mat4 viewMatrix;

			struct 
			{
				mat4 projtionMatrix;
				mat4 inverseProjtionViewMatrix;
				mat4 trafmoredScaledProjtionViewMatrix;
				mat4 modelProjtionViewMatrix;
				struct
				{
					struct Quade2D
					{
						struct Side
						{
							vec3 bottem;
							vec3 top;
						};
						Side left;
						Side rigth;
					};
					Quade2D far;
					Quade2D near;

				} viewFustrum;
				float far;
				float near;
				float fov;
				float aspect;
			} camera;
			struct
			{
				vec3 postion;
				vec3 normale;
				vec2 textureStCoord[2];
				uint32_t indices[2];
			} geomtry;
			struct
			{
				Buffer prame;
				Texture texture[8];
			} materiel;
			struct
			{
				vec3 center;
				float radius;
			} sphere;
			struct 
			{
				vec3 min;
				vec3 max;
			} aabb;
			ID id;			
		} component;

	};

	struct TimeUinte
	{
		float millesSecounds;
		float microSecounds;
	};

	struct Pass 
	{
		struct {
			Texture texture[8];
			Texture depth;
			union { float renderSize; vec2 renderSize; vec3 renderSize; };
		} outPut;
		Entity camera;
	};

	struct
	{
		Entity entity;
		Pass pass;
		struct
		{
			std::map<std::string, Buffer> uniform;
			std::map<std::string, Buffer> vertex;
			std::map<std::string, Buffer> texture;
			std::map<std::string, Buffer> storage;
		} buffer;

	} local;

	struct
	{
		std::map<std::string, Pass> pass;
		float gamma;
		TimeUinte alphaTime;
		TimeUinte deltaTime;
		uint frameIndex;
	} globel;
}
#endif
namespace Rynex {

}