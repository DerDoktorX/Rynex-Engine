#include "rypch.h"
#include "Renderer3D.h"

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Renderer/Objects/BoxAABB.h"
#include "Rynex/DataStructs/HashPtrTable.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Rynex {

#if 1

    struct LigthUniformPtr
    {
        glm::vec4* ShadowTexSizePtr;
        glm::mat4* ShadowMatrixPtr;
    };
    namespace Utils {

        struct TextureAtlas
        {
            glm::vec4* UniformST_CoordPtr = nullptr;
            float* ShadowMapSizePtr = nullptr;
            

            
            TextureAtlas(const TextureAtlas&) = default;
            TextureAtlas(TextureAtlas&&) = default;
            TextureAtlas(glm::vec4* uniformST_CoordPtr, float* shadowMapSizePtr)
                : UniformST_CoordPtr(uniformST_CoordPtr),
                ShadowMapSizePtr(shadowMapSizePtr) { }
        };

        struct AtlasTexture
        {
            glm::ivec2 Size = { 0, 0 };
        }; 
        
        
        struct point
        {
            int x, y;
        };

        struct box
        {
            point topleft;
            point bottomright;
        };





        inline static std::vector<box> allocate_texture_atlas(point const& atlas_size, std::vector<int> const& texture_sizes)
        {
            RY_PROFILE_SCOPE("Other Implementaion Atlas Texture");
            // we have to separately sort the indices so that the i-th region
            // of the output corresponds to the i-th texture size of the input
            std::vector<int> sorted(texture_sizes.size());
            for (int i = 0; i < sorted.size(); ++i)
                sorted[i] = i;

            // sort in descending order
            // std::sort(sorted.begin(), sorted.end(), [&](int i, int j) {
            //     return texture_sizes[i] > texture_sizes[j];
            //     });

            std::vector<point> ladder;

            point pen{ 0, 0 };

            std::vector<box> result;
            result.resize(texture_sizes.size());

            for (int i : sorted)
            {
                int const size = texture_sizes[i];

                // allocate a texture region
                result[i] = { {pen.x, pen.y}, {pen.x + size, pen.y + size} };

                // shift the pen to the right
                pen.x += size;

                // update the ladder
                if (!ladder.empty() && ladder.back().y == pen.y + size)
                    ladder.back().x = pen.x;
                else
                    ladder.push_back({ pen.x, pen.y + size });

                if (pen.x == atlas_size.x)
                {
                    // the pen hit the right edge of the atlas
                    ladder.pop_back();

                    pen.y += size;
                    if (!ladder.empty())
                        pen.x = ladder.back().x;
                    else
                        pen.x = 0;
                }
            }

            return result;
        }
       
#if 0 
        struct AtelesTexture
        {
            int Pixel = 0;
            int Count = 0;
            int OneRow = 0;
            int OneColume = 0;
            int TexCount = 0;

        };
        struct TextureAtles
        {
            glm::ivec2 TexPos = { 0, 0 };
            glm::ivec2 TexSize = { 0, 0 };

            glm::ivec2 PixelPos = { 0, 0 };
            glm::ivec2 PixelSize = { 0, 0 };


            glm::vec2 st_CoordPos = { 0.0f, 0.0f };
            float st_CoordSize = 0.0f;
        };
       




        inline static uint32_t CalceQuadePixelSize(uint32_t count, uint32_t size)
        { 
            return count * size * size;
        }

        inline static glm::uvec2 CaluceQuadeCount(uint32_t textureCount)
        {
            int columns = std::ceil(std::sqrt(textureCount));
            int rows = std::ceil(static_cast<double>(textureCount) / columns);
            return glm::uvec2(columns, rows);
        }

       

        inline static void OrgenizeTexturePlace(uint32_t index, uint32_t colume, uint32_t rows ,uint32_t size, glm::vec4* texPos)
        {
            glm::uvec2 posIndex = {
                    (index % colume),
                    (index / colume)
            };
            float stdCoordOffsetX =  posIndex.y * size;
            float stdCoordOffsetY =  posIndex.x * size;
            float stdCoordStepPosX = colume;
            float stdCoordStepPosY = rows;

            


            *texPos = { 
                stdCoordOffsetX, stdCoordOffsetY,
                stdCoordStepPosX, stdCoordStepPosY
            };
        }

        inline static glm::uvec4 Calculate(uint32_t pixxelSum, uint32_t count, uint32_t pixel, uint32_t countSmalestTex)
        {
            glm::uvec2 sizeCount = CaluceQuadeCount(countSmalestTex);
            glm::uvec2 sizePixxel = sizeCount * pixel;
            uint32_t pixxelAtelesTex = sizePixxel.x * sizePixxel.y;
            int32_t rest = pixxelAtelesTex - pixxelSum;




            RY_CORE_ASSERT(pixxelAtelesTex >= pixxelSum );


            return pixxelAtelesTex >= pixxelSum ? glm::uvec4{ sizeCount.x, sizeCount.y, sizePixxel.x, sizePixxel.y } : glm::uvec4{ 0u, 0u, 0u, 0u };
        }

        inline static void CalulateTextures(uint32_t& row, uint32_t& colume, glm::uvec2& sizePixxel, uint32_t pixxel, uint32_t count, uint32_t sizeOffset)
        {
            uint32_t textures = 0;
            uint32_t dimaensionSize = std::sqrt(sizeOffset), 
                textureComume , textureRow;
            float texturedimaensionSize = 1.0f / dimaensionSize;
            bool state = textures < count;
            uint32_t minTexureSize = (sizePixxel.x / colume);
            while (state)
            {
                textureComume = colume * static_cast<float>(texturedimaensionSize);
                textureRow = row * static_cast<float>(texturedimaensionSize);
                textures = textureComume * textureRow;
                state = textures < count;
                if (state)
                {
                    colume++;
                    row--;
                    sizePixxel.x = minTexureSize * colume;
                }
            }
        
           
        }

        inline static void CalulateTex(uint32_t row, uint32_t colume, const glm::uvec2& sizeCount, std::vector<uint32_t>& offsetRow , uint32_t pixxel, uint32_t count, uint32_t sizeOffset)
        {
            float texSizeDiemension = 1.0f /  sizeOffset;
            uint32_t columeTex = colume * texSizeDiemension;
            uint32_t rowTex = row * texSizeDiemension;
            for (uint32_t iTex = 0; iTex < count; iTex++)
            {
                uint32_t x = (iTex % (columeTex));
                uint32_t y = (static_cast<float>(iTex) / rowTex) ;
                uint32_t sizeAvible = 0;
                RY_CORE_ASSERT(y * sizeOffset < sizeCount.y, "Some Miss Calultion!");

                for (uint32_t iCoume = y; iCoume < (sizeOffset * y); iCoume++)
                {
                    RY_CORE_ASSERT((offsetRow[iCoume] * x) < sizeCount.x, "Some Miss Calultion!");
                    if ((offsetRow[iCoume] * x) < sizeCount.x)
                        sizeAvible++;
                    else
                        break;
                }

                if (sizeAvible < y)
                {
                    RY_CORE_ASSERT(false);
                }

                for (uint32_t iCoume = y; iCoume < (sizeOffset + y); iCoume++)
                {
                    offsetRow[iCoume] += x;
                    RY_CORE_ASSERT(offsetRow[iCoume] < sizeCount.x, "Some Miss Calultion!")
                }


            }
        }



        inline static void OrgenizeTexturePlace()
        {

            


            uint32_t count2048 = 5;
            uint32_t count1024 = 7;
            uint32_t count512 = 10;

            uint32_t pixel2048 = CalceQuadePixelSize(count2048, 2048);      //  51.200
            uint32_t pixel1028 = CalceQuadePixelSize(count1024, 1024);      //  50.176
            uint32_t pixel512 = CalceQuadePixelSize(count512, 512);         //  51.200

            uint32_t pixxelSum = pixel2048 + pixel1028 + pixel512;          // 152.576

            if (pixxelSum == 0)
                return;
            glm::uvec4 result;

            if (pixel512 != 0)
                result = Calculate(pixxelSum, count512, 512, count512 + (4 * count1024) + (4 * 4 * count2048));
            else if (pixel1028 != 0)
                result = Calculate(pixxelSum, count1024, 1024,  count1024 + (4 * count2048));
            else if (pixel2048 != 0)
                result = Calculate(pixxelSum, count2048, 2048, count2048);
            else
                return;

            glm::uvec2 sizeCount = { result.x, result.y };
            glm::uvec2 sizePixxel = { result.z, result.w };
            uint32_t row = sizeCount.x;
            uint32_t colume = sizeCount.x;
            glm::uvec2 offsetPixxelPos = { 0.0f, 0.0f };





            

            
            CalulateTextures(row, colume, sizePixxel, 2048u, count2048, 16u);
            std::vector<uint32_t> rowOffset;
            rowOffset.resize(row);uint32_t beginNotEmpty = 0;
            uint32_t endeNotEmpty = row;
            sizeCount.x = row;
            sizeCount.y = colume;

            CalulateTex(row, colume, sizeCount, rowOffset, 2048u, count2048, 4u);

            for(auto& offset : rowOffset)
                RY_CORE_INFO("Empty Space {}", sizeCount.x - offset);
#if 0
            for (uint32_t i = 0; i < count2048;i+= 16)
            {
                glm::uvec2 posIndex = {
                    (i % colume),
                    (i / colume) 
                };
                
                float xOffset = offsetPixxelPos.x / sizePixxel.x;
                if (xOffset < 1.0f)
                {
                    offsetPixxelPos.x = 0;
                    offsetPixxelPos.y += 2048u;
                }
                RY_CORE_ASSERT(offsetPixxelPos.x <= sizePixxel.x && offsetPixxelPos.y <= sizePixxel.y);
                RY_CORE_INFO("2048px Texture: {0}, colume: {1}, row: {2} \t Position Pixxel {0, }", i, colume, row);
                
                offsetPixxelPos.x += 2048u;
            }
#endif
            
               

                
        }
#endif

        inline static glm::ivec2 CalulateAtlesSizeTexSpace(const glm::dvec2& quadeSamlestdTexSpaceSize, int countLargestTexture, int offsetLargestTextureSpaceDimension)
        {
            double trasledValue = 1.0 / static_cast<double>(offsetLargestTextureSpaceDimension);
            glm::ivec2 quadeLargestTexSpaceSize;
            glm::dvec2 quadeSamlestdTexutureSpaceSize = glm::ceil(quadeSamlestdTexSpaceSize);
            int countLargestTex;
            RY_CORE_ASSERT(!isinf(trasledValue));
            do {
                
                quadeLargestTexSpaceSize = {
                    quadeSamlestdTexutureSpaceSize.x / static_cast<double>(offsetLargestTextureSpaceDimension),
                    quadeSamlestdTexutureSpaceSize.y / static_cast<double>(offsetLargestTextureSpaceDimension)
                };
                countLargestTex = quadeLargestTexSpaceSize.x * quadeLargestTexSpaceSize.y;


                if(countLargestTex < countLargestTexture)
                {
                    quadeSamlestdTexutureSpaceSize.x++;
                    quadeSamlestdTexutureSpaceSize.y = quadeSamlestdTexutureSpaceSize.y > offsetLargestTextureSpaceDimension ? quadeSamlestdTexutureSpaceSize.y-1 : quadeSamlestdTexutureSpaceSize.y;
                }
                else
                {
                    break;
                }
            } 
            while (true);

            return quadeSamlestdTexutureSpaceSize;
        }

        inline static glm::dvec2 CalculateAtlesQuadeTexSpace(double textureCount)
        {
            // double x = std::ceil(std::sqrt(textureCount));
            // double y = std::ceil(textureCount / x);
            double x = std::sqrt(textureCount);
            double y = textureCount / x;
            return glm::dvec2(x, y);
        }

        inline static glm::ivec2 CalculateAtlesSizeTexSpace(int countTotaleSmalesTextureSpace, int countLargestTexture, int offsetLargestTextureSpaceDimension)
        {
            glm::dvec2 quadeTexSpaceSize = CalculateAtlesQuadeTexSpace(countTotaleSmalesTextureSpace);
            glm::ivec2 result = CalulateAtlesSizeTexSpace(quadeTexSpaceSize, countLargestTexture, offsetLargestTextureSpaceDimension);
            return result;
        }

        inline static void CalculateTextureAtles(LigthUniformPtr& texUniformPtr, const glm::vec2& stepSize_ST_Coord, const glm::ivec2& textureSpacePostion, const glm::ivec2& textureSpaceSize)
        {
            float step_ST_CoordX = static_cast<float>(textureSpacePostion.x) / textureSpaceSize.x;
            float step_ST_CoordY = static_cast<float>(textureSpacePostion.y) / textureSpaceSize.y;
            glm::vec2 offset_ST_Coord = { step_ST_CoordX, step_ST_CoordY };

            *texUniformPtr.ShadowTexSizePtr = glm::vec4(stepSize_ST_Coord, offset_ST_Coord);

        }

        inline static void OffsetAdding(glm::ivec2& offsetPositon, int offsetSmalestTextureSpaceDimension, const glm::ivec2& lastOffsetPositon, const glm::ivec2& lastOffsetPositon2, int lastOffsetEffectY, int lastOffsetEffectY2 , const AtlasTexture& atlasTexture)
        {
            offsetPositon.x += offsetSmalestTextureSpaceDimension;
            
            RY_CORE_ASSERT(offsetPositon.x <= atlasTexture.Size.x);
            if (offsetPositon.x == atlasTexture.Size.x)
            {
                offsetPositon.y += offsetSmalestTextureSpaceDimension;
                if (lastOffsetPositon.x != 0 && lastOffsetEffectY >= offsetPositon.y)
                    offsetPositon.x = lastOffsetPositon.x;
                else if(lastOffsetPositon2.x != 0 && lastOffsetEffectY2 >= offsetPositon.y)
                    offsetPositon.x = lastOffsetPositon2.x;
                else
                    offsetPositon.x = 0;
            }
        }

        inline static void OffsetAdding(glm::ivec2& offsetPositon, int offsetSmalestTextureSpaceDimension, const AtlasTexture& atlasTexture)
        {
            offsetPositon.x += offsetSmalestTextureSpaceDimension;

            RY_CORE_ASSERT(offsetPositon.x <= atlasTexture.Size.x);
            if (offsetPositon.x == atlasTexture.Size.x)
            {
                offsetPositon.y += offsetSmalestTextureSpaceDimension;
                offsetPositon.x = 0;
            }
        }

        inline static void OffsetAdding(glm::ivec2& offsetPositon, int offsetSmalestTextureSpaceDimension, const glm::ivec2& lastOffsetPositon, int lastOffsetEffectY, const AtlasTexture& atlasTexture)
        {
            offsetPositon.x += offsetSmalestTextureSpaceDimension;

            RY_CORE_ASSERT(offsetPositon.x <= atlasTexture.Size.x);
            if (offsetPositon.x == atlasTexture.Size.x)
            {
                offsetPositon.y += offsetSmalestTextureSpaceDimension;
                offsetPositon.x = 0;
            }
        }

        inline static glm::ivec2 CalulateAtlesTexture(std::vector<LigthUniformPtr>& texturesAtlasUniforms,  const AtlasTexture& atlasTexture, const glm::ivec2& lastOffsetPositon, const glm::ivec2& lastOffsetPositon2, int offsetSmalestTextureSpaceDimension)
        {
            int countTexture = texturesAtlasUniforms.size();
            glm::ivec2 offsetPositon = lastOffsetPositon;
            int lastTexOffsetSize = (offsetSmalestTextureSpaceDimension % 4) * 2;
            int lastTexOffsetSize2 = lastTexOffsetSize != 0 ? (offsetSmalestTextureSpaceDimension % 8) * 2 : 0;

            int lastOffsetEffectY = lastOffsetPositon.y + lastTexOffsetSize;
            int lastOffsetEffectY2 = lastOffsetPositon2.y + lastTexOffsetSize2;

            int pixxelSizeX = 512 * atlasTexture.Size.x;
            int pixxelSizeY = 512 * atlasTexture.Size.y;

            float stepSize_ST_CoordX = 1.0 / atlasTexture.Size.x;
            float stepSize_ST_CoordY = 1.0 / atlasTexture.Size.y;

            glm::vec2 stepSize_ST_Coord = { stepSize_ST_CoordX, stepSize_ST_CoordY };
            glm::ivec2 pixxelSize = { pixxelSizeX, pixxelSizeY };


            for (LigthUniformPtr texUnifromPtr : texturesAtlasUniforms)
            {
                
                // int pixxelOffsetX = 512 * offsetPositon.x;
                // int pixxelOffsetY = 512 * offsetPositon.y;
                // 
                // float offset_ST_CoordX = pixxelOffsetX / stepSize_ST_CoordX;
                // float offset_ST_CoordY = pixxelOffsetY / stepSize_ST_CoordY;
                // *texUnifromPtr = glm::vec4(stepSize_ST_CoordX, stepSize_ST_CoordY,offset_ST_CoordX, pixxelOffsetY);
                glm::ivec2 offset = {0,0};
                CalculateTextureAtles(texUnifromPtr, stepSize_ST_Coord, offsetPositon, atlasTexture.Size);
                OffsetAdding(offsetPositon, offsetSmalestTextureSpaceDimension, lastOffsetPositon, lastOffsetPositon2, lastOffsetEffectY, lastOffsetEffectY2, atlasTexture);
#if 0                 
                 RY_CORE_INFO("Tex Pos: {}, {} Size: {}", texAtlas.Position.x, texAtlas.Position.y, texAtlas.Size);
                 TextureAtlas& texAtlas = texturesAtlas.emplace_back(offsetPositon, offsetSmalestTextureSpaceDimension);
#endif
            }

            return offsetPositon;
        }

        
#if 0 
        static void OrgenizeTexturePlace()
        {
            
            AtlasTexture atlasTexture;

            uint32_t count2048 = 5;
            uint32_t count1024 = 8;
            uint32_t count512 = 11;

           

            



            RY_PROFILE_SCOPE("My Implementaion Atlas Texture");




            atlasTexture.Size = CalculateAtlesSizeTexSpace(count512 + (count1024 * 4) + (count2048 * 4 * 4), count2048 ? count2048 : count1024 ? count1024 : count512, 4);

            std::vector<TextureAtlas> texturesAtlas;
            texturesAtlas.reserve(count512 + count1024 + count2048);
            
            // RY_CORE_INFO("Atlas Texture Size: {}, {}", atlasTexture.Size.x, atlasTexture.Size.y);
            glm::ivec2 offset0 = CalulateAtlesTexture(uniformTexures2048, texturesAtlas, atlasTexture,{0,0}, { 0,0 },count2048, 4);
            glm::ivec2 offset1 = CalulateAtlesTexture(uniformTexures1024, texturesAtlas, atlasTexture, offset0, { 0,0 }, count1024, 2);
            glm::ivec2 offset2 = CalulateAtlesTexture(uniformTexures512, texturesAtlas, atlasTexture, offset0, offset1,count512, 1);
        }
#endif

        static void AtelesTextureImplentionFunktion()
        {
#if 0
           

            OrgenizeTexturePlace();
#else 
            RY_PROFILE_SCOPE("Test Implementaion Atlas Texture");
            std::initializer_list<int> texSize = {
                2048, 2048, 2048, 2048, 2048,
                1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
                512,  512,   512,  512,  512,  512,  512,  512, 512, 512, 512,
            };
            // std::vector<box> textures = allocate_texture_atlas({ 6144, 0 }, texSize);

            // OrgenizeTexturePlace();
#endif
        }
    }

