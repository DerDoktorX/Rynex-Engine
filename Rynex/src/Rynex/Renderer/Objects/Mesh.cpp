#include "rypch.h"
#include "Mesh.h"

#include "Rynex/Renderer/Rendering/Renderer3D.h"

namespace Rynex {

    Mesh::Mesh(std::vector<MeshVertex>& meshVertex, std::vector<unsigned int>& meshIndex, std::vector<MeshTexture>& meshTexures)
        : m_Textures(meshTexures)
        , m_VertexArray(VertexArray::Create())
    {
        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(meshVertex.data(), meshVertex.size() * sizeof(MeshVertex), BufferDataUsage::StaticDraw);
        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(meshIndex.data(), meshIndex.size(), BufferDataUsage::StaticDraw);
        vertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Postion"},
            {ShaderDataType::Float2, "a_UV"},
            {ShaderDataType::Float3, "a_Normals"},
        });
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);
        m_VertexArray->SetPrimitv(VertexArray::Primitv::Traingle);
    }

        

    Mesh::~Mesh()
    {
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

    void Mesh::Create()
    {
    }
}