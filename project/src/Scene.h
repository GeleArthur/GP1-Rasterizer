#include <memory>
#include <vector>

class Material;

namespace dae
{
    class Texture;
}

namespace dae
{
    struct Vertex;
    struct Mesh;
}

class Scene
{
public:
    Scene();
    ~Scene();

    void AddMesh(std::unique_ptr<dae::Mesh>&& mesh);
    void AddTexture(std::unique_ptr<dae::Texture> texture);
    void AddMaterial(Material material);
    void CreateBuffers();
private:
    std::vector<dae::Vertex> m_AllVertices;
    std::vector<uint32_t> m_AllIndices;

    std::vector<std::unique_ptr<dae::Mesh>> m_Meshes;
    std::vector<std::unique_ptr<dae::Texture>> m_Textures;
    std::vector<std::unique_ptr<Material>> m_Materials;
    
    
};
