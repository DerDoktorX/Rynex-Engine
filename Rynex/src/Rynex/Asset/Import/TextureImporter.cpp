#include "rypch.h"
#include "TextureImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"

#include <stb_image.h>
#include <stb_image_write.h>

namespace Rynex{
	namespace Utils {

	}


	Ref<Texture> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
#if 0
		std::filesystem::path filePath = (Project::GetActiveProjectDirectory() / metadata.FilePath).string();
#else
		std::filesystem::path filePath = metadata.AbsolutePath;
#endif
		return LoadTexture(filePath);
	}

	Ref<Texture> TextureImporter::LoadTexture(const std::filesystem::path& path)
	{
		std::filesystem::path extension = path.extension();
		if(".png" != extension
			&& ".jpeg" != extension
			&& ".jpg" != extension
			&& ".hdr"  != extension)
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture! not sepoted extension {}", extension);
			return nullptr;
		}
		int width, height, channels, req_comp;
		stbi_set_flip_vertically_on_load(1);
		std::string pathStr = path.string();
		const char* pathChar = pathStr.c_str();
		
		stbi_uc* dataBytePtr = stbi_load(pathChar, &width, &height, &channels, STBI_default); // STBI_rgb_alpha
		

		if (nullptr == dataBytePtr)
		{
			RY_CORE_ERROR("Coud not Load Image! {}", path);
			return nullptr;
		}


		TextureSpecification spec;
		spec.Width = static_cast<uint32_t>(width);
		spec.Height = static_cast<uint32_t>(height);
		spec.Target = TextureTarget::Texture2D;
		spec.FilteringMode = TextureFilteringMode::Linear;
		spec.WrappingSpec = {
			TextureWrappingMode::Repeate,
			TextureWrappingMode::Repeate,
		};
		if(stbi_is_hdr(pathChar))
		{
			switch (channels)
			{
				case 1:
				{
					spec.Format = TextureFormat::R8;
					break;
				}
				case 2:
				{
					spec.Format = TextureFormat::RG8;
					break;
				}
				case 3:
				{
					spec.Format = TextureFormat::RGB8;
					break;
				}
				case 4:
				{
					spec.Format = TextureFormat::RGBA16F;
					break;
				}
				default:
					RY_CORE_ASSERT(false);
			}
		}
		else
		{
			switch (channels)
			{
				case 1:
				{
					spec.Format = TextureFormat::R8;
					break;
				}
				case 2:
				{
					spec.Format = TextureFormat::RG8;
					break;
				}
				case 3:
				{
					spec.Format = TextureFormat::S_RGB8;
					break;
				}
				case 4:
				{
					spec.Format = TextureFormat::S_RGBA8;
					break;
				}
				default:
					RY_CORE_ASSERT(false);
			}
		}
		Ref<Texture> texture;
		
			
		uint32_t byteSize = width * height * channels;
		std::vector<unsigned char> vData(byteSize);
		std::memcpy(vData.data(), dataBytePtr, byteSize);
		texture = Texture::CreateAsync(spec, std::move(vData));
		RY_CORE_INFO("Succesfull Loade A-Sync Texture {}/{} withe {} Channels from Path {}", width, height, channels, path);
		
		stbi_image_free(dataBytePtr);
		return texture;
	}

	bool TextureImporter::ReLoadeTexture(AssetHandle handle, const std::filesystem::path& path)
	{
#if 1
		std::filesystem::path extension = path.extension();
		if (".png" != extension
			&& ".jpeg" != extension
			&& ".jpg" != extension
			&& ".hdr" != extension)
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture! not sepoted extension {}", extension);
			return nullptr;
		}

		RY_CORE_WARN("In Dev Funktion: ReLoadeTexture2D!");
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		std::string pathStr = path.string();
		const char* pathChar = pathStr.c_str();
		stbi_uc* dataBytePtr = stbi_load(pathChar, &width, &height, &channels, STBI_default);

		if (nullptr == dataBytePtr)
		{
			RY_CORE_ERROR("Coud not Load Image! {}", path);
			return false;
		}

		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		uint32_t bytesSize = width * height * channels;
		texture->SetData(dataBytePtr, bytesSize);
