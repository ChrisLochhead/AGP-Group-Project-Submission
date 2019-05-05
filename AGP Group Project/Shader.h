#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
private:
	GLuint Program;
public:
    // Constructor generates the shader on the fly
	Shader(const GLchar *vertexPath, const GLchar *fragmentPath);
    // Uses the current shader
	void Use();
	GLuint getID();
};

#endif