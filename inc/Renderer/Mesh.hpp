#pragma once

#include <iostream>
#include <string_view>

#include <glad/gl.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Renderer/GPU/Shader.hpp"
#include "Renderer/GPU/VertexArray.hpp"
#include "Renderer/GPU/IndexBuffer.hpp"
#include "Renderer/GPU/VertexBuffer.hpp"
#include "Renderer/GPU/VertexBufferLayout.hpp"

namespace Renderer
{

// A textureless mesh class
class Mesh
{
    public:
        // Constructor that loads a mesh from a file, it expects model that contains only one mesh
        Mesh(std::string_view model_path)
        {
            Assimp::Importer importer;

            const aiScene* scene = importer.ReadFile(
                model_path.data(), 
                aiProcess_Triangulate);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cerr << "Assimp error: Couldn't load scene from file.\n";
                return;
            }

            setup_mesh(scene);
        }

        auto draw() const -> void
        {
            m_VAO->Bind();
            m_IBO->Bind();

            glDrawElements(GL_TRIANGLES, m_IBO->GetCount(), GL_UNSIGNED_INT, nullptr);

            m_IBO->Unbind();
            m_VAO->Unbind();
        }
        
    private:
        std::unique_ptr<GPU::VertexArray>  m_VAO;
        std::unique_ptr<GPU::IndexBuffer>  m_IBO;
        std::unique_ptr<GPU::VertexBuffer> m_VBO;

        // Fetches the mesh data from the scene and sets up the buffers
        auto setup_mesh(const aiScene* scene) -> void
        {
            if (scene->mNumMeshes != 1)
            {
                std::cerr << "Assimp error: Expected one mesh in the model.\n";
                return;
            }

            const aiMesh* mesh = scene->mMeshes[0];

            struct Vertex
            {
                glm::vec3 position;
                glm::vec3 normal;
            };

            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;

                auto& position = mesh->mVertices[i];
                auto& normal = mesh->mNormals[i];

                vertex.position = {
                    position.x,
                    position.y,
                    position.z
                };

                vertex.normal = {
                    normal.x,
                    normal.y,
                    normal.z
                };

                vertices.push_back(vertex);
            }

            std::vector<uint> indices;
            indices.reserve(mesh->mNumFaces * 3);

            for (size_t i = 0; i < mesh->mNumFaces; i++)
            {
                auto& face = mesh->mFaces[i];

                for (size_t j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            m_VBO = std::make_unique<GPU::VertexBuffer>(
                vertices.data(), 
                vertices.size() * sizeof(Vertex));

            GPU::VertexBufferLayout layout;
            layout.Push<float>(3);
            layout.Push<float>(3);

            m_VAO = std::make_unique<GPU::VertexArray>();
            m_VAO->AddBuffer(*m_VBO, layout);

            m_IBO = std::make_unique<GPU::IndexBuffer>(
                indices.data(), 
                indices.size());

            m_VBO->Unbind();
            m_VAO->Unbind();
            m_IBO->Unbind();
        }
}; // class Mesh

} // namespace Renderer
