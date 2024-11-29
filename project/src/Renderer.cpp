//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <array>
#include <execution>

#include "BetterPraseObj.h"
#include "Maths.h"
#include "Utils.h"
#include "Vector.h"
#include "Texture.h"


using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
    m_pWindow(pWindow),
    m_CoreCount(std::thread::hardware_concurrency())
{
    //Initialize
    SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

    //Create Buffers
    m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
    m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
    m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);
    depthBuffer.resize(static_cast<size_t>(m_Width) * m_Height);

    //Initialize Camera
    m_Camera.Initialize(60.f, {.0f, 0.0f, 0.0f});
    m_BinnedVertexOut.resize(m_CoreCount);
    m_CoresIds.resize(m_CoreCount);
    std::iota(m_CoresIds.begin(), m_CoresIds.end(), 0);


    //
    // m_Meshes.push_back(std::make_unique<Mesh>(
    // 	std::vector<Vertex>{
    // 		{{-3, 3, -2}, {colors::White}, {0,0} },
    // 		{{0, 3, -2}, {colors::White}, {0.5,0}},
    // 		{{3, 3, -2}, {colors::White}, {1,0}},
    // 		{{-3, 0, -2}, {colors::White}, {0,0.5}},
    // 		{{0, 0, -2}, {colors::White}, {0.5, 0.5}},
    // 		{{3, 0, -2}, {colors::White}, {1, 0.5}},
    // 		{{-3, -3, -2}, {colors::White}, {0,1}},
    // 		{{0, -3, -2}, {colors::White}, {0.5,1}},
    // 		{{3, -3, -2}, {colors::White}, {1,1}},
    // 	},
    // 	std::vector<uint32_t>{
    // 		3, 0, 4, 1, 5, 2,
    // 		2, 6,
    // 		6, 3, 7, 4, 8, 5,
    // 	},
    // 	PrimitiveTopology::TriangleStrip,
    // 	Matrix{
    // 		{1,0,0,0},
    // 		{0,1,0,0},
    // 		{0,0,1,0},
    // 		{0,0,0,1},
    // 	}
    // ));
    //
    // m_Meshes.push_back(std::make_unique<Mesh>(
    // 	std::vector<Vertex>{
    // 		{{-3, 3, -2}, {colors::White}, {0,0} },
    // 		{{0, 3, -2}, {colors::White}, {0.5,0}},
    // 		{{3, 3, -2}, {colors::White}, {1,0}},
    // 		{{-3, 0, -2}, {colors::White}, {0,0.5}},
    // 		{{0, 0, -2}, {colors::White}, {0.5, 0.5}},
    // 		{{3, 0, -2}, {colors::White}, {1, 0.5}},
    // 		{{-3, -3, -2}, {colors::White}, {0,1}},
    // 		{{0, -3, -2}, {colors::White}, {0.5,1}},
    // 		{{3, -3, -2}, {colors::White}, {1,1}},
    // 	},
    // 	std::vector<uint32_t>{
    // 		3,0,4, 
    // 		0,1,4, 
    // 		4,1,5, 
    // 		1,2,5, 
    // 		6,3,7, 
    // 		3,4,7, 
    // 		7,4,8, 
    // 		4,5,8 
    // 	},
    // 	PrimitiveTopology::TriangleList,
    // 	Matrix{
    // 		{1,0,0,0},
    // 		{0,1,0,0},
    // 		{0,0,1,0},
    // 		{10,0,1,1},
    // 	}
    // ));

    std::vector<Vertex> putput;
    std::vector<uint32_t> putputince;

    Utils::ParseOBJ("resources/tuktuk.obj", putput, putputince);
    // Utils::BetterParseObj("resources/Boat.obj", putput, putputince);

    Matrix<float> pain{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };

    m_Meshes.push_back(std::make_unique<Mesh>(
        std::move(putput), std::move(putputince), PrimitiveTopology::TriangleList, pain
    ));

    m_Meshes.push_back(std::make_unique<Mesh>(
        std::vector<Vertex>{
            {{0, 0, 10}, {colors::White}, {0, 0}},
            {{0, 3, 10}, {colors::White}, {0.5, 0}},
            {{3, 3, 10}, {colors::White}, {1, 0}},
        },
        std::vector<uint32_t>{
            0, 1, 2
        },
        PrimitiveTopology::TriangleList,
        Matrix<float>{
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }
    ));

    m_Texture = Texture::LoadFromFile("resources/tuktuk.png");
}

Renderer::~Renderer()
{
}

