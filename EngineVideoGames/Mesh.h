#pragma once
#include "glm/glm.hpp"
#include <vector>

class IndexedModel
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> weights;
	std::vector<glm::ivec3> joint_indices;
    std::vector<unsigned int> indices;
    
    void CalcNormals();
	inline void* GetData(int indx) const 
	{
		switch(indx)
		{
			/*
			case 0: return (void*)&positions[0]; 
			case 1: return (void*)&colors[0]; 
			case 2: return (void*)&normals[0]; 
			case 3: return (void*)&texCoords[0]; 
			case 4: return (void*)&indices[0]; 
			default: return (void*)0;
			*/
			
			case 0: return (void*)&positions[0];
			case 1: return (void*)&colors[0];
			case 2: return (void*)&normals[0];
			case 3: return (void*)&weights[0];
			case 4: return (void*)&texCoords[0];
			case 5: return (void*)&indices[0];
			default: return (void*)0;
			
		}
	}

};

struct Vertex
	{
	public:
		Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal, const glm::vec3& color)
		{
			this->pos = pos;
			this->texCoord = texCoord;
			this->normal = normal;
			this->color = color;
		}

		glm::vec3* GetPos() { return &pos; }
		glm::vec2* GetTexCoord() { return &texCoord; }
		glm::vec3* GetNormal() { return &normal; }
		glm::vec3* GetColor() { return &color; }
		glm::vec3* GetWeight() { return &weight; }
	
	private:
		glm::vec3 pos;
		glm::vec2 texCoord;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec3 weight;
	};

struct LineVertex
	{
	public:
		LineVertex(const glm::vec3& pos, const glm::vec3& color)
		{
			this->pos = pos;
			this->color = color;
		}

		glm::vec3* GetPos() { return &pos; }
		glm::vec3* GetColor() { return &color; }
	
	private:
		glm::vec3 pos;
		glm::vec3 color;
	};

class BoundingBox
{
	glm::vec3 center;     // center coordinates
	glm::vec3 size;       // distance between the center of the box to its side in each dimension 
	glm::vec3 xInit;      // x axis of the box. default value (1,0,0)		  
	glm::vec3 yInit;      // y axis of the box. default value (0,1,0)		 
	glm::vec3 zInit;	  // z axis of the box. default value (0,0,1)
};

IndexedModel CubeTriangles();

IndexedModel AxisGenerator();

IndexedModel TethrahedronGenerator();

IndexedModel OctahedronGenerator();