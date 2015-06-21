//
//  Expansion.h
//  nanohtml
//
//  Created by Ilya Mizus on 20.06.15.
//
//

class Expansion;

#ifndef __nanohtml__Expansion__
#define __nanohtml__Expansion__

#include "Window.h"

class Expansion
{
public:
	virtual void documentLoaded(Window& window) = 0;
	virtual void render(Window& window) = 0;
	virtual void draw(Window& window) = 0;
	
	virtual void cursorPosition(Window& window, double x, double y) = 0;
	virtual void mouseButton(Window& window, int button, int action, int mods) = 0;
	virtual void scroll(Window& window, double xoffset, double yoffset) = 0;

};

#endif /* defined(__nanohtml__Expansion__) */
