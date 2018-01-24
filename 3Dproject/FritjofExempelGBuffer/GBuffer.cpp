#include "GBuffer.h"

#include <iostream>

//Constructor
GBuffer::GBuffer()
{
	//Nothing
}

//Destructor
GBuffer::~GBuffer()
{
	//Nothing
}

//Init
bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	//Create the FBO (FrameBufferObject)
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	//Create the gbuffer textures
	glGenTextures(5, _textures);
	for (unsigned int i = 0; i < 5; i++)
	{
		glBindTexture(GL_TEXTURE_2D, _textures[i]);  //Bind the texture things will apply to
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, 0);  //Creates the storage area of the texture (without initializing it)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _textures[i], 0);  //Attaches the texture to the FBO target
	}
	//Specular map (different because it needs four values instead of three like the rest)
	glGenTextures(1, &_specularTexture);
	glBindTexture(GL_TEXTURE_2D, _specularTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, _specularTexture, 0);

	//Depth
	glGenTextures(1, &_depthTexture);
	glBindTexture(GL_TEXTURE_2D, _depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0);
	
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 , GL_COLOR_ATTACHMENT4 , GL_COLOR_ATTACHMENT5 };
	glDrawBuffers(6, DrawBuffers);  //This makes us able to use Multiple Render Targets

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("FB error, status: 0x%x\n", Status);
		std::cout << "Normal fbo error" << std::endl;
		return false;
	}


	glGenFramebuffers(1, &_aofbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _aofbo);

	glGenTextures(1, &_aoTexture);
	glBindTexture(GL_TEXTURE_2D, _aoTexture);  //Bind the texture things will apply to
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, WindowWidth, WindowHeight, 0, GL_RED, GL_FLOAT, 0);  //Creates the storage area of the texture (without initializing it)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _aoTexture, 0);  //Attaches the texture to the FBO target

	GLenum drawBuffer[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffer);

	Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("FB error, status: 0x%x\n", Status);
		std::cout << "AO fbo error" << std::endl;
		return false;
	}


	glGenFramebuffers(1, &_lightPassFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _lightPassFbo);

	glGenTextures(1, &_lightPassTexture);
	glBindTexture(GL_TEXTURE_2D, _lightPassTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _lightPassTexture, 0);
	
	GLenum LightPassDrawBuffer[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, LightPassDrawBuffer);

	Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("FB error, status: 0x%x\n", Status);
		std::cout << "Light pass fbo error" << std::endl;
		return false;
	}


	//Restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}


void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
}


void GBuffer::BindForSSAOPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _aofbo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textures[0]);
}


void GBuffer::BindForLightPasses(bool gaussianActive)
{
	for (unsigned int i = 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
	}
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, _specularTexture);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, _aoTexture);
	if (gaussianActive)
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _lightPassFbo);
	else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GBuffer::BindForGaussianBlurPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _lightPassTexture);
}