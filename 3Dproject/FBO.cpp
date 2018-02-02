#include "FBO.h"

FBO::FBO(int width, int height)
{
	this->width = width;
	this->height = height;
}

FBO::~FBO()
{
}

void FBO::Init()
{
	//generate and bind FBO
	glGenFramebuffers(1, &fboID);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	// generate and bind rendertexture
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// generate and bind depth buffer
	glGenRenderbuffers(1, &depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthID);

	// Set "texID" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texID, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   // Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	// The fullscreen quad's FBO
	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat gQuadVertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	glGenBuffers(1, &quadID);
	glBindBuffer(GL_ARRAY_BUFFER, quadID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gQuadVertexBufferData), gQuadVertexBufferData, GL_STATIC_DRAW);
}

void FBO::BindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glViewport(0, 0, width, height);
}

void FBO::UnbindFBO(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
}

GLuint FBO::GetTexID() const
{
	return texID;
}

GLuint FBO::GetQuadID() const
{
	return quadID;
}