#endif
   
#pragma region Uniform_Ligths_Structs

    struct AmbientData
    {
        float Color[3] = {-1,-2,-3};
        float Intensitie= -4;
        AmbientData() = default;
        AmbientData(glm::vec3& color, float intensitie)
        {
            Color[0] = color.x;
            Color[1] = color.y;
            Color[2] = color.z;
            Intensitie = intensitie;
        }
    };

    struct DrirectionData
    {
        int Aktive[4] = { 0, - 1, -2, -3 };
        Ligths::DrirectionLigtheData Drirecion[8];
    };

    struct PointData
    {
        int Aktive[4] = { 0,-2,-3,-4 };
        Ligths::PointLigtheData Point[32];
    }; 
    
    struct SpotData
    {
        int Aktive[4] = { 0,-1,-2,-3 };
        Ligths::SpotLigtheData Spot[32];
    };

    struct DrirectioShadownData
    {
        glm::mat4 ViewProjtion[8];
    };

#pragma endregion

    struct Camera3DStorage
    {
        glm::mat4 inverseViewProjetion;
        glm::vec3 WorldCenterViewFustrum;
        std::array<glm::vec4, 8> WorldViewFustrum;
        
    };
    
    struct RenderMesh {
        Ref<Mesh> MeshR;
        Ref<Material> Material;
        int entityID;
        glm::mat4 ModelMatrix;
    };

    struct RenderAABBBox
    {
        BoxAABB BoxAABB;
        glm::mat4 modelMatrix;
        int entityID;
    };

    struct RenderAABBCamaer
    {
        std::array<glm::vec4, 8> BoxFustrum;
        glm::mat4 modelMatrix;
        int entityID;
    };

    struct LigtheShadow
    {
        glm::mat4 ViewProjectionMatrixLigth[8];
        int ShadowCameraID[4];
    };

    struct SubmitSingleDrawCallData 
    {
        glm::mat4 Matrix = glm::mat4(1.0f);
        Ref<Mesh> MeshR = nullptr;
        Ref<Material> Materiel = nullptr;
        int EntityID = -1;

        SubmitSingleDrawCallData() = default;
        SubmitSingleDrawCallData(const SubmitSingleDrawCallData&) = default;
        SubmitSingleDrawCallData(SubmitSingleDrawCallData&&) = default;
    };

    struct SubmitSingleDrawCall
    {
        RenderFunc Func;
        SubmitSingleDrawCallData Data;
        SubmitSingleDrawCall() = default;
       


        SubmitSingleDrawCall(RenderFunc func, const SubmitSingleDrawCallData& Data)
            : Func(func), Data(Data)
        {
        }

        SubmitSingleDrawCall(RenderFunc func, SubmitSingleDrawCallData&& Data)
            : Func(func), Data(std::move(Data)) 
        { 
        }
        

    };

   

    struct RendererData
    {
        Camera Camera;

        CameraData CameraDataMat4;
        Camera3DStorage CameraStorage;

        CameraData LigthDirtCam;


        int RenderChacheAktive = 0;
        int RenderDefaultAktive = 0;
        int RendererDefaultModeFlags = 0;
        
        Ref<Shader> Default3DShader = nullptr;
        Ref<Texture> WhiteTexture = nullptr;

        Ref<UniformBuffer> AmbientLigthBuffer = nullptr;
        Ref<UniformBuffer> DirectionelLigthBuffer = nullptr;
        Ref<UniformBuffer> SpotLigthBuffer = nullptr;
        Ref<UniformBuffer> PointLigthBuffer = nullptr;
        glm::ivec4 AkitiveLigthsIndexes = { 0,0,0,0 };

        DrirectionData BufferDrirectionData;
        AmbientData BufferAmbientData;
        PointData BufferPointData;
        SpotData BufferSpotData;

        Ref<UniformBuffer> ShadowsViewMatriciesBuffer = nullptr;
        const uint32_t MaxLigthDrirection = 8;
        const uint32_t MaxLigthSpot = 32;
        const uint32_t MaxLigthPoint = 32;
        const uint32_t MaxLigthAmbient = 1;

        uint32_t CountLigthDrirection = 0;
        uint32_t CountLigthSpot = 0;
        uint32_t CountLigthPoint = 0;
        uint32_t CountLigthAmbient = 0;

        std::vector<LigthUniformPtr> ShadowMapTexture2048;
        std::vector<LigthUniformPtr> ShadowMapTexture1024;
        std::vector<LigthUniformPtr> ShadowMapTexture512;
        std::vector<LigthUniformPtr> ShadowMapTexture256;


        const uint32_t ShadowTextureSize = 512;
        const uint32_t MaxShadowDrirection = 4;
        const uint32_t MaxShadowSpot = 4;
        const uint32_t MaxShadowPoint = 0;
        const uint32_t MaxShadowTexturesSize = MaxShadowDrirection + MaxShadowSpot + MaxShadowPoint;
        const uint32_t MaxShadowTexturesByteSize = MaxShadowTexturesSize * sizeof(glm::mat4);



        uint32_t CountShadowDrirection = 0;
        uint32_t CountShadowSpot = 0;
        uint32_t CountShadowPoint = 0;

        uint32_t CountShadowMaps = 0;

        Ref<StorageBuffer> LigtheViewProjtionMatriciesBuffer = nullptr;
        Ref<StorageBuffer> ShadowMatriciesBuffer = nullptr;


        LigtheShadow ShadowsMatricies;
        Ref<UniformBuffer> CameraViewProjBuffer = nullptr;
        Ref<Shader> ComputeTranslationTextureSpace = nullptr;

        // AABB
        Ref<Shader> BoxAABBShader = nullptr;
        Ref<VertexArray> BoxAABBVertexArray = nullptr;
        Ref<VertexBuffer> BoxAABBVertexBuffer = nullptr;
        glm::vec3 BoxAABB[8] = {
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
        };
        
        uint32_t InstencCountAABB = 0;
#if RY_INSTECING
        HashPtrTable InstenceMesh;
#endif
        Ref<Framebuffer> ShadowFramebuffer;
        Ref<Texture> ShadowTexture;
        Ref<RendererPass> RenderPass;

        uint32_t FrameBufferLayoutIndex = 0;

        Renderer3D::Statistics Stats;

        std::vector<std::function<void()>> RenderFunktions;
        std::vector<SubmitSingleDrawCall> SubmiteRenderFunktions;

        std::vector<RenderMesh> RenderMeshSceneData;
        std::vector<RenderAABBBox> RenderAABBBoxSceneData;

        std::vector<CameraData> RenderAABBCameraSceneData;

    };

    static RendererData s_Data;

