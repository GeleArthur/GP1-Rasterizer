//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <algorithm>
#include <array>
#include <execution>

#include "Maths.h"
#include "Utils.h"
#include "Vector.h"
#include "Texture.h"


using namespace dae;

using Vector3 = Vector<3,float>;

Renderer::Renderer(SDL_Window* pWindow) :
    m_pWindow(pWindow)
{
    //Initialize
    SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

    //Create Buffers
    m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
    m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
    m_pBackBufferPixels = static_cast<uint32_t*>(m_pBackBuffer->pixels);
    depthBuffer.resize(static_cast<size_t>(m_Width) * m_Height);

    //Initialize Camera
    m_Camera.Initialize(45.f, {.0f, 0.0f, -50.0f});

    m_CoreCount = std::thread::hardware_concurrency();
    m_CoresIds.resize(m_CoreCount);
    std::iota(m_CoresIds.begin(), m_CoresIds.end(), 0);


    
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

    Utils::ParseOBJ("resources/vehicle.obj", putput, putputince);
    
    Matrix<float> pain = Matrix<float>::CreateRotationY(0);
    
    m_Meshes.push_back(std::make_unique<Mesh>(
        std::move(putput), std::move(putputince), PrimitiveTopology::TriangleList, pain
    ));
    
    // m_Meshes.push_back(std::make_unique<Mesh>(
    //     std::vector<Vertex>{
    //         {.position= {0, -5, 0}, .color= {colors::White}, .uv= {0, 0}, .normal = {0, 0, -1}, .tangent = {1, 0, 0}},
    //         {.position= {20, -5, 0}, .color= {colors::White}, .uv= {1, 0}, .normal = {0, 0, -1}, .tangent = {1, 0, 0}},
    //         {.position= {0, -5, 1}, .color= {colors::White}, .uv= {1, 1}, .normal = {0, 0, -1}, .tangent = {1, 0, 0}},
    //         {.position= {20, -5, 1}, .color= {colors::White}, .uv= {1, 1}, .normal = {0, 0, -1}, .tangent = {1, 0, 0}},
    //     },
    //     std::vector<uint32_t>{
    //         2,1,0, 2,3,1
    //     },
    //     PrimitiveTopology::TriangleList,
    //     Matrix<float>{
    //         {1, 0, 0, 0},
    //         {0, 1, 0, 0},
    //         {0, 0, 1, 0},
    //         {0, 0, 0, 1},
    //     }
    // ));
    //
    m_Lights.push_back({0.577f, -0.577f, 0.577f});
    
    
    m_Texture = Texture::LoadFromFile("resources/vehicle_diffuse.png");
    m_NormalTexture = Texture::LoadFromFile("resources/vehicle_normal.png");
    m_GlossTexture = Texture::LoadFromFile("resources/vehicle_gloss.png");
    m_SpecularTexture = Texture::LoadFromFile("resources/vehicle_specular.png");
}

Renderer::~Renderer()
{
}

void Renderer::Update(Timer* pTimer)
{
    m_Camera.Update(pTimer);

    if (m_Rotating)
    {
        for (const auto & mesh : m_Meshes)
        {
            mesh->worldMatrix *= Matrix<float>::CreateRotationY(pTimer->GetElapsed());
        }
    }
}

void Renderer::Render()
{
    //@START
    //Lock BackBuffer
    SDL_FillRect(m_pBackBuffer, nullptr, SDL_MapRGB(m_pBackBuffer->format, 100, 100, 100));
    SDL_LockSurface(m_pBackBuffer);

    std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::max());

    for (const std::unique_ptr<Mesh>& mesh : m_Meshes)
    {
        mesh->vertices_clipped.clear();
        m_clippedTriangles.clear();

        VertexStage(mesh->vertices, mesh->vertices_out, mesh->worldMatrix);
        m_clippedTriangles.reserve(mesh->vertices_out.size());
        
        int coreInterval = (static_cast<int>( mesh->vertices_out.size()/ 3 / static_cast<int>(m_CoreCount) ))+1;

        std::for_each(std::execution::seq, m_CoresIds.begin(), m_CoresIds.end(), [&](int coreId)
        {
            for (int i = coreInterval*3 * coreId; i < (coreInterval*3 * coreId)+coreInterval; i+=3)
            {
                if (mesh->indices.size() <= i) continue;
                Vertex_Out& v0 = mesh->vertices_out[mesh->indices[i + 0]];
                Vertex_Out& v1 = mesh->vertices_out[mesh->indices[i + 1]];
                Vertex_Out& v2 = mesh->vertices_out[mesh->indices[i + 2]];

                if (!FrustemCulling(Vector3{v0.position}, Vector3{v1.position}, Vector3{v2.position}))
                {
                    m_clippedTriangles.push_back(v0);
                    m_clippedTriangles.push_back(v1);
                    m_clippedTriangles.push_back(v2);
                }
            }
        });

        
        
        coreInterval = (static_cast<int>( m_clippedTriangles.size()/ 3 / static_cast<int>(m_CoreCount) ))+1;
        
        std::for_each(std::execution::seq, m_CoresIds.begin(), m_CoresIds.end(), [&](int coreId)
        {
            for (int i = coreInterval*3 * coreId; i < (coreInterval*3 * coreId)+coreInterval; i+=3)
            {
                if (m_clippedTriangles.size()-3 <= i) continue;
                
                Vertex_Out& vertex0 = m_clippedTriangles[i + 0];
                Vertex_Out& vertex1 = m_clippedTriangles[i + 1];
                Vertex_Out& vertex2 = m_clippedTriangles[i + 2];
                RenderPixels(vertex0, vertex1, vertex2, depthBuffer);
            }
        
        });
        
    }


    //@END
    //Update SDL Surface
    SDL_UnlockSurface(m_pBackBuffer);
    SDL_BlitSurface(m_pBackBuffer, nullptr, m_pFrontBuffer, nullptr);
    SDL_UpdateWindowSurface(m_pWindow);
}


