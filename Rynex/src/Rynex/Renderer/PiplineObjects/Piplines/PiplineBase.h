#pragma once
#include "rypch.h"
#define RY_ENABLE_ENUM_NAMESPACE_PIPLINE_RENDER 0

#define RY_INTERNEL_CHECK_IF_BIT_IS_SET_PRINT(bit)	RY_CORE_WARN("Flage Set: " bit)
#define RY_CHECK_IF_BIT_IS_SET_PRINT(value, bit)	if( (value & bit) != 0 ) RY_INTERNEL_CHECK_IF_BIT_IS_SET_PRINT(#bit)
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>


namespace Rynex {

	struct ViewPassStorage;
	class Materiel;
	class RenderTarget;

	class ElementPiplinEntityPtr
	{
	public:
		virtual ~ElementPiplinEntityPtr() {};

		virtual void SetTransform(const glm::mat4& matrix) = 0;
		virtual void SetMateriel(Ref<Material> material) = 0;

		virtual bool IsVaild() const = 0;
		virtual void Clear() = 0;
		
	};

	typedef enum PiplineResultState
	{
		// Result_None is shoud never happen
		Result_None							= 0,
		Result_Success						= BIT(0),
		

		// state is set if we have not enough space for more RenderShape / Textexurs / Shading defenition / RenderObject
		Result_NoRenderShapeSpaceLeft		= BIT(1),
		Result_NoTextexurSpaceLeft			= BIT(2),
		Result_NoShadeDefinitionSpaceLeft	= BIT(3),
		Result_NoRenderObjectSpaceLeft		= BIT(4),
		Result_NoShaderSpaceLeft			= BIT(5),

		// state we dont allowe Resurce  RenderShape / Texture / Shade definition / RenderObject / Shader 
		Result_NotAllowedRenderShape		= BIT(6),
		Result_NotAllowedTexture			= BIT(7),
		Result_NotAllowedShadeDefinition	= BIT(8),
		Result_NotAllowedRenderObject		= BIT(9),
		Result_NotAllowedShader				= BIT(10),

		Result_Error						= BIT(11),

		Result_AllNoSpaceLeft				= Result_NoTextexurSpaceLeft
		| Result_NoShadeDefinitionSpaceLeft 
		| Result_NoRenderObjectSpaceLeft

		| Result_NoRenderShapeSpaceLeft,

		Result_AllNotAllowed				= Result_NotAllowedTexture 
		| Result_NotAllowedShadeDefinition 
		| Result_NotAllowedRenderObject 
		| Result_NotAllowedShader


	} PiplineResultState;

	typedef enum PiplineManagingState
	{
		// Managing_None is disable all Manager option 
		Managing_None						= 0,

		// Managing_SortEnabele enable 1 of ... set how to Sort RenderObjcts bevor in draw func draw to Target, disable option to
		Managing_SortRenderObjEnabele		= BIT(0),


		// sort withe the postion what is in eth for- and backgund and sort by distence front to back / back to front
		Managing_SortRenderObjFrontToBack	= BIT(1),
		Managing_SortRenderObjBackToFront	= BIT(2),

		// sort the RenderObj that all, withe the same shadeDefintion index render in order.
		// Can used simultaneously, withe Managing_SortRenderObjTexture
		// Prioritization is based on Managing_SortRenderObjTexture
		Managing_SortRenderObjShade			= BIT(3),

		// sort the RenderObj that all, withe the same Texture render in order. 
		// Can used simultaneously, withe Managing_SortRenderObjShade
		// Prioritization is based on Managing_SortRenderObjTexture
		Managing_SortRenderObjTexture		= BIT(4),

		// sort the RenderObj after the Entity Integer
		Managing_SortRenderObjEntity		= BIT(5)

		
	} PiplineManagingState;

		

	class PiplineRenderBase
	{
	protected:
		enum {
			// Hash_BindingPointMultyplyNumberBitMove defines the bit shift per element when combining 
			// multiple pointers into a 64-bit hash. Each pointer is shifted by (index * 16) bits, 
			// allowing up to 4 pointers (16*4 = 64 bits) to be uniquely combined without overlap.
			// This creates distinct hash values for different pointer combinations in the vector.
			// Note: Limited to 4 elements due to 64-bit width, but sufficient for current use case.
			Hash_BindingPointMultyplyNumberBitMove = 16
											
		};

	public:

		virtual ~PiplineRenderBase() {}

		virtual BufferLayout GetExpetedOutput() const = 0;
		virtual void SetExpetedOutput(const BufferLayout& input) = 0;