#define PRIMARY_USE_SHADOWMAP_MIN_SIZE      512
#define PRIMARY_USE_SHADOWMAP_SIZE          2048

#define PRIMARY_USE_SHADOWMAP_VALUE         (PRIMARY_USE_SHADOWMAP_SIZE / PRIMARY_USE_SHADOWMAP_MIN_SIZE)

#define PRIMARY_USE_SHADOWMAP_INTERALE(size) RY_COMBINE_MOAKRO(s_Data.ShadowMapTexture,size)
#define PRIMARY_USE_SHADOWMAP               PRIMARY_USE_SHADOWMAP_INTERALE(PRIMARY_USE_SHADOWMAP_SIZE)
#define PRIMARY_USE_SHADOWMAP_QUADE_VALUE   (PRIMARY_USE_SHADOWMAP_VALUE * PRIMARY_USE_SHADOWMAP_VALUE)


    // Layout binding slots
    // 0:  !!! Not Up to date !!!  Camera(ViewProjectionMatrix, ViewMatrix, ProjectionMatrix, CamerPosition)
    // 1:  !!! Not Up to date !!!  Model ...  (ModelMatrix, Color, Alpha, ...)
    //
    // 2:  !!! Not Up to date !!!  AmbientLigth(Color, Intensitie)
    // 3:  !!! Not Up to date !!!  DrirectionLigth(Aktive, (Color, Position, Intensitie)[8])
    // 4:  !!! Not Up to date !!!  PointLigth(Aktive, (Color, Distence, Position, Intensitie)[32])
    // 5:  !!! Not Up to date !!!  SpotLigthe(Aktive, (Color, Intensitie, Position, Distence, Direction, Inner, Outer)[32])
    


    void Renderer3D::Init()
    {
#if RY_OLD_RENDER_SYSTEM
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Renderer3D::Init");
        s_Data.RenderFunktions.reserve(100ull);
        s_Data.SubmiteRenderFunktions.reserve(100ull);
        s_Data.Default3DShader = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/Default_Rynex-Editor_Shader.glsl");

#if 0
        {
            s_Data.ComputeTranslationTextureSpace = AssetManager::GetAsset<Shader>("D:/dev/Rynex-Test-Projects/Test-Project-System/Assets/Shaders/Comput-LVP-In-ShadowMatrix.glsl");

            glm::mat4 viewProjtionMatririx = glm::mat4(1.0f);
            glm::ivec4 sizeData = { 1, -1, -2, -3 };
            s_Data.LigtheViewProjtionMatriciesBuffer = StorageBuffer::Create(&viewProjtionMatririx,sizeof(glm::mat4), StorageBuffer::Type::Dynamic);
            s_Data.ShadowMatriciesBuffer = StorageBuffer::Create(sizeof(glm::mat4), StorageBuffer::Type::Dynamic);
            Ref<UniformBuffer> sizeBuffer = UniformBuffer::Create(&sizeData, sizeof(glm::mat4), 2);
            sizeBuffer->Bind(2);
            s_Data.LigtheViewProjtionMatriciesBuffer->Bind(0);
            s_Data.ShadowMatriciesBuffer->Bind(1);
            s_Data.ComputeTranslationTextureSpace->Bind();
            RenderCommand::DispatcheCompute({ 1ul, 1ul, 1ul });
            s_Data.ComputeTranslationTextureSpace->UnBind();
            
        }
#elif 1
        Utils::AtelesTextureImplentionFunktion();
        
#endif // TODO: Implement Compute Matrix Shadow In The Shadow Pipline
        
        {
            s_Data.BoxAABBShader = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/BoxAABB.glsl");
            s_Data.BoxAABBVertexArray = VertexArray::Create();

            s_Data.BoxAABBVertexBuffer = VertexBuffer::Create(s_Data.BoxAABB, 8 * sizeof(glm::vec3), BufferDataUsage::DynamicDraw);

            s_Data.BoxAABBVertexBuffer->SetLayout({ { ShaderDataType::Float3, "a_Postion" } });

            uint32_t incies[] = {
                0, 1,
                1, 2,
                2, 3,
                3, 0,
                0, 4,
                4, 5,
                5, 6,
                6, 7,
                7, 4,
                7, 3,
                6, 2,
                5, 1
            };

            Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(incies, 24, BufferDataUsage::StaticDraw);
            s_Data.BoxAABBVertexArray->AddVertexBuffer(s_Data.BoxAABBVertexBuffer);
            s_Data.BoxAABBVertexArray->SetIndexBuffer(indexBuffer);
            s_Data.BoxAABBVertexArray->SetPrimitv(VertexArray::Primitv::Line);
        }

        {
            s_Data.CameraViewProjBuffer = UniformBuffer::Create(&s_Data.CameraDataMat4, sizeof(CameraData),
                {
                     { ShaderDataType::Float4x4, "ViewProjectionMatrix" },
                     { ShaderDataType::Float4x4, "ViewMatrix" },
                     { ShaderDataType::Float4x4, "ProjectionMatrix" },
                     { ShaderDataType::Float3,   "CamerPosition" },
                     { ShaderDataType::Int,      "Empty"}
                }, 0);


        }

        {
            std::vector<BufferElement> element;
            uint32_t count = s_Data.CountShadowMaps;
            element.reserve(1ull + count);
            for (uint32_t i = 0; i < count; i++)
                element.emplace_back(ShaderDataType::Float4x4, "ViewProjectionMatrix", i);

            element.emplace_back(ShaderDataType::Int4, "ShadowCameraID");

            
            s_Data.ShadowsViewMatriciesBuffer = UniformBuffer::Create(&s_Data.ShadowsMatricies, sizeof(s_Data.ShadowsMatricies), element, 0);
        }

        {
            s_Data.AmbientLigthBuffer = UniformBuffer::Create(&s_Data.BufferAmbientData, sizeof(AmbientData),
                {
                    { ShaderDataType::Float3, "Color" },
                    { ShaderDataType::Float, "Intensitie" },
                }, 2);
        }

        {
            std::vector<BufferElement> element;
            element.reserve((8ull * 3ull) + 1ull);
            element.emplace_back(ShaderDataType::Int4, "Aktiv");
            for (uint32_t i = 0; i < 8; i++)
            {
                element.emplace_back(ShaderDataType::Float4x4, "ShadowMatrix", i);

                element.emplace_back(ShaderDataType::Float4, "ShadowMapPos", i);

                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);

                element.emplace_back(ShaderDataType::Float3, "Dirction", i);
                element.emplace_back(ShaderDataType::Float, "ShadowMapSize", i);
            }


            s_Data.DirectionelLigthBuffer = UniformBuffer::Create(&s_Data.BufferDrirectionData, sizeof(DrirectionData), element, 3);
        }

        {
            std::vector<BufferElement> element;
            element.reserve((32ull * 4ull) + 1ull);
            element.emplace_back(ShaderDataType::Int4, "Aktiv");
            for (uint32_t i = 0; i < 32; i++)
            {
                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float, "Distence", i);
                element.emplace_back(ShaderDataType::Float3, "Postion", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
            }


            s_Data.PointLigthBuffer = UniformBuffer::Create(&s_Data.BufferPointData, sizeof(PointData), element, 4);
        }

        {
            std::vector<BufferElement> element;
            element.reserve((32ull * 6ull) + 1ull);
            // element.resize((32 * 6));
            element.emplace_back(ShaderDataType::Int4, "Aktiv");
            for (uint32_t i = 0; i < 32; i++)
            {
                element.emplace_back(ShaderDataType::Float4x4, "ShadowMatrix", i);

                element.emplace_back(ShaderDataType::Float4, "ShadowMapPos", i);

                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);

                
                element.emplace_back(ShaderDataType::Float3, "Postion", i);
                element.emplace_back(ShaderDataType::Float, "Distence", i);

                element.emplace_back(ShaderDataType::Float3, "Direction", i);
                element.emplace_back(ShaderDataType::Float, "Inner", i);

                element.emplace_back(ShaderDataType::Float, "Outer", i);
                element.emplace_back(ShaderDataType::Float3, "Empty", i);
            }

            s_Data.SpotLigthBuffer = UniformBuffer::Create(&s_Data.BufferSpotData, sizeof(SpotData), element, 5);
        }

        {
            s_Data.ShadowFramebuffer = Framebuffer::Create({
                PRIMARY_USE_SHADOWMAP_MIN_SIZE, PRIMARY_USE_SHADOWMAP_MIN_SIZE,
                {
                    { TexFrom::DepthComp24, 1, { TexWarp::ClampEdge, TexWarp::ClampEdge, TexWarp::ClampEdge }, TexFilter::Linear, TexComp::Lequal }
                }
            });
            s_Data.ShadowTexture = s_Data.ShadowFramebuffer->GetDepthTexture();
        }

