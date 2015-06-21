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
	litehtml::css_selector sel(litehtml::media_query_list::ptr(0));
	sel.parse(".teletype");

	teletypeElements = window.getDocument()->root()->select_all(sel);

	// Taking inner items out
	teletypeElementsContents.clear();
	for (auto item = teletypeElements.begin(); item != teletypeElements.end(); item++)
	{
		litehtml::html_tag* tag = ((litehtml::html_tag*)(litehtml::element*)(*item));
		
		litehtml::elements_vector contents = tag->children();
		teletypeElementsContents.push_back(contents);
		tag->children().clear();
	}
}

void TeletypeExpansion::render(Window& window)
{
	auto contents = teletypeElementsContents.begin();
	for (auto teletypeItem = teletypeElements.begin(); teletypeItem != teletypeElements.end(); teletypeItem++, contents++)
	{
		litehtml::html_tag* teletypePtr = ((litehtml::html_tag*)(litehtml::element*)(*teletypeItem));
		
		double dt = glfwGetTime() - startTimeSec;
		const char* startTime = teletypePtr->get_style_property("teletype-start-time", true);
		if (startTime != NULL)
		{
			dt -= stod(std::string(startTime));
		}
		
		int charsToPrint = fmax(0, dt * 20.0);
		
		// Counting inners to print

		teletypePtr->children().clear();
		int p = 0;
		for (auto item = contents->begin(); item != contents->end(); item++)
		{
			std::string itemText;
			(*item)->get_text(itemText);
			
			if (p + itemText.length() < charsToPrint)
			{
				teletypePtr->appendChild(*item);
				p += itemText.length();
			}
			else if (p < charsToPrint)
			{
				litehtml::el_text::ptr inPtr = new litehtml::el_text(itemText.substr(0, charsToPrint - p).c_str(), window.getDocument());
				
				teletypePtr->appendChild(inPtr);
				inPtr->parse_styles();
				break;
			}
			else
			{
				break;
			}
		}
		if (teletypePtr->children().size() == 0)
		{
			teletypePtr->get_position().height = 0;
		}
		
	}
}

void TeletypeExpansion::draw(Window& window)
{
	for (auto teletypeItem = teletypeElements.begin(); teletypeItem != teletypeElements.end(); teletypeItem++)
	{
		litehtml::html_tag* teletypePtr = ((litehtml::html_tag*)(litehtml::element*)(*teletypeItem));

		if (teletypePtr->children().size() == 0)
		{
			teletypePtr->get_position().height = 0;
			teletypePtr->get_position().width = 0;
		}
	}
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
