#include "rypch.h"
#include "ScriptGlue.h"
#include "ScriptingEngine.h"

#include <glm/glm.hpp>

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

#include "Rynex/Core/KeyCodes.h"
#include "Rynex/Core/Input.h"
#include "Rynex/Asset/Base/AssetManager.h"

#define USE_HASCOMPONET_FUNC_CS 0

#define RY_TEXTURE_CS 1
#define RY_SHADER_CS 1
#define RY_VERTEXBUFFER_CS 1
#define RY_INDEXBUFFER_CS 1
#define RY_VERTEXARRAY_CS 1
#define RY_TRANSFORM_COMPONENT_CS 1
#define RY_GEOMTRY_COMPONENT_CS 1
#define RY_SCENE_CS 1

namespace Rynex {
#if 1
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

	static Entity GetFromSceneEntity(UUID entityID)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		return scene->GetEntitiyByUUID(entityID);
	}


	template<typename T>
	static T GetFromEntityComponent(Entity entity)
	{
		if (entity.HasComponent<T>())
			return entity.GetComponent<T>();
		else
			return nullptr;
	}

#define RY_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Rynex.InternalCalls::" #Name, Name)
	// Exampels Defins
	static void NativLog(MonoString* text)
	{
		char* cStr = mono_string_to_utf8(text);
		printf("C++ says ->%s\n", cStr);
		mono_free(cStr);
	}

	static void NativLog_Vec3(glm::vec3* parmeter, glm::vec3* outresult)
	{
		RY_CORE_WARN("Value: {0}, {1}, {2}", parmeter->x, parmeter->y, parmeter->z);

		*outresult = glm::normalize(*parmeter);

	}

	static float NativLog_Vec3Dot(glm::vec3* parmeter)
	{
		RY_CORE_WARN("Value: {0}, {1}, {2}", parmeter->x, parmeter->y, parmeter->z);
		return glm::dot(*parmeter, *parmeter);

	}
#endif
	// Entity Defins
	// TransformComponent
#if RY_TRANSFORM_COMPONENT_CS
	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		*outTranslation = entity.GetComponent<TransformComponent>().Transaltion;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* inTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		entity.GetComponent<TransformComponent>().Transaltion = *inTranslation;
	}

	static void TransformComponent_GetRotation(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		*outTranslation = entity.GetComponent<TransformComponent>().Rotation;
	}

	static void TransformComponent_SetRotation(UUID entityID, glm::vec3* inTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		entity.GetComponent<TransformComponent>().Rotation = *inTranslation;
	}

	static void TransformComponent_GetScale(UUID entityID, glm::vec3* outTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		*outTranslation = entity.GetComponent<TransformComponent>().Scale;
	}

	static void TransformComponent_SetScale(UUID entityID, glm::vec3* inTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		entity.GetComponent<TransformComponent>().Scale = *inTranslation;
	}

	static void TransformComponent_GetChange(UUID entityID, bool* outTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		*outTranslation = entity.GetComponent<TransformComponent>().Change;
	}

	static void TransformComponent_SetChange(UUID entityID, bool* inTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<TransformComponent>()) return;
		entity.GetComponent<TransformComponent>().Change = *inTranslation;
	}
#endif

#if RY_GEOMTRY_COMPONENT_CS
	static void GeometryComponent_GetGeometry(UUID entityID, AssetHandle* outTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		if (geomtryC.Geometry)
			*outTranslation = geomtryC.Geometry->Handle;
		else
			*outTranslation = AssetHandle(0);
	}

	static void GeometryComponent_SetGeometry(UUID entityID, AssetHandle* inTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		geomtryC.Geometry = AssetManager::GetLocaleAsset<VertexArray>(*inTranslation);

	}

	static void GeometryComponent_GetBuffer(UUID entityID, AssetHandle* outTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		if (geomtryC.Buffer)
			*outTranslation = geomtryC.Buffer->Handle;
		else
			*outTranslation = AssetHandle(0);
		
	}

	static void GeometryComponent_SetBuffer(UUID entityID, AssetHandle* inTranslation)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;
		auto& geomtryC = entity.GetComponent<GeomtryComponent>();
		geomtryC.Buffer = AssetManager::GetLocaleAsset<VertexBuffer>(*inTranslation);
	}

