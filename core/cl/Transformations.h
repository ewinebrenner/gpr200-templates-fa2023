#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"

namespace celLib {
	inline ew::Mat4 Identity()
	{
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};

	inline ew::Mat4 Scale(ew::Vec3 s)
	{
		//scale matrix
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0,  0,  1
		);
	};

	inline ew::Mat4 RotateX(float rad)
	{
		//Rotation Matrix (X)
		// rotate around the x axis (pitch)
		return ew::Mat4(
			1, 0, 0, 0,
			0, cos(rad), -sin(rad), 0,
			0, sin(rad),  cos(rad), 0,
			0, 0, 0, 1

		);
	};

	inline ew::Mat4 RotateY(float rad)
	{
		//Rotation Matrix (Y)
		// rotate around the y axis (yaw)
		return ew::Mat4(
			cos(rad), 0, sin(rad), 0,
			0, 1, 0, 0,
			-sin(rad), 0, cos(rad), 0,
			0, 0, 0, 1
		);
	};

	inline ew::Mat4 RotateZ(float rad) 
	{
		//Rotation Matrix (Z)
		// rotate around the z-axis(roll)
		return ew::Mat4(
			cos(rad), -sin(rad), 0, 0,
			sin(rad), cos(rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};

	inline ew::Mat4 Translate(ew::Vec3 t) 
	{
		//Translation matrix, xyz
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	struct Transform 
	{
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const 
		{
			//TODO:Complete Transform::getModelMatrix
			//used to scale rotate and translate vertices in that order
			//for rotation, rotate around the Z axis, then X axis, then Y axis
			//Matrix multiplication goes from right to left

			ew::Mat4 scaleMatrix = celLib::Scale(scale);  //ambiguous call to overloaded function
			ew::Mat4 rotMatrix = RotateZ(rotation.z) * RotateX(rotation.x) * RotateY(rotation.y);
			ew::Mat4 transMatrix = celLib::Translate(position);
			ew::Mat4 modelMatrix = transMatrix * rotMatrix * scaleMatrix;
			return modelMatrix;
		}
	};

	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up)
	{
		//use ew::Cross for cross product!
		//T^-1eye * R^-1eye = viewMatrix
		ew::Vec3 forward = ew::Normalize(eye - target); 
		ew::Vec3 right = ew::Normalize(ew::Cross(up,forward));
		ew::Vec3 newUp = ew::Normalize(ew::Cross(forward, right));
		ew::Mat4 rotInverse = ew::Mat4(right.x, right.y, right.z, 0,
									   newUp.x, newUp.y, newUp.z, 0,
									   forward.x, forward.y, forward.z, 0,
									   0, 0, 0, 1);
		ew::Mat4 transInverse = ew::Mat4(1, 0, 0, -eye.x,
										 0, 1, 0, -eye.y, 
										 0, 0, 1, -eye.z, 
										 0, 0, 0, 1);
		return rotInverse * transInverse;

	};
	//Orthographic Projection
	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far)
	{
		//left(l) = near, right(r) = far, bottom(b) = -height, top(t) = height, near(n) , far(f)
		float right = (aspect * height)/2; //aspect / height * height?
		float left = -right / 2;
		float bottom = -height/2;
		float top = height/2;
		return ew::Mat4(2/(right - left), 0, 0, -(right + left)/(right - left),
					    0, 2/(top - bottom), 0, -(top+bottom)/(top-bottom),
						0, 0, -2/(far - near), -(far+near)/(far-near),
				        0, 0, 0, 1);
	};
	// fov = vertical aspect ratio (radians)
	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far)
	{
		//x,y,z,w
		return ew::Mat4(1/(tan(fov/2)*aspect), 0, 0, 0,
					    0, 1/(tan(fov/2)), 0, 0,
					    0, 0, (near+far)/(near-far), (2*far*near)/(near-far),
						0, 0, -1, 0);
	};
}