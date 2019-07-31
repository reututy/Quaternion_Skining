#define GLEW_STATIC
#include <GL\glew.h>
#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define CONTROL_POINT_SCALE 0.1

bool once = false;

//For weight calculations only:
std::vector<glm::vec3> weight_head;
std::vector<glm::vec3> weight_body1;
std::vector<glm::vec3> weight_tail;

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

static void printMat(const glm::mat3 mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			std::cout << mat[j][i] << " ";
		std::cout << std::endl;
	}
}

Game::Game() :Scene()
{
	head = nullptr;
	body1 = nullptr;
}

Game::~Game(void)
{
	delete head;
	delete body1;
}

Game::Game(glm::vec3 position,float angle,float hwRelation,float near, float far) : Scene(position,angle,hwRelation,near,far)
{ 
	std::vector<glm::mat4> ctrlPointsVec;
	float x = 1.0;

	glm::mat4 mat = glm::mat4(glm::vec4(0.0 - x, 0.0, 0.0, 1.0), glm::vec4(0.2 - x, 0.7, 0.0, 1.0),
					glm::vec4(1.5 - x, 1.0, 0.0, 1.0), glm::vec4(2.0 - x, 1.0, 0.0, 1.0));
	//glm::mat4 mat = glm::mat4(glm::vec4(0.0, 0.0, 0.0 - x, 1.0), glm::vec4(0.0, 0.7, 0.2 - x, 1.0),
		//glm::vec4(0.0, 1.0, 1.5 - x, 1.0), glm::vec4(0.0, 1.0, 2.0 - x, 1.0));
	
	ctrlPointsVec.push_back(mat);
	head = new Bezier1D(ctrlPointsVec);
	MIN_CTRL = 3;
	MAX_CTRL = 3;
	ctrlPointsVec.clear();

	mat = glm::mat4(glm::vec4(0.0 - x, 1.0, 0.0, 1.0), glm::vec4(1.0 - x, 1.0, 0.0, 1.0),
		  glm::vec4(2.0 - x, 1.0, 0.0, 1.0), glm::vec4(3.0 - x, 1.0, 0.0, 1.0));
	//mat = glm::mat4(glm::vec4(0.0, 1.0, 0.0 - x, 1.0), glm::vec4(0.0, 1.0, 0.5 - x, 1.0),
		//glm::vec4(0.0, 1.0, 1.0 - x, 1.0), glm::vec4(0.0, 1.0, 2.0 - x, 1.0));
	ctrlPointsVec.push_back(mat);
	body1 = new Bezier1D(ctrlPointsVec);
	MIN_CTRL = 3;
	MAX_CTRL = 3;
	ctrlPointsVec.clear();

	mat = glm::mat4(glm::vec4(0.0 - x, 1.0, 0.0, 1.0), glm::vec4(0.5 - x, 1.0, 0.0, 1.0),
		  glm::vec4(1.7 - x, 0.7, 0.0, 1.0), glm::vec4(2.0 - x, 0.0, 0.0, 1.0));
	//mat = glm::mat4(glm::vec4(0.0, 1.0, 0.0 - x, 1.0), glm::vec4(0.0, 1.0, 0.5 - x, 1.0),
		//glm::vec4(0.0, 0.7, 1.7 - x, 1.0), glm::vec4(0.0, 0.0, 2.0 - x, 1.0));
	ctrlPointsVec.push_back(mat);
	tail = new Bezier1D(ctrlPointsVec);
	MIN_CTRL = 3;
	MAX_CTRL = 3;
	ctrlPointsVec.clear();
}

void Game::addShape(int type,int parent,unsigned int mode, Bezier1D* curve)
{
	chainParents.push_back(parent);
	if(type!=BezierLine && type!=BezierSurface)
		shapes.push_back(new Shape(type,mode));
	else
	{
		if(type == BezierLine)
			shapes.push_back(new Shape(curve,30,30,false,mode));
		else
			shapes.push_back(new Shape(curve,30,30,true,mode));
	}
}