#endif

#if 1
	struct GeomtryInput
	{
		glm::vec3 position; 
		glm::vec2 uv;
		glm::vec3 nomale;
	};

	// GeomtryComponent
	static void GeometryComponent_SetVertex(UUID entityID, void* vertex, uint32_t byteSize)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;

		GeomtryInput* vertexInput = (GeomtryInput*)vertex;
		uint32_t count = byteSize / sizeof(GeomtryInput);
		
		GeomtryComponent& geomtry = entity.GetComponent<GeomtryComponent>();

		// Save Settings, Befor Destroying The Object.

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(byteSize);
		vertexBuffer->SetData(vertex, byteSize);
		vertexBuffer->SetLayout(geomtry.Buffer->GetLayout());
		Ref<VertexArray> vertexArray = VertexArray::Create();
		

		vertexArray->SetPrimitv(geomtry.Geometry->GetPrimitv());
		vertexArray->AddVertexBuffer(vertexBuffer);

		// Overide the old, Creat New Buffer.
		geomtry.Buffer = vertexBuffer;
		geomtry.Geometry = vertexArray;

		for (int i = 0; i < count; i++)
			RY_CORE_TRACE("postion[{0}] : x{1}, y{2}, z{3}", i, vertexInput[i].position.x, vertexInput[i].position.y, vertexInput[i].position.z);
		for (int i = 0; i < count; i++)
			RY_CORE_TRACE("normal[{0}] : x{1}, y{2}, z{3}", i, vertexInput[i].nomale.x, vertexInput[i].nomale.y, vertexInput[i].nomale.z);
		for (int i = 0; i < count; i++)
			RY_CORE_TRACE("uv[{0}] : x{1}, y{2}", i, vertexInput[i].uv.x, vertexInput[i].uv.y);

		vertexBuffer->GetBufferData();
	}

	static void GeomtryComponent_SetIndex(UUID entityID, uint32_t* index, uint32_t count)
	{
		Entity entity = GetFromSceneEntity(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;

		Ref<VertexArray>& vertex = entity.GetComponent<GeomtryComponent>().Geometry;
		Ref<IndexBuffer> inicies = IndexBuffer::Create(index, count);
		vertex->SetIndexBuffer(inicies);
	}

	static void GeomtryComponent_SetPrimitv(UUID entityID, int primitv)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;

		auto& geometryC = entity.GetComponent<GeomtryComponent>();
		if(geometryC.Geometry)
			geometryC.Geometry->SetPrimitv((VertexArray::Primitv)primitv);
		else
		{
			geometryC.Geometry = VertexArray::Create();
			geometryC.Geometry->SetPrimitv((VertexArray::Primitv)primitv);
		}
	}
#endif

#if 0
	static void GeomtryComponent_SetLayout(UUID entityID, int conte, )
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		Entity entity = scene->GetEntitiyByUUID(entityID);
		if (!entity.HasComponent<GeomtryComponent>()) return;

		auto& geometryC = entity.GetComponent<GeomtryComponent>();
		geometryC.Buffer->SetLayout();

		_SetPrimitv

	}
#endif //TODO: Create SetPrimitv Funktion!

