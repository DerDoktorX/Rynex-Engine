#include "rypch.h"
#include "Mesh.h"

#include "Rynex/Core/Application.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include <Rynex/Renderer/Materials/BasicMaterial.h>

namespace Rynex {

    Mesh::Mesh(std::vector<MeshVertex>&& meshVertex, std::vector<uint32_t>&& meshIndex, std::vector<MeshTexture>&& meshTexures, bool async)
        : m_Textures(std::move(meshTexures))
        , m_VertexArray(nullptr)
        
    {
      

        std::vector<MeshVertex> meshvertex = std::move(meshVertex);
        
        std::vector<unsigned char> meshVertexData;
        meshVertexData.reserve(meshvertex.size() * sizeof(MeshVertex));
        meshVertexData.insert(
            meshVertexData.end(),
            reinterpret_cast<unsigned char*>(meshvertex.data()),
            reinterpret_cast<unsigned char*>(meshvertex.data() + meshvertex.size()));


        if(async)
        {
            
            std::vector<std::filesystem::path> paths;
            paths.reserve(m_Textures.size());
            for (MeshTexture& tex : m_Textures)
            {
                // AssetManager::GetAssetAsync<Texture>(tex.Path, &tex.TextureResur);
                paths.emplace_back(tex.Path);
            }
            m_Material = Material::CreateBasicAsysnc(std::move(paths));

            m_VertexBuffer = VertexBuffer::CreateAsync(std::move(meshVertexData), meshvertex.size() * sizeof(MeshVertex), BufferDataUsage::StaticDraw, {
                {ShaderDataType::Float3, "a_Postion"},
                {ShaderDataType::Float2, "a_UV"},
                {ShaderDataType::Float3, "a_Normals"},
            });
            m_IndexBuffer = IndexBuffer::CreateAsync(std::move(meshIndex), meshIndex.size(), BufferDataUsage::StaticDraw); 
            
            Application::Get().SubmiteToMainThreedQueue([this]() {
                InitAsync();
            });
        }
        else
        {
            std::vector<std::filesystem::path> paths;
            paths.reserve(m_Textures.size());
            for (MeshTexture& tex : m_Textures)
            {
                // tex.TextureResur = AssetManager::GetAsset<Texture>(tex.Path);
                paths.emplace_back(tex.Path);
            }
            m_Material = Material::CreateBasic(paths);
            m_VertexBuffer = VertexBuffer::Create(meshVertexData.data(), meshvertex.size() * sizeof(MeshVertex), BufferDataUsage::StaticDraw, {
                {ShaderDataType::Float3, "a_Postion"},
                {ShaderDataType::Float2, "a_UV"},
                {ShaderDataType::Float3, "a_Normals"},
            });
           
            m_IndexBuffer = IndexBuffer::Create(meshIndex.data(), meshIndex.size(), BufferDataUsage::StaticDraw);
            InitAsync();
        }

        meshvertex.clear();
        meshvertex.shrink_to_fit();
  
        
    }

    Mesh::Mesh(std::vector<MeshVertex>&& meshVertex, std::vector<unsigned int>&& meshIndex, std::vector<MeshTexture>&& meshTexures, const Ref<Material>& material, bool async)
        : m_Textures(std::move(meshTexures))
        , m_VertexArray(nullptr)
        , m_Material(material)
    {
    }



    Mesh::~Mesh()
    {
    }

    void Mesh::InitAsync()
    {
        m_VertexArray = VertexArray::Create();
        m_VertexArray->SetPrimitv(VertexArray::Primitv::Traingle);
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
        m_VertexArray->SetBoxAABB({ ShaderDataType::Float3, "a_Postion" });
       

    }

    void Mesh::OnDraw(const Ref<Shader>& shader)
    {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        std::string number;
        shader->Bind();
        for (unsigned int i = 0; i < m_Textures.size(); i++)
        {

            std::string& name = m_Textures[i].Type;
            if (name == "u_Texture_Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "u_Texture_Speculare")
                number = std::to_string(specularNr++);

            shader->SetInt(("material." + name + number).c_str(), i);
            m_Textures[i].TextureResur->Bind(i);
        }


        // draw mesh
        m_VertexArray->Bind();
        Renderer3D::DrawMesh(m_VertexArray);
    }

   
}