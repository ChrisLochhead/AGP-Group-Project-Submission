#ifndef ANIMMESH
#define ANIMMESH

#include <iostream>
#include <vector>
#include <string>

#include "GL\glew.h"
#include "glm\glm.hpp"
#include "assimp\Importer.hpp"
#include "Shader.h"

using namespace std;
typedef unsigned int uint;
#define NUM_BONES_PER_VEREX 4

struct AnimVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 text_coords;
};

struct AnimTexture
{
	GLuint id;
	string type;
	aiString path;
};

struct BoneMatrix
{
	aiMatrix4x4 offset_matrix;
	aiMatrix4x4 final_world_transform;

};

struct VertexBoneData
{
	uint ids[NUM_BONES_PER_VEREX];   // we have 4 bone ids for EACH vertex & 4 weights for EACH vertex
	float weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		memset(ids, 0, sizeof(ids));    // init all values in array = 0
		memset(weights, 0, sizeof(weights));
	}

	void addBoneData(uint bone_id, float weight);
};

class AnimMesh
{
public:
	AnimMesh(vector<AnimVertex> vertic, vector<GLuint> ind, vector<AnimTexture> textur, vector<VertexBoneData> bone_id_weights);
	AnimMesh() {};
	~AnimMesh();

	// Render mesh
	void Draw(GLuint shaders_program);

private:
	//Mesh data
	vector<AnimVertex> vertices;
	vector<GLuint> indices;
	vector<AnimTexture> textures;
	vector<VertexBoneData> bones_id_weights_for_each_vertex;

	//buffers
	GLuint VAO;
	GLuint VBO_vertices;
	GLuint VBO_bones;
	GLuint EBO_indices;

	GLuint VVAO, VVBO, VEBO;

	GLuint DVAO, DVBO, DEBO;

	//inititalize buffers
	void SetupMesh();
};

#endif // !MESH