void Game::Init()
{
	addShape(Axis, -1, LINES, nullptr); //0 Add Axis
	SetNumOfShape();
	addShape(BezierLine, -1, LINE_STRIP, head); //1 Add curve
	SetNumOfShape();
	weight_head = SaveWeights(30, 30, head);	//For weight calculations only

	//Translate all scene away from camera
	myTranslate(glm::vec3(0, 0, -20), 0);

	//Axis scale:
	pickedShape = 0;
	shapeTransformation(yScale, 20);
	shapeTransformation(xScale, 20);
	shapeTransformation(zScale, 20);

	addShape(Cube, -1, TRIANGLES, nullptr); //2 Add Cube for copying
	SetNumOfShape();
	pickedShape = 2;
	HideShape(pickedShape);

	num_of_shapes = MIN_CTRL;
	num_of_shapes = CreateCurveControlPoints(num_of_shapes, head);
	MAX_CTRL = num_of_shapes;

	//create 3d of head to copy:
	addShape(Scene::Shapes::BezierSurface, -1, QUADS, head); //7 Add head to copy
	SetNumOfShape();
	num_of_head = 7;
	for (int i = MIN_CTRL - 2; i < MAX_CTRL; i++)
	{
		if (i != 2 && i != 10)
			HideShape(i);
	}

	//create 3d of a tail to copy:
	addShape(BezierLine, -1, LINE_STRIP, tail); //8 Add curve
	SetNumOfShape();
	weight_head = SaveWeights(30, 30, tail); //For weight calculations only
	num_of_shapes = CreateCurveControlPoints(9, tail); //returns 13

	addShape(Scene::Shapes::BezierSurface, -1, QUADS, tail); // 13 Add tail to copy
	SetNumOfShape();
	num_of_tail = 13;
	for (int i = 8; i < num_of_shapes + 1; i++)
	{
		if (i != 13)
			HideShape(i);
	}

	//create 3d of a cylinder to copy:
	addShape(BezierLine, -1, LINE_STRIP, body1); //14 Add curve 
	SetNumOfShape();
	weight_head = SaveWeights(30, 30, body1); //For weight calculations only
	num_of_shapes = CreateCurveControlPoints(15, body1); //returns 19

	addShape(Scene::Shapes::BezierSurface, -1, QUADS, body1); // 19 Add cylinder to copy 
	SetNumOfShape();
	num_of_body1 = 19;
	for (int i = 14; i < num_of_shapes + 1; i++)
	{
		if (i != 19)
			HideShape(i);
	}

	HideShape(7); //hides head to copy
	HideShape(13); //hides tail to copy
	HideShape(19); //hides cylinder to copy


	/* Create the snake: */
	addShapeCopy(7, -1, QUADS); //20 Add copy of head (7)
	SetNumOfShape();
	num_of_head = 20;

	addShapeCopy(0, -1, LINES); //21 Add a copy of Axis for the end of the head = for body1
	SetNumOfShape();
	num_of_axis_body1 = 21;
	pickedShape = 21;
	shapeTransformation(yScale, 2);
	shapeTransformation(xScale, 2);
	shapeTransformation(zScale, 2);
	shapeTransformation(xGlobalTranslate, 2.1 / 2);

	addShapeCopy(19, -1, QUADS); //22 Add copy of cylinder (19) for body1
	SetNumOfShape();
	num_of_body1 = 22;
	pickedShape = 22;
	shapeTransformation(xGlobalTranslate, 1.6);
	shapeTransformation(xLocalTranslate, 1.6);

	addShapeCopy(0, -1, LINES); //23 Add a copy of Axis for the end of the body1 = for body2
	SetNumOfShape();
	num_of_axis_body2 = 23;
	pickedShape = 23;
	shapeTransformation(yScale, 2);
	shapeTransformation(xScale, 2);
	shapeTransformation(zScale, 2);
	shapeTransformation(xGlobalTranslate, 4.3 / 4);

	addShapeCopy(19, -1, QUADS); //24 Add copy of cylinder (19) for body2
	SetNumOfShape();
	num_of_body2 = 24;
	pickedShape = 24;
	//shapeTransformation(xGlobalTranslate, 4.4 / 2);
	shapeTransformation(xGlobalTranslate, 1.6);
	shapeTransformation(xLocalTranslate, 1.6);

	addShapeCopy(0, -1, LINES); //25 Add a copy of Axis for the end of the body2 = for body3
	SetNumOfShape();
	num_of_axis_body3 = 25;
	pickedShape = 25;
	shapeTransformation(yScale, 2);
	shapeTransformation(xScale, 2);
	shapeTransformation(zScale, 2);
	shapeTransformation(xGlobalTranslate, 6.5 / 6);

	addShapeCopy(19, -1, QUADS); //26 Add copy of cylinder (19) for body3
	SetNumOfShape();
	num_of_body3 = 26;
	pickedShape = 26;
	//shapeTransformation(xGlobalTranslate, 6.6 / 3);
	shapeTransformation(xGlobalTranslate, 1.6);
	shapeTransformation(xLocalTranslate, 1.6);

	addShapeCopy(0, -1, LINES); //27 Add a copy of Axis for the end of the body3 = for tail
	SetNumOfShape();
	num_of_axis_tail = 27;
	pickedShape = 27;
	shapeTransformation(yScale, 2);
	shapeTransformation(xScale, 2);
	shapeTransformation(zScale, 2);
	shapeTransformation(xGlobalTranslate, 8.7 / 8);

	addShapeCopy(13, -1, QUADS); //28 Add copy of tail (13)
	SetNumOfShape();
	num_of_tail = 28;
	pickedShape = 28;
	//shapeTransformation(xGlobalTranslate, 8.8 / 4);
	shapeTransformation(xGlobalTranslate, 1.6);
	shapeTransformation(xLocalTranslate, 1.6);

	//Set the parents to connect the snake:
	setParent(num_of_head, -1);
	setParent(num_of_axis_body1, num_of_head);
	setParent(num_of_body1, num_of_head);
	setParent(num_of_axis_body2, num_of_body1);
	setParent(num_of_body2, num_of_body1);
	setParent(num_of_axis_body3, num_of_body2);
	setParent(num_of_body3, num_of_body2);
	setParent(num_of_axis_tail, num_of_body3);
	setParent(num_of_tail, num_of_body3);
	
	//To hide the connected axis:
	HideShape(num_of_axis_body1);
	HideShape(num_of_axis_body2);
	HideShape(num_of_axis_body3);
	HideShape(num_of_axis_tail);

	pickedShape = 22;

	/* An example: */
	/*
	addShape(Axis,-1,LINES);
	addShape(Octahedron,-1,TRIANGLES);
	//addShapeFromFile("../res/objs/torus.obj",-1,TRIANGLES);
	addShapeCopy(1,-1,TRIANGLES);
	addShape(Cube,1,LINE_LOOP);
	addShapeCopy(3,2,LINE_LOOP);
	
	
	//translate all scene away from camera
	myTranslate(glm::vec3(0,0,-20),0);

	pickedShape = 0;

	shapeTransformation(yScale,10);
	shapeTransformation(xScale,10);
	shapeTransformation(zScale,10);

	
	ReadPixel();
	
	pickedShape = 2;
	shapeTransformation(zLocalRotate,45);	

	pickedShape = 1;

	shapeTransformation(zGlobalTranslate,-10);
	shapeTransformation(yScale,3.30f);
	shapeTransformation(xScale,3.30f);
	shapeTransformation(zScale,3.30f);

	pickedShape =3;
	shapeTransformation(yScale,3.30f);
	shapeTransformation(xScale,3.30f);
	shapeTransformation(zScale,3.30f);

	pickedShape = -1;
	Activate();
	*/
}

