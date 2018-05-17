#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include "Renderer.h"
class VertexBuffer
{
private:
	unsigned int id;
public:
	VertexBuffer();
	~VertexBuffer();

	void init(const void * data, unsigned int size);
	void bind() const;
	void unBind() const;
};

#endif
