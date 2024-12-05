#include "Scene.h"

#include "DataTypes.h"
#include "Material.h"
#include "Texture.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::AddMesh(std::unique_ptr<dae::Mesh>&& mesh)
{
    m_Meshes.push_back(std::move(mesh));
}

void Scene::AddTexture(std::unique_ptr<dae::Texture> texture)
{
    m_Textures.push_back(std::move(texture));
}

void Scene::AddMaterial(Material material)
{
    
}

void Scene::CreateBuffers()
{
    // m_AllVertices.clear();
    // m_AllIndices.clear();
    //
    // uint32_t offset = 0u;
    // for (const std::unique_ptr<dae::Mesh>& mesh : m_Meshes)
    // {
    //     for (const dae::Vertex& vertex : mesh->vertices)
    //     {
    //         m_AllVertices.push_back(vertex);
    //     }
    //     
    // }
    
}
