//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
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
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
}

Renderer::~Renderer()
{
	//delete[] m_pDepthBufferPixels;
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
	const std::vector<Vertex> vertices_world
	{
		{{0, 2,0}, {1,0,0}},
		{{1.5f, -1.0f, 0}, {1,0,0}},
		{{-1.5f, -1.f, 0.f}, {1,0,0}},

		{{0.f, 4.0f, 2.0f} , {1,0,0}},
		{{3.0f, -2.0f, 2.0f}, {0,1,0}},
		{{-3.f, -2.f, 2.f} , {0,0,1}},
	};

	const Mesh quadMesh{
		{		
			std::vector<Vertex>{
				{{-3, 3, -2},{colors::White}},
				{{0, 3, -2},{colors::White}},
				{{3, 3, -2},{colors::White}},
				{{-3, 0, -2},{colors::White}},
				{{0, 0, -2},{colors::White}},
				{{3, 0, -2},{colors::White}},
				{{-3, -3, -2},{colors::White}},
				{{0, -3, -2},{colors::White}},
				{{3, -3, -2},{colors::White}},
			}
		},
		{
			{
				3,0,4, 
				0,1,4, 
				4,1,5, 
				1,2,5, 
				6,3,7, 
				3,4,7, 
				7,4,8, 
				4,5,8 
			}
		},
		PrimitiveTopology::TriangleList,
	};

	// const std::vector<Vertex> vertices_ndc{
	// 	{{0.f, 0.5f, 1.0f}, colors::White},
	// 	{{0.5f, -0.5f, 1.0f}, colors::White},
	// 	{{-0.5f, -0.5f, 1.0f}, colors::White}
	// };

	std::vector<Vertex> vertices_screen_space{};
	std::vector<float> depth_buffer(m_Height * m_Width, std::numeric_limits<float>::max());
	vertices_screen_space.reserve(vertices_world.size());

	VertexTransformationFunction(vertices_world, vertices_screen_space);

	for (auto pos = vertices_screen_space.begin(); pos != vertices_screen_space.end(); pos+=3)
	{
		Vector2 v0 = {(pos->position.x + 1)/ 2.0f * m_Width,     (1 - pos->position.y)/ 2.0f * m_Height};
		Vector2 v1 = {((pos+1)->position.x + 1)/ 2.0f * m_Width, (1 - (pos+1)->position.y)/ 2.f * m_Height};
		Vector2 v2 = {((pos+2)->position.x + 1)/ 2.0f * m_Width, (1 - (pos+2)->position.y)/ 2.f * m_Height};

		int minX = std::max(0, static_cast<int>(std::min({v0.x, v1.x, v2.x})));
		int minY = std::max(0, static_cast<int>(std::min({v0.y, v1.y, v2.y})));
		int maxX = std::min(m_Width, static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x}))));
		int maxY = std::min(m_Height, static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y}))));

		for (int px{minX}; px < maxX; ++px)
		{
			for (int py{minY}; py < maxY; ++py)
			{
				Vector2 pixelLocation = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f};

				ColorRGB finalColor{ 0, 0, 0 };

				//(v1.x - v0.x) * (pixelLocation.y - v0.y) - (v1.y - v0.y) * (pixelLocation.x - v0.x)

				float distV2 = Vector2::Cross(v1 - v0, pixelLocation - v0);
				float distV0 = Vector2::Cross(v2 - v1, pixelLocation - v1);
				float distV1 = Vector2::Cross(v0 - v2, pixelLocation - v2);

				if(distV2 > 0 && distV0 > 0 && distV1 > 0)
				{
					const float area = distV2 + distV0 + distV1;
					distV0 = (distV0 / area);
					distV1 = (distV1 / area);
					distV2 = (distV2 / area);

					const float depth = pos->position.z * distV0  + (pos+1)->position.z * distV1 + (pos+2)->position.z * distV2 ;

					if(depth_buffer[px + (py * m_Width)] >= depth)
					{
						depth_buffer[px + (py * m_Width)] = depth;
						finalColor = (pos->color * distV0 + (pos+1)->color * distV1 + (pos+2)->color * distV2);
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


	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	float aspect = float(m_Width)/m_Height;

	for (int i{}; i < vertices_in.size(); ++i)
	{
		Vector3 vertices_ndc = vertices_in[i].position;
		vertices_ndc = m_Camera.invViewMatrix.TransformPoint(vertices_ndc);
		vertices_ndc.x = vertices_ndc.x/vertices_ndc.z;
		vertices_ndc.y = vertices_ndc.y/vertices_ndc.z;

		vertices_ndc.x = vertices_ndc.x/(aspect*m_Camera.fov);
		vertices_ndc.y = vertices_ndc.y/m_Camera.fov;


		vertices_out.push_back({vertices_ndc , vertices_in[i].color});

	}

}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