#if RY_INSTECING
        {
            s_Data.InstenceMesh = HashPtrTable(512);
        }
#endif
        s_Data.WhiteTexture = Texture::Default();



        {
            // Get Shadow maps and counter in Stats for posible Displaying

            s_Data.Stats.ShadowsTex = s_Data.ShadowTexture;
        }
#else
#endif
    }

    void Renderer3D::Shutdown()
    {
#if RY_OLD_RENDER_SYSTEM
        RY_CORE_INFO("Renderer3D::Shutdown");
        s_Data.AmbientLigthBuffer.reset();
        s_Data.BoxAABBShader.reset();
        s_Data.BoxAABBVertexArray.reset();
        s_Data.BoxAABBVertexBuffer.reset();
        s_Data.CameraViewProjBuffer.reset();
        s_Data.Default3DShader.reset();
        s_Data.DirectionelLigthBuffer.reset();
        s_Data.PointLigthBuffer.reset();
       
        s_Data.SpotLigthBuffer.reset();
        s_Data.WhiteTexture.reset();
        s_Data.RenderAABBBoxSceneData.clear();
        s_Data.RenderAABBBoxSceneData.shrink_to_fit();

        s_Data.RenderAABBCameraSceneData.clear();
        s_Data.RenderAABBCameraSceneData.shrink_to_fit();

#else
#endif

    }

