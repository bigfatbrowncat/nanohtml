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
		wrappers.push_back(NULL);
		
		tag->children().clear();
	}
}

void TeletypeExpansion::render(Window& window)
{
	auto contents = teletypeElementsContents.begin();
	auto wrapper = wrappers.begin();
	for (auto teletypeItem = teletypeElements.begin(); teletypeItem != teletypeElements.end(); teletypeItem++, contents++, wrapper++)
	{
		litehtml::html_tag* teletypePtr = ((litehtml::html_tag*)(litehtml::element*)(*teletypeItem));
		
		double dt = glfwGetTime() - startTimeSec;
		const char* startTime = teletypePtr->get_style_property("teletype-start-time", true);
		if (startTime != NULL)
		{
			dt -= stod(std::string(startTime));
		}

		double velocity = 20;
		const char* velocityStr = teletypePtr->get_style_property("teletype-velocity", true);
		if (velocityStr != NULL)
		{
			velocity = stod(std::string(velocityStr));
		}
		
		int charsToPrint = fmax(0, dt * velocity);
		
		// Counting inners to print

		teletypePtr->children().clear();
		int p = 0;
		for (auto item = contents->begin(); item != contents->end(); item++)
		{
			std::string itemText;
			(*item)->get_text(itemText);
			
			if (p + itemText.length() < charsToPrint)
			{
				if ((*wrapper) != NULL)
				{
					(*wrapper)->clearRecursive();
				}

				teletypePtr->appendChild(*item);
				p += itemText.length();
			}
			else if (p < charsToPrint)
			{
				if ((*wrapper) != NULL)
				{
					(*wrapper)->clearRecursive();
				}

				*wrapper = new litehtml::el_div(window.getDocument());
				(*wrapper)->set_tagName("div");
				litehtml::el_div::ptr wrapPtr1 = new litehtml::el_div(window.getDocument());
				wrapPtr1->set_tagName("div");
				litehtml::el_div::ptr wrapPtr2 = new litehtml::el_div(window.getDocument());
				wrapPtr2->set_tagName("div");

				litehtml::el_text::ptr inPtr1 = new litehtml::el_text(itemText.substr(0, charsToPrint - p).c_str(), window.getDocument());
				litehtml::el_text::ptr inPtr2 = new litehtml::el_text(itemText.substr(charsToPrint - p, itemText.length() - (charsToPrint - p)).c_str(), window.getDocument());

				wrapPtr1->appendChild(inPtr1);
				wrapPtr2->appendChild(inPtr2);
				
				(*wrapper)->appendChild(wrapPtr1);
				(*wrapper)->appendChild(wrapPtr2);

				teletypePtr->appendChild(*wrapper);

				{
					litehtml::media_query_list::ptr media;
					litehtml::css css, css1, css2;
					
					css.parse_stylesheet(DEFAULT_STYLESHEET, 0, 0, media);
					css1.parse_stylesheet("div { display: inline-block }", 0, 0, media);
					css2.parse_stylesheet("div { visibility: hidden }", 0, 0, media);
					
					css.sort_selectors();
					css1.sort_selectors();
					css2.sort_selectors();

					(*wrapper)->apply_stylesheet(css);
					wrapPtr1->apply_stylesheet(css);
					wrapPtr2->apply_stylesheet(css);

					(*wrapper)->apply_stylesheet(css1);
					wrapPtr2->apply_stylesheet(css2);
				}

				(*wrapper)->parse_styles();
/*				wrapPtr1->parse_styles();
				wrapPtr2->parse_styles();
				inPtr1->parse_styles();
				inPtr2->parse_styles();*/

/*				inPtr1->on_mouse_over();
				inPtr2->on_mouse_over();*/

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
