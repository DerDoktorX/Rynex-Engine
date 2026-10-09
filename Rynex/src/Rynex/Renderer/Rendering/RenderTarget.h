#pragma once


#include <Rynex/Renderer/Rendering/ShaderComputeList.h>
#include <Rynex/Core/BufferData.h>






namespace Rynex {
	class PiplineRefBaseVec;
	class PiplineRenderBase;


	class RenderTarget
	{
	private:
		struct AlphaPiplineBase
		{
			Ref<PiplineRenderBase> m_BindingLayoutPipline;
			float m_Distend;


			AlphaPiplineBase(const Ref<PiplineRenderBase>& piline, float distenz)
				: m_BindingLayoutPipline(piline)
				, m_Distend(distenz)
			{
			}

			static bool SortByDistend(const AlphaPiplineBase& a, const AlphaPiplineBase& b)
			{
				return a.m_Distend < b.m_Distend;
			}
		};
	public:
		RenderTarget();
		RenderTarget(const Ref<Framebuffer>& fb);
		static Ref<RenderTarget> Copy(const Ref<RenderTarget>& renderTarget);
		~RenderTarget();
		
		void AddDrawPassVec(std::vector<ShaderDrawResource>& drawListVec);
		void AddDrawPass(Const_ShaderDrawResource_Ref drawList);
		ShaderDrawResource GetDrawPassOrAdd(const Ref<Shader>& shader);
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
		bool HasDrawPass(Const_ShaderDrawResource_RefPtr drawList);
#endif
		static bool HasDrawPass(Const_ShaderDrawResourceWeak_Ref drawList);
		bool HasDrawPass(const Ref<Shader>& shader, uint32_t index) const;
		ShaderDrawResourceWeak_Ref GetDrawPass(uint32_t index);
		uint32_t AddNewDrawPass(const Ref<Shader>& shader, Const_ShaderDrawResourceWeak_Ref shaderDrawList);

		void BindFramebuffer()const;
		int DrawBufferList();
		int DrawBufferList(int renderMode);

		int DrawSingle();
		int DrawSingle(int renderMode);

		void UnbindFB()const;

		void DrawSort()const;
		void SortShaderDraw();
		void SortList();
		void SortedPiplineList();
		void SortedPiplineAlphaList();

		void ResizeView(const glm::ivec4& renderViewSize);
		void ResizeView(const glm::vec4& renderViewSize);
		void ResizeView(float xOffset, float yOffset, float withe, float heigth) { ResizeView(glm::vec4{ withe, heigth, xOffset, yOffset }); }
		void ResizeView(const glm::vec2& offset, const glm::vec2& size) { ResizeView(glm::vec4{ size,  offset }); }

		void ClearShaderDrawList();

		void ClearFramebuffer() { m_FB = nullptr; m_ClearColorFuncVec.clear(); };


		void SetFramebuffer(const Ref<Framebuffer>& fb);
		void SetClearColorAttachment(uint32_t index, glm::vec4 clearColor);
		void SetClearColorAttachment(uint32_t index, glm::ivec4 clearColor);
		void SetClearColorAttachment(uint32_t index, int clearColor);

		std::vector<Ref<PiplineRenderBase>> GetPiplineRefVecCopy() const;


		const std::vector<ShaderDrawResourceWeak>& GetShaderDrawList() const { return m_ShadeDrawList; }

		const Ref<Framebuffer>& GetFramebuffer() const { return m_FB; }


		bool HasShader(const Ref<Shader>& shader) const;

		const glm::vec4& GetRenderViewSize() const { return m_RenderViewSize; }

		void AddPipline(Ref<PiplineRenderBase> pipline);
		void AddPiplineAlpha(Ref<PiplineRenderBase> pipline, float distenz);

		void SetPiplineAlphaDistend(uint32_t index, float distend);//distend

		

		int DrawPiplineList();
		int DrawPiplineList(int mode);

		int DrawAlphaPiplineList();
		int DrawAlphaPiplineList(int mode);

