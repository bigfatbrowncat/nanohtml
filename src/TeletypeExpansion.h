//
//  TeletypeExpansion.h
//  nanohtml
//
//  Created by Ilya Mizus on 21.06.15.
//
//

#ifndef __nanohtml__TeletypeExpansion__
#define __nanohtml__TeletypeExpansion__

#include "Window.h"
#include "Expansion.h"

#include "../src/html_tag.h"
#include "../src/el_text.h"
#include "../src/el_space.h"

class TeletypeExpansion : public Expansion
{
private:
	litehtml::elements_vector teletypeElements;
	std::list<litehtml::elements_vector> teletypeElementsContents;

	double startTimeSec;
public:
	TeletypeExpansion(double startTimeSec) : startTimeSec(startTimeSec) { }
	
	virtual void documentLoaded(Window& window);
	virtual void render(Window& window);
	virtual void draw(Window& window);
	
	virtual void cursorPosition(Window& window, double x, double y);
	virtual void mouseButton(Window& window, int button, int action, int mods);
	virtual void scroll(Window& window, double xoffset, double yoffset);
};

#endif /* defined(__nanohtml__TeletypeExpansion__) */
