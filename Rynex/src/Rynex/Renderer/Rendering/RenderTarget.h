#pragma once
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Core/BufferData.h>


#define RY_TEST_SORT_EFICENTS 0

// #define RY_SHADER_DRAW_LIST_SHEARD_PTR

#define RY_SSBO_VARIENTS
#define RY_TEXTURE_VARIENTS

namespace Rynex {
	class PiplineRefBaseVec;
	class PiplineRenderBase;

	static constexpr size_t g_TextureBindArrayCount = 16ull;
	static constexpr size_t g_UniformBindArrayCount = 36ull;
	static constexpr size_t g_StorageBindArrayCount = 8ull;

#ifdef RY_TEXTURE_VARIENTS
	using TextureVarients = std::variant<Ref<Texture>, Ref<LinkedTextureArray>>;
	using TextureBindArray = std::array<TextureVarients, g_TextureBindArrayCount>;
#else
	typedef std::array<Ref<Texture>, g_TextureBindArrayCount> TextureBindArray;
#endif

	using UniformBindArray = std::array<Ref<UniformBuffer>, g_UniformBindArrayCount>;
#ifdef RY_SSBO_VARIENTS
	using StorageBufferVarients = std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>;
	using StorageBindArray = std::array<StorageBufferVarients, g_StorageBindArrayCount>;
#else
	typedef std::array<Ref<StorageBuffer>, g_StorageBindArrayCount > StorageBindArray;
#endif

	struct BindingShaderLayoutStatic
	{
		TextureBindArray m_BindTextures; // bindTextures
		UniformBindArray m_BindUniform;
		StorageBindArray m_BindStorage;

		bool operator==(const Ref<BindingShaderLayoutStatic>& right) const
		{
			return IsEqualSort(right);
		}

		bool operator==(const BindingShaderLayoutStatic& right) const
		{
			return IsEqualSort(right);
		}

