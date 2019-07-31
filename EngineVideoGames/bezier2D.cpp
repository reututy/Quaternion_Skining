#include "bezier2D.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <glm/geometric.hpp>
#include <glm/gtx/perpendicular.hpp>

#define BLUE glm::vec3(0.2667f, 0.3137f, 0.6196f)
#define BLUEL glm::vec3(0.0f, 0.0f, 1.0f)
#define CIRCLE_CONST 0.55228475
#define epsilon 1e-10

Bezier2D::Bezier2D(void)
{
	circularSubdivision = 4;
}

Bezier2D::Bezier2D(Bezier1D &b, glm::vec3 axis, int circularSubdivision)
{
	this->circularSubdivision = circularSubdivision;
	this->main_curve = b;
	this->axis = axis;
}

Bezier2D::~Bezier2D(void)
{

}

IndexedModel Bezier2D::GetSurface(int resT, int resS)
{
	IndexedModel index_model;
	int segS = 0, segT = 0;
	float t = 0.0;
	float s = 0.0;
	float t_inc = (float)1 / (resT - 1);
	float s_inc = (float)1 / (resS - 1);
	glm::vec3 vec_pos_top_left;
	glm::vec3 vec_pos_top_right;
	glm::vec3 vec_pos_bottom_right;
	glm::vec3 vec_pos_bottom_left;

	for (int main_curve_runner = 0; main_curve_runner < main_curve.GetNumSegs()*resT - 1; main_curve_runner++)
	{
		if (main_curve_runner != 0 && main_curve_runner % resT == 0)
		{
			segT++;
		}
		if (main_curve_runner % (resT) == 0)
		{
			t = 0.0;
		}
		//std::cout << "SegT is: " << segT << "  Iteration No: " << main_curve_runner << std::endl;
		for (int second_curve_runner = 0; second_curve_runner < resS*circularSubdivision - 1; second_curve_runner++)
		{
			if (main_curve_runner != 0 && main_curve_runner % resS == 0)
			{
				segS++;
			}
			if (second_curve_runner % (resS) == 0)
			{
				s = 0.0;
			}

			vec_pos_top_left = *GetVertex(segT, segS, t, s).GetPos();
			vec_pos_top_right = *GetVertex(segT, segS, t + t_inc, s).GetPos();
			vec_pos_bottom_left = *GetVertex(segT, segS, t, s + s_inc).GetPos();
			vec_pos_bottom_right = *GetVertex(segT, segS, t + t_inc, s + s_inc).GetPos();
			index_model.positions.push_back(vec_pos_top_left);
			index_model.positions.push_back(vec_pos_top_right);
			index_model.positions.push_back(vec_pos_bottom_right);
			index_model.positions.push_back(vec_pos_bottom_left);
			index_model.colors.push_back(BLUE);
			index_model.colors.push_back(BLUE);
			index_model.colors.push_back(BLUE);
			index_model.colors.push_back(BLUE);
			index_model.normals.push_back(GetNormal(segT, segS, t, s));
			index_model.normals.push_back(GetNormal(segT, segS, t + t_inc, s));
			index_model.normals.push_back(GetNormal(segT, segS, t + t_inc, s + s_inc));
			index_model.normals.push_back(GetNormal(segT, segS, t, s + s_inc));
			index_model.indices.push_back((main_curve_runner*(resS*circularSubdivision - 1) * 4 + second_curve_runner));
			index_model.indices.push_back((main_curve_runner*(resS*circularSubdivision - 1) * 4 + second_curve_runner) + 1);
			index_model.indices.push_back((main_curve_runner*(resS*circularSubdivision - 1) * 4 + second_curve_runner) + 2);
			index_model.indices.push_back((main_curve_runner*(resS*circularSubdivision - 1) * 4 + second_curve_runner) + 3);
			index_model.texCoords.push_back(glm::vec2(t, s));
			index_model.texCoords.push_back(glm::vec2(t + t_inc, s));
			index_model.texCoords.push_back(glm::vec2(t + t_inc, s + s_inc));
			index_model.texCoords.push_back(glm::vec2(t, s + s_inc));
			/* New Added: */
			index_model.weights.push_back(glm::normalize(CalcWeight(segT, segS, t, s)));
			index_model.weights.push_back(glm::normalize(CalcWeight(segT, segS, t + t_inc, s)));
			index_model.weights.push_back(glm::normalize(CalcWeight(segT, segS, t + t_inc, s + s_inc)));
			index_model.weights.push_back(glm::normalize(CalcWeight(segT, segS, t, s + s_inc)));

			s += s_inc;
		}
		t += t_inc;
	}
	return index_model;
}

Vertex Bezier2D::GetVertex(int segmentT, int segmentS, float t, float s)
{
	glm::vec4 pos_vec_t = glm::vec4(*(main_curve.GetVertex(segmentT, t)).GetPos(), 1);
	//axis = glm::vec3(0, 0, 1); //added
	axis = glm::vec3(1, 0, 0); //added
	pos_vec_t = pos_vec_t*glm::rotate(360 * s, axis);
	glm::vec3 normal = GetNormal(segmentT, segmentS, t, s);
	return Vertex(glm::vec3(pos_vec_t), glm::vec2(0.0f, 0.0f), normal, BLUEL);
}

glm::vec3 Bezier2D::GetNormal(int segmentT, int segmentS, float t, float s)
{
	glm::vec3 velT = main_curve.GetVelosity(segmentT, t);
	glm::vec3 posT = *(main_curve.GetVertex(segmentT, t)).GetPos();

	//axis = glm::vec3(0, 0, 1); //added
	axis = glm::vec3(1, 0, 0); //added

	glm::vec3 c0 = *(main_curve.GetControlPoint(0, 0)).GetPos();
	glm::vec3 normalized_axis = glm::normalize(axis);
	glm::mat4 rotateMat = glm::rotate(360.0f * s, axis);
	glm::vec3 posS = glm::vec3(rotateMat * glm::vec4(posT, 1));

	glm::vec3 radius_vec = posS - c0;
	glm::vec3 center = c0 + normalized_axis * (glm::dot(radius_vec, normalized_axis));
	glm::vec3 velS = glm::cross(radius_vec, axis);

	if (glm::length(velS) < epsilon)
		return axis;

	glm::vec3 normal = -glm::normalize(glm::cross(velT, velS));
	return normal;
}

glm::vec3 Bezier2D::CalcWeight(int segmentT, int segmentS, float t, float s)
{
	float f1 = 0, f3 = 0;
	if (t > 0.5)
		f3 = (1 - 4.0f*(1 - t)*t)*(1 - t) / 2.0f + (1 - 4.0f*(1 - t)*t)*t / 2.0f;
	else
		f1 = (1 - 4.0f*(1 - t)*t)*(1 - t) / 2.0f + (1 - 4.0f*(1 - t)*t)*t / 2.0f;
	float f2 = (2.0f*(1 - t)*(t + 0.0) + 0.5f);
	return glm::vec3(f1, f2, f3);
}