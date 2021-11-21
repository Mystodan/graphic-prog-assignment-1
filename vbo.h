#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <vector>
#include <glad/glad.h>

class VBO {
private:
	// Gets byte size
	int getBytes(const std::vector<GLfloat>vertices);

public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(std::vector<GLfloat>&vertices);
	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};

#endif