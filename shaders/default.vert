#version 430 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in float aMode;

// Outputs the color for the Fragment Shader
out vec3 color;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;




void main() {
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(aPos, 1.0);
	
	// Assigns the colors from the Vertex Data to "color"
	if(aMode == 0.f) { color = vec3(0,0,0);}
	else if(aMode == 1.f) { color = vec3(0.1,0.2,0.7);}
	else if(aMode == 2.f) { color = vec3(0,0,0);}
	else if(aMode == 3.f) { color = vec3(1,1,1);}
	else if(aMode == 4.f) { color = vec3(1,0,0);}
	else if(aMode == 5.f) { color = vec3(1,1,1);}
}