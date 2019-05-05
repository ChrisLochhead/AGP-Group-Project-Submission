#ifndef SKYBOX
#define SKYBOX

#include "GL\glew.h"

#include "glm\glm.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "SOIL2\SOIL2.h"

#include "Shader.h"

using namespace std;

class SkyBox
{
public:
	static SkyBox* Instance()
	{
		static SkyBox inst;
		return &inst;
	}

	void init();
	void draw();
	void update(glm::mat4 VP_matr);

private:
	SkyBox();
	~SkyBox();

	GLuint VAO_skybox;
	GLuint VBO_vertices_textures;

	GLuint cube_texture_id;
	GLuint createCubeTexture();

	glm::mat4 VP_matrix; // WITHOUT model for skybox

	Shader *skyBoxShader;
};

#endif