#if 1
		stbi_image_free(dataBytePtr);
		return true;
		
		
		
		
		
#endif // IMPORTENT! Becouse we Free the Image prt, at the ende of the conststuctor. We don't use the stbi Funktion for this because thar crated some Isuse! Withe Free meamory that alrdy has Freed.
#endif
	}

	bool TextureImporter::SaveTexture(const Ref<Texture>& tex, const std::filesystem::path& path)
	{
		RY_CORE_ASSERT(nullptr != tex, "no vaild Texture!");
		RY_CORE_ASSERT(path.has_parent_path(), "this Path dont Exist!");

		int width = static_cast<int>(tex->GetWidth());
		int height = static_cast<int>(tex->GetHeight());
		const TextureSpecification& spec = tex->GetSpecification();
		int channels = 0;
		std::vector<uint8_t> dataVec;
		switch (spec.Format)
		{
		case TextureFormat::R8:
		{
			channels = 1;
			break;
		}
		case TextureFormat::RG8:
		{
			channels = 2;
			break;
		}
		case TextureFormat::RGB32F:
		case TextureFormat::RGB16F:
		case TextureFormat::S_RGB8:
		case TextureFormat::RGB8:
		{
			dataVec = tex->GetCurrentRenderData(TextureFormat::RGB8);
			channels = 3;
			break;
		}
		case TextureFormat::RGBA32F:
		case TextureFormat::RGBA16F:
		case TextureFormat::S_RGBA8:
		case TextureFormat::RGBA8:
		{
			dataVec = tex->GetCurrentRenderData(TextureFormat::RGBA8);
			channels = 4;
			break;
		}
		default:
			RY_CORE_ASSERT(false);
			channels = 0;
		}
		
		if (channels == 0)
			return false;

		void* dataPtr = dataVec.data();

		stbi_flip_vertically_on_write(1);
		
		std::string pathStr = path.string();
		const char* pathChar = pathStr.c_str();

		stbi_write_png(pathChar, width, height, channels, dataPtr, width * channels);
		
		
		return true;
	}

	static inline uint32_t PackPixelColor(const uint8_t* pixel, uint32_t channelCount)
	{
		uint32_t color = 0;
		RY_CORE_ASSERT(channelCount <= 4, "To larg pxel bytes!");
		for (uint32_t i = 0; i < channelCount; i++)
		{
			color |= static_cast<uint32_t>(pixel[i]) << (8 * i);
		}
		return color;
	}