void Game::Update(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, const int shaderIndx)
{
	int prev_shape = pickedShape;
	if (!once) {
		MoveControlCubes();
	}
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV, shaderIndx);
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("Normal", Normal, shaderIndx);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
	if(shaderIndx == 0)
		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
	else 
		s->SetUniform4f("lightColor",0.1f,0.8f,0.7f,1.0f);
	s->Unbind();
}

void Game::WhenRotate()
{
	if (pickedShape >= 0)
		GetShapeTransformation();
}

void Game::WhenTranslate()
{
	bool no_preservation = !(pickedShape <= 4 || pickedShape >= MAX_CTRL - 2);
	if (pickedShape >= 3 && pickedShape < MAX_CTRL) //Make sure that it only happens in the case of the cubes
	{
		glm::vec4 trans_vec = GetShapeTransformation()*glm::vec4(0, 0, 0, 1);
		//move the relevent control point
		head->MoveControlPoint((pickedShape - MIN_CTRL), (pickedShape - MIN_CTRL), no_preservation, trans_vec);
		//move the relevent cube incident to that specific control point
		shapes[1]->GetMesh()->InitLine(head->GetLine(30));
	}
}

void Game::Motion()
{
	if(isActive)
	{
		int p = pickedShape;
		pickedShape = 2;
		shapeTransformation(zLocalRotate,0.75);
		pickedShape = p;
	}
}

