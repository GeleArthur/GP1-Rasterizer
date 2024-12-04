#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"
#include "DataTypes.h"

struct SDL_Window;
struct SDL_Surface;


namespace dae
{
	struct Vertex_Out;
	struct Mesh;
	struct Vertex;
	class Timer;
	class Texture;
	class Scene;

	enum class ShadingMode
	{
		observed_area,
		diffuse,
		specular,
		combined
	};

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render();
		void RenderPixels(const Vertex_Out& vertex0, const Vertex_Out& vertex1, const Vertex_Out& vertex2, std::vector<float>& depth_buffer);

		bool SaveBufferToImage() const;
		bool FrustemCulling(const Vector<3,float>& v0, const Vector<3,float>& v1, const Vector<3,float>& v2) const;
		bool CheckInFrustum(const Vector<3, float>& v) const;
		ColorRGB FragmentShader(const Vertex_Out& vertexIn, float diffuseReflectance, float shininess);
		std::vector<Vector<3,float>>& GetLights();

		void VertexStage(const std::vector<Vertex>& vertices_in, std::vector<Vertex_Out>& vertices_out, const Matrix<float>& world_matrix) const;

		void ChangeShadingMode();
		void ToggleRotation();
		void ToggleNormalMaps();
		void ToggleDepthRendering();
		Camera& GetCamera();

		template<typename T>
		using Buffer2D= std::vector<std::vector<T>>;

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};
		std::vector<float> depthBuffer{};

		std::vector<std::unique_ptr<Mesh>> m_Meshes;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Texture> m_NormalTexture;
		std::unique_ptr<Texture> m_SpecularTexture;
		std::unique_ptr<Texture> m_GlossTexture;
		std::vector<Vector<3,float>> m_Lights;

		Buffer2D<Vertex_Out> m_BinnedVertexOut;
		std::vector<int> m_CoresIds;
		unsigned int m_CoreCount;

		Camera m_Camera{};

		int m_Width{};
		int m_Height{};

		bool m_DEBUG_MoveMouse{};

		bool m_RenderDepth{};
		ShadingMode m_ShadingMode{ShadingMode::combined};
		bool m_Rotating{true};
		bool m_UseNormalMaps{true};

	};
}
