#define _CRT_SECURE_NO_WARNINGS

#include "SkyBox.h"

#include <iostream>
#include <vector>
#include <cstring>

//#include "SDL.h"

SkyBox::SkyBox()
{

}

SkyBox::~SkyBox()
{
	glDeleteTextures(1, &cube_texture_id);
}

void SkyBox::init()
{
	float offset_side_cube = 1.0f;

	GLfloat skybox_vertices[] = {
		// Positions          
		-offset_side_cube,  offset_side_cube, -offset_side_cube, // front
		-offset_side_cube, -offset_side_cube, -offset_side_cube,
		offset_side_cube, -offset_side_cube, -offset_side_cube,
		offset_side_cube, -offset_side_cube, -offset_side_cube,
		offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,

		-offset_side_cube, -offset_side_cube,  offset_side_cube, // left
		-offset_side_cube, -offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube, -offset_side_cube,
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,

		offset_side_cube, -offset_side_cube, -offset_side_cube, // right
		offset_side_cube, -offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube, -offset_side_cube,
		offset_side_cube, -offset_side_cube, -offset_side_cube,

		-offset_side_cube, -offset_side_cube,  offset_side_cube, // back
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,

		-offset_side_cube,  offset_side_cube, -offset_side_cube, // top
		offset_side_cube,  offset_side_cube, -offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube,  offset_side_cube,  offset_side_cube,
		-offset_side_cube,  offset_side_cube,  -offset_side_cube,

		-offset_side_cube, -offset_side_cube, -offset_side_cube, // bottom
		offset_side_cube, -offset_side_cube, -offset_side_cube,
		offset_side_cube, -offset_side_cube,  offset_side_cube,
		offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube,  offset_side_cube,
		-offset_side_cube, -offset_side_cube, -offset_side_cube
	};

	// VBO
	glGenBuffers(1, &VBO_vertices_textures);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices_textures);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VAO
	glGenVertexArrays(1, &VAO_skybox);
	glBindVertexArray(VAO_skybox);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices_textures);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cube_texture_id = createCubeTexture();

	skyBoxShader = new Shader("skybox.vert", "skybox.frag");

}

void SkyBox::update(glm::mat4 VP_matr)
{
	// get MV matrix from world around whom draw skybox
	VP_matrix = VP_matr;
}

void SkyBox::draw()
{
	// draw skybox LAST in scene ( optimization in vertex shader )
	glDepthFunc(GL_LEQUAL); // optimization in shaders
	glUseProgram(skyBoxShader->getID());
	glBindVertexArray(VAO_skybox);
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader->getID(), "VP"), 1, GL_FALSE, glm::value_ptr(VP_matrix));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS); // return to default
}

GLuint SkyBox::createCubeTexture()
{
	// skybox textures
	const char *cubeTexFiles[6] = {
		"assets/Skybox/back.bmp", "assets/Skybox/front.bmp",
		"assets/Skybox/right.bmp", "assets/Skybox/left.bmp",
		"assets/Skybox/top.bmp", "assets/Skybox/bottom.bmp"
	};

	// init buffer and sides
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	};

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// read files and set as skybox
	for (int i = 0; i < 6; i++) {
		int width, height;
		unsigned char *image = SOIL_load_image(cubeTexFiles[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(sides[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}

	return texID;
}