int Game::CreateCurveControlPoints(int counter, Bezier1D *curve)
{
	glm::vec3 control_point;
	int modulu = 0;
	for (int k = 0; k < curve->GetNumSegs(); k++)
	{
		modulu = 0;
		for (int i = 0; i < 4; i++)
		{
			addShapeCopy(2, -1, TRIANGLES);
			pickedShape = counter++;
			control_point = *(curve->GetControlPoint(k, i)).GetPos();
			//scaling the cube
			shapeTransformation(xScale, CONTROL_POINT_SCALE);
			shapeTransformation(yScale, CONTROL_POINT_SCALE);
			shapeTransformation(zScale, CONTROL_POINT_SCALE);
			//move the cube to the control point
			if (modulu++ != 3 || (i == 3 && k == curve->GetNumSegs() - 1))
			{
				shapeTransformation(xGlobalTranslate, control_point.x / CONTROL_POINT_SCALE);
				shapeTransformation(yGlobalTranslate, control_point.y / CONTROL_POINT_SCALE);
				shapeTransformation(zGlobalTranslate, control_point.z / CONTROL_POINT_SCALE);
			}
			else
			{
				HideShape(pickedShape);
			}
		}
	}
	return counter;
}

void Game::MoveControlCubes()
{
	int old_picked_shape = pickedShape;
	glm::vec3 control_point;

	for (int i = MIN_CTRL; i < MAX_CTRL; i++)
	{
		if (!((i - 2) % 4) == 0 && i != MAX_CTRL - 1)
		{
			pickedShape = i;
			glm::vec4 curr_pos = GetShapeTransformation()*glm::vec4(0, 0, 0, 1);
			control_point = *(head->GetControlPoint((i - MIN_CTRL) / 4, (i - MIN_CTRL) % 4)).GetPos();
			control_point = (control_point)-glm::vec3(curr_pos);
			shapeTransformation(xGlobalTranslate, control_point.x);
			shapeTransformation(yGlobalTranslate, control_point.y);
			shapeTransformation(zGlobalTranslate, control_point.z);
		}
	}
	pickedShape = old_picked_shape;
}

/*
void CalaIt(glm::vec4 dqRot[3], glm::vec4 dqTrans[3])
{
	glm::vec3 weights = weight_head.at(0);

	int i = 0;
	glm::vec4 b_rot = glm::vec4(0);
	glm::vec4 b_trans = glm::vec4(0);

	for (i = 0; i < 3; i++)
	{
		b_rot += dqRot[i] * weights[i];
		b_trans += dqTrans[i] * weights[i];
	}

	glm::vec4 c_rot_n = b_rot / glm::length(b_rot);
	glm::vec4 c_trans_n = b_trans / glm::length(b_rot);

	float w0 = c_rot_n[0];
	float x0 = c_rot_n[1];
	float y0 = c_rot_n[2];
	float z0 = c_rot_n[3];

	float wE = c_trans_n[0];
	float xE = c_trans_n[1];
	float yE = c_trans_n[2];
	float zE = c_trans_n[3];

	float t0 = 2.0*(-wE * x0 + xE * w0 - yE * z0 + zE * y0);
	float t1 = 2.0*(-wE * y0 + xE * z0 + yE * w0 - zE * x0);
	float t2 = 2.0*(-wE * z0 - xE * y0 + yE * x0 + zE * w0);

	glm::mat4 M = glm::mat4(1.0 - 2.0*y0*y0 - 2.0*z0*z0,
				2.0*x0*y0 + 2.0*w0*z0,
				2.0*x0*z0 - 2.0*w0*y0,
				0,

				2.0*x0*y0 - 2.0*w0*z0,
				1 - 2.0*x0*x0 - 2.0*z0*z0,
				2.0*y0*z0 + 2.0*w0*x0,
				0,

				2.0*x0*z0 + 2.0*w0*y0,
				2.0*y0*z0 - 2.0*w0*z0,
				1 - 2.0*x0*x0 - 2.0*y0*y0,
				0,

				t0, t1, t2, 1.0);

}
*/

