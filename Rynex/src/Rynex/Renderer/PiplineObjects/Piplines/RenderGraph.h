#pragma once
#include "rypch.h"
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {

	template<typename T>
	struct RenderTextureResource
	{
		T name;
		TextureSpecification specification;
	};

	enum class FullscreenEffekt
	{
		None = 0,
		Compute, Screenquad
	};

	template<typename T>
	struct RenderPassType
	{
		T name;
		std::vector<T> outputVec;
		T shader;
		T drawList;
		FullscreenEffekt fullscreen;
	};


	struct RenderTextureResourceSerilze
	{
		uint32_t handleID;
		TextureSpecification specification;
	};

	using RenderPassSerilze = RenderPassType<typename std::string>;

	using RenderTextureResourceRuntime = uint32_t;

	
	struct RenderPassRuntime : RenderPassType<uint32_t>
	{
		BufferLayout layout;
	};
	
	class RenderGraphSerializers;

	class RenderGraph
	{
	public:
		inline static constexpr const char* const s_NoneTextureNameSerilze = "None";
		inline static constexpr const char* const s_MainImgeTextureNameSerilze = "MainImage";

		inline static constexpr const char* const s_NoneRenderPassesSerilze = "None";
		inline static constexpr const char* const s_NoneDrawListSerilze = "None";

		inline static constexpr const char* const s_NoneShaderSerilze = "None";


		inline static constexpr uint32_t s_NoneTextureNameRuntime = MAXUINT32;
		inline static constexpr uint32_t s_MainTextureNameRuntime = 0u;
		inline static constexpr uint32_t s_FirstTextureNameRuntime = 1u;

		inline static constexpr uint32_t s_NoneRenderPassRuntime = MAXUINT32;
		inline static constexpr uint32_t s_FirstRenderPassRuntime = 0u;

		inline static constexpr uint32_t s_NoneDrawListRuntime = MAXUINT32;
		inline static constexpr uint32_t s_FirstDrawListRuntime = 0u;

		inline static constexpr uint32_t s_NoneShaderRuntime = MAXUINT32;
		inline static constexpr uint32_t s_FirstShaderRuntime = 0u;
	public:
		struct Runtime
		{
			std::vector<RenderPassRuntime> renderPassesVec;
			uint32_t maxCountRenderTextureID = s_FirstTextureNameRuntime;
			uint32_t maxCountRenderPassesID = s_FirstRenderPassRuntime;
			uint32_t maxCountDrawListID = s_FirstDrawListRuntime;
			uint32_t maxCountShaderID = s_FirstShaderRuntime;
		};

		struct Serilize
		{
			std::map<std::string, RenderTextureResourceSerilze> renderTextureResourceMap;
			std::map<std::string, uint32_t> shaderMap;
			std::map<std::string, uint32_t> drawListMap;

			std::vector<RenderPassSerilze> renderPassesVec;
		};
	public:
		RenderGraph()
			: m_Runtime()
			, m_Serilize()
		{
		}

		RenderGraph(const RenderGraph& rendergraph)
			: m_Runtime(rendergraph.m_Runtime)
			, m_Serilize(rendergraph.m_Serilize)
		{ 
			CheckRenderGraphValid();
		}

		RenderGraph(const Serilize& serilize, const Runtime& runtime)
			: m_Runtime(runtime)
			, m_Serilize(serilize)
		{
			CheckRenderGraphValid();
		}

		

		~RenderGraph()
		{

		}


		uint32_t GetRenderTextureResourceID(const std::string& resourceName) const
		{
			using Map = std::map<std::string, RenderTextureResourceSerilze>;
			const Map& mapRenderTexture = m_Serilize.renderTextureResourceMap;
			Map::const_iterator it = mapRenderTexture.find(resourceName);

			
			RY_CORE_ASSERT(it != mapRenderTexture.end(), "this name is not found!");
			uint32_t handleID = it->second.handleID;
			return handleID;
			
		}

		uint32_t GetRenderPassesID(const std::string& resourceName) const
		{
			uint32_t handleID = 0u;
			for (const RenderPassSerilze& renderPass : m_Serilize.renderPassesVec)
			{
				if (renderPass.name == resourceName)
					return handleID;
				handleID++;
			}

			RY_CORE_ASSERT(false, "No render Pass is defined withe!");
			return s_NoneRenderPassRuntime;
		}
		
		uint32_t GetDrawListResourceID(const std::string& resourceName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapDrawList = m_Serilize.drawListMap;
			Map::const_iterator it = mapDrawList.find(resourceName);

			RY_CORE_ASSERT(it != mapDrawList.end(), "this name is not found!");
			uint32_t handleID = it->second;
			return handleID;
		}

		uint32_t GetShaderResourceID(const std::string& resourceName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapShader = m_Serilize.shaderMap;
			Map::const_iterator it = mapShader.find(resourceName);

			RY_CORE_ASSERT(it != mapShader.end(), "this name is not found!");
			uint32_t handleID = it->second;
			return handleID;
		}
	private:

		
#pragma region CheckIsAllVaild
		static constexpr void AssertPrintTextureIDReservedHandle(bool check)
		{
			if (!check)
				return;

			struct ReservedState 
			{
				uint32_t id;
				const char* name;
			};

			constexpr ReservedState resevedTextureHandelsStatesArray[] = {
					{s_MainTextureNameRuntime, s_MainImgeTextureNameSerilze}
			};

			for (auto& [id, name] : resevedTextureHandelsStatesArray)
			{
				RY_CORE_FATAL("resved Names and Id Handles: ({}, {})", id, name);
			}
			RY_CORE_ASSERT(false, "this funktion works only withe ID that is not reserved IDs");
		}

		
		void CheckTextureResourceFromRenderPass(uint32_t textureRuntimeID, const std::string& textureSerilzerName) const
		{
			using Map = std::map<std::string, RenderTextureResourceSerilze>;
			const Map& mapRenderTexture = m_Serilize.renderTextureResourceMap;

			RY_CORE_ASSERT(textureRuntimeID < m_Runtime.maxCountRenderTextureID, "outsid vaild shader id range!");
			Map::const_iterator it = mapRenderTexture.find(textureSerilzerName);
			RY_CORE_ASSERT(it != mapRenderTexture.end(), "this name is not found");
			const uint32_t& renderTextureID = it->second.handleID;

			RY_CORE_ASSERT(renderTextureID == textureRuntimeID, "not equal Index");
		}

		void CheckShaderResourceFromRenderPass(uint32_t shaderRuntimeID, const std::string& shaderSerilzerName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapShader = m_Serilize.shaderMap;

			RY_CORE_ASSERT(shaderRuntimeID < m_Runtime.maxCountShaderID, "outsid vaild shader id range!");
			Map::const_iterator it = mapShader.find(shaderSerilzerName);
			RY_CORE_ASSERT(it != mapShader.end(), "this name is not found");
			const uint32_t& shaderListNameID = it->second;

			RY_CORE_ASSERT(shaderListNameID == shaderRuntimeID, "not equal Index");
		}

		void CheckDrawListResourceFromRenderPass(uint32_t drawListRuntimeID, const std::string& drawListSerilzerName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapDrawList = m_Serilize.drawListMap;

			RY_CORE_ASSERT(drawListRuntimeID < m_Runtime.maxCountDrawListID, "outsid vaild shader id range!");
			Map::const_iterator it = mapDrawList.find(drawListSerilzerName);
			RY_CORE_ASSERT(it != mapDrawList.end(), "this name is not found");
			const uint32_t& shaderListNameID = it->second;

			RY_CORE_ASSERT(shaderListNameID == drawListRuntimeID, "not equal Index");
		}

		void CheckSingleOutPutTexture(const std::vector<uint32_t>& renderPassesRuntimeoutputVec, const std::vector<std::string>& renderPassesSerilzeOutputVec) const
		{
			const uint32_t& textureRuntimeID = renderPassesRuntimeoutputVec.front();
			const std::string& textureSerilzerName = renderPassesSerilzeOutputVec.front();
			bool isTextureNameMainSerilzer = s_MainImgeTextureNameSerilze == textureSerilzerName;
			bool isTextureNameMainRuntime = s_MainTextureNameRuntime == textureRuntimeID;
			bool isMain = isTextureNameMainSerilzer && isTextureNameMainRuntime;
			bool isNotMain = !isTextureNameMainSerilzer && !isTextureNameMainRuntime;
			bool isVaild = isNotMain || isMain;
			RY_CORE_ASSERT(isVaild, "not Vaild");


			if (isNotMain)
			{
				CheckTextureResourceFromRenderPass(textureRuntimeID, textureSerilzerName);
			}
		}
		void CheckOutPutsTexture(const std::vector<uint32_t>& renderPassesRuntimeoutputVec, const std::vector<std::string>& renderPassesSerilzeOutputVec) const
		{
			const uint32_t countRenderPassesVec = renderPassesRuntimeoutputVec.size();


			for (uint32_t i = 0; i < countRenderPassesVec; i++)
			{
				const uint32_t& textureRuntimeID = renderPassesRuntimeoutputVec.at(i);
				const std::string& textureSerilzerName = renderPassesSerilzeOutputVec.at(i);

				CheckTextureResourceFromRenderPass(textureRuntimeID, textureSerilzerName);
			}
		}

		void CheckRenderPassShader(const RenderPassRuntime& renderPassRuntime, const RenderPassSerilze& renderPassSerilze) const
		{
			const uint32_t& shaderRuntimeID = renderPassRuntime.shader;
			const std::string& shaderSerilzeName = renderPassSerilze.shader;
			CheckShaderResourceFromRenderPass(shaderRuntimeID, shaderSerilzeName);

		}

		void CheckRenderPassDrawList(const RenderPassRuntime& renderPassRuntime, const RenderPassSerilze& renderPassSerilze) const
		{
			const uint32_t& drawListRuntimeID = renderPassRuntime.drawList;
			const std::string& drawListSerilzeName = renderPassSerilze.drawList;
			CheckDrawListResourceFromRenderPass(drawListRuntimeID, drawListSerilzeName);
		}


		void CheckRenderPassOutput(const RenderPassRuntime& renderPassRuntime, const RenderPassSerilze& renderPassSerilze) const
		{
			const uint32_t countRenderPassesVec = renderPassRuntime.outputVec.size();
			RY_CORE_ASSERT(renderPassRuntime.outputVec.size() == renderPassSerilze.outputVec.size());
			if (1u == countRenderPassesVec)
			{
				CheckSingleOutPutTexture(renderPassRuntime.outputVec, renderPassSerilze.outputVec);
			}
			else
			{
				CheckOutPutsTexture(renderPassRuntime.outputVec, renderPassSerilze.outputVec);
			}
		}

		void CheckRenderGraphValid() const
		{			
			const uint32_t countRenderPassesVec = m_Runtime.renderPassesVec.size();

			bool isRenderTextureResourceVec = countRenderPassesVec == m_Serilize.renderPassesVec.size();
			bool isRenderPasses = m_Runtime.renderPassesVec.size() == m_Serilize.renderPassesVec.size();
			RY_CORE_ASSERT(isRenderPasses && isRenderPasses);
			for (uint32_t i = 0; i < countRenderPassesVec; i++)
			{
				const RenderPassRuntime& renderPassRuntime = m_Runtime.renderPassesVec.at(i);
				const RenderPassSerilze& renderPassSerilze = m_Serilize.renderPassesVec.at(i);
				RY_CORE_ASSERT(i == renderPassRuntime.name, "not index in order");
				CheckRenderPassOutput(renderPassRuntime, renderPassSerilze);
				CheckRenderPassDrawList(renderPassRuntime, renderPassSerilze);
				CheckRenderPassDrawList(renderPassRuntime, renderPassSerilze);
				
			}
		}
#pragma endregion
		
	private:
		Runtime m_Runtime;
		Serilize m_Serilize;
	private:
		friend RenderGraphSerializers;
	};
}