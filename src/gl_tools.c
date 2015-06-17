//
//  gl_tools.c
//  nanohtml
//
//  Created by Ilya Mizus on 16.06.15.
//
//

#include <string.h>
#include <stdio.h>

#include <GL3/gl3w.h>

#include "gl_tools.h"

void getGLVersion(int *major, int *minor)
{
	const char *verstr = (const char *) glGetString(GL_VERSION);
	if ((verstr == NULL) || (sscanf(verstr,"%d.%d", major, minor) != 2))
	{
		*major = *minor = 0;
		fprintf(stderr, "Invalid GL_VERSION format!!!\n");
	}
}

void getGLSLVersion(int *major, int *minor)
{
	int gl_major, gl_minor;
	getGLVersion(&gl_major, &gl_minor);
	
	*major = *minor = 0;
	if (gl_major == 1)
	{
		/* GL v1.x can only provide GLSL v1.00 as an extension */
		const char *extstr = (const char *) glGetString(GL_EXTENSIONS);
		if ((extstr != NULL) &&	(strstr(extstr, "GL_ARB_shading_language_100") != NULL))
		{
			*major = 1;
			*minor = 0;
		}
	}
	else if (gl_major >= 2)
	{
		/* GL v2.0 and greater must parse the version string */
		const char *verstr =
		(const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);
		
		if((verstr == NULL) || (sscanf(verstr, "%d.%d", major, minor) != 2))
		{
			*major = *minor = 0;
			fprintf(stderr, "Invalid GL_SHADING_LANGUAGE_VERSION format!!!\n");
		}
	}
}

BOOL glInit()
{
	if (gl3wInit())
	{
		printf("Problem initializing OpenGL\n");
		return FALSE;
	}
	
	int maj, min, slmaj, slmin;
	getGLVersion(&maj, &min);
	getGLSLVersion(&slmaj, &slmin);
	
	printf("OpenGL version: %d.%d\n", maj, min);
	printf("GLSL version: %d.%d\n", slmaj, slmin);
	
	return TRUE;
}