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

		Camera(const Vector<3,float>& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle},
			nearPlane{1.0f},
			farPlane{1000.0f}
		{
		}


		Vector<3,float> origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector<3,float> forward{Vector<3,float>::UnitZ};
		Vector<3,float> up{Vector<3,float>::UnitY};
		Vector<3,float> right{Vector<3,float>::UnitX};

		float totalPitch{};
		float totalYaw{};
		float speed{1};

		Matrix<float> invViewMatrix{};
		Matrix<float> viewMatrix{};

		float nearPlane{};
		float farPlane{};

		void Initialize(float _fovAngle = 90.f, Vector<3,float> _origin = {0.f,0.f,0.f}, float _nearPlane = 1.0f, float _farPlane = 1000.f)
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);
			nearPlane = _nearPlane;
			farPlane = _farPlane;

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			//TODO W1
			//ONB => invViewMatrix
			//Inverse(ONB) => ViewMatrix

			right = Vector<3,float>::Cross(Vector<3,float>::UnitY, forward).Normalized();
			up = Vector<3,float>::Cross(forward, right).Normalized();

			viewMatrix = Matrix<float>{
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

			Vector<3,float> input{};
			if(pKeyboardState[SDL_SCANCODE_W])
			{
				input.z += speed;
			}
			if(pKeyboardState[SDL_SCANCODE_S])
			{
				input.z -= speed;
			}
			if(pKeyboardState[SDL_SCANCODE_A])
			{
				input.x -= speed;
			}
			if(pKeyboardState[SDL_SCANCODE_D])
			{
				input.x += speed;
			}

			if(mouseState == SDL_BUTTON_RMASK)
			{
				totalPitch += -float(mouseY) * 0.01f;
				totalYaw += float(mouseX) * 0.01f;
			}
			else if(mouseState == SDL_BUTTON_LMASK)
			{
				input.z += -float(mouseY) * 0.3f;

				totalYaw += float(mouseX) * 0.01f;
			}


			forward = Matrix<float>::CreateRotation(totalPitch, totalYaw, 0).TransformVector(Vector<3,float>::UnitZ);
			forward.Normalize();
			origin += (forward * input.z + right * input.x) * 5 * deltaTime;

			//Update Matrices
			CalculateViewMatrix();
			// CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}