void Renderer::Update(Timer* pTimer)
{
    m_Camera.Update(pTimer);

    m_Meshes[0]->worldMatrix = Matrix<float>::CreateRotationY(pTimer->GetTotal());
}

void Renderer::Render()
{
    //@START
    //Lock BackBuffer
    SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));
    SDL_LockSurface(m_pBackBuffer);

    std::ranges::fill(depthBuffer, std::numeric_limits<float>::max());

    for (const std::unique_ptr<Mesh>& mesh : m_Meshes)
    {
        mesh->vertices_clipped.clear();

        VertexStage(mesh->vertices, mesh->vertices_out, mesh->worldMatrix);

        for (int i = 0; i < (mesh->primitiveTopology == PrimitiveTopology::TriangleList
                                 ? mesh->indices.size()
                                 : mesh->indices.size() - 2); i +=
             mesh->primitiveTopology == PrimitiveTopology::TriangleList ? 3 : 1)
        {
            const Vertex_Out* vertex0{};
            const Vertex_Out* vertex1{};
            const Vertex_Out* vertex2{};

            switch (mesh->primitiveTopology)
            {
            case PrimitiveTopology::TriangleList:
                vertex0 = &mesh->vertices_out[mesh->indices[i]];
                vertex1 = &mesh->vertices_out[mesh->indices[i + 1]];
                vertex2 = &mesh->vertices_out[mesh->indices[i + 2]];
                break;
            case PrimitiveTopology::TriangleStrip:
                if (!(i & 1)) // isEven
                {
                    vertex0 = &mesh->vertices_out[mesh->indices[i]];
                    vertex1 = &mesh->vertices_out[mesh->indices[i + 1]];
                    vertex2 = &mesh->vertices_out[mesh->indices[i + 2]];
                }
                else // odd
                {
                    vertex0 = &mesh->vertices_out[mesh->indices[i]];
                    vertex2 = &mesh->vertices_out[mesh->indices[i + 1]];
                    vertex1 = &mesh->vertices_out[mesh->indices[i + 2]];
                }
                break;
            }

            if (FrustemCulling(vertex0->position, vertex1->position, vertex2->position)) continue;

            RenderPixels(*vertex0, *vertex1, *vertex2, depthBuffer, *m_Texture);
        }
    }


    //@END
    //Update SDL Surface
    SDL_UnlockSurface(m_pBackBuffer);
    SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
    SDL_UpdateWindowSurface(m_pWindow);
}


