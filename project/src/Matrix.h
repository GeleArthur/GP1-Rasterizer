#pragma once
#include "Vector.h"

namespace dae {
	struct Matrix
	{
		Matrix() = default;
		Matrix(
			const Vector<3,float>& xAxis,
			const Vector<3,float>& yAxis,
			const Vector<3,float>& zAxis,
			const Vector<3,float>& t);

		Matrix(
			const Vector<4,float>& xAxis,
			const Vector<4,float>& yAxis,
			const Vector<4,float>& zAxis,
			const Vector<4,float>& t);

		Matrix(const Matrix& m);

		Vector<3,float> TransformVector(const Vector<3,float>& v) const;
		Vector<3,float> TransformVector(float x, float y, float z) const;
		Vector<3,float> TransformPoint(const Vector<3,float>& p) const;
		Vector<3,float> TransformPoint(float x, float y, float z) const;

		Vector<4,float> TransformPoint(const Vector<4,float>& p) const;
		Vector<4,float> TransformPoint(float x, float y, float z, float w) const;

		const Matrix& Transpose();
		const Matrix& Inverse();

		Vector<3,float> GetAxisX() const;
		Vector<3,float> GetAxisY() const;
		Vector<3,float> GetAxisZ() const;
		Vector<3,float> GetTranslation() const;

		static Matrix CreateTranslation(float x, float y, float z);
		static Matrix CreateTranslation(const Vector<3,float>& t);
		static Matrix CreateRotationX(float pitch);
		static Matrix CreateRotationY(float yaw);
		static Matrix CreateRotationZ(float roll);
		static Matrix CreateRotation(float pitch, float yaw, float roll);
		static Matrix CreateRotation(const Vector<3,float>& r);
		static Matrix CreateScale(float sx, float sy, float sz);
		static Matrix CreateScale(const Vector<3,float>& s);
		static Matrix Transpose(const Matrix& m);
		static Matrix Inverse(const Matrix& m);

		static Matrix CreateLookAtLH(const Vector<3,float>& origin, const Vector<3,float>& forward, const Vector<3,float>& up);
		static Matrix CreatePerspectiveFovLH(float fovy, float aspect, float near, float far);

		Vector<4,float>& operator[](int index);
		Vector<4,float> operator[](int index) const;
		Matrix operator*(const Matrix& m) const;
		const Matrix& operator*=(const Matrix& m);
		bool operator==(const Matrix& m) const;

	private:

		//Row-Major Matrix
		Vector<4,float> data[4]
		{
			{1,0,0,0}, //xAxis
			{0,1,0,0}, //yAxis
			{0,0,1,0}, //zAxis
			{0,0,0,1}  //T
		};

		// v0x v0y v0z v0w
		// v1x v1y v1z v1w
		// v2x v2y v2z v2w
		// v3x v3y v3z v3w
	};
}