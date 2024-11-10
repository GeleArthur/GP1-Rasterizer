#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels }
	{
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		//TODO
		//Load SDL_Surface using IMG_LOAD
		//Create & Return a new Texture Object (using SDL_Surface)
		SDL_Surface* pSurface = IMG_Load(path.c_str());
		Texture* pTexture = new Texture(pSurface);

		return pTexture;
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		Uint8 red, green, blue;
		Vector2 clamped = {std::clamp(uv.x, 0.0f, 1.0f), std::clamp(uv.y, 0.0f, 1.0f)};
		
		SDL_GetRGB(m_pSurfacePixels[
			static_cast<int>(clamped.x * static_cast<float>(m_pSurface->w - 1)) +
			static_cast<int>(clamped.y * static_cast<float>(m_pSurface->h - 1)) * (m_pSurface->w)
			],
			m_pSurface->format, &red, &green, &blue);

		return ColorRGB{static_cast<float>(red)/255.0f, static_cast<float>(green)/255.0f, static_cast<float>(blue)/255.0f};
	}
}