void Game::CalcQuaternions(glm::mat4& pre_mat, glm::mat4& curr_mat, glm::mat4& post_mat)
{
	
	//TODO: if there's a problem, maybe need to work with gettraslate() and getrot()

	glm::vec3 weights = weight_head.at(0);

	//Transation quaternions:
	//TODO: if there's a problem, need to check the w = {0,1} and where = {start, end}
	//glm::vec4 pre_trans_q_vec = glm::vec4(pre_mat[0][3] / 2, pre_mat[1][3] / 2, pre_mat[2][3] / 2, 0);
	//glm::vec4 curr_trans_q_vec = glm::vec4(curr_mat[0][3] / 2, curr_mat[1][3] / 2, curr_mat[2][3] / 2, 0);
	//glm::vec4 post_trans_q_vec = glm::vec4(post_mat[0][3] / 2, post_mat[1][3] / 2, post_mat[2][3] / 2, 0);

	//Transformation issues:
	glm::vec4 pre_trans_q_vec = glm::vec4(pre_mat[3][0] / 2, pre_mat[3][1] / 2, pre_mat[3][2] / 2, 0);
	glm::vec4 curr_trans_q_vec = glm::vec4(curr_mat[3][0] / 2, curr_mat[3][1] / 2, curr_mat[3][2] / 2, 0);
	glm::vec4 post_trans_q_vec = glm::vec4(post_mat[3][0] / 2, post_mat[3][1] / 2, post_mat[3][2] / 2, 0);

	//std::cout << "pre_trans_q_vec:" << pre_trans_q_vec .x << " " << pre_trans_q_vec.y << " " << pre_trans_q_vec.z << " " << std::endl;

	int k = 1;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			curr_mat[j][i] = k + 2;
			k++;
		}
	}

	//Rot quaternions:
	glm::mat3 pre_mat_3 = glm::mat3(glm::vec3(pre_mat[0]), glm::vec3(pre_mat[1]), glm::vec3(pre_mat[2]));
	glm::mat3 curr_mat_3 = glm::mat3(glm::vec3(curr_mat[0]), glm::vec3(curr_mat[1]), glm::vec3(curr_mat[2]));
	glm::mat3 post_mat_3 = glm::mat3(glm::vec3(post_mat[0]), glm::vec3(post_mat[1]), glm::vec3(post_mat[2]));

	glm::quat pre_rot_q = glm::quat_cast(pre_mat_3);
	glm::quat curr_rot_q = glm::quat_cast(curr_mat_3);
	glm::quat post_rot_q = glm::quat_cast(post_mat_3);


	//std::cout << "pre_mat_3:" << std::endl;
	//printMat(pre_mat_3);
	//std::cout << "curr_mat_3:" << std::endl;
	//printMat(curr_mat_3);
	//std::cout << "post_mat_3:" << std::endl;
	//printMat(post_mat_3);

	//std::cout << "pre_rot_q:" << pre_rot_q.x << " " << pre_rot_q.y << " " << pre_rot_q.z << " " << std::endl;
	//std::cout << "curr_rot_q:" << curr_rot_q.x << " " << curr_rot_q.y << " " << curr_rot_q.z << " " << std::endl;
	//std::cout << "post_rot_q:" << post_rot_q.x << " " << post_rot_q.y << " " << post_rot_q.z << " " << std::endl;

	//glm::detail::tquat<float, glm::highp> pre_rot_q = glm::quat_cast(pre_mat);
	//glm::detail::tquat<float, glm::highp> curr_rot_q = glm::quat_cast(curr_mat);
	//glm::detail::tquat<float, glm::highp> post_rot_q = glm::quat_cast(post_mat);
	//glm::dualquat quatDul;

	for (int i = 0; i < 3;  i++)
	{
		pre_rot_q[i] *= weights[i];
		curr_rot_q[i] *= weights[i];
		post_rot_q[i] *= weights[i];

		pre_trans_q_vec[i] *= weights[i];
		curr_trans_q_vec[i] *= weights[i];
		post_trans_q_vec[i] *= weights[i];
	}

	glm::vec4 pre_rot_q_vec = glm::vec4(pre_rot_q.x, pre_rot_q.y, pre_rot_q.z, pre_rot_q.w);
	glm::vec4 curr_rot_q_vec = glm::vec4(curr_rot_q.x, curr_rot_q.y, curr_rot_q.z, curr_rot_q.w);
	glm::vec4 post_rot_q_vec = glm::vec4(post_rot_q.x, post_rot_q.y, post_rot_q.z, post_rot_q.w);
	
	glm::vec4 dqRot = pre_rot_q_vec + curr_rot_q_vec + post_rot_q_vec;
	glm::vec4 dqTrans = pre_trans_q_vec + curr_trans_q_vec + post_trans_q_vec;

	//Convert Back to matrix:
	dqTrans *= 2.0f;
	dqTrans *= 2.0f;
	dqTrans *= 2.0f;

	glm::mat4 dq_mat = ConvertQVec4ToRotMat(dqRot);

	//dq_mat[0][3] = dqTrans.x;
	//dq_mat[1][3] = dqTrans.y;
	//dq_mat[2][3] = dqTrans.z;
	//dq_mat[3][3] = 1;

	//Transformation issues:
	dq_mat = glm::transpose(dq_mat);
	dq_mat[3][0] = dqTrans.x;
	dq_mat[3][1] = dqTrans.y;
	dq_mat[3][2] = dqTrans.z;
	dq_mat[3][3] = 1;

	//std::cout << "dq_mat:" << std::endl;
	//printMat(dq_mat);
	
}

