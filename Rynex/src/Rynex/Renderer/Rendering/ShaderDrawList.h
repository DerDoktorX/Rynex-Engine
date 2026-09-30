#pragma once
#include <Rynex/Renderer/Mesh/MeshStatic.h>

#define RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
#define RY_TEXTURE_VARIANTS

#define RY_TEST_SORT_EFFICIENT 0

// #define RY_SHADER_DRAW_LIST_SHEARD_PTR

namespace Rynex {

    constexpr size_t g_TextureBindArrayCount = 16ull;
	constexpr size_t g_UniformBindArrayCount = 36ull;
	constexpr size_t g_StorageBindArrayCount = 8ull;

#ifdef RY_TEXTURE_VARIANTS
	using TextureVariants = std::variant<Ref<Texture>, Ref<LinkedTextureArray>>;
	using TextureBindArray = std::array<TextureVariants, g_TextureBindArrayCount>;
#else
	using TextureBindArray = std::array<Ref<Texture>, g_TextureBindArrayCount>;
#endif

	using UniformBindArray = std::array<Ref<UniformBuffer>, g_UniformBindArrayCount>;
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
	using StorageBufferVarints = std::variant<Ref<StorageBuffer>, Ref<BindlesTextureArray>>;
	using StorageBindArray = std::array<StorageBufferVarints, g_StorageBindArrayCount>;
#else
	using StorageBindArray = std::array<Ref<StorageBuffer>, g_StorageBindArrayCount >;
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

		bool IsEqualSort(const BindingShaderLayoutStatic& right) const;
        bool IsEqualSort(const Ref<BindingShaderLayoutStatic>& right) const;
        bool IsEqualComplete(const Ref<BindingShaderLayoutStatic>& right) const;

        void Sort();
        void Clear();
    };




#if 0
	using BindBufferGPU = std::variant<Ref<UniformBuffer>, Ref<Texture>, Ref<LinkedTextureArray>, Ref<StorageBuffer>, Ref<BindlesTextureArray>>;
	using BindBufferContainer = std::vector<std::pair<uint32_t, BindBufferGPU>>;

	struct BindingShaderLayoutDynamic
	{
		BindBufferContainer bindBufferVec;

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

		ShaderDrawList();

		ShaderDrawList(const Ref<Shader>& program, const Ref<VertexArray>& vao
		    , const uint32_t indicesCount = 0u
			, const Ref<IndirectBuffer>& drawBuffer = nullptr
			, const Mesh::PerDrawObject& drawElement = Mesh::PerDrawObject()
			, const BindingShaderLayoutStatic& bindingLayout = BindingShaderLayoutStatic()
			, const int renderMode = 0
		);


		void Sort();
		void Clear();
#ifdef RY_USE_REF_BINDING_SHADER_LAYOUT
		TextureBindArray& GetBindTextures() { return m_BindingLayout->m_BindTextures; }
		UniformBindArray& GetBindUniform() { return m_BindingLayout->m_BindUniform; }
		StorageBindArray& GetBindStorage() { return m_BindingLayout->m_BindStorage; }

		const TextureBindArray& GetBindTextures() const { return m_BindingLayout->m_BindTextures; }
		const UniformBindArray& GetBindUniform() const { return m_BindingLayout->m_BindUniform; }
		const StorageBindArray& GetBindStorage() const { return m_BindingLayout->m_BindStorage; }

#else
		TextureBindArray& GetBindTextures();
		UniformBindArray& GetBindUniform();
		StorageBindArray& GetBindStorage();

		const TextureBindArray& GetBindTextures() const;
		const UniformBindArray& GetBindUniform() const;
		const StorageBindArray& GetBindStorage() const;

#endif
	};

	RY_NONE_MEBER_OPERATOR_BOOL(ShaderDrawList, == , &&,
		m_ShaderProgram, m_BindingLayout
		, m_DrawBuffer, m_VAO, m_IndicesCount
		, m_DrawElement.m_Count, m_DrawElement.m_InstanceCount, m_DrawElement.m_FirstIndex, m_DrawElement.m_BaseVertex, m_DrawElement.m_BaseInstance
		, m_RenderMode
	);

	RY_NONE_MEBER_OPERATOR_BOOL(ShaderDrawList, != , ||,
		m_ShaderProgram, m_BindingLayout
		, m_DrawBuffer, m_VAO, m_IndicesCount
		, m_DrawElement.m_Count, m_DrawElement.m_InstanceCount, m_DrawElement.m_FirstIndex, m_DrawElement.m_BaseVertex, m_DrawElement.m_BaseInstance
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




}