#if RY_TEXTURE_CS
	static void Texture_Create_Withe_Heigth(AssetHandle* handle, uint32_t withe, uint32_t height)
	{
		Ref<Texture> texture = Texture::Create(withe, height);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}


	static void Texture_Create_Spec_Data_ByteSize(AssetHandle* handle, TextureSpecification* spec, void* data, uint32_t size)
	{
		Ref<Texture> texture = Texture::Create(*spec, data, size);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}

	static void Texture_Create_Spec(AssetHandle* handle, TextureSpecification* spec)
	{

		Ref<Texture> texture = Texture::Create(*spec);
		texture->Handle = AssetHandle();
		texture->Handle = *handle = AssetManager::CreatLocaleAsset<Texture>(texture);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<Texture>(texture->Handle) == texture, "Texture_Create_Spec Created Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Texture Create form C# id = {0}", (uint32_t)*handle);
	}

	static void Texture_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetLocaleAsset<Texture>(handle), "Texture_Destroy Texture is there, it shoud by Destroyt!");
	}

	static void Texture_GetSpecification(AssetHandle handle, TextureSpecification* spec)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetWidth Texture is nullptr!");
		*spec = texture->GetSpecification();
	}

	static void Texture_GetWidth(AssetHandle handle, uint32_t* width)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetWidth Texture is nullptr!");
		*width = texture->GetWidth();
	}

	static void Texture_GetHeight(AssetHandle handle, uint32_t* height)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetHeight Texture is nullptr!");
		*height = texture->GetHeight();
	}

	static void Texture_GetRenderID(AssetHandle handle, uint32_t* renderID)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_GetRenderID Texture is nullptr!");
		*renderID = texture->GetRenderID();
	}

	static void Texture_SetData(AssetHandle handle, void* data, uint32_t size)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_SetData Texture is nullptr!");
		texture->SetData(data, size);
	}

	static void Texture_Resize(AssetHandle handle, uint32_t height, uint32_t width)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_Resize Texture is nullptr!");
		texture->Resize(width, height);
	}

	static void Texture_Bind(AssetHandle handle, uint32_t slot)
	{
		Ref<Texture> texture = AssetManager::GetLocaleAsset<Texture>(handle);
		RY_CORE_ASSERT(texture, "Texture_Bind Texture is nullptr!");
		texture->Bind(slot);
	}
#endif

