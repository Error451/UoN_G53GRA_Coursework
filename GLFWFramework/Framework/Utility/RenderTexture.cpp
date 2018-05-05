#include "RenderTexture.h"

int RenderTexture::bufferCount = 0;

GLenum RenderTexture::DrawBuffers[8] = {
	GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,GL_COLOR_ATTACHMENT7
};

RenderTexture::RenderTexture(int width, int height, GLenum format, bool isDepth)
	:width(width), height(height), colorFormat(format), isDepth(isDepth)
{
	if (bufferCount == maxBufferNum)
	{
		std::cout << "Render texture generation error: number of buffer exceed limits \n";
		return;
	}

	id = bufferCount;

	glGenFramebuffers(1, &framebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);

	//The texture we're going to render to
	glGenTextures(1, &renderedTexture);

	//"bind" the newly created texture -> all future texture function will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	//give an empty image to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, GL_RGBA, GL_FLOAT, 0);

	// Poor filtering. Needed ! -> adjust texture size for other texture or viewport
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	if (isDepth)
	{
		glGenRenderbuffers(1, &depthrenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
	}
	else
	{
		depthrenderbuffer = -1;
	}
	
	//set "rendered texture" as color attachment 0
	glFramebufferTexture(GL_FRAMEBUFFER, DrawBuffers[id], renderedTexture, 0);

	//set the list of draw buffers
	glDrawBuffers(1, &DrawBuffers[id]); //1 -> how many buffers will be drawn

								   // Always check that the framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "***************FBO " << id << " binding failed*************** \n";
		iscomplete = false;
	}
	else
	{
		std::cout << "Render texture " << id << " generated  successfully" << "\n";
		iscomplete = true;
	}

	//increase counter
	bufferCount++;

}

RenderTexture::~RenderTexture()
{
	//release all declared memory chunks
	glDeleteBuffers(1, &framebufferName);
	glDeleteRenderbuffers(1, &depthrenderbuffer);
	glDeleteTextures(1, &renderedTexture);
}
