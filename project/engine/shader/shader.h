#pragma once

#include "engine/namespace.h"

class				engine::shader
{
	friend class	engine::program;

public :

	enum class		type : GLuint
	{
		vertex = GL_VERTEX_SHADER,
		fragment = GL_FRAGMENT_SHADER
	};

					shader(type type, const std::string &source);
					~shader();

private :

	GLuint			object{0};
};