#if RY_SHADER_CS
	static void Shader_Create_Source_Name(AssetHandle* handle, MonoString* source, MonoString* name)
	{
		char* c_strSource = mono_string_to_utf8(source);
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = Shader::Create(c_strSource, c_strName);
		shader->Handle = AssetHandle();
		shader->Handle = *handle = AssetManager::CreatLocaleAsset<Shader>(shader);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<Shader>(shader->Handle) == shader, "Shader_Create_Source_Name Created Shader is not the same like in the Regestry!");
		RY_CORE_INFO("Shader Create form C# id = {0}", (uint32_t)*handle);
		mono_free(c_strSource);
		mono_free(c_strName);
	}

	static void Shader_Create_Source_Name_VertexSrc_FragmentSrc(AssetHandle* handle, MonoString* name, MonoString* vertexSrc, MonoString* fragmentSrc)
	{
		char* c_strVertexSrc = mono_string_to_utf8(vertexSrc);
		char* c_strFragmentSrc = mono_string_to_utf8(fragmentSrc);
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = Shader::Create(c_strName, c_strVertexSrc, c_strFragmentSrc);
		shader->Handle = AssetHandle();
		shader->Handle = *handle = AssetManager::CreatLocaleAsset<Shader>(shader);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<Shader>(shader->Handle) == shader, "Shader_Create_Source_Name Shader Texture is not the same like in the Regestry!");
		RY_CORE_INFO("Shader Create form C# id = {0}", (uint32_t)*handle);
		mono_free(c_strName);
		mono_free(c_strVertexSrc);
		mono_free(c_strFragmentSrc);
	}
	
	static void Shader_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetLocaleAsset<Shader>(handle), "Shader_Destroy Shader is there, it shoud by Destroyt!");
	}

	static void Shader_ReganrateShader(AssetHandle handle, MonoString* source)
	{
		char* c_strSource = mono_string_to_utf8(source);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_ReganrateShader Shader is nullptr!");
		shader->ReganrateShader(c_strSource);
		mono_free(c_strSource);
	}

	static void Shader_Bind(AssetHandle handle)
	{
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_Bind Shader is nullptr!");
		shader->Bind();
	}

	static void Shader_UnBind(AssetHandle handle)
	{
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_UnBind Shader is nullptr!");
		shader->UnBind();
	}

	static void Shader_SetUniformValue(AssetHandle handle, MonoString* name, void* value, ShaderDataType type)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetUniformValue Shader is nullptr!");
		shader->SetUniformValue(c_strName, value, type);
		mono_free(c_strName);

	}

	static void Shader_SetInt(AssetHandle handle, MonoString* name, int value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetInt Shader is nullptr!");
		shader->SetInt(c_strName, value);
		mono_free(c_strName);
	}

	static void Shader_SetIntArray(AssetHandle handle, MonoString* name, int* value, uint32_t count)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetIntArray Shader is nullptr!");
		shader->SetIntArray(c_strName, value, count);
		mono_free(c_strName);
	}

	static void Shader_SetFloat3(AssetHandle handle, MonoString* name, glm::vec3* value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetFloat3 Shader is nullptr!");
		shader->SetFloat3(c_strName, *value);
		mono_free(c_strName);
	}

	static void Shader_SetFloat4(AssetHandle handle, MonoString* name, glm::vec4* value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetFloat4 Shader is nullptr!");
		shader->SetFloat4(c_strName, *value);
		mono_free(c_strName);
	}

	static void Shader_SetMat4(AssetHandle handle, MonoString* name, glm::mat4* value)
	{
		char* c_strName = mono_string_to_utf8(name);
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_SetMat4 Shader is nullptr!");
		shader->SetMat4(c_strName, *value);
		mono_free(c_strName);
	}

	static void Shader_GetName(AssetHandle handle, MonoString* name)
	{
		Ref<Shader> shader = AssetManager::GetLocaleAsset<Shader>(handle);
		RY_CORE_ASSERT(shader, "Shader_GetName Shader is nullptr!");
		name = mono_string_new_wrapper(shader->GetName().c_str());
	}
#endif