#pragma region UniformLigths

    void Renderer3D::SetLigthUniform(AmbientLigthComponent* ambient)
    {

        if (s_Data.CountLigthAmbient >= s_Data.MaxLigthAmbient || ambient == nullptr)
        {
            AmbientData ambientData(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);
            s_Data.BufferAmbientData = ambientData;
        }
        else
        {
            AmbientData ambientData(ambient->Color, ambient->Intensitie);
            s_Data.BufferAmbientData = ambientData;
        }
            
        // s_Data.BufferDrirectionData.CameraPositionGlobel[0] = s_Data.CameraPos.x;
        // s_Data.BufferDrirectionData.CameraPositionGlobel[1] = s_Data.CameraPos.y;
        // s_Data.BufferDrirectionData.CameraPositionGlobel[2] = s_Data.CameraPos.z;

       

        
    }

    void Renderer3D::SetLigthUniform(DrirektionleLigthComponent& drirektion, const glm::mat4& matrix, const glm::mat4& viewProjetionShadow)
    {
        if (s_Data.CountLigthDrirection < s_Data.MaxLigthDrirection)
        {
            Ligths::DrirectionLigtheData drirectionLigtheData(drirektion.Color, drirektion.Intensitie, matrix, viewProjetionShadow, s_Data.ShadowTextureSize   );
            s_Data.BufferDrirectionData.Drirecion[s_Data.CountLigthDrirection] = drirectionLigtheData;
            s_Data.CountLigthDrirection++;
        }
    }

    void Renderer3D::SetLigthUniform(SpotLigthComponent& spot, const glm::mat4& matrix, const glm::mat4& viewProjetionShadow)
    {
        if (s_Data.CountLigthSpot < s_Data.MaxLigthSpot)
        {
            Ligths::SpotLigtheData spotLigtheData(spot.Color, glm::vec3(matrix[3]), glm::vec3(matrix[2]), spot.Intensitie, spot.Distence, spot.Inner, spot.Outer, viewProjetionShadow);
            s_Data.BufferSpotData.Spot[s_Data.CountLigthSpot] = spotLigtheData;
            s_Data.CountLigthSpot++;
        }
    }

    void Renderer3D::SetLigthUniform(PointLigthComponent& point, const glm::mat4& matrix)
    {
        if (s_Data.CountLigthPoint < s_Data.MaxLigthPoint )
        {
            Ligths::PointLigtheData pointLigtheData(point.Color, glm::vec3(matrix[3]), point.Intensitie, point.Distence);
            s_Data.BufferPointData.Point[s_Data.CountLigthPoint] = pointLigtheData;
            s_Data.CountLigthPoint++;
        }
    }

    void Renderer3D::SetShadowsUniform()
    {       
        s_Data.BufferSpotData.Aktive[0] = s_Data.CountLigthSpot;
        s_Data.SpotLigthBuffer->SetData(&s_Data.BufferSpotData, 0, sizeof(int) * 4 + sizeof(Ligths::SpotLigtheData) * s_Data.CountLigthSpot);

        s_Data.BufferPointData.Aktive[0] = s_Data.CountLigthPoint;
        s_Data.PointLigthBuffer->SetData(&s_Data.BufferPointData, 0, sizeof(int) * 4 + sizeof(Ligths::PointLigtheData) * s_Data.CountLigthPoint);

        s_Data.BufferDrirectionData.Aktive[0] = s_Data.CountLigthDrirection;
        s_Data.DirectionelLigthBuffer->SetData(&s_Data.BufferDrirectionData, 0, sizeof(int) * 4 + sizeof(Ligths::DrirectionLigtheData) * s_Data.CountLigthDrirection);

        s_Data.BufferAmbientData.Intensitie = s_Data.CountLigthDrirection != 0  ? s_Data.BufferAmbientData.Intensitie : 0.0f;
        s_Data.AmbientLigthBuffer->SetData(&s_Data.BufferAmbientData, 0, sizeof(AmbientData));

        s_Data.AmbientLigthBuffer->Bind(2);
        s_Data.DirectionelLigthBuffer->Bind(3);
        s_Data.SpotLigthBuffer->Bind(4);
        s_Data.PointLigthBuffer->Bind(5);

        s_Data.ShadowMapTexture2048.clear();
        s_Data.ShadowMapTexture1024.clear();
        s_Data.ShadowMapTexture512.clear();
        s_Data.ShadowMapTexture256.clear();
    }

    glm::mat4 Renderer3D::CalculateShadowDirectionelMatrix(const glm::mat4& view, const glm::vec3& min, const glm::vec3& max)
    {
       
       glm::mat4 ProjectionMatrix = glm::ortho(
           min.x, max.x, min.y, max.y, -min.z , -max.z
       );
       return ProjectionMatrix * view;
    }
   
#pragma endregion


#pragma region Frame_Start_Ende

    void Renderer3D::BeginFrame()
    {   
      
        s_Data.InstencCountAABB = 0;
    }

    void Renderer3D::EndFrame()
    {
     
    }

#pragma endregion
      
#pragma region RenderComponetFunktions

    void Renderer3D::RenderComponet(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID, RenderFunc func)
    {
        for (auto& mesh : model.MeshD)
        {
            func(modelMatrix, mesh.MeshR, mesh.Materiel ? mesh.Materiel : mesh.MeshR->GetMateriel(), entityID);
        }
    }

    void Renderer3D::RenderComponet(const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID, RenderFunc func)
    {
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for (const auto& mesh : node.Meshes)
            {
                func(modelMatrix * node.Matrix, mesh, mesh->GetMateriel(), entityID);
            }
        }
    }

    void Renderer3D::SubmitComponet(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID, RenderFunc func)
    {
        for (const MeshDynamic& meshNode : model.MeshD)
        {
            const Ref<Mesh>& mesh = meshNode.MeshR;
            const Ref<Material>& materiel = meshNode.Materiel ? meshNode.Materiel : mesh->GetMateriel();
            
            s_Data.RenderFunktions.emplace_back([useFunction = func, useMateriel = materiel, useMesh = mesh, mat4 = modelMatrix, enttID = entityID]() {
                useFunction(mat4, useMesh, useMateriel, enttID);
            });
            // RenderFunc check = SubmiteDrawMeshMeshShadow;
            // if (func.target_type() == check.target_type())
            // {
            //     materiel->SetMatrix(modelMatrix);
            // }
            // s_Data.SubmiteRenderFunktions.emplace_back(func, SubmitSingleDrawCallData{ modelMatrix, mesh, materiel, entityID });
        }
    }

    void Renderer3D::SubmitComponet(const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID, RenderFunc func)
    {
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for (const Ref<Mesh>& mesh : node.Meshes)
            {
                
                const Ref<Material>& materiel = mesh->GetMateriel();

                glm::mat4 matrix = modelMatrix * node.Matrix;
                s_Data.RenderFunktions.emplace_back([useFunction = func, useMateriel = materiel, useMesh = mesh, mat4 = matrix, enttID = entityID]() {
                    useFunction(mat4, useMesh, useMateriel, enttID);
                });


                // RenderFunc check = SubmiteDrawMeshMeshShadow;
                // if (func.target_type() == check.target_type())
                // {
                //     materiel->SetMatrix(modelMatrix);
                // }
                // s_Data.SubmiteRenderFunktions.emplace_back(func, SubmitSingleDrawCallData{ modelMatrix, mesh, materiel, entityID });
            }
        }
    }

    void Renderer3D::RenderComponet(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, const Ref<Material>& materiel, int entityID, RenderFunc func)
    {
        for (auto& mesh : model.MeshD)
        {
            func(modelMatrix, mesh.MeshR, materiel, entityID);
        }
    }

    void Renderer3D::RenderComponet(const glm::mat4& modelMatrix, const StaticMeshComponent& model, const Ref<Material>& materiel, int entityID, RenderFunc func)
    {
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for (const auto& mesh : node.Meshes)
            {
                func(modelMatrix * node.Matrix, mesh, materiel, entityID);
            }
        }
    }

#pragma endregion


