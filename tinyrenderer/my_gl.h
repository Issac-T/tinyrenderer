#ifndef __MY_GL_H__
#define __MY_GL_H__

#include "tgaimage.h"
#include "geometry.h"

struct IShader
{
	virtual ~IShader();
	virtual Vec4f vertex(int iface, int nthvert) = 0;
};

#endif // !__MY_GL_H__

