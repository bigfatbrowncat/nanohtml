//
//  TeletypeExpansion.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 21.06.15.
//
//

#include "default_style.h"

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
		
		int charsToPrint = fmax(0, dt * 20);
		
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
				litehtml::el_div::ptr wrapPtr = new litehtml::el_div(window.getDocument());
				wrapPtr->set_tagName("div");
				litehtml::el_div::ptr wrapPtr1 = new litehtml::el_div(window.getDocument());
				wrapPtr1->set_tagName("div");
				litehtml::el_div::ptr wrapPtr2 = new litehtml::el_div(window.getDocument());
				wrapPtr2->set_tagName("div");

				litehtml::el_text::ptr inPtr1 = new litehtml::el_text(itemText.substr(0, charsToPrint - p).c_str(), window.getDocument());
				litehtml::el_text::ptr inPtr2 = new litehtml::el_text(itemText.substr(charsToPrint - p, itemText.length() - (charsToPrint - p)).c_str(), window.getDocument());
				
				wrapPtr1->appendChild(inPtr1);
				wrapPtr2->appendChild(inPtr2);
				
				wrapPtr->appendChild(wrapPtr1);
				wrapPtr->appendChild(wrapPtr2);

				teletypePtr->appendChild(wrapPtr);

				{
					litehtml::media_query_list::ptr media;
					litehtml::css css1, css2;
					css1.parse_stylesheet("div { display: inline-block }", 0, 0, media);
					css2.parse_stylesheet("div { visibility: hidden }", 0, 0, media);
					css1.sort_selectors();
					css2.sort_selectors();
					wrapPtr->apply_stylesheet(css1);
					wrapPtr2->apply_stylesheet(css2);
				}


/*				wrapPtr1->parse_styles();
				wrapPtr2->parse_styles();*/
				wrapPtr->parse_styles();

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