#if 0
	static std::vector<uint8_t> ExtractColorRegion(
		const uint8_t* byteDataPtr,
		uint32_t channelCount,
		uint32_t byteSize,
		glm::ivec2 size,
		std::vector<uint32_t>& ignorColorsVec,
		glm::ivec2& outRegionOffset,
		glm::ivec2& outRegionSize)
	{
		outRegionOffset = glm::ivec2(0, 0);
		outRegionSize = glm::ivec2(0, 0);

		// --- Sicherheitschecks ---
		const uint32_t expectedByteSize =
			static_cast<uint32_t>(size.x) * static_cast<uint32_t>(size.y) * channelCount;

		if (byteDataPtr == nullptr ||
			channelCount == 0 || channelCount > 4 ||
			size.x <= 0 || size.y <= 0 ||
			expectedByteSize > byteSize)
		{
			return {};
		}

		auto isIgnored = [&](uint32_t color) -> bool
			{
				return std::find(ignorColorsVec.begin(), ignorColorsVec.end(), color) != ignorColorsVec.end();
			};

		// --- 1. Zielfarbe bestimmen: erste Farbe im Bild, die nicht ignoriert wird ---
		uint32_t targetColor = 0;
		bool foundTarget = false;

		for (int32_t y = 0; y < size.y && !foundTarget; ++y)
		{
			for (int32_t x = 0; x < size.x; ++x)
			{
				const size_t index = (static_cast<size_t>(y) * size.x + x) * channelCount;
				const uint32_t color = PackPixelColor(byteDataPtr + index, channelCount);
				if (!isIgnored(color))
				{
					targetColor = color;
					foundTarget = true;
					break;
				}
			}
		}

		if (!foundTarget)
		{
			RY_CORE_INFO("no Color Found!");
			return {}; // Keine passende (nicht ignorierte) Farbe im Bild gefunden
		}

		// --- 2. Bounding-Box aller Pixel mit der Zielfarbe ermitteln ---
		int32_t minX = std::numeric_limits<int32_t>::max();
		int32_t minY = std::numeric_limits<int32_t>::max();
		int32_t maxX = std::numeric_limits<int32_t>::min();
		int32_t maxY = std::numeric_limits<int32_t>::min();

		for (int32_t y = 0; y < size.y; ++y)
		{
			for (int32_t x = 0; x < size.x; ++x)
			{
				const size_t index = (static_cast<size_t>(y) * size.x + x) * channelCount;
				const uint32_t color = PackPixelColor(byteDataPtr + index, channelCount);
				if (color == targetColor)
				{
					minX = std::min(minX, x);
					minY = std::min(minY, y);
					maxX = std::max(maxX, x);
					maxY = std::max(maxY, y);
				}
			}
		}

		// Sollte nicht passieren, da targetColor garantiert mindestens einmal vorkommt
		if (maxX < minX || maxY < minY)
		{
			RY_CORE_ASSERT(false);
			return {};
		}

		const int32_t regionWidth = maxX - minX + 1;
		const int32_t regionHeight = maxY - minY + 1;

		outRegionOffset = glm::ivec2(minX, minY);
		outRegionSize = glm::ivec2(regionWidth, regionHeight);

		// --- 3. Hintergrundfarbe für "andere" Pixel bestimmen ---
		// Bei vorhandenem Alpha-Kanal -> transparent, sonst -> weiß.
		uint8_t backgroundPixel[4] = { 255, 255, 255, 255 };
		if (channelCount == 4)
		{
			// RGBA -> komplett transparent
			backgroundPixel[0] = 0;
			backgroundPixel[1] = 0;
			backgroundPixel[2] = 0;
			backgroundPixel[3] = 0;
		}
		else if (channelCount == 2)
		{
			// z.B. Gray+Alpha -> transparent
			backgroundPixel[0] = 0;
			backgroundPixel[1] = 0;
		}
		// Bei 1 oder 3 Kanälen (kein Alpha) bleibt backgroundPixel = weiß (255...)

		// --- 4. Ausschnitt kopieren ---
		std::vector<uint8_t> result(
			static_cast<size_t>(regionWidth) * regionHeight * channelCount);

		for (int32_t y = 0; y < regionHeight; ++y)
		{
			for (int32_t x = 0; x < regionWidth; ++x)
			{
				const size_t outIndex = (static_cast<size_t>(y) * regionWidth + x) * channelCount;

				const int32_t srcX = minX + x;
				const int32_t srcY = minY + y;
				const size_t srcIndex = (static_cast<size_t>(srcY) * size.x + srcX) * channelCount;

				const uint32_t color = PackPixelColor(byteDataPtr + srcIndex, channelCount);

				if (color == targetColor)
				{
					for (uint32_t c = 0; c < channelCount; ++c)
					{
						result[outIndex + c] = byteDataPtr[srcIndex + c];
					}
				}
				else
				{
					for (uint32_t c = 0; c < channelCount; ++c)
					{
						result[outIndex + c] = backgroundPixel[c];
					}
				}
			}
		}
		RY_CORE_TRACE("Extraxt Imgag");
		return result;
	}

	std::vector<uint8_t> ExtractConnectedColorRegion(
		const uint8_t* byteDataPtr,
		uint32_t channelCount,
		uint32_t byteSize,
		glm::ivec2 size,
		std::vector<uint32_t>& ignorColorsVec,
		glm::ivec2 seedPos,
		glm::ivec2& outRegionOffset,
		glm::ivec2& outRegionSize)
	{
		outRegionOffset = glm::ivec2(0, 0);
		outRegionSize = glm::ivec2(0, 0);

		// --- Sicherheitschecks ---
		const uint32_t expectedByteSize =
			static_cast<uint32_t>(size.x) * static_cast<uint32_t>(size.y) * channelCount;

		if (byteDataPtr == nullptr ||
			channelCount == 0 || channelCount > 4 ||
			size.x <= 0 || size.y <= 0 ||
			expectedByteSize > byteSize ||
			seedPos.x < 0 || seedPos.y < 0 ||
			seedPos.x >= size.x || seedPos.y >= size.y)
		{
			return {};
		}

		auto isIgnored = [&](uint32_t color) -> bool
			{
				return std::find(ignorColorsVec.begin(), ignorColorsVec.end(), color) != ignorColorsVec.end();
			};

		auto pixelByteIndex = [&](int32_t x, int32_t y) -> size_t
			{
				return (static_cast<size_t>(y) * size.x + x) * channelCount;
			};

		auto pixelSample = [](uint32_t targetColor, uint32_t pixelColor) -> bool
			{
				glm::u8vec4 pixelColorRGBA = static_cast<glm::u8vec4>(pixelColor);
				glm::u8vec4 targetColorRGBA = static_cast<glm::u8vec4>(targetColor);
				glm::i16vec4 colorDirection =glm::i16vec4(pixelColorRGBA) - glm::i16vec4(targetColorRGBA);
				for (uint8_t i = 0; i < 3; i++)
				{
					colorDirection[i] = 0 < colorDirection[i] ? colorDirection[i] : -colorDirection[i];
				}
				constexpr int16_t offRange = 45;
				// int16_t colorDirectionLength = glm::length(colorDirection);
				glm::u8vec4 minPixelColorRGBA = targetColorRGBA;

				minPixelColorRGBA.r -= offRange;
				minPixelColorRGBA.g -= offRange;
				minPixelColorRGBA.b -= offRange;
				glm::u8vec4 maxPixelColorRGBA = targetColorRGBA;

				maxPixelColorRGBA.r += offRange;
				maxPixelColorRGBA.g += offRange;
				maxPixelColorRGBA.b += offRange;



				for (uint8_t i = 0; i < 3; i++)
				{
					if (offRange < colorDirection[i])
						return false;
				}


			};
		// --- 1. Zielfarbe = Farbe am Seed-Pixel ---
		const uint32_t targetColor = PackPixelColor(byteDataPtr + pixelByteIndex(seedPos.x, seedPos.y), channelCount);
		
		if (isIgnored(targetColor))
		{
			return {}; // Seed-Farbe ist eine ignorierte Farbe
		}

		// --- 2. Flood-Fill (4er-Nachbarschaft) iterativ per Stack ---
		std::vector<uint8_t> visited(static_cast<size_t>(size.x) * size.y, 0);
		std::vector<glm::ivec2> stack;
		stack.reserve(1024);
		stack.push_back(seedPos);
		visited[static_cast<size_t>(seedPos.y) * size.x + seedPos.x] = 1;

		int32_t minX = seedPos.x, maxX = seedPos.x;
		int32_t minY = seedPos.y, maxY = seedPos.y;

		// Direkte Nachbarn: links, rechts, oben, unten (keine Diagonalen)
		

		constexpr int32_t kOffsets[8][2] = {
			{-1,  1 }, { 0,  1}, { 1,  1},
			{-1,  0 },           { 1,  0},
			{-1, -1 }, { 0, -1}, { 1, -1}
		};


		while (!stack.empty())
		{
			const glm::ivec2 current = stack.back();
			stack.pop_back();

			minX = std::min(minX, current.x);
			maxX = std::max(maxX, current.x);
			minY = std::min(minY, current.y);
			maxY = std::max(maxY, current.y);

			for (const auto& offset : kOffsets)
			{
				const int32_t nx = current.x + offset[0];
				const int32_t ny = current.y + offset[1];

				if (nx < 0 || ny < 0 || nx >= size.x || ny >= size.y)
				{
					continue;
				}

				const size_t visitedIndex = static_cast<size_t>(ny) * size.x + nx;
				if (visited[visitedIndex] != 0)
				{
					continue;
				}

				const uint32_t neighborColor = PackPixelColor(byteDataPtr + pixelByteIndex(nx, ny), channelCount);
				if (pixelSample(targetColor, neighborColor))
				{
					visited[visitedIndex] = 1;
					stack.push_back(glm::ivec2(nx, ny));
				}
			}
		}

		const int32_t regionWidth = maxX - minX + 1;
		const int32_t regionHeight = maxY - minY + 1;

		outRegionOffset = glm::ivec2(minX, minY);
		outRegionSize = glm::ivec2(regionWidth, regionHeight);

		// --- 3. Hintergrundfarbe für "andere" Pixel bestimmen ---
		// Bei vorhandenem Alpha-Kanal -> transparent, sonst -> weiß.
		uint8_t backgroundPixel[4] = { 255, 255, 255, 255 };
		if (channelCount == 4)
		{
			// RGBA -> komplett transparent
			backgroundPixel[0] = 0;
			backgroundPixel[1] = 0;
			backgroundPixel[2] = 0;
			backgroundPixel[3] = 0;
		}
		else if (channelCount == 2)
		{
			// z.B. Gray+Alpha -> transparent
			backgroundPixel[0] = 0;
			backgroundPixel[1] = 0;
		}
		// Bei 1 oder 3 Kanälen (kein Alpha) bleibt backgroundPixel = weiß (255...)

		// --- 4. Ausschnitt kopieren ---
		std::vector<uint8_t> result(
			static_cast<size_t>(regionWidth) * regionHeight * channelCount);

		for (int32_t y = 0; y < regionHeight; ++y)
		{
			for (int32_t x = 0; x < regionWidth; ++x)
			{
				const size_t outIndex = (static_cast<size_t>(y) * regionWidth + x) * channelCount;

				const int32_t srcX = minX + x;
				const int32_t srcY = minY + y;
				const size_t visitedIndex = static_cast<size_t>(srcY) * size.x + srcX;
				const size_t srcIndex = pixelByteIndex(srcX, srcY);

				if (visited[visitedIndex] != 0)
				{
					// Gehört zur zusammenhängenden Region -> Originalpixel übernehmen
					for (uint32_t c = 0; c < channelCount; ++c)
					{
						result[outIndex + c] = byteDataPtr[srcIndex + c];
					}
				}
				else
				{
					// Gehört NICHT zur Region (auch bei evtl. gleicher Farbe, aber nicht verbunden)
					for (uint32_t c = 0; c < channelCount; ++c)
					{
						result[outIndex + c] = backgroundPixel[c];
					}
				}
			}
		}

		return result;
	}