		void ClearPiplineList();
		void ClearAlphaPiplineList();

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
		uint32_t GetDrawListCount() const { return m_ShadeDrawList.Size(); }
#else
		uint32_t GetDrawListCount() const { return m_ShadeDrawList.size(); }
#endif
		uint32_t GetPiplineListBaseCount() const { return m_PiplineBaseVec.size(); }

		void ClearFramebufferImageList();
		void ClearFramebufferDepth();
		
	private:
		
		static bool IsDataTypeValidToAttachment(const FramebufferTextureSpecification& frameTexSpec, const int& value);
		static bool IsDataTypeValidToAttachment(const FramebufferTextureSpecification& frameTexSpec, const glm::vec4& value);
		static bool SortRenderList(ShaderDrawResourceWeak_Ref aWeak, ShaderDrawResourceWeak_Ref bWeak);
		static bool SortRenderListNotEqualShader(const Ref<Shader>& aShader, const Ref<Shader>& bShader);
		static bool SortRenderListNotEqual(Const_ShaderDrawResource_RefPtr aWeak, Const_ShaderDrawResource_RefPtr bWeak);
		template<typename T, size_t _Size>
		constexpr static glm::u64vec2 SortRenderListChangeBindBointCount(const std::array<T, _Size>& aBindArray, const std::array<T, _Size>& bBindArray)
		{
			size_t equalNotCount = 0ull;
			size_t equalCount = 0ull;
			for (size_t i = 0; i < _Size; i++)
			{
				if constexpr (std::is_same_v<T, std::variant<Ref<Texture>, Ref<LinkedTextureArray>>>)
				{

					bool aResult = std::visit([](auto& textureA) { return nullptr == textureA; }, aBindArray[i]);
					bool bResult = std::visit([](auto& textureB) { return nullptr == textureB; }, bBindArray[i]);

					if (aResult && bResult)
						continue;
				}
				if constexpr (!std::is_same_v<T, std::variant<Ref<Texture>, Ref<LinkedTextureArray>>>)
				{
					if (nullptr == aBindArray[i] && nullptr == bBindArray[i])
						continue;
				}

				if (aBindArray[i] == bBindArray[i])
					equalCount++;
				else
					equalNotCount++;
			}
			return glm::u64vec2(equalCount, equalNotCount);
		}

		void CheckExecuteDrawList(uint32_t drawListCount);
	private:
		Ref<Framebuffer> m_FB;
		std::vector<std::function<void(const Ref<Framebuffer>&, uint32_t)>> m_ClearColorFuncVec;
		glm::vec4 m_RenderViewSize;	// Size(x,y) / Offset(z,w)	
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
		MapVectorWeak<ShaderDrawList>		m_ShadeDrawList;
#else
		std::vector<ShaderDrawResource>		m_ShadeDrawList;
#endif
		std::vector<Ref<PiplineRenderBase>> m_PiplineBaseVec;


		
		std::vector<AlphaPiplineBase>		m_PiplineAlphaBaseVec;
		

	};
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
	template<>
	inline glm::u64vec2 RenderTarget::SortRenderListChangeBindBointCount<std::variant<Ref<StorageBuffer>, Ref<BindlesdTextureArray>>, g_StorageBindArrayCount>(
		const StorageBindArray& aBindArray, const StorageBindArray& bBindArray)
	{
		size_t equalNotCount = 0ull;
		size_t equalCount = 0ull;
		for (size_t i = 0; i < g_StorageBindArrayCount; i++)
		{
			int64_t aPtrSSBO = std::visit(
				[](const auto& aRef) { int64_t ptrValue = reinterpret_cast<int64_t>(aRef.get()); return ptrValue; }
			, aBindArray[i]);

			int64_t bPtrSSBO = std::visit(
				[](const auto& bRef) { int64_t ptrValue = reinterpret_cast<int64_t>(bRef.get()); return ptrValue; }
			, bBindArray[i]);


			if (aPtrSSBO != 0 && bPtrSSBO != 0)
				continue;

			if (aPtrSSBO == bPtrSSBO)
				equalCount++;
			else
				equalNotCount++;
		}
		return glm::u64vec2(equalCount, equalNotCount);
	}
#endif
}



