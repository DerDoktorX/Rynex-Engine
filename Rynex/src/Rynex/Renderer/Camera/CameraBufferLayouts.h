#pragma once
#include <glm/glm.hpp>

namespace Rynex {

	struct DebugeCamerData
	{
		DebugeCamerData()
			: InverseProjetionViewMatrix(glm::mat4(-1.0f))
		{
		}

		DebugeCamerData(const glm::mat4& inverseProjetionView)
			: InverseProjetionViewMatrix(inverseProjetionView)
		{
		}

		glm::mat4 InverseProjetionViewMatrix;
	};

	// M = WorldMatrix or CameraMatrix
	// P = Projection Matrix -> NDC [-1, 1], Right-Handed
	// V = ViewMatrix = inverse(M)
	// xyz = Position Vector = (M[3][0], M[3][1], M[3][2]) * M[3][3]
	struct CameraData
	{
		glm::mat4 ViewProjectionMatrix; // VP = V * P
		glm::mat4 ViewMatrix;			// V
		glm::mat4 ProjectionMatrix;		// P
		glm::vec3 Position;				// (WorldMatrix[3][0] * , WorldMatrix[3][1], WorldMatrix[3][2])
		float Gamma = 1.0f;
		glm::vec3 ViewDirection;		// normalize ( V[1][0], V[1][1], V[1][2] ) viewDirtion NOT FORWARD DIR
		float Empty = 0.0f;

		CameraData()
			: ViewProjectionMatrix(glm::mat4(-1.0f))
			, ViewMatrix(glm::mat4(-1.0f))
			, ProjectionMatrix(glm::mat4(-1.0f))
			, Position(glm::vec3(-1.0f))
			, Gamma(1.0f)
			, ViewDirection(glm::vec3(-1.0f))
		{
		}

		CameraData(const glm::mat4& viewProjection, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position, const glm::vec3& direction)
			: ViewProjectionMatrix(glm::mat4(-1.0f))
			, ViewMatrix(glm::mat4(-1.0f))
			, ProjectionMatrix(glm::mat4(-1.0f))
			, Position(glm::vec3(-1.0f))
			, Gamma(1.0f)
			, ViewDirection(direction)
		{
		}

		CameraData(const glm::mat4& viewProjection, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position, const glm::vec3& direction, float gamma)
			: ViewProjectionMatrix(viewProjection)
			, ViewMatrix(view)
			, ProjectionMatrix(projection)
			, Position(position)
			, Gamma(gamma)
			, ViewDirection(direction)

		{
		}

		
	};

	struct DisplayData
	{
		DisplayData()
			: ViewPortConf(glm::ivec4(-1, -1, -2, -2))
		{
		}

		DisplayData(const glm::ivec4& viewPortConf)
			: ViewPortConf(viewPortConf)
		{
		}

		DisplayData(const glm::uvec4& viewPortConf)
			: ViewPortConf(viewPortConf)
		{
		}

		DisplayData(const glm::vec4& viewPortConf)
			: ViewPortConf(viewPortConf)
		{
		}

		DisplayData(const glm::ivec2& size)
			: ViewPortConf(0, 0, size)
		{
		}

		DisplayData(const glm::ivec2& size, const glm::ivec2& offset)
			: ViewPortConf(size, offset)
		{
		}


		glm::ivec4 ViewPortConf; // w/z = withe/heigth | x/y = offset(x/y)
	};


}