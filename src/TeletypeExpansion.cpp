//
//  TeletypeExpansion.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 21.06.15.
//
//


#include "TeletypeExpansion.h"

void TeletypeExpansion::documentLoaded(Window& window)
{
	teletypePtr = &((litehtml::html_tag&)*window.getDocument()->root()->select_one(".teletype"));

	// Getting inner items
	contents = teletypePtr->children();
	teletypePtr->children().clear();
}

void TeletypeExpansion::render(Window& window)
{
	double dt = glfwGetTime() - startTimeSec;
	const char* startTime = teletypePtr->get_style_property("teletype-start-time", true);
	if (startTime != NULL)
	{
		dt -= stod(std::string(startTime));
	}
	
	charsToPrint = fmax(0, dt * 20.0);
	
	// Counting inners to print

	teletypePtr->children().clear();
	int p = 0;
	for (auto item = contents.begin(); item != contents.end(); item++)
	{
		std::string itemText;
		(*item)->get_text(itemText);
		
		if (p + itemText.length() <= charsToPrint)
		{
			teletypePtr->appendChild(*item);
			p += itemText.length();
		}
		else
		{
			litehtml::el_text::ptr inPtr = new litehtml::el_text(itemText.substr(0, charsToPrint - p).c_str(), window.getDocument());
			teletypePtr->appendChild(inPtr);
			inPtr->parse_styles();
			//teletypePtr->place_element(inPtr, 1000);
			break;
		}
	}
	
	//litehtml::el_text::ptr inPtr = new litehtml::el_text(text.substr(0, charsToPrint).c_str(), window.getDocument());
	//teletypePtr->appendChild(inPtr);
}

void TeletypeExpansion::draw(Window& window)
{
	
}

void TeletypeExpansion::cursorPosition(Window& window, double x, double y)
{
	
}

void TeletypeExpansion::mouseButton(Window& window, int button, int action, int mods)
{
	
}

void TeletypeExpansion::scroll(Window& window, double xoffset, double yoffset)
{
	
}
