#pragma once
#include <Rynex/Renderer/Rendering/Draw/ProtypeDrawCalls.h>

namespace Rynex {

	
	struct DrawCallKey
	{
		Shader* shaderPtr;
		DrawCallProcess* drawProcesPtr;
		VertexBuffer* gemotryShapBufferPtr;
		IndexBuffer* gemotryOrderBufferPtr;

		bool operator==(const DrawCallKey& rigth)
		{
			bool isShaderPtr = this->shaderPtr == rigth.shaderPtr;
			bool isDrawCallProcess = this->drawProcesPtr == rigth.drawProcesPtr;
			bool isGemotryShapBufferPtr = this->gemotryShapBufferPtr == rigth.gemotryShapBufferPtr;
			bool isGemotryOrderBufferPtr = this->gemotryOrderBufferPtr == rigth.gemotryOrderBufferPtr;
			bool result = isShaderPtr && isDrawCallProcess && isGemotryShapBufferPtr && isGemotryOrderBufferPtr;
			return result;
		}
	};

	struct DrawCallKeyHash
	{
		size_t operator()(const DrawCallKey& key) const
		{
			constexpr const size_t hashConstant = 0x9E3779b9;
			size_t shaderHash = reinterpret_cast<size_t>(key.shaderPtr);
			size_t drawProcesHash = reinterpret_cast<size_t>(key.drawProcesPtr);
			size_t gemotryShapeHash = reinterpret_cast<size_t>(key.gemotryShapBufferPtr);
			size_t gemotryOrderHash = reinterpret_cast<size_t>(key.gemotryOrderBufferPtr);

			size_t hash = shaderHash;

			gemotryShapeHash += hashConstant;
			gemotryShapeHash += (hash << 9);
			hash ^= gemotryShapeHash;

			gemotryOrderHash += hashConstant;
			gemotryOrderHash += (hash << 6);
			hash ^= gemotryOrderHash;

			drawProcesHash += hashConstant;
			drawProcesHash += (hash << 3);
			hash ^= drawProcesHash;
			
			return hash;
		}
	};
}