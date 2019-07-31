#pragma once
#include "scene.h"
#include "bezier1D.h"

class Game : public Scene
{
	Bezier1D *head;
	Bezier1D *body1;
	Bezier1D *tail;

	int num_of_head;
	int num_of_body1;
	int num_of_body2;
	int num_of_body3;
	int num_of_tail;

	int num_of_axis_body1;
	int num_of_axis_body2;
	int num_of_axis_body3;
	int num_of_axis_tail;

	int num_of_shapes;
	int MIN_CTRL;
	int MAX_CTRL;
public:
	Game(void);
	~Game(void);
	Game(glm::vec3 position,float angle,float hwRelation,float near, float far);
	void Init();
	void addShape(int type,int parent,unsigned int mode, Bezier1D* curve);
//	void Update( glm::mat4 MVP ,glm::mat4 *jointTransforms,const int length,const int  shaderIndx);
	void Update(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, const int shaderIndx);
	void WhenRotate();
	void WhenTranslate();
	void Motion();

	/* Reut's addings:*/
	void CalcQuaternions(glm::mat4& pre_mat, glm::mat4& curr_mat, glm::mat4& post_mat);
	glm::mat4& ConvertQVec4ToRotMat(glm::vec4 vec);
	void SkinningUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::vec4 dqRot[5], glm::vec4 dqTrans[5], const int shaderIndx, int index);
	void LBSUpdate(const glm::mat4 &Projection, const glm::mat4& pre_mat, const glm::mat4 &curr_mat, const glm::mat4& post_mat, const int shaderIndx);
	//For weight calculations only:
	std::vector<glm::vec3> SaveWeights(int resT, int resS, Bezier1D* main_curve);
	glm::vec3 CalcWeight(int segmentT, int segmentS, float t, float s);

	int CreateCurveControlPoints(int counter, Bezier1D *curve);
	void MoveControlCubes();
	int GetNumOfShapes();
	void SetNumOfShape();
	void SetNumOfShapes(int value);
	int GetMINCTRL();
	int GetMAXCTRL();

	int GetNumOfHead();
	int GetNumOfBody1();
	int GetNumOfBody2();
	int GetNumOfBody3();
	int GetNumOfTail();

	int GetNumOfAxisBody1();
	int GetNumOfAxisBody2();
	int GetNumOfAxisBody3();
	int GetNumOfAxisTail();
};

