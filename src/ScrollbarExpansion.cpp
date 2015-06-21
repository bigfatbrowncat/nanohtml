//
//  ScrollbarExpansion.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 20.06.15.
//
//

#include "ScrollbarExpansion.h"

#ifdef WIN32
#	define SCROLL_VEL		25
#else
#	define SCROLL_VEL		5
#endif

void ScrollbarExpansion::documentLoaded(Window& window)
{
	scrollPtr = window.getDocument()->root()->select_one("body");
	
	// Adding a scrollbar to <body>
	scrollbarPtr = new litehtml::el_div(window.getDocument());
	scrollbarPtr->set_attr("id", "scrollbar");
	scrollPtr->parent()->appendChild(scrollbarPtr);
	
}

void ScrollbarExpansion::render(Window& window)
{
	
}

void ScrollbarExpansion::draw(Window& window)
{
	// Neutralizing overscroll
	
	scrollDivHeight = scrollPtr->height();
	
	double yScrollVel = 0;
	if (yScrollPos < -scrollDivHeight + window.getHeight()) {
		double delta = yScrollPos - (-scrollDivHeight + window.getHeight());
		yScrollVel = -delta / 2;
	}
	if (yScrollPos > 0) {
		yScrollVel = -yScrollPos / 2 + 0.1;
	}
	
	yScrollPos += yScrollVel / 2;
	
	// Setting scroll position
	scrollPtr->get_position().y = yScrollPos;
	
	// Seting scrollbar size and position
	if (scrollDivHeight > window.getHeight())
	{
		int sbHeight = fmax(window.getHeight() * window.getHeight() / scrollDivHeight, 30.0) - 4;
		int sbPos = (window.getHeight() - sbHeight - 4) * (1.0 - yScrollPos / (scrollDivHeight - window.getHeight())) - window.getHeight() + sbHeight + 6;
		std::stringstream ss; ss << "visibility: visible; border-radius: 3.5px; position: absolute; right: 2px; top: " << sbPos << "px; width: 7px; height: " << sbHeight << "px; background-color: rgba(0, 0, 0, 0.4);";
		scrollbarPtr->set_attr("style", ss.str().c_str());
		scrollbarPtr->parse_styles();
	} else {
		std::stringstream ss; ss << "visibility: hidden;";
		scrollbarPtr->set_attr("style", ss.str().c_str());
		scrollbarPtr->parse_styles();
	}
}

void ScrollbarExpansion::cursorPosition(Window& window, double x, double y)
{
	// Dragging the scrollbar
	if (lmbIsDown && lmbDownElement == scrollbarPtr) {
		scrollbarCursorX = lmbDownX - scrollbarPtr->left();
		scrollbarCursorY = lmbDownY - scrollbarPtr->top();
		
		scrollbarCursorDeltaX = x - lmbDownX;
		scrollbarCursorDeltaY = y - lmbDownY;
		
		// Calculate the new scrollbar position here
		printf("gotcha! %lf %lf\n", scrollbarCursorX, scrollbarCursorY);
	}
}

void ScrollbarExpansion::mouseButton(Window& window, int button, int action, int mods)
{
	litehtml::position::vector v;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		lmbIsDown = true;
		lmbDownX = window.getCursorX();
		lmbDownY = window.getCursorY();
		lmbDownElement = window.elementUnderCursor();
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		lmbIsDown = false;
	}
}

void ScrollbarExpansion::scroll(Window& window, double xoffset, double yoffset)
{
	// Neutralizing overscroll
	if ((yScrollPos < -scrollDivHeight + window.getHeight()) && yoffset < 0) {
		yoffset = 0;
	}
	if (yScrollPos > 0 && yoffset > 0) {
		yoffset = 0;
	}
	
	yScrollPos += yoffset * SCROLL_VEL;
}
