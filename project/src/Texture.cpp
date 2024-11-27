#include "Texture.h"

#include <memory>

#include <SDL_image.h>

#include "Vector.h"

static Uint8 lookup_0[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
	83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117,
	118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
	150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181,
	182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213,
	214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245,
	246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

static Uint8 lookup_1[] = {
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80,
	82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148,
	150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212,
	214, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 255
};

static Uint8 lookup_2[] = {
	0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149,
	153, 157, 161, 165, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 238, 242, 246, 250, 255
};

static Uint8 lookup_3[] = {
	0, 8, 16, 24, 32, 41, 49, 57, 65, 74, 82, 90, 98, 106, 115, 123, 131, 139, 148, 156, 164, 172, 180, 189, 197, 205, 213, 222, 230, 238, 246, 255
};

static Uint8 lookup_4[] = {
	0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255
};

static Uint8 lookup_5[] = {
	0, 36, 72, 109, 145, 182, 218, 255
};

static Uint8 lookup_6[] = {
	0, 85, 170, 255
};

static Uint8 lookup_7[] = {
	0, 255
};

static Uint8 lookup_8[] = {
	255
};

Uint8* SDL_expand_byte[9] = {
	lookup_0,
	lookup_1,
	lookup_2,
	lookup_3,
	lookup_4,
	lookup_5,
	lookup_6,
	lookup_7,
	lookup_8
};

namespace dae
{
	Texture::Texture(SDL_Surface* p_surface) :
		m_pSurface{ p_surface },
		m_pSurfacePixels{ static_cast<uint32_t*>(p_surface->pixels) }
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

	dae::ColorRGB Texture::Sample(const Vector<2, float>& uv) const
	{
		Uint8 red, green, blue;
		Vector<2, float> clamped = { std::clamp(uv.x, 0.0f, 1.0f), std::clamp(uv.y, 0.0f, 1.0f) };

		const uint32_t pixel = m_pSurfacePixels[
			static_cast<int>(clamped.x * static_cast<float>(m_pSurface->w - 1)) +
			static_cast<int>(clamped.y * static_cast<float>(m_pSurface->h - 1)) * (m_pSurface->w)
		];

		unsigned int v;
		v = (pixel & m_pSurface->format->Rmask) >> m_pSurface->format->Rshift;
		red = SDL_expand_byte[m_pSurface->format->Rloss][v];
		v = (pixel & m_pSurface->format->Gmask) >> m_pSurface->format->Gshift;
		green = SDL_expand_byte[m_pSurface->format->Gloss][v];
		v = (pixel & m_pSurface->format->Bmask) >> m_pSurface->format->Bshift;
		blue = SDL_expand_byte[m_pSurface->format->Bloss][v];

		// m_pSurface->format.
		// SDL_GetRGB(m_pSurfacePixels[
		// 	static_cast<int>(clamped.x * static_cast<float>(m_pSurface->w - 1)) +
		// 	static_cast<int>(clamped.y * static_cast<float>(m_pSurface->h - 1)) * (m_pSurface->w)
		// 	],
		// 	m_pSurface->format, &red, &green, &blue);

		return dae::ColorRGB{ static_cast<float>(red) / 255.0f, static_cast<float>(green) / 255.0f, static_cast<float>(blue) / 255.0f };
	}
	std::unique_ptr<Texture> Texture::LoadFromFile(const std::string& path)
	{
		SDL_Surface* pSurface = IMG_Load(path.c_str());
		return std::unique_ptr<Texture>(new Texture{pSurface});
	}

}
