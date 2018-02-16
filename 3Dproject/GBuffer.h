#ifndef GBUFFER_H
#define GBUFFER_H
#include <GL\glew.h>
class GBuffer
{
public:
	enum TextureType
	{
		POSITION,
		DIFFUSE,
		NORMAL,
		TEXCOORD,
		NUM_TEXTURES
	};

	GBuffer();
	~GBuffer();

	bool Init(unsigned int windowWidth, unsigned int windowHeight);
	void BindForWriting();
	void BindForReading();
private:
	GLuint fboID;
	GLuint textureIDs[NUM_TEXTURES];
	GLuint depthTextureID;
};
#endif
