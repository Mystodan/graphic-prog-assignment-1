#version 430 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in float aMode;


// Outputs the color for the Fragment Shader
out vec3 color;

// Imports the camera matrix from the main function
uniform mat4 transformGhost;


void main() {
	// Outputs the positions/coordinates of all vertices
	gl_Position = transformGhost * vec4(aPos, 1.0);

	// Assigns the colors from the Vertex Data to "color"
	if(aMode == 0.f)	  { color = vec3(1.f,0.f,0.f);}
	else if(aMode == 1.f) { color = vec3(1.f,0.5f,1.f);}
	else if(aMode == 2.f) { color = vec3(0.9f,0.6f,0.3f);}
	else if(aMode == 3.f) { color = vec3(0.f,0.9f,1.f);}
}