		// withe This Function you submit Some Resurce From the Rendertaget to the Pipline, 
		// like some camer: postion, view direction, matrix and / or Rendert-Textures from the Target (depth-texture(shadow), rgb-texture(miro))
		virtual void SubmitRenderTargetResurces(ViewPassStorage& viewPass) = 0;
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>& renderTarget) = 0;
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer) = 0;

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer) = 0;
		virtual void SetDisplayUniformBuffer(Ref<UniformBuffer> camerbuffer) = 0;


		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID) = 0;
		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) = 0;


		// Set flags like: sorting_enable, sorting_front_to_back, sorting_back_to_front, caling_in_view for data manging
		// caling: for check befor Submit in render List if it is view Space of the Camera
		// sorting: is for manuel sorting bevore rendering, that the closet is render first or last and the furthest the oppsite, (this maniging has the isue that we need to uplode alle the data)
		// notize: if you want only to change some flags getthe Flags state changing it and set the new flags
		virtual void SetDataMangingFlags(PiplineManagingState flags) = 0;

		// set the draw flags
		virtual void SetRenderFlags(int flags) = 0;

		virtual PiplineManagingState GetDataMangingFlags() const = 0;
		virtual int GetRenderFlags() const = 0;
		
		// exexute the curend stored RenderObjects, withe the stored Rendergflags, withe a draw call auto clearing is a optione
		virtual void DrawNow() = 0;

		// exexute the curend stored RenderObjects, use the passed renderFlags, withe a draw call auto clearing is a optione
		virtual void DrawNow(int flags) = 0;

		// how many Render Object has this Object At this timepoint.
		virtual uint32_t GetCurentEntityRender() const = 0;

		// how many Render Object this Object ever managed in his life time.
		virtual uint32_t GetMaxEntityRender() const = 0;

		// how many frames in one ro we didin't need to update One Entity, if there are 0 RenderObjects its Expexted to get a Higher number.
		virtual uint32_t GetFrameCountNotUpdate() const = 0;

		// how many often we tolerate that not update.
		virtual uint32_t GetMaxFrameCountNotUpdate() const = 0;

		// if we have curently no game objects Stored to render
		virtual bool Empty() const = 0;

		// this function returns: if there is Max RenderObjects Cunt and ths can't take more it returns true to store in a antoher pipline
		virtual bool IsFull()const = 0;

		// clear the list of render objects, for the next Frame, this fuction is called maybe automticly
		virtual void Clear() = 0;
		virtual void ClearRenderObjects() = 0;


		virtual bool IsToLongNotUpdated() const
		{
			const uint32_t maxFrameCountNotUpdated = this->GetMaxFrameCountNotUpdate();
			const uint32_t curentFrameCountNotUpdated = this->GetFrameCountNotUpdate();
			bool result = maxFrameCountNotUpdated < curentFrameCountNotUpdated;
			return result;
		}

		// for sorting resurces for draw call exexution order, 0 is not Vaild value
		// if we, use no index one resurce we get a combined number of the first 2, 3 binding points back
		virtual uint64_t GetShaderNumber() const = 0;
		virtual uint64_t GetVertexBufferNumber() const = 0;
		virtual uint64_t GetIndexBufferNumber() const = 0;
		virtual uint64_t GetIndirectBufferNumber() const = 0;
		virtual uint64_t GetTextureNumber() const = 0;
		virtual uint64_t GetUniformBufferNumber() const = 0;
		virtual uint64_t GetStorageBufferNumber() const = 0;

		virtual Ref<PiplineRenderBase> Copy() const = 0;

		static bool SortePiplineRenderBase(const Ref<PiplineRenderBase>& a, const Ref<PiplineRenderBase>& b)
		{
			RY_CORE_ASSERT(nullptr != a && nullptr != b);
			using SortTuple = std::tuple<uint64_t, uint64_t, uint64_t>;
			SortTuple aTuple = std::make_tuple(
				a->GetShaderNumber()
				, a->GetIndexBufferNumber()
				, a->GetTextureNumber()
			);
			SortTuple bTuple = std::make_tuple(
				b->GetShaderNumber()
				, b->GetIndexBufferNumber()
				, b->GetTextureNumber()

			);
			bool result = aTuple < bTuple;
			return result;
		}

		// Check if vertex and index buffer are set or not and submit if nesseayr
		// return shows if we haved updated the buffer or not
		static bool CheckVAOFromMeshSingleShade(Ref<VertexArray>& vao, Ref<MeshSingle>& meshSingle)
		{
			if (HasVAOFromMeshSingleSomeShadeBuffer(vao, meshSingle))
				return false;

			RY_DESTROY_REF(vao);
			const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
			const Ref<IndexBuffer>& iabShade = meshSingle->GetShadeIndexBuffer();
			
			vao = VertexArray::Create();
			vao->AddVertexBuffer(vab);
			vao->SetIndexBuffer(iabShade);
			vao->SetPrimitv(VertexArray::Primitv::Traingle);
			return true;
		}

		// Check if vertex and index buffer are set or not and submit if nesseayr
		// return shows if we haved updated the buffer or not
		static bool CheckVAOFromMeshSingleDepth(Ref<VertexArray>& vao, Ref<MeshSingle>& meshSingle)
		{
			if (HasVAOFromMeshSingleSomeDepthBuffer(vao, meshSingle))
				return false;

			RY_DESTROY_REF(vao);
			const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
			const Ref<IndexBuffer>& iabDepth = meshSingle->GetDepthIndexBuffer();



			vao = VertexArray::Create();
			vao->AddVertexBuffer(vab);
			vao->SetIndexBuffer(iabDepth);

			return true;

		}

		// Check if vertex and index buffer are set or not and submit if nesseayr
		// return shows if we haved updated the buffer or not
		static bool CheckVAOFromMeshSingleShape(Ref<VertexArray>& vao, Ref<MeshSingle>& meshSingle)
		{
			return CheckVAOFromMeshSingleShade(vao, meshSingle);
		}


		static bool HasVAOFromMeshSingleSomeBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
		{
			RY_CORE_ASSERT(nullptr != meshSingle);

			if (nullptr == vao)
				return false;

			bool result = HasVAOFromMeshSingleShadeIndexBuffer(vao, meshSingle)
				|| HasVAOFromMeshSingleDepthIndexBuffer(vao, meshSingle)
				|| HasVAOFromMeshSingleVertexBuffer(vao, meshSingle);

			return result;
		}

		static bool HasVAOFromMeshSingleSomeShadeBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
		{
			RY_CORE_ASSERT(nullptr != meshSingle);

			if (nullptr == vao)
				return false;

			bool result = HasVAOFromMeshSingleShadeIndexBuffer(vao, meshSingle)
				|| HasVAOFromMeshSingleVertexBuffer(vao, meshSingle);
			return result;
		}

		static bool HasVAOFromMeshSingleSomeDepthBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
		{
			RY_CORE_ASSERT(nullptr != meshSingle);

			if (nullptr == vao)
				return false;

			bool result = HasVAOFromMeshSingleDepthIndexBuffer(vao, meshSingle)
				|| HasVAOFromMeshSingleVertexBuffer(vao, meshSingle);
			return result;
		}

		static bool HasVAOFromMeshSingleVertexBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
		{
			RY_CORE_ASSERT(nullptr != meshSingle);
			RY_CORE_ASSERT(nullptr != vao);

			const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();

			const std::vector<VertexArray::VertexElements>& vabVec = vao->GetVertexBuffers();
			for (const VertexArray::VertexElements& e : vabVec)
			{
				const Ref<VertexBuffer>& buffer = e.Buffer;
				if (vab == buffer)
				{
					return true;
				}
			}
			return false;
		}

		static bool HasVAOFromMeshSingleShadeIndexBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
		{
			RY_CORE_ASSERT(nullptr != meshSingle);
			RY_CORE_ASSERT(nullptr != vao);

			const Ref<IndexBuffer>& iabShade = meshSingle->GetShadeIndexBuffer();
			const Ref<IndexBuffer>& iab = vao->GetIndexBuffer();
			return iabShade == iab;
		}

		static bool HasVAOFromMeshSingleDepthIndexBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
		{
			RY_CORE_ASSERT(nullptr != meshSingle);
			RY_CORE_ASSERT(nullptr != vao);

			const Ref<IndexBuffer>& iabDepth = meshSingle->GetDepthIndexBuffer();
			const Ref<IndexBuffer>& iab = vao->GetIndexBuffer();
			return iabDepth == iab;
		}

		static void PrintPlineResult(PiplineResultState result)
		{
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_None);

			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_Success);

			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NoRenderObjectSpaceLeft);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NoRenderShapeSpaceLeft);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NoShadeDefinitionSpaceLeft);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NoShaderSpaceLeft);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NoTextexurSpaceLeft);

			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NotAllowedRenderObject);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NotAllowedShadeDefinition);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NotAllowedShader);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NotAllowedTexture);
			RY_CHECK_IF_BIT_IS_SET_PRINT(result, Result_NotAllowedRenderShape);
		}


	};


}