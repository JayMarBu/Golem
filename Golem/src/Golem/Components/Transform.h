#pragma once
#include "Component.h"
#include "../DataStructures/TreeNode.h"


namespace golem
{
	class Transform : public Component, public TreeNode<Transform>
	{
		// Members ********************************************************************************
	public:
		glm::vec3 translation;

		glm::quat rotation;

		glm::vec3 scale;

	private:
		glm::vec3 rotVec;

		// Methods ********************************************************************************
	public:
		Transform();
		Transform(const glm::vec3& pos, const glm::quat& rot = glm::quat(1.0f,0.0f,0.0f,0.0f), const glm::vec3& scale = glm::vec3(1.0f));

		void Rotation(const float x, const float y, const float z, bool degrees = true);
		void Rotation(const glm::vec3& rot, bool degrees = true) { Rotation(rot.x,rot.y,rot.z, degrees);}

		void RotationX(const float x, bool degrees = true);
		void RotationY(const float y, bool degrees = true);
		void RotationZ(const float z, bool degrees = true);

		glm::vec3 CalculateEulerAngles();
		glm::vec3 EulerAngles(bool InDegrees = true) { if(InDegrees){return rotVec;} else { return glm::radians(rotVec);}}

		glm::mat4 Mat4();
		operator glm::mat4() {return Mat4();}

		glm::mat3 NormalMatrix();
	};
}