		bool operator!=(const BindingShaderLayoutStatic& right) const
		{
			return !IsEqualSort(right);
		}
		bool IsEqualSort(const BindingShaderLayoutStatic& right) const
		{
			constexpr size_t sTextureMultyplyer = 1;
			constexpr size_t sUniformMultyplyer = 1;
			constexpr size_t sStorageMultyplyer = 1;
			int eqaul = 0;
			int notEqual = 0;
			for (int i = 0; i < m_BindTextures.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				bool isThisValidSSBO = std::visit([](const auto& refThis) { return nullptr == refThis; }, this->m_BindStorage[i]);
				bool isRightValidSSBO = std::visit([](const auto& refRight) { return nullptr == refRight; }, right.m_BindStorage[i]);

				if (isThisValidSSBO && isRightValidSSBO)
					continue;
#else
				if (this->m_BindStorage[i] && right->m_BindStorage[i])
					continue;
#endif

#ifdef RY_SSBO_VARIENTS
				if (this->m_BindTextures[i] == right.m_BindTextures[i])
#else
				if (this->m_BindTextures[i] == right->m_BindTextures[i])
#endif
					eqaul += sTextureMultyplyer;
				else
					notEqual += sTextureMultyplyer;
			}

			for (int i = 0; i < m_BindUniform.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS

				bool isThisValidSSBO = std::visit([](const auto& refThis)->bool { return nullptr == refThis; }, this->m_BindStorage[i]);
				bool isRightValidSSBO = std::visit([](const auto& refRight)->bool { return nullptr == refRight; }, right.m_BindStorage[i]);

				if (isThisValidSSBO && isRightValidSSBO)
					continue;
#else
				if (this->m_BindStorage[i] && right->m_BindStorage[i])
					continue;
#endif

				if (this->m_BindUniform[i] == right.m_BindUniform[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
			}

			for (int i = 0; i < m_BindStorage.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				const int64_t thisPtrSSBO = std::visit(
				    [](const auto& refThis)-> int64_t
				    {
				        int64_t ptrValue = reinterpret_cast<int64_t>(refThis.get());
				        return ptrValue;
				    }, this->m_BindStorage[i]);

				int64_t rightPtrSSBO = std::visit(
				    [](const auto& refRight)-> int64_t
				    {
				        int64_t ptrValue = reinterpret_cast<int64_t>(refRight.get());
				        return ptrValue;
				    }, right.m_BindStorage[i]);

				if (0ll != thisPtrSSBO && 0ull != rightPtrSSBO)
					continue;

				if (thisPtrSSBO == rightPtrSSBO)
					eqaul += sStorageMultyplyer;
				else
					notEqual += sStorageMultyplyer;
#else
				if (this->m_BindStorage[i] == rigth->m_BindStorage[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
#endif
			}
			bool result = eqaul > notEqual;
			return result;
		}


		bool IsEqualSort(const Ref<BindingShaderLayoutStatic>& right) const
		{
			constexpr size_t sTextureMultyplyer = 1;
			constexpr size_t sUniformMultyplyer = 1;
			constexpr size_t sStorageMultyplyer = 1;
			int eqaul = 0;
			int notEqual = 0;
			for (int i = 0; i < m_BindTextures.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				bool isthisVaildSSBO = std::visit(
				    [](const auto& refThis)->bool
				    { return nullptr == refThis; }
				    , this->m_BindStorage[i]);
				bool isRigthVaildSSBO = std::visit(
				    [](const auto& refRight)->bool
				    { return nullptr == refRight; }
				    , right->m_BindStorage[i]);

				if (isthisVaildSSBO && isRigthVaildSSBO)
					continue;
#else
				if (this->m_BindStorage[i] && right->m_BindStorage[i])
					continue;
#endif

#ifdef RY_SSBO_VARIENTS
				if (this->m_BindTextures[i] == right->m_BindTextures[i])
#else
				if (this->m_BindTextures[i] == right->m_BindTextures[i])
#endif
					eqaul += sTextureMultyplyer;
				else
					notEqual += sTextureMultyplyer;
			}

			for (int i = 0; i < m_BindUniform.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS

				const bool isThisValidSSBO = std::visit([](const auto& refThis) -> bool  { return nullptr == refThis; }, this->m_BindStorage[i]);
				const bool isRightValidSSBO = std::visit([](const auto& refRight)-> bool { return nullptr == refRight; }, right->m_BindStorage[i]);

				if (isThisValidSSBO && isRightValidSSBO)
					continue;
#else
				if (this->m_BindStorage[i] && right->m_BindStorage[i])
					continue;
#endif

				if (this->m_BindUniform[i] == right->m_BindUniform[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
			}

			for (int i = 0; i < m_BindStorage.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				const int64_t thisPtrSSBO = std::visit(
				    [](const auto& refThis) -> int64_t
				    {
				        int64_t ptrValue = reinterpret_cast<int64_t>(refThis.get());
				        return ptrValue;
				    }, this->m_BindStorage[i]);
				const int64_t rightPtrSSBO = std::visit(
				    [](const auto& refRight) -> int64_t
				    {
				        int64_t ptrValue = reinterpret_cast<int64_t>(refRight.get());
				        return ptrValue;
				    }, right->m_BindStorage[i]);

				if (thisPtrSSBO != 0ll && rightPtrSSBO != 0ll)
					continue;

				if (thisPtrSSBO == rightPtrSSBO)
					eqaul += sStorageMultyplyer;
				else
					notEqual += sStorageMultyplyer;
#else
				if (this->m_BindStorage[i] == rigth->m_BindStorage[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
#endif
			}
			bool result = eqaul > notEqual;
			return result;
		}

		bool IsEqualComplet(const Ref<BindingShaderLayoutStatic>& rigth) const
		{
			constexpr size_t sTextureMultyplyer = 1;
			constexpr size_t sUniformMultyplyer = 1;
			constexpr size_t sStorageMultyplyer = 1;
			int eqaul = 0;
			int notEqual = 0;
			for (int i = 0; i < m_BindTextures.size(); i++)
			{
				if (this->m_BindTextures[i] == rigth->m_BindTextures[i])
					notEqual += sTextureMultyplyer;
			}

			for (int i = 0; i < m_BindUniform.size(); i++)
			{
				if (this->m_BindUniform[i] != rigth->m_BindUniform[i])
					notEqual += sUniformMultyplyer;
			}

			for (int i = 0; i < m_BindStorage.size(); i++)
			{
				if (this->m_BindStorage[i] != rigth->m_BindStorage[i])
					notEqual += sStorageMultyplyer;
			}
			
			
			bool result = 0 == notEqual;
			return result;
		}

		void Sort()
		{
#ifdef RY_TEXTURE_VARIENTS //aVariant
			std::sort(m_BindTextures.begin(), m_BindTextures.end(),
				[](std::variant<Ref<Texture>, Ref<LinkedTextureArray>>& aVariant, std::variant<Ref<Texture>, Ref<LinkedTextureArray>>& bVariant)
				{ 
					return std::visit([&bVariant](auto& aValue) -> bool
						{
							if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<Texture>>)
							{
								return std::visit(
								    [&aValue](auto& bValue) -> bool
								    {
									if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<Texture>>)
										return nullptr != aValue && nullptr == bValue;
									return false;
									}, bVariant);
							}
							else if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<LinkedTextureArray>>)
							{
								return std::visit(
								    [&aValue](auto& bValue) -> bool
								    {
									    if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<LinkedTextureArray>>)
									    	return nullptr != aValue && nullptr == bValue;
									    return false;
									}, bVariant);
							}
                            return false;
						}, aVariant);
				}
			);
#else
			std::sort(m_BindTextures.begin(), m_BindTextures.end(), [](Ref<Texture>& a, Ref<Texture>& b) {return nullptr != a && nullptr == b;  });
#endif

#ifdef RY_SSBO_VARIENTS
			std::sort(m_BindStorage.begin(), m_BindStorage.end(),
				[](std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>& aVariant, std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>& bVariant)
				{
					return std::visit(
					    [&bVariant](auto& aValue)-> bool
						{
							if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<StorageBuffer>>)
							{
								return std::visit(
								    [&aValue](auto& bValue) -> bool
								    {
									    if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<StorageBuffer>>)
									    	return nullptr != aValue && nullptr == bValue;
									    return false;
									}, bVariant);
							}
							else if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<BindlesTextureArray>>)
							{
								return std::visit(
								    [&aValue](auto& bValue) -> bool
								    {
									    if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<BindlesTextureArray>>)
									    	return nullptr != aValue && nullptr == bValue;
									    return false;
									}, bVariant);
							}
						}, aVariant);
				}
			);
#else
			std::sort(m_BindStorage.begin(), m_BindStorage.end(), [](Ref<StorageBuffer>& a, Ref<StorageBuffer>& b) {return nullptr != a && nullptr == b;  });
#endif
			std::sort(m_BindUniform.begin(), m_BindUniform.end(),
			    [](const Ref<UniformBuffer>& a, const Ref<UniformBuffer>& b) -> bool
			    {
			        return nullptr != a && nullptr == b;
			    }
			);
		}

