#pragma once

// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

//topology -> triangles
//a very simple cube only has position - light bulb
const GLfloat simpleCubeVerts[24] = {
	// front
	-1.0, -1.0,  1.0,
	1.0, -1.0,  1.0,
	1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// back
	-1.0, -1.0, -1.0,
	1.0, -1.0, -1.0,
	1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0,
};
const GLuint simpleCubeIndices[36] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// top
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// bottom
	4, 0, 3,
	3, 7, 4,
	// left
	4, 5, 1,
	1, 0, 4,
	// right
	3, 2, 6,
	6, 7, 3,
};


//topology -> triangles
// Cube vertices with full detail -> normal, texture coordinates and indices
const GLfloat cubeVerts[24][3] = {
	// Front face
	{ 0.5f, -0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, -0.5f, 0.5f },
	// Back face
	{ -0.5f, -0.5f, -0.5f },{ -0.5f, 0.5f, -0.5f },{ 0.5f, 0.5f, -0.5f },{ 0.5f, -0.5f, -0.5f },
	// Left face
	{ -0.5f, -0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, -0.5f },{ -0.5f, -0.5f, -0.5f },
	// Right face
	{ 0.5f, -0.5f, -0.5f },{ 0.5f, 0.5f, -0.5f },{ 0.5f, 0.5f, 0.5f },{ 0.5f, -0.5f, 0.5f },
	// Top face
	{ 0.5f, 0.5f, -0.5f },{ -0.5f, 0.5f, -0.5f },{ -0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },
	// Bottom face
	{ 0.5f, -0.5f, 0.5f },{ -0.5f, -0.5f, 0.5f },{ -0.5f, -0.5f, -0.5f },{ 0.5f, -0.5f, -0.5f }
};
// Cube normals
const GLfloat cubeNorms[24][3] = {
	// Front face
	{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },
	// Back face
	{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f, -1.0f },
	// Left face
	{ -1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },{ -1.0f, 0.0f, 0.0f },
	// Right face
	{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },
	// Top face
	{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
	// Bottom face
	{ 0.0f, -1.0f, 0.0f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, -1.0f, 0.0f }
};
//Cube texture coordinates
const GLfloat cubeTexCoords[24][2] = {
	//Front face
	{ 1.0, 0.0 },{ 1.0, 1.0 },{ 0.0, 1.0 },{ 0.0, 0.0 },
	// Back face
	{ 1.0, 0.0 },{ 1.0, 1.0 },{ 0.0, 1.0 },{ 0.0, 0.0 },
	// Left face
	{ 1.0, 0.0 },{ 1.0, 1.0 },{ 0.0, 1.0 },{ 0.0, 0.0 },
	// Right face
	{ 1.0, 0.0 },{ 1.0, 1.0 },{ 0.0, 1.0 },{ 0.0, 0.0 },
	// Top face
	{ 1.0, 1.0 },{ 0.0, 1.0 },{ 0.0, 0.0 },{ 1.0, 0.0 },
	// Left face
	{ 0.0, 0.0 },{ 1.0, 0.0 },{ 1.0, 1.0 },{ 0.0, 1.0 },
};
// Cube indices
const GLuint cubeIndices[12][3] = {
	{ 0, 1, 2 },{ 0, 2, 3 }, // Front face
	{ 4, 5, 6 },{ 4, 6, 7 }, // Back face
	{ 8, 9, 10 },{ 8, 10, 11 }, // Left face
	{ 12, 13, 14 },{ 12, 14, 15 }, // Right face
	{ 16, 17, 18 },{ 16, 18, 19 }, // Top face
	{ 20, 21, 22 },{ 20, 22, 23 } // Bottom face
};

//topology -> triangle_strips
//a quad with only position and texture coordinates(full screen quad if no MVP applied)
const GLfloat quadVerts[4][3] = {
	{ -1.0f, 1.0f, 0.0f },{ -1.0f, -1.0f, 0.0f },
	{ 1.0f, 1.0f, 0.0f },{ 1.0f, -1.0f, 0.0f }
};
const GLfloat quadNorms[4][3] = 
{
	{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f }
};
//quad texture coordinates - scaled
const GLfloat quadTexCoords[4][2] = {
	{ 0.0f, 1.0f },{ 0.0f, 0.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f }
};

/*
//quad texture coordinates
const GLfloat quadTexCoords[4][2] = {
	{ 0.0f, 1.0f },{ 0.0f, 0.0f },{ 1.0f, 1.0f },{ 1.0f, 0.0f }
};
*/