#if RY_VERTEXBUFFER_CS
	static void VertexBuffer_Create_Vertices_Size(AssetHandle* handle, void* vertices, uint32_t size)
	{
		
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, size);
		vertexBuffer->Handle = AssetHandle();
		vertexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<VertexBuffer>(vertexBuffer);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<VertexBuffer>(vertexBuffer->Handle) == vertexBuffer, "VertexBuffer_Create_Vertices_Size Created VertexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("VertexBuffer Create form C# id = {0}", (uint32_t)*handle);
	}

	static void VertexBuffer_Create_Vertices_Size_Usage(AssetHandle* handle, void* vertices, uint32_t size, BufferDataUsage usage)
	{
		

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, size, usage);
		vertexBuffer->Handle = AssetHandle();
		vertexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<VertexBuffer>(vertexBuffer);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<VertexBuffer>(vertexBuffer->Handle) == vertexBuffer, "VertexBuffer_Create_Vertices_Size Created VertexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("VertexBuffer Create form C# id = {0}", (uint32_t)*handle);
		
	}

	static void VertexBuffer_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetLocaleAsset<VertexBuffer>(handle), "VertexBuffer_Destroy VertexBuffer is there, it shoud by Destroyt!");
	}

	static void VertexBuffer_Bind(AssetHandle handle)
	{
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_Bind VertexBuffer is nullptr!");
		vertexBuffer->Bind();
	}

	static void VertexBuffer_UnBind(AssetHandle handle)
	{
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_UnBind VertexBuffer is nullptr!");
		vertexBuffer->UnBind();
	}

	static void VertexBuffer_SetBufferElementToLayout(AssetHandle handle,
		uint32_t elementCount,
		uint32_t elementIndex,
		MonoString* bufferElementName,
		ShaderDataType bufferElementType, bool bufferElementnormilized)
	{
		char* name = mono_string_to_utf8(bufferElementName);

		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_SetLayout VertexBuffer is nullptr!");
		const BufferLayout& layoutChache = vertexBuffer->GetLayout();
		std::vector<BufferElement> bufferElements;

		int index = 0;
		for (auto& bufferElement : layoutChache.GetElements())
		{
			if (index >= elementIndex)
				break;
			bufferElements.emplace_back(bufferElement.Type, bufferElement.Name, bufferElement.Normilized);
			index++;
		}
		bufferElements.emplace_back(bufferElementType, name, bufferElementnormilized);

		vertexBuffer->SetLayout(bufferElements);
		mono_free(name);
	}

	static void VertexBuffer_GetElementFromLayout(AssetHandle handle,
		uint32_t* elementCount,
		uint32_t elementIndex,
		MonoString* bufferElementName,
		ShaderDataType* bufferElementType, uint32_t* bufferElementSize, bool* bufferElementnormilized, uint32_t* bufferElementOffset, uint32_t* bufferElementCount)
	{
	
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_GetLayout VertexBuffer is nullptr!");

		const BufferLayout& layout = vertexBuffer->GetLayout();
		*elementCount = (uint32_t)layout.GetLength();
		if(0<*elementCount)
		{
			std::vector<BufferElement> bufferElements = layout.GetElements();
			bufferElementName = mono_string_new_wrapper(bufferElements[elementIndex].Name.c_str());
			bufferElements[elementIndex].Type = *bufferElementType;
			bufferElements[elementIndex].Size = *bufferElementSize;
			bufferElements[elementIndex].Offset = *bufferElementOffset;
			bufferElements[elementIndex].Normilized = *bufferElementnormilized;
		}
		
	}

	static void VertexBuffer_SetData(AssetHandle handle, void* data, uint32_t size)
	{

		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_SetData VertexBuffer is nullptr!");
		vertexBuffer->SetData(data, size);
	}

	static void VertexBuffer_GetByteSize(AssetHandle handle, uint32_t* size)
	{
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handle);
		RY_CORE_ASSERT(vertexBuffer, "VertexBuffer_GetByteSize VertexBuffer is nullptr!");
		*size = vertexBuffer->GetByteSize();
	}
#endif

#if RY_INDEXBUFFER_CS
	static void IndexBuffer_Create_Indices32_Size_Usage(AssetHandle* handle, uint32_t* indices, uint32_t size, BufferDataUsage usage)
	{
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, size, usage);
		indexBuffer->Handle = AssetHandle();
		indexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<IndexBuffer>(indexBuffer);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<IndexBuffer>(indexBuffer->Handle) == indexBuffer, "IndexBuffer_Create_Indices32_Size_Usage Created IndexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("IndexBuffer Create form C# id = {0}, renderID = {1}", (uint32_t)*handle, indexBuffer->GetRenderID());
		
	}

	static void IndexBuffer_Create_Indices16_Size_Usage(AssetHandle* handle, uint16_t* indices, uint32_t size, BufferDataUsage usage)
	{


		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, size, usage);
		indexBuffer->Handle = AssetHandle();
		indexBuffer->Handle = *handle = AssetManager::CreatLocaleAsset<IndexBuffer>(indexBuffer);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<IndexBuffer>(indexBuffer->Handle) == indexBuffer, "IndexBuffer_Create_Indices16_Size_Usage Created IndexBuffer is not the same like in the Regestry!");
		RY_CORE_INFO("IndexBuffer Create form C# id = {0}", (uint32_t)*handle);
	}

	static void IndexBuffer_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetLocaleAsset<IndexBuffer>(handle), "IndexBuffer_Destroy IndexBuffer is there, it shoud by Destroyt!");
	}

	static void IndexBuffer_Bind(AssetHandle handle)
	{
		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_Bind IndexBuffer is nullptr!");
		indexBuffer->Bind();
	}

	static void IndexBuffer_UnBind(AssetHandle handle)
	{
		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_UnBind IndexBuffer is nullptr!");
		RY_CORE_INFO("IndexBuffer UnBind form C# id = {0}, renderID = {1}", (uint32_t)handle, indexBuffer->GetRenderID());
		indexBuffer->UnBind();
	}

	static void IndexBuffer_SetData32(AssetHandle handle, uint32_t* indices, uint32_t size)
	{
		
		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_SetData32 IndexBuffer is nullptr!");
		indexBuffer->SetData(indices, size);
	}

	static void IndexBuffer_SetData16(AssetHandle handle, uint16_t* indices, uint32_t size)
	{

		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_SetData16 IndexBuffer is nullptr!");
		indexBuffer->SetData(indices, size);
	}

	static void IndexBuffer_GetCount(AssetHandle handle,  uint32_t* count)
	{

		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_GetCount IndexBuffer is nullptr!");
		*count = indexBuffer->GetCount();
	}

	static void IndexBuffer_GetElementByte(AssetHandle handle, uint32_t* elementByte)
	{
		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handle);
		RY_CORE_ASSERT(indexBuffer, "IndexBuffer_GetElementByte IndexBuffer is nullptr!");
		*elementByte = indexBuffer->GetElementByte();
	}
