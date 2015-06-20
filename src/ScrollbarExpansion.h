//
//  ScrollbarExpansion.h
//  nanohtml
//
//  Created by Ilya Mizus on 20.06.15.
//
//

class ScrollbarExpansion;

#ifndef __nanohtml__ScrollbarExpansion__
#define __nanohtml__ScrollbarExpansion__

#include <litehtml.h>

#include "Window.h"
#include "Expansion.h"

class ScrollbarExpansion : public Expansion
{
private:
	litehtml::el_div::ptr scrollbarPtr;
	litehtml::element::ptr scrollPtr;
	
	double yScrollPos = 0;
	int scrollDivHeight = 0;

	double scrollbarCursorX = 0, scrollbarCursorY = 0;
	double scrollbarCursorDeltaX = 0, scrollbarCursorDeltaY = 0;
	
	bool lmbIsDown = false;
	double lmbDownX = 0, lmbDownY = 0;
	litehtml::element::ptr lmbDownElement = NULL;

public:
	virtual void documentLoaded(Window& window);
	virtual void draw(Window& window);
	
	virtual void cursorPosition(Window& window, double x, double y);
	virtual void mouseButton(Window& window, int button, int action, int mods);
	virtual void scroll(Window& window, double xoffset, double yoffset);

};

#endif /* defined(__nanohtml__ScrollbarExpansion__) */
