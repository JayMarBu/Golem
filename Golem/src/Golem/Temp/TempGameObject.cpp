#include "golpch.h"
#include "TempGameObject.h"

namespace golem
{

	glm::mat4 CTransform::mat4()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		glm::mat4 mat{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f} };

		if (parent == nullptr)
			return mat;

		return parent->mat4() * mat;
	}

	glm::mat3 CTransform::invRotationMat()
	{
		glm::mat3 rotX{
			{1, 0, 0},
			{0, glm::cos(rotation.x), -glm::sin(rotation.x)},
			{0, glm::sin(rotation.x),  glm::cos(rotation.x)}
		};
		glm::mat3 rotY{
			{ glm::cos(rotation.y), 0, glm::sin(rotation.y)},
			{0,	1, 0},
			{-glm::sin(rotation.y), 0, glm::cos(rotation.y)}
		};
		glm::mat3 rotZ{
			{glm::cos(rotation.x), -glm::sin(rotation.x), 0},
			{glm::sin(rotation.x),  glm::cos(rotation.x), 0},
			{0, 0, 1}
		};

		return rotX * rotY * rotZ;
	}

	glm::mat3 CTransform::normalMatrix()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);

		const glm::vec3 inverseScale = 1.0f / scale;

		return glm::mat3{
			{
				inverseScale.x * (c1 * c3 + s1 * s2 * s3),
				inverseScale.x * (c2 * s3),
				inverseScale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				inverseScale.y * (c3 * s1 * s2 - c1 * s3),
				inverseScale.y * (c2 * c3),
				inverseScale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				inverseScale.z * (c2 * s1),
				inverseScale.z * (-s2),
				inverseScale.z * (c1 * c2),
			} };
	}

	glm::mat4 CTransform::positionRotMat()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		glm::mat4 mat{
			{
				(c1 * c3 + s1 * s2 * s3),
				(c2 * s3),
				(c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				(c3 * s1 * s2 - c1 * s3),
				(c2 * c3),
				(c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				(c2 * s1),
				(-s2),
				(c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f} };

		if (parent == nullptr)
			return mat;

		return parent->positionRotMat() * mat;
	}

	glm::mat4 CTransform::scaleMat()
	{
		return glm::scale(glm::mat4{ 1 }, scale);
	}

}