#pragma once
#include <memory>
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"
#include "Vector.h"

namespace dae
{
	class Texture
	{
	public:
		~Texture();

		static std::unique_ptr<Texture> LoadFromFile(const std::string& path);
		ColorRGB Sample(const Vector<2,float>& uv) const;

	private:
		explicit Texture(SDL_Surface* p_surface);

		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
	};
}