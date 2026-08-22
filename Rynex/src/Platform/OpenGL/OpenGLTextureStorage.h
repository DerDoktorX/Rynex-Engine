#pragma once
namespace Rynex {
#if 1
	class OpenGLTextureObject
	{
	public:
		struct ObjectTypeOpenGL
		{
			uint32_t target, formate, interleFormat, levelMidMaps, samples;
			ObjectTypeOpenGL();
			ObjectTypeOpenGL(uint32_t target);
			ObjectTypeOpenGL(uint32_t target, uint32_t fromat);
			ObjectTypeOpenGL(uint32_t target, uint32_t fromat, uint32_t interleFormat);
			ObjectTypeOpenGL(uint32_t target, uint32_t fromat, uint32_t interleFormat, uint32_t levelMidMaps);
			ObjectTypeOpenGL(uint32_t target, uint32_t fromat, uint32_t interleFormat, uint32_t levelMidMaps, uint32_t samples);

			bool operator==(const ObjectTypeOpenGL& left)
			{
				bool rTarget = this->target == left.target;
				bool rFormate = this->formate == left.formate;
				bool rInterleFormat = this->interleFormat == left.interleFormat;
				bool rLevelMidMaps = this->levelMidMaps == left.levelMidMaps;
				bool rSamples = this->samples == left.samples;
				bool result = rTarget && rFormate && rInterleFormat && rLevelMidMaps && rSamples;
				return result;
			}

			bool operator!=(const ObjectTypeOpenGL& left)
			{
				bool rTarget = this->target != left.target;
				bool rFormate = this->formate != left.formate;
				bool rInterleFormat = this->interleFormat != left.interleFormat;
				bool rLevelMidMaps = this->levelMidMaps != left.levelMidMaps;
				bool rSamples = this->samples != left.samples;
				bool result = rTarget || rFormate || rInterleFormat || rLevelMidMaps || rSamples;
				return result;
			}
		};
		struct DimensionOpenGL
		{
			uint32_t withe, heigth, depth;
			DimensionOpenGL()
				: withe(1u), heigth(1u), depth(1u) {
			}

			DimensionOpenGL(uint32_t withe, uint32_t heigth = 1u, uint32_t depth = 1u)
				: withe(withe), heigth(heigth), depth(depth) {
			}

			DimensionOpenGL(const glm::uvec3& size)
				: withe(size.x), heigth(size.y), depth(size.z) {
			}


			bool operator==(const DimensionOpenGL& left) const
			{
				bool rWithe = this->withe == left.withe;
				bool rHeigth = this->heigth == left.heigth;
				bool rDepth = this->depth == left.depth;
				bool result = rWithe && rHeigth && rDepth;
				return result;
			}

			bool operator!=(const DimensionOpenGL& left) const
			{
				bool rWithe = this->withe != left.withe;
				bool rHeigth = this->heigth != left.heigth;
				bool rDepth = this->depth != left.heigth;
				bool result = rWithe || rHeigth || rDepth;
				return result;
			}

			constexpr uint32_t& operator[](uint32_t dimensionIndex)
			{
				switch (dimensionIndex)
				{
				case 0u:
					return withe;
				case 1u:
					return heigth;
				case 2u:
					return depth;
				default:
					break;
				}
				RY_CORE_ASSERT(false, "Outside of vaild range from [0, 1, 2], return depth");
				return depth;
			}

			const uint32_t& operator[](uint32_t dimensionIndex) const
			{
				switch (dimensionIndex)
				{
				case 0u:
					return withe;
				case 1u:
					return heigth;
				case 2u:
					return depth;
				default:
					break;
				}
				RY_CORE_ASSERT(false, "Outside of vaild range from [0, 1, 2], return depth");
				return depth;
			}
		};

		struct TextureCopyData
		{
			uint32_t renderID;
			OpenGLTextureObject::DimensionOpenGL offset;
			OpenGLTextureObject::ObjectTypeOpenGL type;

			bool operator==(const TextureCopyData& left)
			{
				bool rRenderID = this->renderID != left.renderID;
				bool rType = this->type == left.type;
				bool result = rRenderID && rType;
				return result;
			}