		void Clear()
		{
#ifdef RY_TEXTURE_VARIENTS
			for (TextureVarients& texure : m_BindTextures)
			{
				std::visit([](auto& ref) { RY_DESTROY_REF(ref); }, texure);
			}
#else
			for (Ref<Texture>& tex : m_BindTextures)
			{
				RY_DESTROY_REF(tex);
			}
#endif

			for (Ref<UniformBuffer>& uniform : m_BindUniform)
			{
				RY_DESTROY_REF(uniform);
			}
#ifdef RY_SSBO_VARIENTS
			for (StorageBufferVarients& storage : m_BindStorage)
			{
				std::visit([](auto& ref) { RY_DESTROY_REF(ref); }, storage);
			}
#else
			for (Ref<StorageBuffer>& storage : m_BindStorage)
			{
				RY_DESTROY_REF(storage);
			}
#endif
		}

	};

	


#if 0	
	using BindBufferGPU = std::variant<Ref<UniformBuffer>, Ref<Texture>, Ref<LinkedTextureArray>, Ref<StorageBuffer>, Ref<BindlesTextureArray>>;
	using BindBufferContainer = std::vector<std::pair<uint32_t, BindBufferGPU>>;

	struct BindingShaderLayoutDynamic
	{
		BindBufferContainer bindeBufferVec;

	};
#endif