void Renderer::RenderPixels(const Vertex_Out& vertex0, const Vertex_Out& vertex1, const Vertex_Out& vertex2,
                            std::vector<float>& depth_buffer, const Texture& texture)
{
    Vector<2, float> v0 = {(vertex0.position.x + 1) / 2.0f * m_Width, (1 - vertex0.position.y) / 2.0f * m_Height};
    Vector<2, float> v1 = {(vertex1.position.x + 1) / 2.0f * m_Width, (1 - vertex1.position.y) / 2.0f * m_Height};
    Vector<2, float> v2 = {(vertex2.position.x + 1) / 2.0f * m_Width, (1 - vertex2.position.y) / 2.0f * m_Height};

    int minX = std::max(0, static_cast<int>(std::min({v0.x, v1.x, v2.x})));
    int minY = std::max(0, static_cast<int>(std::min({v0.y, v1.y, v2.y})));
    int maxX = std::min(m_Width, static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x}))));
    int maxY = std::min(m_Height, static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y}))));

    for (int px{minX}; px < maxX; ++px)
    {
        for (int py{minY}; py < maxY; ++py)
        {
            Vector<2, float> pixelLocation = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f};



            // (v1.x - v0.x) * (pixelLocation.y - v0.y) - (v1.y - v0.y) * (pixelLocation.x - v0.x)

            float distV2 = Vector<2, float>::Cross(v1 - v0, pixelLocation - v0);
            float distV0 = Vector<2, float>::Cross(v2 - v1, pixelLocation - v1);
            float distV1 = Vector<2, float>::Cross(v0 - v2, pixelLocation - v2);

            if (distV2 >= 0 && distV0 >= 0 && distV1 >= 0)
            {
                const float area = distV2 + distV0 + distV1;
                distV0 = (distV0 / area);
                distV1 = (distV1 / area);
                distV2 = (distV2 / area);

                const float depth = 1.0f / (1.0f / vertex0.position.z * distV0 + 1.0f / vertex1.position.z * distV1 +
                    1.0f / vertex2.position.z * distV2);

                if (depth > 1 || depth < 0)
                {
                    continue;
                }

                if (depth_buffer[px + (py * m_Width)] >= depth)
                {
                    depth_buffer[px + (py * m_Width)] = depth;

                    // #if DEBUG
                    if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_T])
                    {
                        int mousex;
                        int mousey;
                        SDL_GetMouseState(&mousex, &mousey);
                        if (px == mousex && py == mousey)
                        {
                            __debugbreak();
                        }
                    }
                    // #endif
                    

                    float depthW = 1.0f / (1.0f / vertex0.position.w * distV0 + 1.0f / vertex1.position.w * distV1 +
                        1.0f / vertex2.position.w * distV2);
                    Vector<2, float> uv = (vertex0.uv / vertex0.position.w * distV0 + vertex1.uv / vertex1.position.w *
                        distV1 + vertex2.uv / vertex2.position.w * distV2) * depthW;

                    Vector<3, float> normal = (vertex0.normal / vertex0.position.w * distV0 + vertex1.normal / vertex1.position.w *
                            distV1 + vertex2.normal / vertex2.position.w * distV2) * depthW;

                    ColorRGB finalColor{0, 0, 0};

                    switch (m_ShadingMode)
                    {
                    case ShadingMode::texture:
                        finalColor = texture.Sample(uv);
                        break;
                    case ShadingMode::depthBuffer:
                        {
                            float depthWRemapped = Remap(depth, 0.8f, 1, 0.0f, 1.0f);
                            finalColor = ColorRGB{depthWRemapped, depthWRemapped, depthWRemapped};
                            break;
                        }
                    case ShadingMode::modelNormals:
                        finalColor = ColorRGB{Remap(normal.x, -1, 1, 0, 1), Remap(normal.y, -1, 1, 0, 1), Remap(normal.z, -1, 1, 0, 1)};
                    }
                    
                    //Update Color in Buffer
                    finalColor.MaxToOne();

                    m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
                                                                          static_cast<uint8_t>(finalColor.r * 255),
                                                                          static_cast<uint8_t>(finalColor.g * 255),
                                                                          static_cast<uint8_t>(finalColor.b * 255));
                }
            }
        }
    }
}

void Renderer::VertexStage(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix<float>& world_matrix) const
{
    const float aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
    auto projection = Matrix<float>::CreatePerspectiveFovLH(m_Camera.fov, aspect, m_Camera.nearPlane, m_Camera.farPlane);
    const Matrix<float> worldViewProjectionMatrix = world_matrix * m_Camera.invViewMatrix * projection;

    std::transform(std::execution::seq, vertices_in.cbegin(), vertices_in.cend(), vertices_out.begin(),
       [&](const Vertex& v)
       {
           Vector<4, float> thing = worldViewProjectionMatrix.TransformPoint(Vector<4, float>{v.position, 1});

           thing.x = thing.x / thing.w;
           thing.y = thing.y / thing.w;
           thing.z = thing.z / thing.w;

           return Vertex_Out{thing, v.color, v.uv, world_matrix.TransformVector(v.normal)};
       });
}

void Renderer::ChangeRenderMode()
{
    switch (m_ShadingMode)
    {
    case ShadingMode::texture:
        m_ShadingMode = ShadingMode::depthBuffer;
        std::cout << "depth" << '\n';
        break;
    case ShadingMode::depthBuffer:
        m_ShadingMode = ShadingMode::modelNormals;
        std::cout << "normals" << '\n';
        break;
        case ShadingMode::modelNormals:
        m_ShadingMode = ShadingMode::texture;
        std::cout << "texture" << '\n';
            break;
    }
}

ColorRGB Renderer::LambertBRDF()
{
    return {0, 0, 0};
}


bool Renderer::FrustemCulling(const Vector<3, float>& v0, const Vector<3, float>& v1, const Vector<3, float>& v2) const
{
    if (CheckInFrustum(v0)) return true;
    if (CheckInFrustum(v1)) return true;
    if (CheckInFrustum(v2)) return true;
    return false;
}

bool Renderer::CheckInFrustum(const Vector<3, float>& v) const
{
    if (v.x < -1 || v.x > 1) return true;
    if (v.y < -1 || v.y > 1) return true;
    if (v.z < 0 || v.z > 1) return true;
    return false;
}

ColorRGB Renderer::FragmentShaderShowNormals()
{
    return {};
}

bool Renderer::SaveBufferToImage() const
{
    return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
