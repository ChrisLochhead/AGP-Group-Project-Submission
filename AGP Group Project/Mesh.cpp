#include "Mesh.h"

void Mesh::Draw(Shader shader)
{
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = this->textures[i].type;

		if (name == "texture_diffuse")
		{
			ss << diffuseNr++; // Transfer GLuint to stream
		}
		else if (name == "texture_specular")
		{
			ss << specularNr++; // Transfer GLuint to stream
		}
		else if (name == "texture_normal")
		{
			cout << "called normal map" << endl;
			ss << normalNr++; // Transfer GLuint to stream
		}

		number = ss.str();
		// Now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.getID(), (name + number).c_str()), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}

	// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
	glUniform1f(glGetUniformLocation(shader.getID(), "material.shininess"), 16.0f);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::DrawVMesh(Shader shader)
{

	// Draw mesh
	glBindVertexArray(this->VVAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void Mesh::DrawTextured(Shader shader, GLuint tex1, GLuint tex2)
{
	glActiveTexture(GL_TEXTURE0); // Active proper texture unit before binding
	glUniform1i(glGetUniformLocation(shader.getID(), "textureUnit0"), 0);
	//cout << glGetUniformLocation(shader.getID(), "textureUnit0") << endl;
	glBindTexture(GL_TEXTURE_2D, tex2);

	glActiveTexture(GL_TEXTURE1); // Active proper texture unit before binding
	glUniform1i(glGetUniformLocation(shader.getID(), "textureUnit1"), 1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::DrawDMesh(Shader shader)
{
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;

	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
										  // Retrieve texture number (the N in diffuse_textureN)
		stringstream ss;
		string number;
		string name = this->textures[i].type;

		if (name == "texture_diffuse")
		{
			ss << diffuseNr++; // Transfer GLuint to stream
		}
		else if (name == "texture_specular")
		{
			ss << specularNr++; // Transfer GLuint to stream
		}
		else if (name == "texture_normal")
		{
			ss << normalNr++; // Transfer GLuint to stream
		}

		number = ss.str();
		// Now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shader.getID(), (name + number).c_str()), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}


	// Draw mesh
	glBindVertexArray(this->DVAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	for (GLuint i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

void Mesh::setupVMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VVAO);
	glGenBuffers(1, &this->VVBO);
	glGenBuffers(1, &this->VEBO);

	glBindVertexArray(this->VVAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VVBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->VEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, TexCoords));
	// vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
	// vertex bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, BiTangent)));


	glBindVertexArray(0);
}

void Mesh::setupDMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &this->DVAO);
	glGenBuffers(1, &this->DVBO);
	glGenBuffers(1, &this->DEBO);

	glBindVertexArray(this->DVAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->DVBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->DEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, Normal));

	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, TexCoords));

	// vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
	// vertex bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, BiTangent)));

	glBindVertexArray(0);
}