#pragma region SceneShadows

    void Renderer3D::BeginSceneShadow()
    {
        // s_Data.RenderChacheAktive = RenderCommand::GetMode();
        
        
        // RenderCommand::SetMode( Renderer::Mode::Death_Buffer | Renderer::Mode::CallFace_Back | Renderer::Mode::A_Buffer );


    }
    

    
    void Renderer3D::EndSceneShadow()
    {
        
#if 1
        uint32_t count = s_Data.CountShadowMaps;

        if (count == 0)
            return;

        Utils::AtlasTexture atlasTexture;
        atlasTexture.Size = Utils::CalculateAtlesSizeTexSpace(
            PRIMARY_USE_SHADOWMAP.size() * PRIMARY_USE_SHADOWMAP_QUADE_VALUE, 
            PRIMARY_USE_SHADOWMAP.size(), 
            PRIMARY_USE_SHADOWMAP_VALUE);

        glm::ivec2 sizeAtlesTexturePixxel = atlasTexture.Size * PRIMARY_USE_SHADOWMAP_MIN_SIZE;

        const glm::uvec2& sizeTex = s_Data.ShadowFramebuffer->GetFrambufferSize();
        glm::ivec2 sizeiTex = sizeTex;
        if (sizeiTex != sizeAtlesTexturePixxel)
        {
            s_Data.ShadowFramebuffer->Resize(sizeAtlesTexturePixxel.x, sizeAtlesTexturePixxel.y);
        }
        



        glm::ivec2 atlasTextureSize = atlasTexture.Size;
        glm::ivec2 atlasTexturePixxelSize = atlasTexture.Size * PRIMARY_USE_SHADOWMAP_MIN_SIZE;
        glm::dvec2 countTexture = atlasTexturePixxelSize / PRIMARY_USE_SHADOWMAP_SIZE;
        glm::vec2 stepSize_ST_Coord = 1.0 / countTexture;
        glm::ivec2 offsetTextures = { 0, 0 };
        int index = 0;

        s_Data.ShadowFramebuffer->Bind();
        RenderCommand::ClearDepth();        
        
        s_Data.ShadowsViewMatriciesBuffer->SetData(&s_Data.ShadowsMatricies, 0, sizeof(s_Data.ShadowsMatricies));
        s_Data.ShadowsViewMatriciesBuffer->Bind(8);
        SpotData& spotData = s_Data.BufferSpotData;
        for (LigthUniformPtr& texPtr : PRIMARY_USE_SHADOWMAP)
        {
            Utils::CalculateTextureAtles(texPtr, stepSize_ST_Coord, offsetTextures, atlasTexture.Size);
            s_Data.ShadowsViewMatriciesBuffer->SetData(&index, sizeof(s_Data.ShadowsMatricies.ViewProjectionMatrixLigth), sizeof(int));
            s_Data.ShadowsViewMatriciesBuffer->Bind(8);
            
            RenderCommand::SetViewPort(offsetTextures.x * PRIMARY_USE_SHADOWMAP_MIN_SIZE, offsetTextures.y * PRIMARY_USE_SHADOWMAP_MIN_SIZE, PRIMARY_USE_SHADOWMAP_SIZE, PRIMARY_USE_SHADOWMAP_SIZE);
            for (const auto& func : s_Data.RenderFunktions)
            {
                func();
            }
            Utils::OffsetAdding(offsetTextures, PRIMARY_USE_SHADOWMAP_VALUE, atlasTexture);
            index++;
        }
#else
        uint32_t count = s_Data.CountShadowDrirection
            + s_Data.CountShadowSpot
            + s_Data.CountShadowPoint;


        if (count == 0)
            return;

        const glm::uvec2& sizeTex = s_Data.ShadowFramebuffer->GetFrambufferSize();
        uint32_t shadowSize = s_Data.ShadowTextureSize;
        uint32_t caleCount = (sizeTex.x/ shadowSize);
        if (caleCount != count)
        {
            s_Data.ShadowFramebuffer->Resize(shadowSize * count, shadowSize);
        }

        s_Data.ShadowFramebuffer->Bind();
        RenderCommand::ClearDepth();

        s_Data.ShadowsViewMatriciesBuffer->SetData(&s_Data.ShadowsMatricies, 0, sizeof(s_Data.ShadowsMatricies));
        s_Data.ShadowsViewMatriciesBuffer->Bind(8);

        for (int i = 0; i < count; i++)
        {
            RenderCommand::SetViewPort(i * shadowSize, 0, shadowSize, shadowSize);
            
            s_Data.ShadowsViewMatriciesBuffer->SetData(&i, sizeof(s_Data.ShadowsMatricies.ViewProjectionMatrixLigth), sizeof(int));
            s_Data.ShadowsViewMatriciesBuffer->Bind(8);

            for (const auto& func : s_Data.RenderFunktions)
            {
                func();
            }

        }
#endif

        s_Data.RenderFunktions.clear();

        s_Data.ShadowFramebuffer->Unbind();
        s_Data.CameraViewProjBuffer->Bind(0);
        s_Data.ShadowTexture->Bind(7);

        s_Data.CountShadowMaps = 0;

        s_Data.CountShadowDrirection = 0;
        s_Data.CountShadowPoint = 0;
        s_Data.CountShadowSpot = 0;

        s_Data.ShadowMapTexture2048.clear();
        s_Data.ShadowMapTexture1024.clear();
        s_Data.ShadowMapTexture512.clear();
        s_Data.ShadowMapTexture256.clear();
    }

    void Renderer3D::AddShadowMapSpotlLigth(const glm::mat4& viewProjtion)
    {
        if (s_Data.CountShadowSpot >= s_Data.MaxShadowSpot || s_Data.CountShadowMaps >= s_Data.MaxShadowTexturesSize)
            return;
        uint32_t index = s_Data.CountLigthSpot;
        glm::vec4* shadowMapUniformDataSizePtr = &s_Data.BufferSpotData.Spot[index].ShadowMapPos;
        glm::mat4* shadowMapUniformDataMatrixPtr = &s_Data.BufferSpotData.Spot[index].ShadowMatrix;
        
        LigthUniformPtr ligthsUniformPtr = { shadowMapUniformDataSizePtr, shadowMapUniformDataMatrixPtr };
        PRIMARY_USE_SHADOWMAP.emplace_back(ligthsUniformPtr);

        s_Data.ShadowsMatricies.ViewProjectionMatrixLigth[s_Data.CountShadowMaps] = viewProjtion;
        s_Data.CountShadowSpot++;
        s_Data.CountShadowMaps++;
    }

    void Renderer3D::AddShadowMapDirectionelLigth(const glm::mat4& viewProjtion)
    {
        if (s_Data.CountShadowDrirection >= s_Data.MaxLigthDrirection || s_Data.CountShadowMaps >= s_Data.MaxShadowTexturesSize)
            return;

        glm::vec4* shadowMapUniformDataSizePtr = &s_Data.BufferDrirectionData.Drirecion[s_Data.CountShadowDrirection].ShadowMapPos;
        glm::mat4* shadowMapUniformDataMatrixPtr = &s_Data.BufferDrirectionData.Drirecion[s_Data.CountShadowDrirection].ShadowMatrix;

        LigthUniformPtr ligthsUniformPtr = { shadowMapUniformDataSizePtr, shadowMapUniformDataMatrixPtr };

        PRIMARY_USE_SHADOWMAP.emplace_back(ligthsUniformPtr);
        s_Data.ShadowsMatricies.ViewProjectionMatrixLigth[s_Data.CountShadowMaps] = viewProjtion;
        s_Data.CountShadowDrirection++;
        s_Data.CountShadowMaps++;
    }

#pragma endregion

#pragma region SceneRender

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& view)
    {
        s_Data.Camera = camera;
        s_Data.CameraDataMat4.ViewProjectionMatrix = camera.GetProjektion() * view;
        s_Data.CameraDataMat4.ViewMatrix = view;
        s_Data.CameraDataMat4.ProjectionMatrix = camera.GetProjektion();
        glm::vec3 camerPos = glm::vec3(glm::transpose(view)[3]);
        s_Data.CameraDataMat4.Position = camerPos;

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraDataMat4, sizeof(CameraData));
        s_Data.CameraViewProjBuffer->Bind(0);

        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();
    }

    void Renderer3D::EndScene()
    {
#if RY_INSTECING  
        for ( auto&[mesh, matrix, count] : s_Data.InstenceMesh)
        {
            if(Ref<Mesh> meshRef = mesh.lock())
                DrawModdelMesh(meshRef, matrix, count);
            count.clear();
            matrix.clear();
        }
#endif





        s_Data.Stats.EndTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.Stats.DrawTime.emplace_back(
            std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.EndTimePoint).time_since_epoch().count()
            - std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.StartTimePoint).time_since_epoch().count()
        );
        s_Data.RenderFunktions.clear();
        s_Data.CountLigthDrirection = 0;
        s_Data.CountLigthSpot = 0;
        s_Data.CountLigthPoint = 0;
        s_Data.CountLigthAmbient = 0;

        s_Data.CountShadowDrirection = 0;
        s_Data.CountShadowSpot = 0;
        s_Data.CountShadowPoint = 0;
        Renderer::SetMode(s_Data.RendererDefaultModeFlags);
        s_Data.RenderMeshSceneData.clear();
    }

#pragma endregion

#pragma region FrameBufferScene

#pragma endregion



#pragma region DrawSingleMeshFunktions

    void Renderer3D::DrawMesh3D(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID)
    {    
        if (!mesh->IsRady()||!materiel->IsRady())
            return;
        const Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        materiel->SetMatrix(modelMatrix);
        materiel->Bind(&entityID, 1);
        
        DrawObjectRender3D(vertexArray);

        vertexArray->UnBind();
        materiel->UnBind();
    }    
         
    void Renderer3D::DrawMeshFrambuffer(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID)
    {    
        if (!mesh->IsRady() ||! materiel->IsRady())
            return;

        const Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        materiel->SetMatrix(modelMatrix);
        materiel->BindFrameBuffer(s_Data.FrameBufferLayoutIndex, &entityID, 1);

        DrawObjectRender3D(vertexArray);

        vertexArray->UnBind();
        materiel->UnBind();
    }    
         
    void Renderer3D::DrawMeshMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID)
    {
        if (!mesh->IsRady() || !materiel->IsRady())
            return;
        const Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        materiel->SetMatrix(modelMatrix);
        materiel->BindShadow();

        DrawObjectRender3D(vertexArray);

        vertexArray->UnBind();
        materiel->UnBind();
    }

    void Renderer3D::SubmiteDrawMeshMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID)
    {
        if (!mesh->IsRady() || !materiel->IsRady())
            return;
        const Ref<VertexArray> vertexArray = mesh->GetVertexArray();
        materiel->BindShadow();

        DrawObjectRender3D(vertexArray);

        vertexArray->UnBind();
        materiel->UnBind();
    }

