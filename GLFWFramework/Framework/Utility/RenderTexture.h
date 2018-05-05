#pragma once

/*
code from following opengl tutorial
http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
create a rendertexture lie on frame buffer which can be used as a render target instead
of drawing on the screen directly, extremely useful for post-processing like blur
*/

// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>


class RenderTexture
{
public:
	RenderTexture(int width, int height, GLenum format, bool isDepth);
	~RenderTexture();

	//set current texture as render target
	inline void BindRenderTexture() { glBindFramebuffer(GL_FRAMEBUFFER, framebufferName); };

	//redirect rendering pipeline to screen
	inline static void UnbindTexture() { glBindFramebuffer(GL_FRAMEBUFFER, 0); };

	inline bool isComplete() { return iscomplete; }

private:
	int width, height, id;
	GLenum colorFormat;
	bool isDepth;
	bool iscomplete;

	static const int maxBufferNum = 8;
	static int bufferCount;
	static GLenum DrawBuffers[8];

	GLuint framebufferName;
	GLuint renderedTexture;
	GLuint depthrenderbuffer;
};