#endif

#if RY_VERTEXARRAY_CS
	static void VertexArray_Create(AssetHandle* handle)
	{

		Ref<VertexArray> vertexArray = VertexArray::Create();
		vertexArray->Handle = AssetHandle();
		vertexArray->Handle = *handle = AssetManager::CreatLocaleAsset<VertexArray>(vertexArray);
		RY_CORE_ASSERT(AssetManager::GetLocaleAsset<VertexArray>(vertexArray->Handle) == vertexArray, "VertexArray_Create Created VertexArray is not the same like in the Regestry!");
		RY_CORE_INFO("VertexArray Create form C# id = {0}", (uint32_t)*handle);
	}

	static void VertexArray_Destroy(AssetHandle handle)
	{
		AssetManager::DeleteLocaleAsset(handle);
		RY_CORE_ASSERT(!AssetManager::GetLocaleAsset<VertexArray>(handle), "VertexArray_Destroy VertexArray is there, it shoud by Destroyt!");
	}

	static void VertexArray_Bind(AssetHandle handle)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetLocaleAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_Bind VertexArray is nullptr!");
		vertexArray->Bind();
	}

	static void VertexArray_UnBind(AssetHandle handle)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetLocaleAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_UnBind VertexArray is nullptr!");
		vertexArray->UnBind();
	}

	static void VertexArray_AddVertexBuffer(AssetHandle handle, AssetHandle handleVertex)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetLocaleAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_AddVertexBuffer VertexArray is nullptr!");
		Ref<VertexBuffer> vertexBuffer = AssetManager::GetLocaleAsset<VertexBuffer>(handleVertex);
		RY_CORE_ASSERT(vertexBuffer, "VertexArray_AddVertexBuffer VertexBuffer is nullptr!");
		vertexArray->AddVertexBuffer(vertexBuffer);
	}

	static void VertexArray_SetIndexBuffer(AssetHandle handle, AssetHandle handleIndex)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetLocaleAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_SetIndexBuffer Texture is nullptr!");
		Ref<IndexBuffer> indexBuffer = AssetManager::GetLocaleAsset<IndexBuffer>(handleIndex);
		RY_CORE_ASSERT(indexBuffer, "VertexArray_SetIndexBuffer IndexBuffer is nullptr!");
		vertexArray->SetIndexBuffer(indexBuffer);
	}

	static void VertexArray_SetPrimitv(AssetHandle handle, VertexArray::Primitv primitv)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetLocaleAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_SetPrimitv VertexArray is nullptr!");
		vertexArray->SetPrimitv(primitv);
		
	}

	static void VertexArray_GetPrimitv(AssetHandle handle, VertexArray::Primitv* primitv)
	{
		Ref<VertexArray> vertexArray = AssetManager::GetLocaleAsset<VertexArray>(handle);
		RY_CORE_ASSERT(vertexArray, "VertexArray_GetPrimitv VertexArray is nullptr!");
		*primitv = vertexArray->GetPrimitv();
	}
	