			bool operator!=(const TextureCopyData& left)
			{
				bool rRenderID = this->renderID == left.renderID;
				bool rType = this->type != left.type;
				bool result = rRenderID || rType;
				return result;
			}

		};
	public:
		OpenGLTextureObject();
		OpenGLTextureObject(const ObjectTypeOpenGL& object, const DimensionOpenGL& dimension);
		~OpenGLTextureObject();


		void Create(uint32_t renderID);
		void Create(uint32_t renderID, const std::vector<uint8_t>& texData, uint32_t pixelByteSize);

		void SetObjectType(const ObjectTypeOpenGL& object) { m_Object = object; }
		void SetDimension(const DimensionOpenGL& dimension) { m_Dimension = dimension; }

		const ObjectTypeOpenGL& GetObjectType() const noexcept { return m_Object; }
		const DimensionOpenGL& GetDimension() const noexcept { return m_Dimension; }
		uint32_t GetTextureByteSize() const;

		void SetData(uint32_t renderID, const std::vector<uint8_t>& texData, uint32_t pixelByteSize, uint32_t offset = 0u)const;

		void GetData(uint32_t renderID, std::vector<uint8_t>& texData, uint32_t bpp)const;
		void GetData(uint32_t renderID, std::vector<uint8_t>& texData, uint32_t bpp, uint32_t format)const;
		void ClearImage(uint32_t renderID, const uint32_t* clearData, uint32_t type) const { ClearImage(renderID, reinterpret_cast<const uint8_t*>(clearData), type); }
		void ClearImage(uint32_t renderID, const int* clearData, uint32_t type) const { ClearImage(renderID, reinterpret_cast<const uint8_t*>(clearData), type); }
		void ClearImage(uint32_t renderID, const float* clearData, uint32_t type) const { ClearImage(renderID, reinterpret_cast<const uint8_t*>(clearData), type); }
		void ClearImage(uint32_t renderID, const char (*dataPtr)[8], uint32_t type)const { ClearImage(renderID, reinterpret_cast<const uint8_t*>(dataPtr), type); }
		void ClearImage(uint32_t renderID, const uint8_t* clearData, uint32_t type)const;
		void ClearImage(uint32_t renderID, std::nullptr_t, uint32_t type)const;

		void PrintSettings();

		bool CopyFromTextureToTexture(uint32_t srcRenderID, uint32_t dstRenderID, const OpenGLTextureObject& srcTexture, const DimensionOpenGL& copySize);
		bool CopyFromTextureToTexture(uint32_t srcRenderID, uint32_t dstRenderID, const OpenGLTextureObject& srcTexture, const DimensionOpenGL& copySize, const DimensionOpenGL& srcOffset, const DimensionOpenGL& dstOffset);

		static void CopyTextureToTexture(const TextureCopyData& srcCopy, const TextureCopyData& dstCopy, const DimensionOpenGL& dimension);

#if 0
		static uint32_t GetTextureTarget(TextureTarget target, bool multisampled);
#endif
	private:
		void CreateObject1D(uint32_t renderID, const uint8_t* texDataPtr)const;
		void CreateObject2D(uint32_t renderID, const uint8_t* texDataPtr)const;
		void CreateObject3D(uint32_t renderID, const uint8_t* texDataPtr)const;
#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
		void CreateObjectCubeMap(uint32_t renderID, const std::array<uint8_t*, 6>& texDataPtrArray) const;
#else
		void CreateObjectCubeMap(uint32_t renderID, const uint8_t* texDataPtr, uint32_t texureCount) const;
#endif
		uint32_t GetTexture2DByteSize() const;
		uint32_t GetTexture1DByteSize() const;
		uint32_t GetTextureCubeMapCount(const std::vector<uint8_t>& texData)const;