#pragma endregion

 
#if 0
    void Renderer3D::DrawModdelMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID)
    {
        const Ref<Material> material = mesh->GetMateriel();
        DrawModdelMeshShadow(modelMatrix, mesh, material, entityID);
    }


    void Renderer3D::DrawModdelMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& material, int entityID)
    {
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        // const std::vector<MeshTexture>& textures = mesh->GetTextures();

        if (material == nullptr)
        {
            RY_CORE_FATAL("Some Shader don't exist!");
            return;
        }

        material->SetMatrix(modelMatrix);
        
        material->BindShadow();

        RenderCommand::DrawIndexedMeshInstecing(s_Data.CountShadowDrirection, vertexArray);
        s_Data.Stats.DrawCalls++;
    }


    void Renderer3D::DrawModdelMeshShadow(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID)
    {
        for (auto& mesh : model.MeshD)
        {
            if (mesh.MeshR->IsRady())
                DrawModdelMesh(modelMatrix, mesh.MeshR, entityID);
        }
    }

    void Renderer3D::DrawModdelShadow(const glm::mat4& modelMatrix, const std::vector<Ref<Mesh>>& models, int entityID)
    {
       
        for (const Ref<Mesh>& mesh : models)
        {
            DrawModdelMeshShadow(modelMatrix, mesh, entityID);
        }
    }

    void Renderer3D::DrawModdelShadow(const glm::mat4& modelMatrix, const std::vector<NodeData>& nodes, int entityID)
    {

        for (const auto& node : nodes)
        {
            for(const auto& mesh : node.Meshes)
            {
                if(mesh->IsRady())
                    DrawModdelMeshShadow(modelMatrix * node.Matrix, mesh, entityID);
            }
        }
    }

    void Renderer3D::BeginScene(const Ref<EditorCamera>& camera)
    {
        s_Data.CameraDataMat4.ViewProjectionMatrix = camera->GetViewProjection();
        s_Data.CameraDataMat4.ViewMatrix = camera->GetViewMatrix();
        s_Data.CameraDataMat4.ProjectionMatrix = camera->GetProjektion();
        s_Data.CameraDataMat4.Position = camera->GetPosition();
        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraDataMat4, sizeof(CameraData));
        s_Data.CameraViewProjBuffer->Bind(0);
    }

    void Renderer3D::SetRenderCamera(const Camera& camera, const glm::mat4& view, const glm::vec4& centerViewFustrum)
    {
        s_Data.Camera = camera;
        s_Data.CameraDataMat4.ViewProjectionMatrix = camera.GetProjektion() * view;
        s_Data.CameraDataMat4.ViewMatrix = view;
        s_Data.CameraDataMat4.ProjectionMatrix = camera.GetProjektion();
        glm::vec3 camerPos = glm::transpose(view)[3];
        s_Data.CameraDataMat4.Position = camerPos;

        s_Data.CameraStorage.inverseViewProjetion = glm::inverse(s_Data.CameraDataMat4.ViewProjectionMatrix);
        s_Data.CameraStorage.WorldViewFustrum = SceneCamera::GetViewProjetionFustrumWorld(s_Data.CameraStorage.inverseViewProjetion);
        s_Data.CameraStorage.WorldCenterViewFustrum = centerViewFustrum;

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraDataMat4, sizeof(CameraData));
        s_Data.CameraViewProjBuffer->Bind(0);
    }

    void Renderer3D::BeginScene()
    {  
        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();
        s_Data.CameraViewProjBuffer->Bind(0);
    }

    void Renderer3D::BindDefault3DShader(const Ref<Texture>& bindeTexure, const glm::mat4& modelMatrix, int entityID, const glm::vec3& objectColor)
    {
        Ref<Shader>& shader = s_Data.Default3DShader;
        if (shader == nullptr)
            return;

        Renderer::SetMode(s_Data.RendererDefaultModeFlags);
        shader->Bind();
        shader->SetInt("u_EntityID", entityID);
        shader->SetMat4("u_Model", modelMatrix);
        shader->SetMat4("u_ViewProj", s_Data.CameraDataMat4.ViewProjectionMatrix);
        shader->SetFloat3("u_Color", objectColor);
        shader->SetFloat3("u_CamerPos", s_Data.CameraDataMat4.Position);
        shader->SetInt("u_Texture_Diffuse1", 0);
        if (bindeTexure != nullptr)
            bindeTexure->Bind(0);
    }

    void Renderer3D::BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model, int entityID)
    {

        if (material.Materiel == nullptr)
        {
            BindDefault3DShader(Texture::Default(), model, entityID, material.Materiel->GetColor());
            return;
        }
        material.Materiel->SetMatrix(model);
        material.Materiel->Bind(&entityID, 1);

    } 
    
    void Renderer3D::AfterDrawEntity(const MaterialComponent& material)
    {
        RY_PROFILE_FUNCTION();
        material.Materiel->UnBind();
    }

    void Renderer3D::SetMaterial(const MaterialComponent& material, const glm::mat4& modelMatrix, int entityID)
    {
        material.Materiel->SetMatrix(modelMatrix);
        material.Materiel->Bind(&entityID, 1);
    }

    void Renderer3D::DrawModdel(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID, RenderFunc func)
    {
        for (auto& mesh : model.MeshD)
        {
            if (mesh.MeshR->IsRady())
                func(modelMatrix, mesh.MeshR, mesh.Materiel ? mesh.Materiel : mesh.MeshR->GetMateriel(), entityID);
        }
    }

    void Renderer3D::DrawModdel(const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID, RenderFunc func)
    {
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for (const auto& mesh : node.Meshes)
            {
                if (mesh->IsRady())
                    func(modelMatrix * node.Matrix, mesh, mesh->GetMateriel(), entityID);
            }
        }
    }

    void Renderer3D::DrawModdel(const glm::mat4& modelMatrix, const StaticMeshComponent& model, const Ref<Material>& materiel, int entityID, RenderFunc func)
    {
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for (const auto& mesh : node.Meshes)
            {
                if (mesh->IsRady())
                    func(modelMatrix * node.Matrix, mesh, materiel, entityID);
            }
        }
    }

    void Renderer3D::DrawModdel(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID)
    {
#if RY_MODEL_NODE
        for (auto& mesh : model.MeshD)
        {
            if (mesh.MeshR->IsRady())
                DrawModdelMesh(modelMatrix, mesh.MeshR, entityID);
        }

#else
        if (model.MeshR->IsRady())
            DrawModdelMesh(modelMatrix, model.MeshR, entityID);

#endif
    }    
    
    void Renderer3D::DrawModdel(const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID)
    {
#if 0
        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
        const std::vector<glm::mat4>& globels = model.GlobleMeshMatrix;
        for (uint32_t i = 0, length = globels.size(); i < length; i++)
        {
            if (meshes[i]->IsRady())
                Renderer3D::DrawModdelMesh(globels[i], meshes[i], entityID);
        }
#endif
#if RY_MODEL_NODE
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for(const auto& mesh : node.Meshes)
            {
                if (mesh->IsRady())
                    DrawModdelMesh(modelMatrix * node.Matrix, mesh, entityID);
            }
        }
#else
        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
        for (const Ref<Mesh>& mesh : meshes)
        {
#if RY_INSTECING

            MeshInstences& meshInst = s_Data.InstenceMesh.GetItem(mesh);
            meshInst.Mesh = mesh;
            meshInst.EntityIDs.emplace_back(entityID);
            meshInst.ModelMatrix.emplace_back(modelMatrix);
#else
            if (mesh->IsRady())
                DrawModdelMesh(modelMatrix, mesh, entityID);
#endif
        }