	struct ShaderDrawList
	{
		Ref<Shader> m_ShaderProgram;
		Ref<VertexArray> m_VAO;
		uint32_t m_IndicesCount = 0u;
		Ref<IndirectBuffer> m_DrawBuffer;
		Mesh::PerDrawObject m_DrawElement;
		BindingShaderLayoutStatic m_BindingLayout;
		int m_RenderMode = 0;

		ShaderDrawList()
			: m_ShaderProgram(nullptr)
			, m_VAO(nullptr)
			, m_IndicesCount(0u)
			, m_DrawBuffer(nullptr)
			, m_DrawElement({0u, 0u, 0u, -1, 0u})
			, m_BindingLayout()
			, m_RenderMode(0)
		{
		}

		ShaderDrawList(
			const Ref<Shader>& programm
			, const Ref<VertexArray>& vao
			, const uint32_t indicesCount = 0u
			, const Ref<IndirectBuffer>& drawBuffer = nullptr
			, const Mesh::PerDrawObject& drawElement = Mesh::PerDrawObject()
			, const BindingShaderLayoutStatic& bindingLayout = BindingShaderLayoutStatic()
			, const int renderMode = 0
		)
			: m_ShaderProgram(programm)
			, m_VAO(vao)
			, m_IndicesCount(indicesCount)
			, m_DrawBuffer(drawBuffer)
			, m_DrawElement(drawElement)
			, m_BindingLayout(bindingLayout)
			, m_RenderMode(renderMode)
		{
		}

		void Sort()
		{
			m_BindingLayout.Sort();
		}

		void Clear()
		{
			RY_DESTROY_REF(m_ShaderProgram);
			RY_DESTROY_REF(m_DrawBuffer);
			RY_DESTROY_REF(m_VAO);
			m_BindingLayout.Clear();
		}
#ifdef RY_USE_REF_BINDING_SHADER_LAYOUT
		TextureBindArray& GetBindeTextures() { return m_BindingLayout->m_BindTextures; }
		UniformBindArray& GetBindeUniform() { return m_BindingLayout->m_BindUniform; }
		StorageBindArray& GetBindeStorage() { return m_BindingLayout->m_BindStorage; }

		const TextureBindArray& GetBindeTextures() const { return m_BindingLayout->m_BindTextures; }
		const UniformBindArray& GetBindeUniform() const { return m_BindingLayout->m_BindUniform; }
		const StorageBindArray& GetBindeStorage() const { return m_BindingLayout->m_BindStorage; }

#else
		TextureBindArray& GetBindTextures() { return m_BindingLayout.m_BindTextures; }
		UniformBindArray& GetBindUniform() { return m_BindingLayout.m_BindUniform; }
		StorageBindArray& GetBindStorage() { return m_BindingLayout.m_BindStorage; }

		const TextureBindArray& GetBindTextures() const { return m_BindingLayout.m_BindTextures; }
		const UniformBindArray& GetBindUniform() const { return m_BindingLayout.m_BindUniform; }
		const StorageBindArray& GetBindStorage() const { return m_BindingLayout.m_BindStorage; }

#endif
	};