#else
struct SeedGroup
{
	std::string name;               // z.B. Ländername, nur fürs Debugging/Dateinamen
	std::vector<glm::ivec2> seeds;  // ein oder mehrere Seed-Punkte
	uint32_t offRange = 15;
	uint32_t setColor = 0xFFFFFFFF;
};

static bool pixelSample(uint32_t targetColor, uint32_t pixelColor, uint32_t offRange)
	{
		if (offRange == 0) 
		{
			return targetColor == pixelColor;
		}
		glm::u8vec4 pixelColorRGBA = static_cast<glm::u8vec4>(pixelColor);
		glm::u8vec4 targetColorRGBA = static_cast<glm::u8vec4>(targetColor);
		glm::i16vec4 colorDirection = glm::i16vec4(pixelColorRGBA) - glm::i16vec4(targetColorRGBA);
		for (uint8_t i = 0; i < 3; i++)
		{
			colorDirection[i] = 0 < colorDirection[i] ? colorDirection[i] : -colorDirection[i];
		}
		// int16_t colorDirectionLength = glm::length(colorDirection);
		glm::u8vec4 minPixelColorRGBA = targetColorRGBA;

		minPixelColorRGBA.r -= offRange;
		minPixelColorRGBA.g -= offRange;
		minPixelColorRGBA.b -= offRange;
		glm::u8vec4 maxPixelColorRGBA = targetColorRGBA;

		maxPixelColorRGBA.r += offRange;
		maxPixelColorRGBA.g += offRange;
		maxPixelColorRGBA.b += offRange;



		for (uint8_t i = 0; i < 3; i++)
		{
			if (offRange < colorDirection[i])
				return false;
		}

		return true;
	};

