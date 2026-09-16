#pragma once


namespace Rynex {

	struct DebugCameraData
	{
		DebugCameraData()
			: m_InverseProjectionView(glm::mat4(-1.0f))
		{
		}

        explicit DebugCameraData(const glm::mat4& inverseProjectionView)
			: m_InverseProjectionView(inverseProjectionView)
		{
		}

		glm::mat4 m_InverseProjectionView;
	};

	// M = WorldMatrix or CameraMatrix
	// P = Projection Matrix -> NDC [-1, 1], Right-Handed
	// V = ViewMatrix = inverse(M)
	// xyz = Position Vector = (M[3][0], M[3][1], M[3][2]) * M[3][3]
	struct CameraData
	{
		glm::mat4 m_ViewProjectionMatrix; // VP = V * P
		glm::mat4 m_ViewMatrix;			// V
		glm::mat4 m_ProjectionMatrix;		// P
		glm::vec3 m_Position;				// (WorldMatrix[3][0] * , WorldMatrix[3][1], WorldMatrix[3][2])
		float m_Gamma = 1.0f;
		glm::vec3 m_ViewDirection;		// normalize ( V[1][0], V[1][1], V[1][2] ) viewDirtion NOT FORWARD DIR
		float m_Empty = 0.0f;

		CameraData()
			: m_ViewProjectionMatrix(glm::mat4(-1.0f))
			, m_ViewMatrix(glm::mat4(-1.0f))
			, m_ProjectionMatrix(glm::mat4(-1.0f))
			, m_Position(glm::vec3(-1.0f))
			, m_Gamma(1.0f)
			, m_ViewDirection(glm::vec3(-1.0f))
		{
		}

		CameraData(const glm::mat4& viewProjection, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position, const glm::vec3& direction)
			: m_ViewProjectionMatrix(glm::mat4(-1.0f))
			, m_ViewMatrix(glm::mat4(-1.0f))
			, m_ProjectionMatrix(glm::mat4(-1.0f))
			, m_Position(glm::vec3(-1.0f))
			, m_Gamma(1.0f)
			, m_ViewDirection(direction)
		{
		}

		CameraData(const glm::mat4& viewProjection, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position, const glm::vec3& direction, float gamma)
			: m_ViewProjectionMatrix(viewProjection)
			, m_ViewMatrix(view)
			, m_ProjectionMatrix(projection)
			, m_Position(position)
			, m_Gamma(gamma)
			, m_ViewDirection(direction)

		{
		}

		
	};

	struct DisplayData
	{
	    glm::ivec4 m_ViewPortConf; // w/z = withe/heigth | x/y = offset(x/y)
	    
		DisplayData()
			: m_ViewPortConf(glm::ivec4(-1, -1, -2, -2))
		{
		}

		DisplayData(const glm::ivec4& viewPortConf)
			: m_ViewPortConf(viewPortConf)
		{
		}

		DisplayData(const glm::uvec4& viewPortConf)
			: m_ViewPortConf(viewPortConf)
		{
		}

		DisplayData(const glm::vec4& viewPortConf)
			: m_ViewPortConf(viewPortConf)
		{
		}

		DisplayData(const glm::ivec2& size)
			: m_ViewPortConf(0, 0, size)
		{
		}

		DisplayData(const glm::ivec2& size, const glm::ivec2& offset)
			: m_ViewPortConf(size, offset)
		{
		}



	};


}