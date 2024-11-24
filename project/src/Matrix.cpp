#include "Matrix.h"

#include <cassert>

#include "MathHelpers.h"
#include <cmath>

namespace dae {
	Matrix::Matrix(const Vector<3,float>& xAxis, const Vector<3,float>& yAxis, const Vector<3,float>& zAxis, const Vector<3,float>& t) :
		Matrix({ xAxis, 0 }, { yAxis, 0 }, { zAxis, 0 }, { t, 1 })
	{
	}

	Matrix::Matrix(const Vector<4,float>& xAxis, const Vector<4,float>& yAxis, const Vector<4,float>& zAxis, const Vector<4,float>& t)
	{
		data[0] = xAxis;
		data[1] = yAxis;
		data[2] = zAxis;
		data[3] = t;
	}

	Matrix::Matrix(const Matrix& m)
	{
		data[0] = m[0];
		data[1] = m[1];
		data[2] = m[2];
		data[3] = m[3];
	}

	Vector<3,float> Matrix::TransformVector(const Vector<3,float>& v) const
	{
		return TransformVector(v.x, v.y, v.z);
	}

	Vector<3,float> Matrix::TransformVector(float x, float y, float z) const
	{
		return Vector<3,float>{
			data[0].x * x + data[1].x * y + data[2].x * z,
			data[0].y * x + data[1].y * y + data[2].y * z,
			data[0].z * x + data[1].z * y + data[2].z * z
		};
	}

	Vector<3,float> Matrix::TransformPoint(const Vector<3,float>& p) const
	{
		return TransformPoint(p.x, p.y, p.z);
	}

	Vector<3,float> Matrix::TransformPoint(float x, float y, float z) const
	{
		return Vector<3,float>{
			data[0].x * x + data[1].x * y + data[2].x * z + data[3].x,
			data[0].y * x + data[1].y * y + data[2].y * z + data[3].y,
			data[0].z * x + data[1].z * y + data[2].z * z + data[3].z,
		};
	}

	Vector<4,float> Matrix::TransformPoint(const Vector<4,float>& p) const
	{
		return TransformPoint(p.x, p.y, p.z, p.w);
	}

	Vector<4,float> Matrix::TransformPoint(float x, float y, float z, float w) const
	{
		return Vector<4,float>{
			data[0].x * x + data[1].x * y + data[2].x * z + data[3].x,
			data[0].y * x + data[1].y * y + data[2].y * z + data[3].y,
			data[0].z * x + data[1].z * y + data[2].z * z + data[3].z,
			data[0].w * x + data[1].w * y + data[2].w * z + data[3].w
		};
	}

