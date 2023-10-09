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

			ew::Mat4 scaleMatrix = Scale(scale);
			ew::Mat4 rotMatrix = RotateZ(rotation.z) * RotateX(rotation.x) * RotateY(rotation.y);
			ew::Mat4 transMatrix = Translate(position);
			ew::Mat4 modelMatrix = transMatrix * rotMatrix * scaleMatrix;
			return modelMatrix;
		}
	};
}