#endif
        

    }

    void Renderer3D::DrawModdel(const glm::mat4& modelMatrix, const StaticMeshComponent& model, const Ref<Material>& materiel, int entityID)
    {
#if 0
        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
        const std::vector<glm::mat4>& globels = model.GlobleMeshMatrix;
        for (uint32_t i = 0, length = globels.size(); i < length; i++)
        {
            if (meshes[i]->IsRady())
                Renderer3D::DrawModdelMesh(globels[i], meshes[i], entityID);
        }
#endif

#if RY_MODEL_NODE
        const auto& nodeRoods = model.ModelR->GetNodes();
        for (const auto& node : nodeRoods)
        {
            for (const auto& mesh : node.Meshes)
            {
                if (mesh->IsRady())
                    DrawModdelMesh(modelMatrix * node.Matrix, mesh, materiel,  entityID);
            }
        }
#else
        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
        for (const Ref<Mesh>& mesh : meshes)
        {
#if RY_INSTECING

            MeshInstences& meshInst = s_Data.InstenceMesh.GetItem(mesh);
            meshInst.Mesh = mesh;
            meshInst.EntityIDs.emplace_back(entityID);
            meshInst.ModelMatrix.emplace_back(modelMatrix);
#else
            if (mesh->IsRady())
                DrawModdelMesh(modelMatrix, mesh, entityID);
#endif
        }
#endif

    }


    void Renderer3D::DrawModdelSeclection(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID)
    {
        if (!mesh->IsRady())
            return;

     
        const Ref<Shader>& shader = materiel->GetShader();
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        if (shader == nullptr || materiel == nullptr)
        {
            RY_CORE_FATAL("Some Shader don't exist!");
            return;
        }
        materiel->SetMatrix(modelMatrix);
        materiel->Bind(&entityID, 1);


        RenderCommand::DrawIndexedMesh(vertexArray);
        s_Data.Stats.DrawCalls++;
        vertexArray->UnBind();
        shader->UnBind();
    }

    // Experimantle Funktion Instecing
    void Renderer3D::DrawModdelMesh(const Ref<Mesh>& mesh, std::vector<glm::mat4>& modelMatricies, std::vector<int>& entityIDs)
    {
        uint32_t countInstence = modelMatricies.size();
        if (countInstence != entityIDs.size())
            return;
        
        s_Data.Stats.DrawCalls++;
        
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        const std::vector<MeshTexture>& textures = mesh->GetTextures();

        Ref<Shader> shader = nullptr;
        
        RY_CORE_ASSERT(shader != nullptr);

        {
            shader->Bind();
            shader->SetIntArray("u_EntityID", (int32_t*)entityIDs.data(), entityIDs.size());
            shader->SetMat4Array("u_ModelMatrix", (float*)modelMatricies.data(), modelMatricies.size());

        }

        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        std::string number = "";
        for (uint32_t i = 0; i < textures.size(); i++)
        {
            const MeshTexture& tex = textures[i];
            const std::string& name = tex.Type;
            if (name == "u_Texture_Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "u_Texture_Speculare")
                number = std::to_string(specularNr++);


            shader->SetInt(("material." + name + number).c_str(), i);
            if (tex.TextureResur != nullptr)
                tex.TextureResur->Bind(i);
            else
                s_Data.WhiteTexture->Bind(i);
        }

        RenderCommand::DrawIndexedMeshInstecing(countInstence, vertexArray);
        vertexArray->UnBind();
        shader->UnBind();
    }

  


    // Force to make this Funktion default for all Normle Object Draw-Calls
    void Renderer3D::DrawModdelMesh(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID)
    {

        const Ref<Material> material = mesh->GetMateriel();
        DrawModdelMesh(modelMatrix, mesh, material, entityID);

    }
    
    void Renderer3D::DrawModdelMesh(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& material, int entityID)
    { 
        const Ref<Shader>& shader = material->GetShader();
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        // const std::vector<MeshTexture>& textures = mesh->GetTextures();

        if (shader == nullptr || material == nullptr)
        {
            RY_CORE_FATAL("Some Shader don't exist!");
            return;
        }
#if 0
        Renderer::SetMode(material.AlgorithmFlags);
        const std::vector<UniformElement>& unifomrsLayoute = material.UniformLayoute;

        shader->Bind();

        for (const UniformElement& uniform : unifomrsLayoute)
        {
            if (uniform.GloblelResurce && uniform.UUID == 0)
            {
                switch (uniform.ShResourceType)
                {
                case ShaderResourceType::LocalModel:
                {
                    shader->SetMat4(uniform.Name, modelMatrix);
                    break;
                }
                case ShaderResourceType::LocalColor:
                {
                    shader->SetFloat3(uniform.Name, material.Color);
                    break;
                }
                case ShaderResourceType::MainCamerPos:
                {
                    shader->SetFloat3(uniform.Name, s_Data.CameraPos);
                    break;
                }
                case ShaderResourceType::MainCameraViewProjectionMatrix:
                {
                    shader->SetMat4(uniform.Name, s_Data.CameraViewProj);
                    break;
                }
                case ShaderResourceType::MainCameraViewMatrix:
                {
                    shader->SetMat4(uniform.Name, s_Data.CameraView);
                    break;
                }
                case ShaderResourceType::MainCamerProjectionMatrix:
                {
                    shader->SetMat4(uniform.Name, s_Data.CameraProj);
                    break;
                }
                case ShaderResourceType::EnitiyID:
                {
                    shader->SetInt(uniform.Name, entityID);
                    break;
                }
                default:
                    break;
                }
            }
            else
            {
                if (uniform.Type == ShaderDataType::Texture)
                {

                }
                else
                {
                    shader->SetUniformValue(uniform.Name, (void*)uniform.LocalResurce.data(), uniform.Type);
                }
            }

        }

        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        std::string number = "";
        for (uint32_t i = 0; i < textures.size(); i++)
        {
            auto& tex = textures[i];


            const std::string& name = tex.Type;
            if (name == "u_Texture_Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "u_Texture_Speculare")
                number = std::to_string(specularNr++);


            shader->SetInt(("material." + name + number).c_str(), i);
            if (tex.TextureResur != nullptr)
                tex.TextureResur->Bind(i);
            else
                s_Data.WhiteTexture->Bind(i);
        }
#else
        material->SetMatrix(modelMatrix);
        material->Bind(&entityID, 1);

        if (s_Data.ShadowTexture)
            s_Data.ShadowTexture->Bind(0);

#endif
        RenderCommand::DrawIndexedMesh(vertexArray);
        s_Data.Stats.DrawCalls++;
        vertexArray->UnBind();
        shader->UnBind();
    }
#endif


#pragma region DebugeDrawFunktion

    void Renderer3D::DrawLineBoxAABB(const BoxAABB& aabb, const glm::mat4& modelMatrix, int entityID, const glm::vec3& color)
    {
        s_Data.BoxAABBShader->Bind();
        s_Data.BoxAABBShader->SetMat4("u_ViewProj", s_Data.CameraDataMat4.ViewProjectionMatrix);
        

        s_Data.BoxAABBShader->SetFloat3("u_GloblePostion", modelMatrix[3]);
       
        s_Data.BoxAABBShader->SetMat4("u_Model", modelMatrix);
        s_Data.BoxAABBShader->SetInt("u_EntityID", entityID);
        s_Data.BoxAABBShader->SetFloat3("u_Color", color);
        const glm::vec3& min = aabb.GetMin();
        const glm::vec3& max = aabb.GetMax();
        s_Data.BoxAABB[0] = glm::vec3(min.x, min.y, max.z);
        s_Data.BoxAABB[1] = glm::vec3(max.x, min.y, max.z);
        s_Data.BoxAABB[2] = max;
        s_Data.BoxAABB[3] = glm::vec3(min.x, max.y, max.z);

        s_Data.BoxAABB[4] = min;
        s_Data.BoxAABB[5] = glm::vec3(max.x, min.y, min.z);
        s_Data.BoxAABB[6] = glm::vec3(max.x, max.y, min.z);
        s_Data.BoxAABB[7] = glm::vec3(min.x, max.y, min.z);

        s_Data.BoxAABBVertexBuffer->SetData(s_Data.BoxAABB, 8 * sizeof(glm::vec3));
        DrawLine(s_Data.BoxAABBVertexArray);
        s_Data.BoxAABBVertexArray->UnBind();
        s_Data.BoxAABBShader->UnBind();

    }

    void Renderer3D::DrawLineBoxAABB(const std::array<glm::vec4, 8>& aabb, const glm::mat4& matrix, const glm::vec3& position, int entityID, const glm::vec3& color)
    {
        s_Data.BoxAABBShader->Bind();
        s_Data.BoxAABBShader->SetMat4("u_ViewProj", s_Data.CameraDataMat4.ViewProjectionMatrix);
        s_Data.BoxAABBShader->SetMat4("u_Model", matrix);
        s_Data.BoxAABBShader->SetFloat3("u_GloblePostion", position);
        s_Data.BoxAABBShader->SetFloat3("u_Color", color);

        s_Data.BoxAABBShader->SetInt("u_EntityID", entityID);
        std::array<glm::vec3, 8> aabbvec3;
        for (uint32_t i = 0; i < 8; i++)
            aabbvec3.at(i) = aabb[i];

        std::memcpy(aabbvec3.data(), s_Data.BoxAABB, 8 * sizeof(glm::vec3));

        s_Data.BoxAABBVertexBuffer->SetData(s_Data.BoxAABB, sizeof(s_Data.BoxAABB));
        DrawLine(s_Data.BoxAABBVertexArray);
        s_Data.BoxAABBVertexArray->UnBind();
        s_Data.BoxAABBShader->UnBind();

    }

#pragma endregion


#pragma region SelectePrimitveRenderFunktion

    void Renderer3D::DrawObjectRender3D(const Ref<VertexArray>& vertexArray)
    {
        switch (vertexArray->GetPrimitv())
        {
        case VertexArray::Primitv::Traingle:
        {
            DrawMesh(vertexArray);
            return;
        }
        case VertexArray::Primitv::TraingleStrips:
        {
            DrawMeshStrips(vertexArray);

            return;
        }
        case VertexArray::Primitv::TraingleFan:
        {
            RY_CORE_ASSERT(false, "Primitv TraingleFan is not Implemantent jet!");
            // DrawMeshStrips(geomtry.Geometry);
            return;
        }
        case VertexArray::Primitv::Line:
        {
            DrawLine(vertexArray);
            return;
        }
        case VertexArray::Primitv::LineLoop:
        {
            // RY_CORE_ASSERT(false, "Primitv LineLoop is not Implemantent jet!");
            DrawLineLoop(vertexArray);
            return;
        }
        case VertexArray::Primitv::LineStrips:
        {
            RY_CORE_ASSERT(false, "Primitv LineStrips is not Implemantent jet!");
            return;
        }
        case VertexArray::Primitv::Points:
        {
            DrawPoints(vertexArray);
            return;
        }
        case VertexArray::Primitv::Patches:
        {
            DrawPatches(vertexArray);
            return;
        }
        default:
            RY_CORE_ASSERT(false, "Primitv type is Unknown!");
            break;
        }
    };

    void Renderer3D::DrawMesh(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedMesh(vertexArray);
    }
    
    void Renderer3D::DrawMeshStrips(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawStripsMesh(vertexArray);
    }

    void Renderer3D::DrawLine(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedLine(vertexArray);
    }

    void Renderer3D::DrawLineLoop(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedLineLoop(vertexArray);
    }

    void Renderer3D::DrawPoints(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedPoints(vertexArray);
    }

    void Renderer3D::DrawPatches(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawPatches(vertexArray);
    }
#pragma endregion


   

    void Renderer3D::AktivePolyGunMode(bool active)
    {
        RenderCommand::AktivePolyGunMode(active);
    }

    void Renderer3D::SetDethTest(bool active)
    {
        RenderCommand::SetDethTest(active);
    }

    void Renderer3D::SetFace(CallFace callFace)
    {
        RenderCommand::SetFace(callFace);
    }


    void Renderer3D::ResetStats()
    {
        s_Data.Stats.DrawTime.clear();
        s_Data.Stats.DrawCalls = 0;
        s_Data.Stats.FrameCount++;
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }

}