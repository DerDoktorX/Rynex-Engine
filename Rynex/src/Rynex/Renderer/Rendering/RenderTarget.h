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
		TextureBindArray bindeTextures;
		UniformBindArray bindeUniform;
		StorageBindArray bindeStorage;

		bool operator==(const Ref<BindingShaderLayoutStatic>& rigth) const
		{
			return IsEqualSort(rigth);
		}

		bool operator==(const BindingShaderLayoutStatic& rigth) const
		{
			return IsEqualSort(rigth);
		}

		bool operator!=(const BindingShaderLayoutStatic& rigth) const
		{
			return !IsEqualSort(rigth);
		}
		bool IsEqualSort(const BindingShaderLayoutStatic& rigth) const
		{
			constexpr size_t sTextureMultyplyer = 1;
			constexpr size_t sUniformMultyplyer = 1;
			constexpr size_t sStorageMultyplyer = 1;
			int eqaul = 0;
			int notEqual = 0;
			for (int i = 0; i < bindeTextures.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				bool isthisVaildSSBO = std::visit([](const auto& refThis) { return nullptr == refThis; }, this->bindeStorage[i]);
				bool isRigthVaildSSBO = std::visit([](const auto& refRigth) { return nullptr == refRigth; }, rigth.bindeStorage[i]);

				if (isthisVaildSSBO && isRigthVaildSSBO)
					continue;
#else
				if (this->bindeStorage[i] && rigth->bindeStorage[i])
					continue;
#endif

#ifdef RY_SSBO_VARIENTS
				if (this->bindeTextures[i] == rigth.bindeTextures[i])
#else
				if (this->bindeTextures[i] == rigth->bindeTextures[i])
#endif
					eqaul += sTextureMultyplyer;
				else
					notEqual += sTextureMultyplyer;
			}

			for (int i = 0; i < bindeUniform.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS

				bool isThisVaildSSBO = std::visit([](const auto& refThis) { return nullptr == refThis; }, this->bindeStorage[i]);
				bool isRigthVaildSSBO = std::visit([](const auto& refRigth) { return nullptr == refRigth; }, rigth.bindeStorage[i]);

				if (isThisVaildSSBO && isRigthVaildSSBO)
					continue;
#else
				if (this->bindeStorage[i] && rigth->bindeStorage[i])
					continue;
#endif

				if (this->bindeUniform[i] == rigth.bindeUniform[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
			}

			for (int i = 0; i < bindeStorage.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				int64_t thisPtrSSBO = std::visit([](const auto& refThis) { int64_t ptrValue = reinterpret_cast<int64_t>(refThis.get()); return ptrValue; }, this->bindeStorage[i]);
				int64_t rigthPtrSSBO = std::visit([](const auto& refRigth) { int64_t ptrValue = reinterpret_cast<int64_t>(refRigth.get()); return ptrValue; }, rigth.bindeStorage[i]);

				if (thisPtrSSBO != 0ll && rigthPtrSSBO != 0ll)
					continue;

				if (thisPtrSSBO == rigthPtrSSBO)
					eqaul += sStorageMultyplyer;
				else
					notEqual += sStorageMultyplyer;
#else
				if (this->bindeStorage[i] == rigth->bindeStorage[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
#endif
			}
			bool result = eqaul > notEqual;
			return result;
		}


		bool IsEqualSort(const Ref<BindingShaderLayoutStatic>& rigth) const
		{
			constexpr size_t sTextureMultyplyer = 1;
			constexpr size_t sUniformMultyplyer = 1;
			constexpr size_t sStorageMultyplyer = 1;
			int eqaul = 0;
			int notEqual = 0;
			for (int i = 0; i < bindeTextures.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				bool isthisVaildSSBO = std::visit([](const auto& refThis) { return nullptr == refThis; } , this->bindeStorage[i]);
				bool isRigthVaildSSBO = std::visit([](const auto& refRigth) { return nullptr == refRigth; }, rigth->bindeStorage[i]);

				if (isthisVaildSSBO && isRigthVaildSSBO)
					continue;
#else
				if (this->bindeStorage[i] && rigth->bindeStorage[i])
					continue;
#endif

#ifdef RY_SSBO_VARIENTS
				if (this->bindeTextures[i] == rigth->bindeTextures[i])
#else
				if (this->bindeTextures[i] == rigth->bindeTextures[i])
#endif
					eqaul += sTextureMultyplyer;
				else
					notEqual += sTextureMultyplyer;
			}

			for (int i = 0; i < bindeUniform.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS

				bool isThisVaildSSBO = std::visit([](const auto& refThis) { return nullptr == refThis; }, this->bindeStorage[i]);
				bool isRigthVaildSSBO = std::visit([](const auto& refRigth) { return nullptr == refRigth; }, rigth->bindeStorage[i]);

				if (isThisVaildSSBO && isRigthVaildSSBO)
					continue;
#else
				if (this->bindeStorage[i] && rigth->bindeStorage[i])
					continue;
#endif

				if (this->bindeUniform[i] == rigth->bindeUniform[i])
					eqaul += sUniformMultyplyer;
				else
					notEqual += sUniformMultyplyer;
			}

			for (int i = 0; i < bindeStorage.size(); i++)
			{
#ifdef RY_SSBO_VARIENTS
				int64_t thisPtrSSBO = std::visit([](const auto& refThis) { int64_t ptrValue = reinterpret_cast<int64_t>(refThis.get()); return ptrValue; }, this->bindeStorage[i]);
				int64_t rigthPtrSSBO = std::visit([](const auto& refRigth) { int64_t ptrValue = reinterpret_cast<int64_t>(refRigth.get()); return ptrValue; }, rigth->bindeStorage[i]);

				if (thisPtrSSBO != 0ll && rigthPtrSSBO != 0ll)
					continue;

				if (thisPtrSSBO == rigthPtrSSBO)
					eqaul += sStorageMultyplyer;
				else
					notEqual += sStorageMultyplyer;
#else
				if (this->bindeStorage[i] == rigth->bindeStorage[i])
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
			for (int i = 0; i < bindeTextures.size(); i++)
			{
				if (this->bindeTextures[i] == rigth->bindeTextures[i])
					notEqual += sTextureMultyplyer;
			}

			for (int i = 0; i < bindeUniform.size(); i++)
			{
				if (this->bindeUniform[i] != rigth->bindeUniform[i])
					notEqual += sUniformMultyplyer;
			}

			for (int i = 0; i < bindeStorage.size(); i++)
			{
				if (this->bindeStorage[i] != rigth->bindeStorage[i])
					notEqual += sStorageMultyplyer;
			}
			
			
			bool result = 0 == notEqual;
			return result;
		}

		void Sort()
		{
#ifdef RY_TEXTURE_VARIENTS
			std::sort(bindeTextures.begin(), bindeTextures.end(), 
				[](std::variant<Ref<Texture>, Ref<LinkedTextureArray>>& aVarient, std::variant<Ref<Texture>, Ref<LinkedTextureArray>>& bVarient)
				{ 
					return std::visit([&bVarient](auto& aValue)
						{
							if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<Texture>>)
							{
								return std::visit([&aValue](auto& bValue) {
									if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<Texture>>)
										return nullptr != aValue && nullptr == bValue;
									return false;
									}, bVarient);
							}
							else if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<LinkedTextureArray>>)
							{
								return std::visit([&aValue](auto& bValue) {
									if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<LinkedTextureArray>>)
										return nullptr != aValue && nullptr == bValue;
									return false;
									}, bVarient);
							}
						}, aVarient);
				});
#else
			std::sort(bindeTextures.begin(), bindeTextures.end(), [](Ref<Texture>& a, Ref<Texture>& b) {return nullptr != a && nullptr == b;  });
#endif

#ifdef RY_SSBO_VARIENTS
			std::sort(bindeStorage.begin(), bindeStorage.end(),
				[](std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>& aVarient, std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>& bVarient)
				{
					return std::visit([&bVarient](auto& aValue)
						{
							if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<StorageBuffer>>)
							{
								return std::visit([&aValue](auto& bValue) {
									if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<StorageBuffer>>)
										return nullptr != aValue && nullptr == bValue;
									return false;
									}, bVarient);
							}
							else if constexpr (std::is_same_v<std::decay_t<decltype(aValue)>, Ref<BindlesTextureArray>>)
							{
								return std::visit([&aValue](auto& bValue) {
									if constexpr (std::is_same_v<std::decay_t<decltype(bValue)>, Ref<BindlesTextureArray>>)
										return nullptr != aValue && nullptr == bValue;
									return false;
									}, bVarient);
							}
						}, aVarient);
				});
