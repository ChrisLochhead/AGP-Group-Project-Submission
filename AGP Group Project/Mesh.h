#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <assimp/types.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

using namespace std;

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
	glm::vec2 TexCoords;

	//normal mapping variables
	glm::vec3 Tangent;
	glm::vec3 BiTangent;
};

struct Texture
{
    GLuint id;
    string type;
	aiString path;
};

class Mesh
{
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    
    /*  Functions  */
    // Constructor
    Mesh( vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures )
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        // Now that we have all the required data, set the vertex buffers and its attribute pointers.
        this->setupMesh();
		this->setupVMesh();
		this->setupDMesh();
    }
    
    // Render the mesh
	void Draw(Shader shader);

	void DrawVMesh(Shader shader);

	void DrawTextured(Shader shader, GLuint tex1, GLuint tex2);

	void DrawDMesh(Shader shader);
    
private:
    /*  Render data  */
    GLuint VAO, VBO, EBO;

	GLuint VVAO, VVBO, VEBO;

	GLuint DVAO, DVBO, DEBO;


	void setupVMesh();
    
    /*  Functions    */
    // Initializes all the buffer objects/arrays
	void setupMesh();

	void setupDMesh();

};


