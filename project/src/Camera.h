#pragma once
#include <cassert>
#include <iostream>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//ONB => invViewMatrix
			//Inverse(ONB) => ViewMatrix

			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			viewMatrix = Matrix{
					{right.x,   right.y,   right.z,   0},
					{up.x,	    up.y,      up.z,      0},
					{forward.x, forward.y, forward.z, 0},
					{origin.x,  origin.y,   origin.z, 1}
			};

			invViewMatrix = viewMatrix.Inverse();

			// return cameraToWorld;


			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			//TODO W3
			// Matrix::CreatePerspectiveFovLH()

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			Vector3 input;
			if(pKeyboardState[SDL_SCANCODE_W])
			{
				input.z += 1;
			}
			if(pKeyboardState[SDL_SCANCODE_S])
			{
				input.z -= 1;
			}
			if(pKeyboardState[SDL_SCANCODE_A])
			{
				input.x -= 1;
			}
			if(pKeyboardState[SDL_SCANCODE_D])
			{
				input.x += 1;
			}

			if(mouseState == SDL_BUTTON_RMASK)
			{
				totalPitch += float(mouseY) * 0.01f;
				totalYaw += float(mouseX) * 0.01f;
			}
			else if(mouseState == SDL_BUTTON_LMASK)
			{
				input.z += -float(mouseY) * 0.3f;

				totalYaw += float(mouseX) * 0.01f;
			}

			std::cout << input.z << " " << input.x << '\n';

			forward = Matrix::CreateRotation(totalPitch, totalYaw, 0).TransformVector(Vector3::UnitZ);
			forward.Normalize();
			origin += (forward * input.z + right * input.x) * 5 * deltaTime;

			//Update Matrices
			CalculateViewMatrix();
			// CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}
