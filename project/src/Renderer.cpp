//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"

#include <array>

#include "Maths.h"
#include "Texture.h"
#include "Utils.h"

using namespace dae;

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
	m_Camera.Initialize(60.f, {.0f, 5.0f, -30.f});


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

	Matrix pain{
			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{10,0,1,1},
	};


	m_Meshes.push_back(std::make_unique<Mesh>(Mesh{
		std::move(putput), std::move(putputince), PrimitiveTopology::TriangleList, pain
	}));

	// 	m_Meshes.push_back(std::make_unique<Mesh>(
	// 	std::vector<Vertex>{
	// 		{{0, 0, -2}, {colors::White}, {0,0} },
	// 		{{0, 3, -2}, {colors::White}, {0.5,0}},
	// 		{{3, 3, -2}, {colors::White}, {1,0}},
	// 	},
	// 	std::vector<uint32_t>{
	// 		0,1,2
	// 	},
	// 	PrimitiveTopology::TriangleList,
	// 	Matrix{
	// 		{1,0,0,0},
	// 		{0,1,0,0},
	// 		{0,0,1,0},
	// 		{10,0,1,1},
	// 	}
	// ));


	m_Texture = Texture::LoadFromFile("resources/tuktuk.png");
}

Renderer::~Renderer()
{
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
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
		mesh->vertices_out.clear();
		VertexTransformationFunction(mesh->vertices, mesh->vertices_out, mesh->worldMatrix);
		
		switch (mesh->primitiveTopology)
		{
		case PrimitiveTopology::TriangleList:
			for (int i = 0; i < mesh->indices.size(); i+=3 )
			{
				const Vertex_Out& vertex0 = mesh->vertices_out[mesh->indices[i]];
				const Vertex_Out& vertex1 = mesh->vertices_out[mesh->indices[i+1]];
				const Vertex_Out& vertex2 = mesh->vertices_out[mesh->indices[i+2]];

				RenderPixels(vertex0, vertex1, vertex2, depthBuffer, *m_Texture);
			}
			break;
		case PrimitiveTopology::TriangleStrip:
			for (int i = 0; i < mesh->indices.size()-2; ++i )
			{
				if(mesh->indices[i] == mesh->indices[i+1] || mesh->indices[i+1] == mesh->indices[i+2])
				{
					continue;
				}
				
				Vertex_Out* vertex0;
				Vertex_Out* vertex1;
				Vertex_Out* vertex2;

				
				if( !(i & 1) ) // isEven
				{
					vertex0 = &mesh->vertices_out[mesh->indices[i]];
					vertex1 = &mesh->vertices_out[mesh->indices[i+1]];
					vertex2 = &mesh->vertices_out[mesh->indices[i+2]];
				}
				else // odd
				{
					vertex0 = &mesh->vertices_out[mesh->indices[i]];
					vertex2 = &mesh->vertices_out[mesh->indices[i+1]];
					vertex1 = &mesh->vertices_out[mesh->indices[i+2]];
				}

				RenderPixels(*vertex0, *vertex1, *vertex2, depthBuffer, *m_Texture);
			}
			break;
		}

	}


	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}


void Renderer::RenderPixels(const Vertex_Out& vertex0, const Vertex_Out& vertex1, const Vertex_Out& vertex2, std::vector<float>& depth_buffer, const Texture& texture)
{
		Vector2 v0 = {(vertex0.position.x + 1) / 2.0f * m_Width, (1 - vertex0.position.y) / 2.0f * m_Height};
		Vector2 v1 = {(vertex1.position.x + 1) / 2.0f * m_Width, (1 - vertex1.position.y) / 2.0f * m_Height};
		Vector2 v2 = {(vertex2.position.x + 1) / 2.0f * m_Width, (1 - vertex2.position.y) / 2.0f * m_Height};

		int minX = std::max(0, static_cast<int>(std::min({v0.x, v1.x, v2.x})));
		int minY = std::max(0, static_cast<int>(std::min({v0.y, v1.y, v2.y})));
		int maxX = std::min(m_Width, static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x}))));
		int maxY = std::min(m_Height, static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y}))));

		for (int px{minX}; px < maxX; ++px)
		{
			for (int py{minY}; py < maxY; ++py)
			{
				Vector2 pixelLocation = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f};

				ColorRGB finalColor{0, 0, 0};

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

				// (v1.x - v0.x) * (pixelLocation.y - v0.y) - (v1.y - v0.y) * (pixelLocation.x - v0.x)

				float distV2 = Vector2::Cross(v1 - v0, pixelLocation - v0);
				float distV0 = Vector2::Cross(v2 - v1, pixelLocation - v1);
				float distV1 = Vector2::Cross(v0 - v2, pixelLocation - v2);

				if (distV2 >= 0 && distV0 >= 0 && distV1 >= 0)
				{
					const float area = distV2 + distV0 + distV1;
					distV0 = (distV0 / area);
					distV1 = (distV1 / area);
					distV2 = (distV2 / area);

					const float depth = 1.0f/(1.0f/vertex0.position.z * distV0 + 1.0f/vertex1.position.z * distV1 + 1.0f/vertex2.position.z * distV2);

					if (depth > 1 || depth < 0)
					{
						continue;
					}

					if (depth_buffer[px + (py * m_Width)] >= depth)
					{
						depth_buffer[px + (py * m_Width)] = depth;

						const float depthW = 1.0f/(1.0f/vertex0.position.w * distV0 + 1.0f/vertex1.position.w * distV1 + 1.0f/vertex2.position.w * distV2);
						Vector2 uv = (vertex0.uv/vertex0.position.w * distV0 + vertex1.uv/vertex1.position.w * distV1 + vertex2.uv/vertex2.position.w * distV2)*depthW;

						if (!renderDepth)
						{
							finalColor = texture.Sample(uv);
						}else
						{
							float depthWRemapped = Remap(depth, 0.995f, 1.0f, 0.0f, 1.0f);
							finalColor = ColorRGB{ depthWRemapped,depthWRemapped,depthWRemapped };
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

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix& world_matrix) const
{
	float aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	Matrix worldViewProjectionMatrix = world_matrix * m_Camera.invViewMatrix * Matrix::CreatePerspectiveFovLH(m_Camera.fov, aspect, m_Camera.nearPlane, m_Camera.farPlane);

	for (int i{}; i < vertices_in.size(); ++i)
	{
		Vector4 thing = worldViewProjectionMatrix.TransformPoint(Vector4{ vertices_in[i].position, 1 });

		thing.x = thing.x / thing.w;
		thing.y = thing.y / thing.w;
		thing.z = thing.z / thing.w;
		// thing.w = thing.w;
		
		vertices_out.push_back(Vertex_Out{thing, vertices_in[i].color, vertices_in[i].uv});
	}
}

bool ClippingTriangle(const Vertex_Out& v1, const Vertex_Out& v2, const Vertex_Out& v3)
{
	return false;
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
