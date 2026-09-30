#pragma once
#include "rypch.h"
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {

	template<typename T>
	struct RenderTextureResource
	{
		T m_Name;
		TextureSpecification m_Specification;
	};

	enum class FullscreenEffect
	{
		None = 0,
		Compute, ScreenQuad
	};

	template<typename T>
	struct RenderPassType
	{
		T m_Name;
		std::vector<T> m_OutputVec;
		T m_Shader;
		T m_DrawList;
		FullscreenEffect m_Fullscreen;
	};


	struct RenderTextureResourceSerialize
	{
		uint32_t m_HandleID;
		TextureSpecification m_Specification;
	};

	using RenderPassSerialize = RenderPassType<typename std::string>;

	using RenderTextureResourceRuntime = uint32_t;

	
	struct RenderPassRuntime : RenderPassType<uint32_t>
	{
		BufferLayout m_Layout;
	};
	
	class RenderGraphSerializer;

	class RenderGraph
	{
	public:
		inline static constexpr const char* const s_NoneTextureNameSerialize = "None";
		inline static constexpr const char* const s_MainImageTextureNameSerialize = "MainImage";

		inline static constexpr const char* const s_NoneRenderPassesSerialize = "None";
		inline static constexpr const char* const s_NoneDrawListSerialize = "None";

		inline static constexpr const char* const s_NoneShaderSerialize = "None";


		inline static constexpr uint32_t s_NoneTextureNameRuntime = std::numeric_limits<uint32_t>::max();
		inline static constexpr uint32_t s_MainTextureNameRuntime = 0u;
		inline static constexpr uint32_t s_FirstTextureNameRuntime = 1u;

		inline static constexpr uint32_t s_NoneRenderPassRuntime = std::numeric_limits<uint32_t>::max();
		inline static constexpr uint32_t s_FirstRenderPassRuntime = 0u;

		inline static constexpr uint32_t s_NoneDrawListRuntime = std::numeric_limits<uint32_t>::max();
		inline static constexpr uint32_t s_FirstDrawListRuntime = 0u;

		inline static constexpr uint32_t s_NoneShaderRuntime = std::numeric_limits<uint32_t>::max();
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

		struct Serialize
		{
			std::map<std::string, RenderTextureResourceSerialize> renderTextureResourceMap;
			std::map<std::string, uint32_t> shaderMap;
			std::map<std::string, uint32_t> drawListMap;

			std::vector<RenderPassSerialize> renderPassesVec;
		};
	public:
		RenderGraph()
			: m_Runtime()
			, m_Serialize()
		{
		}

		RenderGraph(const RenderGraph& renderGraph)
			: m_Runtime(renderGraph.m_Runtime)
			, m_Serialize(renderGraph.m_Serialize)
		{ 
			CheckRenderGraphValid();
		}

		RenderGraph(const Serialize& serialize, const Runtime& runtime)
			: m_Runtime(runtime)
			, m_Serialize(serialize)
		{
			CheckRenderGraphValid();
		}

		

		~RenderGraph()
		{

		}


		uint32_t GetRenderTextureResourceID(const std::string& resourceName) const
		{
			using Map = std::map<std::string, RenderTextureResourceSerialize>;
			const Map& mapRenderTexture = m_Serialize.renderTextureResourceMap;
			Map::const_iterator it = mapRenderTexture.find(resourceName);

			
			RY_CORE_ASSERT(it != mapRenderTexture.end(), "this name is not found!");
			uint32_t handleID = it->second.m_HandleID;
			return handleID;
			
		}

		uint32_t GetRenderPassesID(const std::string& resourceName) const
		{
			uint32_t handleID = 0u;
			for (const RenderPassSerialize& renderPass : m_Serialize.renderPassesVec)
			{
				if (renderPass.m_Name == resourceName)
					return handleID;
				handleID++;
			}

			RY_CORE_ASSERT(false, "No render Pass is defined withe!");
			return s_NoneRenderPassRuntime;
		}
		
		uint32_t GetDrawListResourceID(const std::string& resourceName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapDrawList = m_Serialize.drawListMap;
			Map::const_iterator it = mapDrawList.find(resourceName);

			RY_CORE_ASSERT(it != mapDrawList.end(), "this name is not found!");
			uint32_t handleID = it->second;
			return handleID;
		}

		uint32_t GetShaderResourceID(const std::string& resourceName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapShader = m_Serialize.shaderMap;
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

			constexpr ReservedState resavedTextureHandelStatesArray[] = {
					{s_MainTextureNameRuntime, s_MainImageTextureNameSerialize}
			};

			for (auto& [id, name] : resavedTextureHandelStatesArray)
			{
				RY_CORE_FATAL("resaved Names and Id Handles: ({}, {})", id, name);
			}
			RY_CORE_ASSERT(false, "this funktion works only withe ID that is not reserved IDs");
		}

		
		void CheckTextureResourceFromRenderPass(uint32_t textureRuntimeID, const std::string& textureSerializerName) const
		{
			using Map = std::map<std::string, RenderTextureResourceSerialize>;
			const Map& mapRenderTexture = m_Serialize.renderTextureResourceMap;

			RY_CORE_ASSERT(textureRuntimeID < m_Runtime.maxCountRenderTextureID, "outside valid shader id range!");
			Map::const_iterator it = mapRenderTexture.find(textureSerializerName);
			RY_CORE_ASSERT(it != mapRenderTexture.end(), "this name is not found");
			const uint32_t& renderTextureID = it->second.m_HandleID;

			RY_CORE_ASSERT(renderTextureID == textureRuntimeID, "not equal Index");
		}

		void CheckShaderResourceFromRenderPass(uint32_t shaderRuntimeID, const std::string& shaderSerializerName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapShader = m_Serialize.shaderMap;

			RY_CORE_ASSERT(shaderRuntimeID < m_Runtime.maxCountShaderID, "outside valid shader id range!");
			Map::const_iterator it = mapShader.find(shaderSerializerName);
			RY_CORE_ASSERT(it != mapShader.end(), "this name is not found");
			const uint32_t& shaderListNameID = it->second;

			RY_CORE_ASSERT(shaderListNameID == shaderRuntimeID, "not equal Index");
		}

		void CheckDrawListResourceFromRenderPass(uint32_t drawListRuntimeID, const std::string& drawListSerializerName) const
		{
			using Map = std::map<std::string, uint32_t>;
			const Map& mapDrawList = m_Serialize.drawListMap;

			RY_CORE_ASSERT(drawListRuntimeID < m_Runtime.maxCountDrawListID, "outside valid shader id range!");
			Map::const_iterator it = mapDrawList.find(drawListSerializerName);
			RY_CORE_ASSERT(it != mapDrawList.end(), "this name is not found");
			const uint32_t& shaderListNameID = it->second;

			RY_CORE_ASSERT(shaderListNameID == drawListRuntimeID, "not equal Index");
		}

		void CheckSingleOutPutTexture(const std::vector<uint32_t>& renderPassesRuntimeOutputVec, const std::vector<std::string>& renderPassesSerializerOutputVec) const
		{
			const uint32_t& textureRuntimeID = renderPassesRuntimeOutputVec.front();
			const std::string& textureSerializerName = renderPassesSerializerOutputVec.front();
			const bool isTextureNameMainSerializer = s_MainImageTextureNameSerialize == textureSerializerName;
			const bool isTextureNameMainRuntime = s_MainTextureNameRuntime == textureRuntimeID;
			const bool isMain = isTextureNameMainSerializer && isTextureNameMainRuntime;
			const bool isNotMain = !isTextureNameMainSerializer && !isTextureNameMainRuntime;
			const bool isValid = isNotMain || isMain;
			RY_CORE_ASSERT(isValid, "not Valid");


			if (isNotMain)
			{
				CheckTextureResourceFromRenderPass(textureRuntimeID, textureSerializerName);
			}
		}
		void CheckOutPutsTexture(const std::vector<uint32_t>& renderPassesRuntimeOutputVec, const std::vector<std::string>& renderPassesSerializeOutputVec) const
		{
			const uint32_t countRenderPassesVec = renderPassesRuntimeOutputVec.size();


			for (uint32_t i = 0; i < countRenderPassesVec; i++)
			{
				const uint32_t& textureRuntimeID = renderPassesRuntimeOutputVec.at(i);
				const std::string& textureSerializeName = renderPassesSerializeOutputVec.at(i);

				CheckTextureResourceFromRenderPass(textureRuntimeID, textureSerializeName);
			}
		}

		void CheckRenderPassShader(const RenderPassRuntime& renderPassRuntime, const RenderPassSerialize& renderPassSerialize) const
		{
			const uint32_t& shaderRuntimeID = renderPassRuntime.m_Shader;
			const std::string& shaderSerializeName = renderPassSerialize.m_Shader;
			CheckShaderResourceFromRenderPass(shaderRuntimeID, shaderSerializeName);

		}

		void CheckRenderPassDrawList(const RenderPassRuntime& renderPassRuntime, const RenderPassSerialize& renderPassSerialize) const
		{
			const uint32_t& drawListRuntimeID = renderPassRuntime.m_DrawList;
			const std::string& drawListSerilzeName = renderPassSerialize.m_DrawList;
			CheckDrawListResourceFromRenderPass(drawListRuntimeID, drawListSerilzeName);
		}


		void CheckRenderPassOutput(const RenderPassRuntime& renderPassRuntime, const RenderPassSerialize& renderPassSerialize) const
		{
			const uint32_t countRenderPassesVec = renderPassRuntime.m_OutputVec.size();
			RY_CORE_ASSERT(renderPassRuntime.m_OutputVec.size() == renderPassSerialize.m_OutputVec.size());
			if (1u == countRenderPassesVec)
			{
				CheckSingleOutPutTexture(renderPassRuntime.m_OutputVec, renderPassSerialize.m_OutputVec);
			}
			else
			{
				CheckOutPutsTexture(renderPassRuntime.m_OutputVec, renderPassSerialize.m_OutputVec);
			}
		}

		void CheckRenderGraphValid() const
		{			
			const uint32_t countRenderPassesVec = m_Runtime.renderPassesVec.size();

			bool isRenderTextureResourceVec = countRenderPassesVec == m_Serialize.renderPassesVec.size();
			bool isRenderPasses = m_Runtime.renderPassesVec.size() == m_Serialize.renderPassesVec.size();
			RY_CORE_ASSERT(isRenderPasses && isRenderPasses);
			for (uint32_t i = 0; i < countRenderPassesVec; i++)
			{
				const RenderPassRuntime& renderPassRuntime = m_Runtime.renderPassesVec.at(i);
				const RenderPassSerialize& renderPassSerialize = m_Serialize.renderPassesVec.at(i);
				RY_CORE_ASSERT(i == renderPassRuntime.m_Name, "not index in order");
				CheckRenderPassOutput(renderPassRuntime, renderPassSerialize);
				CheckRenderPassDrawList(renderPassRuntime, renderPassSerialize);
				CheckRenderPassDrawList(renderPassRuntime, renderPassSerialize);
				
			}
		}
#pragma endregion
		
	private:
		Runtime m_Runtime;
		Serialize m_Serialize;
	private:
		friend RenderGraphSerializer;
	};
}