#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H
#include "Renderer.h"
class IndexBuffer
{
private:
	unsigned int m_id;
	unsigned int m_count;
public:
	IndexBuffer(const unsigned int * data, unsigned int count);
	~IndexBuffer();

	void bind() const;
	void unBind() const;

	inline unsigned int getCount() const{ return m_count; }
};

#endif
