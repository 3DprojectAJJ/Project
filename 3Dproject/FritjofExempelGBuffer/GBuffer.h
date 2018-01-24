#pragma once
#ifndef GBUFFER_H
#define GBUFFER_H

#include <GL\glew.h>

class GBuffer
{
public:
	enum GBUFFER_TEXTURE_TYPE
	{
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_COLOR,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_AMBIENT,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_SPECULAR,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();
	~GBuffer();
	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
	void BindForWriting();
	void BindForSSAOPass();
	void BindForLightPasses(bool gaussianActive);
	void BindForGaussianBlurPass();

private:
	GLuint		_fbo;
	GLuint		_textures[5];
	GLuint		_specularTexture;
	GLuint		_depthTexture;
	GLuint		_aofbo;
	GLuint		_aoTexture;
	GLuint		_lightPassFbo;
	GLuint		_lightPassTexture;
};

#endif