glm::mat4& Game::ConvertQVec4ToRotMat(glm::vec4 vec)
{
	glm::mat4 mat = glm::mat4(0);

	mat[0][0] = 1 - 2 * vec.y*vec.y - 2 * vec.z*vec.z;
	mat[0][1] = 2 * vec.x*vec.y - 2 * vec.w*vec.z;
	mat[0][2] = 2 * vec.w*vec.y + 2 * vec.x*vec.z;

	mat[1][0] = 2 * vec.w*vec.z + 2 * vec.x*vec.y;
	mat[1][1] = 1 - 2 * vec.x*vec.x - 2 * vec.z*vec.z;
	mat[1][2] = 2 * vec.y*vec.z - 2 * vec.w*vec.x;

	mat[2][0] = 2 * vec.x*vec.z - 2 * vec.w*vec.y;
	mat[2][1] = 2 * vec.w*vec.x + 2 * vec.y*vec.z;
	mat[2][2] = 1 - 2 * vec.x*vec.x - 2 * vec.y*vec.y;

	return mat;
}

void Game::SkinningUpdate(const glm::mat4 &MV, const glm::mat4 &Projection, const glm::mat4 &Normal, glm::vec4 dqRot[5], glm::vec4 dqTrans[5], const int shaderIndx, int index)
{
	int prev_shape = pickedShape;
	if (!once) {
		MoveControlCubes();
	}
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MV", MV, shaderIndx);
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("Normal", Normal, shaderIndx);
	s->SetUniform4vArr5("dqRot", dqRot);
	s->SetUniform4vArr5("dqTrans", dqTrans);
	s->SetUniform1i("index", index);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else
		s->SetUniform4f("lightColor", 0.1f, 0.8f, 0.7f, 1.0f);
	s->Unbind();

	//For weight calculations only:
	/*
	CalcQuaternions(glm::mat4(1), shapes[20]->makeTrans(), shapes[22]->makeTrans());
	CalcQuaternions(shapes[20]->makeTrans(), shapes[22]->makeTrans(), shapes[24]->makeTrans());
	CalcQuaternions(shapes[22]->makeTrans(), shapes[24]->makeTrans(), shapes[26]->makeTrans());
	CalcQuaternions(shapes[24]->makeTrans(), shapes[26]->makeTrans(), shapes[28]->makeTrans());
	CalcQuaternions(shapes[26]->makeTrans(), shapes[28]->makeTrans(), glm::mat4(1));
	*/
}

