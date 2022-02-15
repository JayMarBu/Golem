#include "golpch.h"
#include "Transform.h"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtc/matrix_inverse.hpp"

namespace golem
{

	Transform::Transform()
		:translation{0.0f}, rotation{1.0f,0.0f,0.0f,0.0f}, scale{1.0f}, rotVec{0.0f}
	{
	}

	Transform::Transform(const glm::vec3& pos, const glm::quat& rot /*= glm::quat(1.0f,0.0f,0.0f,0.0f)*/, const glm::vec3& s /*= glm::vec3(1.0f)*/)
		: translation{ pos }, rotation{ rot }, scale{ s }, rotVec{ CalculateEulerAngles() }
	{
		
	}

	void Transform::Rotation(const float deg_x, const float deg_y, const float deg_z, bool degrees)
	{
		float yaw, pitch, roll;

		if(degrees)
		{
			rotVec = {deg_x, deg_y, deg_z};

			yaw = glm::radians(deg_x);
			pitch = glm::radians(deg_y);
			roll = glm::radians(deg_z);
		}
		else
		{
			rotVec = { glm::degrees(deg_x), glm::degrees(deg_y), glm::degrees(deg_z) };

			yaw = deg_x;
			pitch = deg_y;
			roll = deg_z;
		}

		rotation.x = glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) - glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
		rotation.y = glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2);
		rotation.z = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2) - glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2);
		rotation.w = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
	}

	void Transform::RotationX(const float deg_x, bool degrees /*= true*/)
	{
		float yaw, pitch, roll;

		if (degrees)
		{
			rotVec.x = deg_x;
			yaw = glm::radians(deg_x);
		}
		else
		{
			rotVec.x = glm::degrees(deg_x);
			yaw = deg_x;
		}

		pitch = glm::radians(rotVec.y);
		roll = glm::radians(rotVec.z);

		rotation.x = glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) - glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
		rotation.y = glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2);
		rotation.z = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2) - glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2);
		rotation.w = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
	}

	void Transform::RotationY(const float deg_y, bool degrees /*= true*/)
	{
		float yaw, pitch, roll;

		if (degrees)
		{
			rotVec.y = deg_y;
			pitch = glm::radians(deg_y);
		}
		else
		{
			rotVec.y = glm::degrees(deg_y);
			pitch = deg_y;
		}

		yaw = glm::radians(rotVec.x);
		roll = glm::radians(rotVec.z);

		rotation.x = glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) - glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
		rotation.y = glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2);
		rotation.z = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2) - glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2);
		rotation.w = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
	}

	void Transform::RotationZ(const float deg_z, bool degrees /*= true*/)
	{
		float yaw, pitch, roll;

		if (degrees)
		{
			rotVec.z = deg_z;
			roll = glm::radians(deg_z);
		}
		else
		{
			rotVec.x = glm::degrees(deg_z);
			roll = deg_z;
		}

		yaw = glm::radians(rotVec.x);
		pitch = glm::radians(rotVec.y);

		rotation.x = glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) - glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
		rotation.y = glm::cos(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2);
		rotation.z = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::sin(yaw / 2) - glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::cos(yaw / 2);
		rotation.w = glm::cos(roll / 2) * glm::cos(pitch / 2) * glm::cos(yaw / 2) + glm::sin(roll / 2) * glm::sin(pitch / 2) * glm::sin(yaw / 2);
	}

	glm::vec3 Transform::CalculateEulerAngles()
	{
		float x,y,z,w;

		x = rotation.x;
		y = rotation.y;
		z = rotation.z;
		w = rotation.w;

		float t0 = 2.0 * (w * x + y * z);
		float t1 = 1.0 - 2.0 * (x * x + y * y);
		float roll = glm::atan(t0, t1);

		float t2 = +2.0 * (w * y - z * x);

		t2 = (t2 > 1.0)? 1.0 : t2;
		t2 = (t2 < -1.0)? -1.0 : t2;

		//t2 = +1.0 if t2 > +1.0 else t2
		//t2 = -1.0 if t2 < -1.0 else t2

		float pitch = glm::asin(t2);

		float t3 = 2.0 * (w * z + x * y);
		float t4 = 1.0 - 2.0 * (y * y + z * z);
		float yaw = glm::atan(t3, t4);
		return {glm::degrees(yaw), glm::degrees(pitch), glm::degrees(roll)};
	}

	glm::mat4 Transform::Mat4()
	{
		glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translation);
		glm::mat4 rotationMat = glm::toMat4(rotation);
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

		return (translationMat * rotationMat * scaleMat);
	}

	glm::mat3 Transform::NormalMatrix()
	{
		glm::mat3 modelMat(Mat4());
		glm::mat3 normalMat = glm::inverseTranspose(modelMat);

		return normalMat;
	}

}