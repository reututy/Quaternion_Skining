#include "bezier1D.h"
#include <iostream>

extern int MIN_CTRL;
extern int MAX_CTRL;

#define BLUE glm::vec3(0.2667f, 0.3137f, 0.6196f)
#define BLUEL glm::vec3(0.0f, 0.0f, 1.0f)

void print_mat(glm::mat4 mat_to_print)
{
	for (int i = 0; i < 4; i++)
	{
		std::cout << mat_to_print[i].x << "   " << mat_to_print[i].y << "   "
			<< mat_to_print[i].z << "   " << mat_to_print[i].w << std::endl;
	}
}

Bezier1D::Bezier1D(void)
{
	AddSegment(glm::mat4(glm::vec4(0.0, 0.0, 0.0, 1.0), glm::vec4(0.0, 5.0, 0.0, 1.0),
		glm::vec4(5.0, 5.0, 0.0, 1.0), glm::vec4(5.0, 0.0, 0.0, 1.0)));

	AddSegment(glm::mat4(glm::vec4(5.0, 0.0, 0.0, 1.0), glm::vec4(5.0, -5.0, 0.0, 1.0),
		glm::vec4(10.0, -5.0, 0.0, 1.0), glm::vec4(10.0, 0.0, 0.0, 1.0)));

	AddSegment(glm::mat4(glm::vec4(10.0, 0.0, 0.0, 1.0), glm::vec4(10.0, 5.0, 0.0, 1.0),
		glm::vec4(15.0, 5.0, 0.0, 1.0), glm::vec4(15.0, 0.0, 0.0, 1.0)));
}

Bezier1D::Bezier1D(std::vector<glm::mat4> ctrlPointsVec) 
{
	for (glm::mat4 seg : ctrlPointsVec) 
		AddSegment(seg);
}

Bezier1D::~Bezier1D(void)
{
	std::cout << "THE DESTRUCTOR WAS SUMMONED" << std::endl;
}

IndexedModel Bezier1D::GetLine(int resT)
{
	IndexedModel index_model;
	float t = 0.0;
	float t_inc = (float)1 / (resT - 1);
	glm::vec3 vec_pos;

	for (int j = 0; j < segments.size(); j++)
	{
		t = 0.0;
		for (int i = 0; i < resT; i++)
		{
			vec_pos = *(GetVertex(j, t).GetPos());
			index_model.positions.push_back(vec_pos);	//TODO: verify order of insertion
			index_model.colors.push_back(BLUE);
			index_model.indices.push_back(j*resT + i);
			t += t_inc;
		}
	}
	return index_model;
}

LineVertex Bezier1D::GetVertex(int segment, float t)
{
	glm::vec4 pos_vec = pow((1 - t), 3)*segments[segment][0] +
		3 * pow((1 - t), 2)*t*segments[segment][1] +
		3 * (1 - t)*t*t*segments[segment][2] +
		t*t*t*segments[segment][3];
	return LineVertex(glm::vec3(pos_vec), BLUE);
}

LineVertex Bezier1D::GetControlPoint(int segment, int indx)
{
	glm::vec3 control_point = glm::vec3(segments.at(segment)[indx]);
	return LineVertex(control_point, BLUE);
}

glm::vec3 Bezier1D::GetVelosity(int segment, float t)
{
	glm::vec4 velosity = -3 * pow((1 - t), 2)*segments[segment][0] +
		(3 - 12 * t + 9 * t*t)*segments[segment][1] +
		(6 * t - 9 * t*t)*segments[segment][2] +
		(3 * t*t)*segments[segment][3];
	return glm::vec3(velosity);
}

void Bezier1D::MoveControlPoint(int segment, int indx, bool preserveC1, glm::vec4 newPosition)
{
	int seg_num = segment / 4;
	int indx_num = indx % 4;
	bool is_connector = ((indx_num == 0) && (seg_num != GetNumSegs()));
	bool forward = (indx_num == 2);
	bool backward = (indx_num == 1);
	glm::vec4 oldPosition = segments.at(seg_num)[indx_num];
	glm::vec4 direction = newPosition - oldPosition;
	segments.at(seg_num)[indx_num] = newPosition;
	//std::cout << "indx is: " << indx << std::endl;
	bool start_ctrl = ((seg_num) == 0 && (indx_num) == 0);
	bool end_ctrl = (seg_num == segments.size() - 1 && indx_num == 3);

	if (preserveC1)
	{
		if (is_connector)
		{
			segments.at((segment) / 4)[1] = segments.at((segment) / 4)[1] + direction;
			if (!start_ctrl)
			{
				segments.at((segment - 1) / 4)[3] = newPosition;
				segments.at((segment - 1) / 4)[2] = segments.at((segment - 1) / 4)[2] + direction;
			}
			//Move the adjacent control point according to the derivative.
		}
		else
		{
			if (forward && (seg_num != GetNumSegs()))
			{
				//Not in the last segment so moving the next segment is valid.
				segments.at(seg_num + 1)[1] = segments.at(seg_num)[3] + segments.at(seg_num)[3] - segments.at(seg_num)[indx_num];
			}
			if (backward && (seg_num != 0))
			{
				segments.at(seg_num - 1)[2] = segments.at(seg_num)[0] + segments.at(seg_num)[0] - segments.at(seg_num)[indx_num];
			}
		}
	}
}

void Bezier1D::AddSegment(glm::mat4 mat)
{
	segments.push_back(mat);
}

int Bezier1D::GetNumSegs()
{
	return segments.size();
}

glm::mat4 Bezier1D::GetSegmentsPosition(int segment)
{
	return segments.at(segment);
}

glm::vec3 Bezier1D::GetAxis()
{
	glm::vec4 first_point = segments[0][0];
	glm::vec4 last_point = segments[segments.size() - 1][3];
	return glm::vec3((last_point - first_point).x, (last_point - first_point).y, (last_point - first_point).z);
}

std::vector<glm::mat4> Bezier1D::GetSegments()
{
	return segments;
}