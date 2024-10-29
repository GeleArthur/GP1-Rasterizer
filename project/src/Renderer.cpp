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
	SDL_LockSurface(m_pBackBuffer);

	const std::vector<Vertex> vertices_world
	{
		{{0.f, 2.0f, 0.0f}},
		{{1.0f, 0.0f, 0.0f}},
		{{-1.f, 0.f, 0.f}}
	};

	// const std::vector<Vertex> vertices_ndc{
	// 	{{0.f, 0.5f, 1.0f}, colors::White},
	// 	{{0.5f, -0.5f, 1.0f}, colors::White},
	// 	{{-0.5f, -0.5f, 1.0f}, colors::White}
	// };

	std::vector<Vertex> vertices_screen_space{};
	vertices_screen_space.reserve(vertices_world.size());

	VertexTransformationFunction(vertices_world, vertices_screen_space);

	for (auto pos = vertices_screen_space.begin(); pos != vertices_screen_space.end(); pos+=3)
	{
		Vector2 v0 = {(pos->position.x + 1)/ 2.0f * m_Width,     (1 - pos->position.y)/ 2.0f * m_Height};
		Vector2 v1 = {((pos+1)->position.x + 1)/ 2.0f * m_Width, (1 - (pos+1)->position.y)/ 2.f * m_Height};
		Vector2 v2 = {((pos+2)->position.x + 1)/ 2.0f * m_Width, (1 - (pos+2)->position.y)/ 2.f * m_Height};

		for (int px{}; px < m_Width; ++px)
		{
			for (int py{}; py < m_Height; ++py)
			{
				Vector2 pixelLocation = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f};

				ColorRGB finalColor{ 0, 0, 0 };


				const float distV2 = Vector2::Cross(v1 - v0, pixelLocation - v0);
				const float distV0 = Vector2::Cross(v2 - v1, pixelLocation - v1);
				const float distV1 = Vector2::Cross(v0 - v2, pixelLocation - v2);

				if(distV2 > 0 && distV0 > 0 && distV1 > 0)
				{
					finalColor = {1,1,1};
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


	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Vertex>& vertices_in, std::vector<Vertex>& vertices_out) const
{
	for (int i{}; i < vertices_in.size(); ++i)
	{
		Vector3 vertices_ndc = vertices_in[i].position;
		vertices_ndc = m_Camera.invViewMatrix.TransformPoint(vertices_ndc);
		vertices_ndc.x = vertices_ndc.x/vertices_ndc.z;
		vertices_ndc.y = vertices_ndc.y/vertices_ndc.z;


		vertices_out.push_back({vertices_ndc , vertices_in[i].color});

	}

}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