void Renderer::RenderPixels(const Vertex_Out& vertex0, const Vertex_Out& vertex1, const Vertex_Out& vertex2, std::vector<float>& depth_buffer)
{
    Vector<2, float> v0 = {(vertex0.position.x + 1) / 2.0f * static_cast<float>(m_Width), (1 - vertex0.position.y) / 2.0f * static_cast<float>(m_Height)};
    Vector<2, float> v1 = {(vertex1.position.x + 1) / 2.0f * static_cast<float>(m_Width), (1 - vertex1.position.y) / 2.0f * static_cast<float>(m_Height)};
    Vector<2, float> v2 = {(vertex2.position.x + 1) / 2.0f * static_cast<float>(m_Width), (1 - vertex2.position.y) / 2.0f * static_cast<float>(m_Height)};

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

                const float depth = 1.0f / (1.0f / vertex0.position.z * distV0 + 1.0f / vertex1.position.z * distV1 + 1.0f / vertex2.position.z * distV2);

                if (depth > 1 || depth < 0)
                {
                    continue;
                }

                if (depth_buffer[px + (py * m_Width)] >= depth)
                {
                    depth_buffer[px + (py * m_Width)] = depth;

                    #if DEBUG
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
                    #endif
                    

                    float depthW = 1.0f / (1.0f / vertex0.position.w * distV0 + 1.0f / vertex1.position.w * distV1 + 1.0f / vertex2.position.w * distV2);

                    Vector<4, float> position = (vertex0.position / vertex0.position.w * distV0 + vertex1.position / vertex1.position.w * distV1 + vertex2.position / vertex2.position.w * distV2) * depthW;
                    Vector<2, float> uv = (vertex0.uv / vertex0.position.w * distV0 + vertex1.uv / vertex1.position.w * distV1 + vertex2.uv / vertex2.position.w * distV2) * depthW;
                    Vector3 normal = (vertex0.normal / vertex0.position.w * distV0 + vertex1.normal / vertex1.position.w * distV1 + vertex2.normal / vertex2.position.w * distV2) * depthW;
                    Vector3 tangent = (vertex0.tangent / vertex0.position.w * distV0 + vertex1.tangent / vertex1.position.w * distV1 + vertex2.tangent / vertex2.position.w * distV2) * depthW;
                    Vector3 viewDirection = (vertex0.viewDirection / vertex0.position.w * distV0 + vertex1.viewDirection / vertex1.position.w * distV1 + vertex2.viewDirection / vertex2.position.w * distV2) * depthW;

                    ColorRGB finalColor;
                    
                    if (m_RenderDepth)
                    {
                        finalColor = ColorRGB{Remap01(0.8f, 1.0f, depth)};
                    }
                    else
                    {
                        finalColor = FragmentShader(
                            Vertex_Out{
                                .position = position,
                                .color = ColorRGB{},
                                .uv = uv,
                                .normal = normal,
                                .tangent = tangent,
                                .viewDirection = viewDirection
                            },
                            7.0f,
                            25.0f);
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

    std::transform(std::execution::par, vertices_in.cbegin(), vertices_in.cend(), vertices_out.begin(),
       [&](const Vertex& v)
       {
           Vector<4, float> transformedPoint = worldViewProjectionMatrix.TransformPoint(Vector<4, float>{v.position, 1});

           transformedPoint.x = transformedPoint.x / transformedPoint.w;
           transformedPoint.y = transformedPoint.y / transformedPoint.w;
           transformedPoint.z = transformedPoint.z / transformedPoint.w;
           
           return Vertex_Out{
               .position = transformedPoint,
               .color = v.color,
               .uv = v.uv,
               .normal = world_matrix.TransformVector(v.normal).Normalized(),
               .tangent = world_matrix.TransformVector(v.tangent).Normalized(),
               .viewDirection = (m_Camera.origin - (world_matrix.TransformPoint(v.position))).Normalized()
           };
       });
}

void Renderer::ChangeShadingMode()
{
    switch (m_ShadingMode)
    {
    case ShadingMode::observed_area:
        m_ShadingMode = ShadingMode::diffuse;
        std::cout << "diffuse" << '\n'; 
        break;
    case ShadingMode::diffuse:
        m_ShadingMode = ShadingMode::specular;
        std::cout << "specular" << '\n'; 
        break;
    case ShadingMode::specular:
        m_ShadingMode = ShadingMode::combined;
        std::cout << "combined" << '\n'; 
        break;
    case ShadingMode::combined:
        m_ShadingMode = ShadingMode::observed_area;
        std::cout << "observed_area" << '\n'; 
        break;
    }
}

void Renderer::ToggleRotation()
{
    std::cout << "Toggle Rotation" << '\n';
    m_Rotating = !m_Rotating;
}

void Renderer::ToggleNormalMaps()
{
    std::cout << "Toggle NormalMaps" << '\n';
    m_UseNormalMaps = !m_UseNormalMaps;
}

void Renderer::ToggleDepthRendering()
{
    std::cout << "Toggle DepthRendering" << '\n';
    m_RenderDepth = !m_RenderDepth;
}

Camera& Renderer::GetCamera()
{
    return m_Camera;
}

bool Renderer::FrustemCulling(const Vector3& v0, const Vector3& v1, const Vector3& v2) const
{
    if (CheckInFrustum(v0)) return true;
    if (CheckInFrustum(v1)) return true;
    if (CheckInFrustum(v2)) return true;
    return false;
}

bool Renderer::CheckInFrustum(const Vector3& v) const
{
    if (v.x < -1 || v.x > 1) return true;
    if (v.y < -1 || v.y > 1) return true;
    if (v.z <  0 || v.z > 1) return true;
    return false;
}

ColorRGB Renderer::FragmentShader(const Vertex_Out& vertexIn, float diffuseReflectance, float shininess)
{
    ColorRGB finalColor{0};

    Vector3 normal;
    if (m_UseNormalMaps)
    {
        const Vector3 realNormal = vertexIn.normal.Normalized();
        const Vector3 realTangent = vertexIn.tangent.Normalized();
        // Do this in the vertex stage?
        const Vector3 binormal = Vector3::Cross(realNormal, realTangent);
        Matrix tangentSpaceAxis{
            Vector<4, float>{realTangent, 0},
            Vector<4, float>{binormal, 0},
            Vector<4, float>{realNormal, 0},
            Vector<4,float>::Zero
        };
        
        Vector3 sampledNormal = Vector3{m_NormalTexture->Sample(vertexIn.uv)};
        normal = Vector3{(2.0f * sampledNormal.x) - 1.0f, (2.0f * sampledNormal.y) - 1.0f, 2.0f * sampledNormal.z - 1.0f};
        normal = tangentSpaceAxis.TransformVector(normal);
    }
    else
    {
        normal = vertexIn.normal.Normalized();
    }

    const Vector3 lightDirection = GetLights().at(0).Normalized(); // TODO: Multilight
    const ColorRGB albedoTexture = m_Texture->Sample(vertexIn.uv);


    // lambert
    const float observableArea = std::max<float>(0.0f, Vector3::Dot(-lightDirection, normal));

    const ColorRGB lambert = (diffuseReflectance * albedoTexture) / PI;

    // Phong
    const auto specularReflectance = m_SpecularTexture->Sample(vertexIn.uv).r;
    const float gloss = m_GlossTexture->Sample(vertexIn.uv).r * shininess;

    const Vector3 reflect = Vector3::Reflect(-lightDirection, normal);
    const float cosAngle = std::max(0.0f,Vector3::Dot(reflect, -vertexIn.viewDirection));

    const float phong = specularReflectance * std::pow(cosAngle, gloss);
    
    switch (m_ShadingMode)
    {
    case ShadingMode::observed_area:
        finalColor = ColorRGB{observableArea};
        break;
    case ShadingMode::diffuse:
        finalColor = lambert * observableArea;
        break;
    case ShadingMode::specular:
        finalColor = ColorRGB{phong};
        break;
    case ShadingMode::combined:
        finalColor = (albedoTexture * 0.3f) + ColorRGB{phong} + lambert * observableArea;
        break;
    }
    
    return finalColor;
}

std::vector<Vector3>& Renderer::GetLights()
{
    return m_Lights;
}

bool Renderer::SaveBufferToImage() const
{
    return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