#else
			std::sort(bindeStorage.begin(), bindeStorage.end(), [](Ref<StorageBuffer>& a, Ref<StorageBuffer>& b) {return nullptr != a && nullptr == b;  });
#endif
			std::sort(bindeUniform.begin(), bindeUniform.end(), [](Ref<UniformBuffer>& a, Ref<UniformBuffer>& b) {return nullptr != a && nullptr == b;  });
		}

		void Clear()
		{
#ifdef RY_TEXTURE_VARIENTS
			for (TextureVarients& texure : bindeTextures)
			{
				std::visit([](auto& ref) { RY_DESTROY_REF(ref); }, texure);
			}
#else
			for (Ref<Texture>& tex : bindeTextures)
			{
				RY_DESTROY_REF(tex);
			}
#endif

			for (Ref<UniformBuffer>& uniform : bindeUniform)
			{
				RY_DESTROY_REF(uniform);
			}
#ifdef RY_SSBO_VARIENTS
			for (StorageBufferVarients& storage : bindeStorage)
			{
				std::visit([](auto& ref) { RY_DESTROY_REF(ref); }, storage);
			}
#else
			for (Ref<StorageBuffer>& storage : bindeStorage)
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
		Ref<Shader> shaderProgramm;
		Ref<VertexArray> vao;
		uint32_t indicesCount = 0u;
		Ref<IndirectBuffer> drawBuffer;
		Mesh::PerDrawObject drawElement;
		BindingShaderLayoutStatic bindingLayout;
		int renderMode = 0;

		ShaderDrawList()
			: shaderProgramm(nullptr)
			, vao(nullptr)
			, indicesCount(0u)
			, drawBuffer(nullptr)
			, drawElement({0u, 0u, 0u, -1, 0u})
			, bindingLayout()
			, renderMode(0)
		{
		}

		ShaderDrawList(
			Ref<Shader> programm
			, Ref<VertexArray>& vao
			, uint32_t indicesCount = 0u
			, Ref<IndirectBuffer> drawBuffer = nullptr
			, Mesh::PerDrawObject drawElement = Mesh::PerDrawObject()
			, BindingShaderLayoutStatic bindingLayout = BindingShaderLayoutStatic()
			, int renderMode = 0
		)
			: shaderProgramm(programm)
			, vao(vao)
			, indicesCount(indicesCount)
			, drawBuffer(drawBuffer)
			, drawElement(drawElement)
			, bindingLayout(bindingLayout)
			, renderMode(renderMode)
		{
		}

		void Sort()
		{
			bindingLayout.Sort();
		}

		void Clear()
		{
			RY_DESTROY_REF(shaderProgramm);
			RY_DESTROY_REF(drawBuffer);
			RY_DESTROY_REF(vao);
			bindingLayout.Clear();
		}
#ifdef RY_USE_REF_BINDING_SHADER_LAYOUT
		TextureBindArray& GetBindeTextures() { return bindingLayout->bindeTextures; }
		UniformBindArray& GetBindeUniform() { return bindingLayout->bindeUniform; }
		StorageBindArray& GetBindeStorage() { return bindingLayout->bindeStorage; }

		const TextureBindArray& GetBindeTextures() const { return bindingLayout->bindeTextures; }
		const UniformBindArray& GetBindeUniform() const { return bindingLayout->bindeUniform; }
		const StorageBindArray& GetBindeStorage() const { return bindingLayout->bindeStorage; }

#else
		TextureBindArray& GetBindeTextures() { return bindingLayout.bindeTextures; }
		UniformBindArray& GetBindeUniform() { return bindingLayout.bindeUniform; }
		StorageBindArray& GetBindeStorage() { return bindingLayout.bindeStorage; }

		const TextureBindArray& GetBindeTextures() const { return bindingLayout.bindeTextures; }
		const UniformBindArray& GetBindeUniform() const { return bindingLayout.bindeUniform; }
		const StorageBindArray& GetBindeStorage() const { return bindingLayout.bindeStorage; }

#endif
	};

	RY_NONE_MEBER_OPERATOR_BOOL(ShaderDrawList, == , &&,
		shaderProgramm, bindingLayout
		, drawBuffer, vao, indicesCount
		, drawElement.Count, drawElement.InstancesCount, drawElement.FirstIndex, drawElement.BaseVertex, drawElement.BaseInstance
		, renderMode
	);

	RY_NONE_MEBER_OPERATOR_BOOL(ShaderDrawList, != , ||,
		shaderProgramm, bindingLayout
		, drawBuffer, vao, indicesCount
		, drawElement.Count, drawElement.InstancesCount, drawElement.FirstIndex, drawElement.BaseVertex, drawElement.BaseInstance
		, renderMode
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
		Ref<Shader> Shader;
		Ref<IndirectBuffer> DispatchBuffer;
		glm::uvec3 DispatchGroups;

		Ref<BindingShaderLayoutStatic> bindingLayout;

		ShaderComputeList()
			: Shader(nullptr)
			, DispatchBuffer(nullptr)
			, DispatchGroups(0u,0u,0u)
			, bindingLayout(nullptr)
		{
			bindingLayout = CreateRef<BindingShaderLayoutStatic>();
		}

		void Sort()
		{
			bindingLayout->Sort();
		}

		void Clear()
		{
			RY_DESTROY_REF(Shader);
			RY_DESTROY_REF(DispatchBuffer);


			bindingLayout->Clear();
			RY_DESTROY_REF(bindingLayout);
		}
	};



	class RenderTarget
	{
	private:
		struct AlphaPiplineBase
		{
			Ref<PiplineRenderBase> Pipline;
			float Distenz;


			AlphaPiplineBase(const Ref<PiplineRenderBase>& piline, float distenz)
				: Pipline(piline)
				, Distenz(distenz)
			{
			}

			static bool SortByDistenz(const AlphaPiplineBase& a, const AlphaPiplineBase& b)
			{
				return a.Distenz < b.Distenz;
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
		void SortePilineList();
		void SortePilineAlphaList();

		void ResizeView(const glm::ivec4& renderViewSize);
		void ResizeView(const glm::vec4& renderViewSize);
		void ResizeView(float xOffset, float yOffset, float withe, float heigth) { ResizeView(glm::vec4{ withe, heigth, xOffset, yOffset }); }
		void ResizeView(const glm::vec2& offset, const glm::vec2& size) { ResizeView(glm::vec4{ size,  offset }); }

		void ClearShaderDrawList();

		void ClearFrambuffer() { m_FB = nullptr; m_ClearColorFuncVec.clear(); };


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

		void SetPiplineAlphaDistenz(uint32_t index, float distenz);

		

		int DrawPilines();
		int DrawPilines(int mode);

		int DrawAlphaPilines();
		int DrawAlphaPilines(int mode);

		void ClearPilines();
		void ClearAlphaPilines();

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
		uint32_t GetDrawListCount() const { return m_ShadeDrawList.Size(); }
#else
		uint32_t GetDrawListCount() const { return m_ShadeDrawList.size(); }
#endif
		uint32_t GetPilineBaseCount() const { return m_PilineBaseVec.size(); }

		void ClearFramebufferImges();
		void ClearFramebufferDepth();
		
	private:
		
		static bool IsDataTypeValidToAtachment(const FramebufferTextureSpecification& frameTexSpec, const int& value);
		static bool IsDataTypeValidToAtachment(const FramebufferTextureSpecification& frameTexSpec, const glm::vec4& value);
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

		void CheckExexuteDrawList(uint32_t drawListCount);
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