	const Matrix& Matrix::Transpose()
	{
		Matrix result{};
		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				result[r][c] = data[c][r];
			}
		}

		data[0] = result[0];
		data[1] = result[1];
		data[2] = result[2];
		data[3] = result[3];

		return *this;
	}

	const Matrix& Matrix::Inverse()
	{
		//Optimized Inverse as explained in FGED1 - used widely in other libraries too.
		const Vector<3,float>& a = data[0];
		const Vector<3,float>& b = data[1];
		const Vector<3,float>& c = data[2];
		const Vector<3,float>& d = data[3];

		const float x = data[0][3];
		const float y = data[1][3];
		const float z = data[2][3];
		const float w = data[3][3];

		Vector<3,float> s = Vector<3,float>::Cross(a, b);
		Vector<3,float> t = Vector<3,float>::Cross(c, d);
		Vector<3,float> u = a * y - b * x;
		Vector<3,float> v = c * w - d * z;

		float det = Vector<3,float>::Dot(s, v) + Vector<3,float>::Dot(t, u);
		assert((!AreEqual(det, 0.f)) && "ERROR: determinant is 0, there is no INVERSE!");
		float invDet = 1.f / det;

		s *= invDet; t *= invDet; u *= invDet; v *= invDet;

		Vector<3,float> r0 = Vector<3,float>::Cross(b, v) + t * y;
		Vector<3,float> r1 = Vector<3,float>::Cross(v, a) - t * x;
		Vector<3,float> r2 = Vector<3,float>::Cross(d, u) + s * w;
		Vector<3,float> r3 = Vector<3,float>::Cross(u, c) - s * z;

		data[0] = Vector<4,float>{ r0.x, r1.x, r2.x, 0.f };
		data[1] = Vector<4,float>{ r0.y, r1.y, r2.y, 0.f };
		data[2] = Vector<4,float>{ r0.z, r1.z, r2.z, 0.f };
		data[3] = { -Vector<3,float>::Dot(b, t), Vector<3,float>::Dot(a, t),-Vector<3,float>::Dot(d, s),Vector<3,float>::Dot(c, s) };

		return *this;
	}

	Matrix Matrix::Transpose(const Matrix& m)
	{
		Matrix out{ m };
		out.Transpose();

		return out;
	}

	Matrix Matrix::Inverse(const Matrix& m)
	{
		Matrix out{ m };
		out.Inverse();

		return out;
	}

	Matrix Matrix::CreateLookAtLH(const Vector<3,float>& origin, const Vector<3,float>& forward, const Vector<3,float>& up)
	{
		//TODO W1


		return {};
	}

	Matrix Matrix::CreatePerspectiveFovLH(float fov, float aspect, float near, float far)
	{
		return {
			{1.0f/(aspect*fov),0       ,0                     ,0},
			{0                ,1.0f/fov,0                     ,0},
			{0                ,0       ,far/(far-near)        ,1.0f},
			{0                ,0       ,(- (far * near)) / (far - near),0}
		};


	}

	Vector<3,float> Matrix::GetAxisX() const
	{
		return data[0];
	}

	Vector<3,float> Matrix::GetAxisY() const
	{
		return data[1];
	}

	Vector<3,float> Matrix::GetAxisZ() const
	{
		return data[2];
	}

	Vector<3,float> Matrix::GetTranslation() const
	{
		return data[3];
	}

	Matrix Matrix::CreateTranslation(float x, float y, float z)
	{
		return CreateTranslation({ x, y, z });
	}

	Matrix Matrix::CreateTranslation(const Vector<3,float>& t)
	{
		return { Vector<3,float>::UnitX, Vector<3,float>::UnitY, Vector<3,float>::UnitZ, t };
	}

	Matrix Matrix::CreateRotationX(float pitch)
	{
		return {
			{1, 0, 0, 0},
			{0, cos(pitch), -sin(pitch), 0},
			{0, sin(pitch), cos(pitch), 0},
			{0, 0, 0, 1}
		};
	}

	Matrix Matrix::CreateRotationY(float yaw)
	{
		return {
			{cos(yaw), 0, -sin(yaw), 0},
			{0, 1, 0, 0},
			{sin(yaw), 0, cos(yaw), 0},
			{0, 0, 0, 1}
		};
	}

	Matrix Matrix::CreateRotationZ(float roll)
	{
		return {
			{cos(roll), sin(roll), 0, 0},
			{-sin(roll), cos(roll), 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1}
		};
	}

	Matrix Matrix::CreateRotation(float pitch, float yaw, float roll)
	{
		return CreateRotation({ pitch, yaw, roll });
	}

	Matrix Matrix::CreateRotation(const Vector<3,float>& r)
	{
		return CreateRotationX(r[0]) * CreateRotationY(r[1]) * CreateRotationZ(r[2]);
	}

	Matrix Matrix::CreateScale(float sx, float sy, float sz)
	{
		return { {sx, 0, 0}, {0, sy, 0}, {0, 0, sz}, Vector<3,float>::Zero };
	}

	Matrix Matrix::CreateScale(const Vector<3,float>& s)
	{
		return CreateScale(s[0], s[1], s[2]);
	}

#pragma region Operator Overloads
	Vector<4,float>& Matrix::operator[](int index)
	{
		assert(index <= 3 && index >= 0);
		return data[index];
	}

	Vector<4,float> Matrix::operator[](int index) const
	{
		assert(index <= 3 && index >= 0);
		return data[index];
	}

	Matrix Matrix::operator*(const Matrix& m) const
	{
		Matrix result{};
		Matrix m_transposed = Transpose(m);

		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				result[r][c] = Vector<4,float>::Dot(data[r], m_transposed[c]);
			}
		}

		return result;
	}

	const Matrix& Matrix::operator*=(const Matrix& m)
	{
		Matrix copy{ *this };
		Matrix m_transposed = Transpose(m);

		for (int r{ 0 }; r < 4; ++r)
		{
			for (int c{ 0 }; c < 4; ++c)
			{
				data[r][c] = Vector<4,float>::Dot(copy[r], m_transposed[c]);
			}
		}

		return *this;
	}

	bool Matrix::operator==(const Matrix& m) const
	{
		return data[0] == m.data[0]
		    && data[1] == m.data[1]
			&& data[2] == m.data[2]
			&& data[3] == m.data[3];
	}

#pragma endregion
}