#endif

#if RY_SCENE_CS

	static void Scene_CreateEntity(MonoString* name ,UUID* handleEntity)
	{
		Scene* scene = ScriptingEngine::GetSceneContext();
		*handleEntity = UUID();
		char* nameC_str = mono_string_to_utf8(name);
		scene->CreateEntityWitheUUID(*handleEntity, nameC_str);
		mono_free(name);
	}

	

#endif

	// Components Defins
#if USE_HASCOMPONET_FUNC_CS
	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
#if 0
		Scene* scene = ScriptingEngine::GetSceneContext();
		RY_CORE_ASSERT(scene, "Error: Entity_HasComponent");
		Entity entity = scene->GetEntitiyByUUID(entityID);
		RY_CORE_ASSERT(entity, "Error: Entity_HasComponent");

	
		MonoType* manageType = mono_reflection_type_get_type(componentType);
		auto end = s_EntityHasComponentFunc.end();
		auto find = s_EntityHasComponentFunc.find(manageType);
		auto& result = s_EntityHasComponentFunc[manageType];
		RY_CORE_ASSERT(s_EntityHasComponentFunc.find(manageType) != s_EntityHasComponentFunc.end(), "Error: Entity_HasComponent");
		
		return s_EntityHasComponentFunc.at(manageType)(entity);
#else
		return false;
#endif
	}
#endif // TODO: Find out what is going on why is That not working?

	// Input Defins
	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename ... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Rynex.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptingEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					
					RY_CORE_ERROR("Could not find component type {}", managedTypename);
					
				}
				else
				{
					RY_CORE_INFO("Could find component type {}", managedTypename);
					s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
				}
			}(), ...);


	}

	template<typename... Component>
	static void RegisterComponents(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}


	void ScriptGlue::RegisterFunktion()
	{

		// Exampels
		RY_ADD_INTERNAL_CALL(NativLog);
		RY_ADD_INTERNAL_CALL(NativLog_Vec3);
		RY_ADD_INTERNAL_CALL(NativLog_Vec3Dot);

		// Components
#if USE_HASCOMPONET_FUNC_CS
		RY_ADD_INTERNAL_CALL(Entity_HasComponent);
#endif
		// Entity
#if RY_TRANSFORM_COMPONENT_CS
		RY_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		RY_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		RY_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetScale);
		RY_ADD_INTERNAL_CALL(TransformComponent_GetChange);
		RY_ADD_INTERNAL_CALL(TransformComponent_SetChange);
#endif

#if RY_GEOMTRY_COMPONENT_CS
		// Geomtry
		// old
		RY_ADD_INTERNAL_CALL(GeomtryComponent_SetIndex);
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetVertex);
		RY_ADD_INTERNAL_CALL(GeomtryComponent_SetPrimitv);
		// new
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetBuffer);
		RY_ADD_INTERNAL_CALL(GeometryComponent_GetBuffer);
		RY_ADD_INTERNAL_CALL(GeometryComponent_SetGeometry);
		RY_ADD_INTERNAL_CALL(GeometryComponent_GetGeometry);
#endif

#if RY_SCENE_CS
		RY_ADD_INTERNAL_CALL(Scene_CreateEntity);
#endif
		