		void SetData1D(uint32_t renderID, const uint8_t* texDataPtr)const;
		void SetData2D(uint32_t renderID, const uint8_t* texDataPtr, int offsetY = -1)const;
		void SetData3D(uint32_t renderID, const uint8_t* texDataPtr, int offsetZ = -1)const;
#ifdef RY_OPENGL_CUBE_MAP_TEXTURE_SEPARTION
		void SetDataCubeMap(uint32_t renderID, const std::array<uint8_t*, 6>& texDataPtrArray)const;
#else
		void SetDataCubeMap(uint32_t renderID, const uint8_t* texDataPtr, uint32_t texureCount, uint32_t offsetCubeMapTexture = 0u)const;
#endif


		// void CreateObject2DMultysample(uint32_t renderID, const void* texDataPtr);
		// void CreateObject3DMultysample(uint32_t renderID, const void* texDataPtr);
		static bool IsInsideVaildCopyRange(DimensionOpenGL textureSize, DimensionOpenGL offsetSize, DimensionOpenGL copySize);
		static bool IsInsideVaildCopyRange(uint32_t textureDimensionSize, uint32_t textureDimensionOffset, uint32_t textureDimensionCopySize);
		static bool Converter1DVecInArray2DCubeMap(const std::vector<uint8_t>& texData, std::array<void*, 6>& texDataPtrArray);
	private:
		ObjectTypeOpenGL m_Object;
		DimensionOpenGL m_Dimension;
	};

	class OpenGLTextureSampler
	{
	public:
		struct FilterOpenGL
		{
			uint32_t Min, Mag;
			FilterOpenGL();
			FilterOpenGL(uint32_t min, uint32_t mag);
		};
		struct WarpOpenGL
		{
			uint32_t S, T, R;
			WarpOpenGL();
			WarpOpenGL(uint32_t v);
			WarpOpenGL(uint32_t s, uint32_t t);
			WarpOpenGL(uint32_t s, uint32_t t, uint32_t r);
		};
		struct CompareOpenGL
		{
			uint32_t Mode, Func;

			CompareOpenGL();
			CompareOpenGL(uint32_t mode, uint32_t func);

		};
		struct LodOpenGL
		{
			float Min, Max, Bias, Anisotropy;
			LodOpenGL(float min = -1000.0f, float max = 1000.0f, float bias = 0.0f, float anisotropy = 1.0f)
				: Min(min), Max(max), Bias(bias), Anisotropy(anisotropy)
			{
			}
		};
	public:
		OpenGLTextureSampler();
		OpenGLTextureSampler(const FilterOpenGL& filter, const WarpOpenGL& warp, const CompareOpenGL& compare, const LodOpenGL& lod, const std::array<float, 4>& borderColor = { 0.0f, 0.0f, 0.0f, 0.0f });
		~OpenGLTextureSampler();

		void SetFilter(const FilterOpenGL& filter) { m_Filter = filter; }
		void SetWarp(const WarpOpenGL& warp) { m_Warp = warp; }
		void SetCompare(const CompareOpenGL& compare) { m_Compare = compare; }
		void SetLod(const LodOpenGL& lod) { m_Lod = lod; }
		void SetBorderColor(const std::array<float, 4>& borderColor) { m_BorderColor = borderColor; }

		void PrintSettings();

		void Create(uint32_t renderID);
		void CreateSampler(uint32_t renderID);

		const FilterOpenGL& GetFilter() const noexcept { return m_Filter; }
		const WarpOpenGL& GetWarp() const noexcept { return m_Warp; }
		const CompareOpenGL& GetCompare() const noexcept { return m_Compare; }
		const LodOpenGL& GetLod() const noexcept { return m_Lod; }
		const std::array<float, 4>& GetBorderColor()const noexcept { return m_BorderColor; }

	private:
		void CreateFilter(uint32_t renderID) const;
		void CreateWarp(uint32_t renderID)const;
		void CreateCompare(uint32_t renderID)const;
		void CreateLod(uint32_t renderID)const;
		void CreateBorderColor(uint32_t renderID)const;
	private:
		FilterOpenGL m_Filter;
		WarpOpenGL m_Warp;
		CompareOpenGL m_Compare;
		LodOpenGL m_Lod;
		std::array<float, 4> m_BorderColor;
	};
#endif
}