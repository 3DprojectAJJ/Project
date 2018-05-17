#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include "Renderer.h"
class VertexBuffer
{
private:
	unsigned int id;
public:
	VertexBuffer(const void * data, unsigned int size);
	~VertexBuffer();

	void bind() const;
	void unBind() const;
};

#endif