#if RY_TEXTURE_CS // Texture
		RY_ADD_INTERNAL_CALL(Texture_Create_Withe_Heigth);
		RY_ADD_INTERNAL_CALL(Texture_Create_Spec);
		RY_ADD_INTERNAL_CALL(Texture_Create_Spec_Data_ByteSize);
		RY_ADD_INTERNAL_CALL(Texture_Destroy);
		RY_ADD_INTERNAL_CALL(Texture_GetSpecification);
		RY_ADD_INTERNAL_CALL(Texture_GetRenderID);
		RY_ADD_INTERNAL_CALL(Texture_GetHeight);
		RY_ADD_INTERNAL_CALL(Texture_GetWidth);
		RY_ADD_INTERNAL_CALL(Texture_Bind);
		RY_ADD_INTERNAL_CALL(Texture_Resize);
		RY_ADD_INTERNAL_CALL(Texture_SetData);
#endif

#if RY_SHADER_CS
		RY_ADD_INTERNAL_CALL(Shader_Create_Source_Name);
		RY_ADD_INTERNAL_CALL(Shader_Create_Source_Name_VertexSrc_FragmentSrc);
		RY_ADD_INTERNAL_CALL(Shader_Destroy);
		RY_ADD_INTERNAL_CALL(Shader_ReganrateShader);
		RY_ADD_INTERNAL_CALL(Shader_Bind);
		RY_ADD_INTERNAL_CALL(Shader_UnBind);
		RY_ADD_INTERNAL_CALL(Shader_SetUniformValue);
		RY_ADD_INTERNAL_CALL(Shader_SetInt);
		RY_ADD_INTERNAL_CALL(Shader_SetIntArray);
#endif

#if RY_VERTEXBUFFER_CS
		RY_ADD_INTERNAL_CALL(VertexBuffer_Create_Vertices_Size);
		RY_ADD_INTERNAL_CALL(VertexBuffer_Create_Vertices_Size_Usage);
		RY_ADD_INTERNAL_CALL(VertexBuffer_Destroy);
		RY_ADD_INTERNAL_CALL(VertexBuffer_GetByteSize);
		RY_ADD_INTERNAL_CALL(VertexBuffer_SetData);
		RY_ADD_INTERNAL_CALL(VertexBuffer_Bind);
		RY_ADD_INTERNAL_CALL(VertexBuffer_UnBind);
		RY_ADD_INTERNAL_CALL(VertexBuffer_GetElementFromLayout);
		RY_ADD_INTERNAL_CALL(VertexBuffer_SetBufferElementToLayout);
		RY_ADD_INTERNAL_CALL(VertexBuffer_SetData);
#endif

#if RY_INDEXBUFFER_CS
		RY_ADD_INTERNAL_CALL(IndexBuffer_Create_Indices32_Size_Usage);
		RY_ADD_INTERNAL_CALL(IndexBuffer_Create_Indices16_Size_Usage);
		RY_ADD_INTERNAL_CALL(IndexBuffer_Destroy);
		RY_ADD_INTERNAL_CALL(IndexBuffer_Bind);
		RY_ADD_INTERNAL_CALL(IndexBuffer_UnBind);
		RY_ADD_INTERNAL_CALL(IndexBuffer_SetData32);
		RY_ADD_INTERNAL_CALL(IndexBuffer_SetData16);
		RY_ADD_INTERNAL_CALL(IndexBuffer_GetCount);
		RY_ADD_INTERNAL_CALL(IndexBuffer_GetElementByte);
#endif

#if RY_VERTEXARRAY_CS
		RY_ADD_INTERNAL_CALL(VertexArray_Create);
		RY_ADD_INTERNAL_CALL(VertexArray_Destroy);
		RY_ADD_INTERNAL_CALL(VertexArray_Bind);
		RY_ADD_INTERNAL_CALL(VertexArray_UnBind);
		RY_ADD_INTERNAL_CALL(VertexArray_AddVertexBuffer);
		RY_ADD_INTERNAL_CALL(VertexArray_SetIndexBuffer);
		RY_ADD_INTERNAL_CALL(VertexArray_SetPrimitv);
		RY_ADD_INTERNAL_CALL(VertexArray_GetPrimitv);
#endif

		// Input
		RY_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}


	void ScriptGlue::RegisterAllComponets()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponents(AllComponents{});
	}
}