void Game::LBSUpdate(const glm::mat4 &Projection, const glm::mat4& pre_mat, const glm::mat4 &curr_mat, const glm::mat4& post_mat, const int shaderIndx)
{
	int prev_shape = pickedShape;
	if (!once) {
		MoveControlCubes();
	}
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("Projection", Projection, shaderIndx);
	s->SetUniformMat4f("pre_mat", pre_mat, shaderIndx);
	s->SetUniformMat4f("curr_mat", curr_mat, shaderIndx);
	s->SetUniformMat4f("post_mat", post_mat, shaderIndx);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else
		s->SetUniform4f("lightColor", 0.1f, 0.8f, 0.7f, 1.0f);
	s->Unbind();
}

int Game::GetNumOfShapes()
{
	return num_of_shapes;
}

void Game::SetNumOfShapes(int value)
{
	num_of_shapes = value;
}

void Game::SetNumOfShape()
{
	pickedShape = shapes.size() - 1;
	shapes[pickedShape]->SetNumOfShape(pickedShape);
	pickedShape = -1;
}

int Game::GetMINCTRL()
{
	return MIN_CTRL;
}

int Game::GetMAXCTRL()
{
	return MAX_CTRL;
}

int Game::GetNumOfHead()
{
	return num_of_head;
}

int Game::GetNumOfBody1()
{
	return num_of_body1;
}

int Game::GetNumOfBody2()
{
	return num_of_body2;
}

int Game::GetNumOfBody3()
{
	return num_of_body3;
}

int Game::GetNumOfTail()
{
	return num_of_tail;
}

int Game::GetNumOfAxisBody1()
{
	return num_of_axis_body1;
}

int Game::GetNumOfAxisBody2()
{
	return num_of_axis_body2;
}

int Game::GetNumOfAxisBody3()
{
	return num_of_axis_body3;
}

int Game::GetNumOfAxisTail()
{
	return num_of_axis_tail;
}


//For weight calculations only:

std::vector<glm::vec3> Game::SaveWeights(int resT, int resS, Bezier1D* main_curve)
{
	std::vector<glm::vec3> weights;
	int segS = 0, segT = 0;
	float t = 0.0;
	float s = 0.0;
	float t_inc = (float)1 / (resT - 1);
	float s_inc = (float)1 / (resS - 1);
	glm::vec3 vec_pos_top_left;
	glm::vec3 vec_pos_top_right;
	glm::vec3 vec_pos_bottom_right;
	glm::vec3 vec_pos_bottom_left;

	for (int main_curve_runner = 0; main_curve_runner < main_curve->GetNumSegs()*resT - 1; main_curve_runner++)
	{
		if (main_curve_runner != 0 && main_curve_runner % resT == 0)
		{
			segT++;
		}
		if (main_curve_runner % (resT) == 0)
		{
			t = 0.0;
		}
		for (int second_curve_runner = 0; second_curve_runner < resS*4 - 1; second_curve_runner++)
		{
			if (main_curve_runner != 0 && main_curve_runner % resS == 0)
			{
				segS++;
			}
			if (second_curve_runner % (resS) == 0)
			{
				s = 0.0;
			}
			weights.push_back(glm::normalize(CalcWeight(segT, segS, t, s)));
			weights.push_back(glm::normalize(CalcWeight(segT, segS, t + t_inc, s)));
			weights.push_back(glm::normalize(CalcWeight(segT, segS, t + t_inc, s + s_inc)));
			weights.push_back(glm::normalize(CalcWeight(segT, segS, t, s + s_inc)));
			s += s_inc;
		}
		t += t_inc;
	}
	return weights;
}

glm::vec3 Game::CalcWeight(int segmentT, int segmentS, float t, float s)
{
	float f1 = 0, f3 = 0;
	if (t > 0.5)
		f3 = (1 - 4.0f*(1 - t)*t)*(1 - t) / 2.0f + (1 - 4.0f*(1 - t)*t)*t / 2.0f;
	else
		f1 = (1 - 4.0f*(1 - t)*t)*(1 - t) / 2.0f + (1 - 4.0f*(1 - t)*t)*t / 2.0f;
	float f2 = (2.0f*(1 - t)*(t + 0.0) + 0.5f);
	return glm::vec3(f1, f2, f3);
}