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

	const std::vector<Vertex> vertices_ndc{
		{{0.f, 0.5f, 1.0f}, colors::White},
		{{0.5f, -0.5f, 1.0f}, colors::White},
		{{-0.5f, -0.5f, 1.0f}, colors::White}
	};

	std::vector<Vertex> vertices_screen_space{};
	vertices_screen_space.reserve(vertices_ndc.size());

	VertexTransformationFunction(vertices_ndc, vertices_screen_space);

	for (auto pos = vertices_screen_space.begin(); pos != vertices_screen_space.end(); pos+=3)
	{
		for (int px{}; px < m_Width; ++px)
		{
			for (int py{}; py < m_Height; ++py)
			{

				Vector3 P = {static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f, 1.0f};
				Vector3 n = {0,0,1};

				{
					Vector3 e = (pos+1)->position - pos->position;
					Vector3 p = P - pos->position;

					if (Vector3::Dot(Vector3::Cross(e, p), n) < 0)
					{
						continue;
					}
				}
				{
					Vector3 e = (pos+2)->position - (pos+1)->position;
					Vector3 p = P - (pos+1)->position;

					if (Vector3::Dot(Vector3::Cross(e, p), n) < 0)
					{
						continue;
					}
				}
				{
					Vector3 e = (pos)->position - (pos+2)->position;
					Vector3 p = P - (pos+2)->position;

					if (Vector3::Dot(Vector3::Cross(e, p), n) < 0)
					{
						continue;
					}
				}


				ColorRGB finalColor{ 1, 1, 1 };

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
		const Vector3& vertices_ndc = vertices_in[i].position;

		vertices_out.push_back(Vertex{
			Vector3{(vertices_ndc.x+1)/2.0f*m_Width, (1-vertices_ndc.y)/2.0f*m_Height, vertices_ndc.z},
			vertices_in[i].color,
		});

	}

}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