static void FloodFillFromSeed(
	const uint8_t* byteDataPtr,
	uint32_t channelCount,
	uint32_t offRange,
	glm::ivec2 size,
	glm::ivec2 seedPos,
	std::vector<uint8_t>& visited,
	int32_t& minX, int32_t& minY,
	int32_t& maxX, int32_t& maxY,
	uint32_t color)
{
	auto pixelByteIndex = [&](int32_t x, int32_t y) -> size_t
		{
			return (static_cast<size_t>(y) * size.x + x) * channelCount;
		};

	const size_t seedFlatIndex = static_cast<size_t>(seedPos.y) * size.x + seedPos.x;
	if (visited[seedFlatIndex] != 0)
	{
		return; // liegt schon in einer zuvor gefüllten Region dieser Gruppe
	}

	const uint32_t targetColor = PackPixelColor(byteDataPtr + pixelByteIndex(seedPos.x, seedPos.y), channelCount);

	std::vector<glm::ivec2> stack;
	stack.reserve(1024);
	stack.push_back(seedPos);
	visited[seedFlatIndex] = 1;

	constexpr int32_t kOffsets[8][2] = {
		{-1, 1}, {0, 1}, {1, 1},
		{-1, 0},         {1, 0},
		{-1,-1}, {0,-1}, {1,-1}
	};

	while (!stack.empty())
	{
		const glm::ivec2 current = stack.back();
		stack.pop_back();

		minX = std::min(minX, current.x);
		maxX = std::max(maxX, current.x);
		minY = std::min(minY, current.y);
		maxY = std::max(maxY, current.y);

		for (const auto& off : kOffsets)
		{
			const int32_t nx = current.x + off[0];
			const int32_t ny = current.y + off[1];
			if (nx < 0 || ny < 0 || nx >= size.x || ny >= size.y) continue;

			const size_t vIdx = static_cast<size_t>(ny) * size.x + nx;
			if (visited[vIdx] != 0) continue;

			const uint32_t neighborColor = PackPixelColor(byteDataPtr + pixelByteIndex(nx, ny), channelCount);
			if (pixelSample(targetColor, neighborColor, offRange)) // dein bisheriger pixelSample-Vergleich
			{
				visited[vIdx] = 1;
				stack.push_back({ nx, ny });
			}
		}
	}
}
std::vector<uint8_t> ExtractConnectedColorRegionMulti(
	const uint8_t* byteDataPtr,
	uint32_t channelCount,
	uint32_t byteSize,
	uint32_t offRange,
	glm::ivec2 size,
	std::vector<uint32_t>& ignorColorsVec,
	const std::vector<glm::ivec2>& seedPosVec,   // mehrere Seeds einer Gruppe
	glm::ivec2& outRegionOffset,
	glm::ivec2& outRegionSize, uint32_t color)
{
	outRegionOffset = glm::ivec2(0, 0);
	outRegionSize = glm::ivec2(0, 0);

	const uint32_t expectedByteSize =
		static_cast<uint32_t>(size.x) * static_cast<uint32_t>(size.y) * channelCount;

	if (byteDataPtr == nullptr || channelCount == 0 || channelCount > 4 ||
		size.x <= 0 || size.y <= 0 || expectedByteSize > byteSize || seedPosVec.empty())
	{
		return {};
	}

	auto isIgnored = [&](uint32_t color)
		{
			return std::find(ignorColorsVec.begin(), ignorColorsVec.end(), color) != ignorColorsVec.end();
		};
	auto pixelByteIndex = [&](int32_t x, int32_t y) -> size_t
		{
			return (static_cast<size_t>(y) * size.x + x) * channelCount;
		};

	std::vector<uint8_t> visited(static_cast<size_t>(size.x) * size.y, 0);

	int32_t minX = std::numeric_limits<int32_t>::max();
	int32_t minY = std::numeric_limits<int32_t>::max();
	int32_t maxX = std::numeric_limits<int32_t>::min();
	int32_t maxY = std::numeric_limits<int32_t>::min();
	bool anySeedOk = false;

	for (const glm::ivec2& seedPos : seedPosVec)
	{
		if (seedPos.x < 0 || seedPos.y < 0 || seedPos.x >= size.x || seedPos.y >= size.y)
			continue;

		const uint32_t seedColor = color == 0xFFFFFFFF ? PackPixelColor(byteDataPtr + pixelByteIndex(seedPos.x, seedPos.y), channelCount) : color;
		if (isIgnored(seedColor))
			continue;

		FloodFillFromSeed(byteDataPtr, channelCount, offRange, size, seedPos, visited, minX, minY, maxX, maxY, color);
		anySeedOk = true;
	}

	if (!anySeedOk) return {};

	const int32_t regionWidth = maxX - minX + 1;
	const int32_t regionHeight = maxY - minY + 1;
	outRegionOffset = { minX, minY };
	outRegionSize = { regionWidth, regionHeight };

	uint8_t bg[4] = { 255, 255, 255, 255 };
	if (channelCount == 4) bg[0] = bg[1] = bg[2] = bg[3] = 0;
	else if (channelCount == 2) bg[0] = bg[1] = 0;

	std::vector<uint8_t> result(static_cast<size_t>(regionWidth) * regionHeight * channelCount);

	for (int32_t y = 0; y < regionHeight; ++y)
		for (int32_t x = 0; x < regionWidth; ++x)
		{
			const size_t outIdx = (static_cast<size_t>(y) * regionWidth + x) * channelCount;
			const int32_t srcX = minX + x, srcY = minY + y;
			const size_t vIdx = static_cast<size_t>(srcY) * size.x + srcX;
			const size_t srcIdx = pixelByteIndex(srcX, srcY);

			const uint8_t* src = (visited[vIdx] != 0) ? (byteDataPtr + srcIdx) : bg;
			for (uint32_t c = 0; c < channelCount; ++c)
				result[outIdx + c] = src[c];
		}

	return result;
}
std::vector<uint8_t> ExtractConnectedColorRegion(
	const uint8_t* byteDataPtr, uint32_t channelCount, uint32_t byteSize, uint32_t offRange,
	glm::ivec2 size, std::vector<uint32_t>& ignorColorsVec, glm::ivec2 seedPos,
	glm::ivec2& outRegionOffset, glm::ivec2& outRegionSize, uint32_t color)
{
	return ExtractConnectedColorRegionMulti(
		byteDataPtr, channelCount,  byteSize, offRange, size, ignorColorsVec,
		{ seedPos }, outRegionOffset, outRegionSize, color);
}
#endif


	void TextureImporter::ExtraxtColorRegionFromImage()
	{
		RY_CORE_INFO("ExtraxtColorRegionFromImage Start");
		int width, height, channels, req_comp;
		const char* pathChar = "D:/dev/No-Risiko-NoFun/World Data/Risk_game_map.png";
		stbi_uc* dataBytePtr = stbi_load(pathChar, &width, &height, &channels, STBI_default);

		uint32_t byteSize = width * height * channels;
		glm::ivec2 size = { width, height };
		std::vector<uint32_t> ignorColor = {
			0xFFFFFFFF,0x00000000,
		};
		



		uint32_t index = 1;
		std::vector<SeedGroup> seedGroups = {
			{ "North-America_Country_01", { {60,62} } },
			{ "North-America_Country_02", { {124,91} } },
			{ "North-America_Country_03", { {102,184},  {190,210}, {184,219}, {205,216}, {220,219},{182,255} } },
			{ "North-America_Country_04", { {129,168} } },
			{ "North-America_Country_05", { {361,33} } },
			{ "North-America_Country_06", { {156,55}, {184,38}, {212,46}, {2120,35}, {234,33} } },
			{ "North-America_Country_07", { {190,103} } },
			{ "North-America_Country_08", { {227,107}, {248,118} } },
			{ "North-America_Country_09", { {113,133} } },

			{ "South-America_Country_01", { {228,379},{250,473} } },
			{ "South-America_Country_02", { {257,342} } },
			{ "South-America_Country_03", { {233,341} } },
			{ "South-America_Country_04", { {188,271} } },

			{ "Africa_Country_01", { {518,284} } },
			{ "Africa_Country_02", { {544,257} } },
			{ "Africa_Country_03", { {522,189} } },
			{ "Africa_Country_04", { {581,360} } },
			{ "Africa_Country_05", { {467,255} } },
			{ "Africa_Country_06", { {509,371} } },

			{ "Europe_Country_01", { {433,100},{417,97} }, 0 },
			{ "Europe_Country_02", { {396,59} }, 0 },
			{ "Europe_Country_03", { {486,105} }, 0 },
			{ "Europe_Country_04", { {503,64} }, 0 },
			{ "Europe_Country_05", { {507,125} }, 0 },
			{ "Europe_Country_06", { {527,109} }, 0 },
			{ "Europe_Country_07", { {422,141} }, 0 },

			{ "Asia_Country_01", { {611,128} }, 0 },
			{ "Asia_Country_02", { {710,152},{819, 99},{785, 218} }, 0 },
			{ "Asia_Country_03", { {667,182},{696, 258} }, 0 },
			{ "Asia_Country_04", { {726,86} } },
			{ "Asia_Country_05", { {840,168},{848, 166},{853, 159} } },
			{ "Asia_Country_06", { {823,107},{844,109} },  0 },
			{ "Asia_Country_07", { {558,156},{517,138} }, 0 },
			{ "Asia_Country_08", { {764,123} }, 0 },
			{ "Asia_Country_09", { {768,236} } },
			{ "Asia_Country_10", { {674,68} },0 },
			{ "Asia_Country_11", { {645,84} }, 0},
			{ "Asia_Country_12", { {767,71} }, 0,  },

			{ "Australia_Country_01", { {877,363} } },
			{ "Australia_Country_02", { {812,278},{767,291},{797,310},{815,315},{823,319},{826,315},{838,316},{826,291}, {840,257 },{833, 250}, {818, 250},{830,244}, {837, 247}, {834, 241}, {826,239},{824, 228} } },
			{ "Australia_Country_03", { {853,295}, {851, 283}, {884,296},{922,306}, {936, 307},{942, 310}, {948,314},{943, 314}, {948,314}, {953, 317}, {951, 319}, {955, 322} } },
			{ "Australia_Country_04", { {826,383} } },

			{ "Conections", { {200,170 } }, 1, 0x000000FF }


		};
		for (const SeedGroup& group : seedGroups)
		{
			glm::ivec2 outRegionOffset{ 0,0 };
			glm::ivec2 outRegionSize{ 0,0 };

			std::vector<uint8_t> colorField = ExtractConnectedColorRegionMulti(
				dataBytePtr, channels, byteSize, group.offRange, size, ignorColor,
				group.seeds, outRegionOffset, outRegionSize, group.setColor);

			if (colorField.empty())
			{
				RY_CORE_WARN("Gruppe {} ergab keine Region", group.name.c_str());
				++index;
				continue;
			}

			std::string outPutPath = "D:/dev/No-Risiko-NoFun/World Data/TestImagExtraxt/"
				+ group.name + ".png";

			stbi_write_png(outPutPath.c_str(), outRegionSize.x, outRegionSize.y,
				channels, colorField.data(), outRegionSize.x * channels);

			++index;
		}
		stbi_image_free(dataBytePtr);
		RY_CORE_INFO("ExtraxtColorRegionFromImage Ende");
	}

}
