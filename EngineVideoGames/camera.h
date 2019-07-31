#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Camera
{
public:
	Camera(const glm::vec3& pos, float fov, float aspect, float zNear, float zFar)
	{
		this->pos = pos;
		this->forward = glm::vec3(0.0f, 0.0f, -1.0f);
		this->up = glm::vec3(0.0f, 1.0f, 0.0f);
		this->projection = glm::perspective(fov, aspect, zNear, zFar);
		this->projection = this->projection * glm::lookAt(pos, pos + forward, up);
		this->fov = fov;
		this->near = zNear;
		this->far = zFar;
		this->relation = aspect;
	}

	void setProjection( float aspect, float zNear, float zFar)
	{
		this->projection = glm::perspective(fov,aspect, zNear, zFar)* glm::lookAt(pos, pos + forward, up);
		this->near = zNear;
		this->far = zFar;
		this->relation = aspect;
	}

	inline glm::mat4 GetViewProjection() const
	{
		return projection ;
	}

	

	void MoveForward(float amt)
	{
		pos += forward * amt;
	}

	void MoveRight(float amt)
	{
		pos += glm::cross(up, forward) * amt;
	}

	void Pitch(float angle)
	{
		glm::vec3 right = glm::normalize(glm::cross(up, forward));

		forward = glm::vec3(glm::normalize(glm::rotate(angle, right) * glm::vec4(forward, 0.0)));
		up = glm::normalize(glm::cross(forward, right));
	}

	void RotateY(float angle)
	{
		static const glm::vec3 UP(0.0f, 1.0f, 0.0f);

		glm::mat4 rotation = glm::rotate(angle, UP);

		forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
		up = glm::vec3(glm::normalize(rotation * glm::vec4(up, 0.0)));
	}
	inline float GetAngle()
	{
		return fov;
	}

	inline float GetNear()
	{
		return near;
	}
	inline float GetFar()
	{
		return far;
	}
	inline float GetWHRelation()
	{
		return relation;
	}
protected:
private:
	glm::mat4 projection;
	glm::vec3 pos;
	glm::vec3 forward;
	glm::vec3 up;
	float fov;
	float far,near,relation;
};

#endif