	RY_NONE_MEBER_OPERATOR_BOOL(ShaderDrawList, == , &&,
		m_ShaderProgram, m_BindingLayout
		, m_DrawBuffer, m_VAO, m_IndicesCount
		, m_DrawElement.m_Count, m_DrawElement.m_InstancesCount, m_DrawElement.m_FirstIndex, m_DrawElement.m_BaseVertex, m_DrawElement.m_BaseInstance
		, m_RenderMode
	);

	RY_NONE_MEBER_OPERATOR_BOOL(ShaderDrawList, != , ||,
		m_ShaderProgram, m_BindingLayout
		, m_DrawBuffer, m_VAO, m_IndicesCount
		, m_DrawElement.m_Count, m_DrawElement.m_InstancesCount, m_DrawElement.m_FirstIndex, m_DrawElement.m_BaseVertex, m_DrawElement.m_BaseInstance
		, m_RenderMode
	);

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
	using ShaderDrawResource = Ref<ShaderDrawList>;
	using ShaderDrawResourceWeak = Weak<ShaderDrawList>;
	using ShaderDrawResourceWeak_Ref = Weak<ShaderDrawList>&;
	using Const_ShaderDrawResourceWeak_Ref = const Weak<ShaderDrawList>&;
	using ShaderDrawResource_Ref = Ref<ShaderDrawList>&;
	using ShaderDrawResource_RefPtr = Ref<ShaderDrawList>&;

	using Const_ShaderDrawResource_Ref = const Ref<ShaderDrawList>&;
	using Const_ShaderDrawResource_RefPtr = const Ref<ShaderDrawList>&;


	template<typename ... Args>
	inline constexpr ShaderDrawResource CreateShaderDrawResource(Args&& ... args)
	{
		return CreateRef<ShaderDrawList>(std::forward<Args>(args)...);
	}

#else
	using ShaderDrawResource = ShaderDrawList;
	using ShaderDrawResource_Ptr = ShaderDrawList*;
	using ShaderDrawResourceWeak = ShaderDrawList;
	using Const_ShaderDrawResourceWeak_Ref = const ShaderDrawList&;
	using ShaderDrawResourceWeak_Ref = ShaderDrawList&;

	using ShaderDrawResource_Ref = ShaderDrawList&;
	using ShaderDrawResource_RefPtr = ShaderDrawList*;

	using Const_ShaderDrawResource_Ref = const ShaderDrawList&;
	using Const_ShaderDrawResource_RefPtr = const ShaderDrawList*;


	template<typename ... Args>
	inline static ShaderDrawResource CreateShaderDrawResource(Args&& ... args)
	{
		return ShaderDrawList(std::forward<Args>(args)...);
	}
#endif
	


	struct ShaderComputeList
	{
		Ref<Shader> m_Shader;
		Ref<IndirectBuffer> m_DispatchBuffer;
		glm::uvec3 m_DispatchGroups;

		Ref<BindingShaderLayoutStatic> m_BindingLayout;

		ShaderComputeList()
			: m_Shader(nullptr)
			, m_DispatchBuffer(nullptr)
			, m_DispatchGroups(0u,0u,0u)
			, m_BindingLayout(nullptr)
		{
			m_BindingLayout = CreateRef<BindingShaderLayoutStatic>();
		}

		void Sort()
		{
			m_BindingLayout->Sort();
		}

		void Clear()
		{
			RY_DESTROY_REF(m_Shader);
			RY_DESTROY_REF(m_DispatchBuffer);


			m_BindingLayout->Clear();
			RY_DESTROY_REF(m_BindingLayout);
		}
	};



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
		uint32_t GetPiplineListBaseCount() const { return m_PilineBaseVec.size(); }

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
		std::vector<Ref<PiplineRenderBase>> m_PilineBaseVec;


		
		std::vector<AlphaPiplineBase>		m_PilineAlphaBaseVec;
		

	};
#ifdef RY_SSBO_VARIENTS
	template<>
	inline glm::u64vec2 RenderTarget::SortRenderListChangeBindBointCount<std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>, g_StorageBindArrayCount>(
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



