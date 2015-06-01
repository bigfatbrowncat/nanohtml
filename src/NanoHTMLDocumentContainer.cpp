//
//  NanoHTMLDocumentContainer.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 31.05.15.
//
//

#include "nanohtml.h"
#include "NanoHTMLDocumentContainer.h"



litehtml::uint_ptr NanoHTMLDocumentContainer::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm)
{
	char *fontFile, *fontFace;
	bool bold = (weight > 500);

	if (strcmp(faceName, "sans") == 0) {
		if (bold)
		{
			fontFile = (char*)"fonts/DroidSans-Bold.ttf";
			fontFace = (char*)"sans_bold";
		}
		else
		{
			fontFile = (char*)"fonts/DroidSans.ttf";
			fontFace = (char*)"sans";
		}
	}
	else
	{
		if (bold)
		{
			fontFile = (char*)"fonts/DroidSerif-Bold.ttf";
			fontFace = (char*)"serif_bold";
		}
		else
		{
			fontFile = (char*)"fonts/DroidSerif-Regular.ttf";
			fontFace = (char*)"serif";
		}
	}

	litehtml::uint_ptr res = (litehtml::uint_ptr)(long)nvgCreateFont(nvgContext, fontFace, fontFile);

	nvgFontFace(nvgContext, fontFace);
	nvgFontSize(nvgContext, size);

	return res;
}

void NanoHTMLDocumentContainer::delete_font(litehtml::uint_ptr hFont)
{
	// TODO Do something here
}

int NanoHTMLDocumentContainer::text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont)
{
	float bounds[4];
	nvgTextBounds(nvgContext, 0, 0, text, NULL, bounds);
	return (int)(bounds[2] - bounds[0]);
}

void NanoHTMLDocumentContainer::draw_text(litehtml::uint_ptr /*hdc*/, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos)
{
	NVGcolor baseColor = nvgRGB(255, 255, 192);
	nvgFontFace(nvgContext, "sans");
	nvgFontSize(nvgContext, get_default_font_size());
	nvgFillColor(nvgContext, baseColor);
	nvgText(nvgContext, pos.left(), pos.top() + get_default_font_size(), text, NULL);
	nvgFill(nvgContext);
}

int NanoHTMLDocumentContainer::pt_to_px(int pt)
{
	// TODO Calculate correctly (int)((float)pt * 96 / dpi);
	return pt;
}

int NanoHTMLDocumentContainer::get_default_font_size() const
{
	return 15;
}

const litehtml::tchar_t* NanoHTMLDocumentContainer::get_default_font_name() const
{
	return "sans";
}

void NanoHTMLDocumentContainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
	
}

void NanoHTMLDocumentContainer::load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready)
{
	
}

void NanoHTMLDocumentContainer::get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz)
{
	
}

void NanoHTMLDocumentContainer::draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg)
{
	
}

void NanoHTMLDocumentContainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
	
}

void NanoHTMLDocumentContainer::set_caption(const litehtml::tchar_t* caption)
{
	
}

void NanoHTMLDocumentContainer::set_base_url(const litehtml::tchar_t* base_url)
{
	
}

void NanoHTMLDocumentContainer::link(litehtml::document* doc, litehtml::element::ptr el)
{
	
}

void NanoHTMLDocumentContainer::on_anchor_click(const litehtml::tchar_t* url, litehtml::element::ptr el)
{
	
}

void NanoHTMLDocumentContainer::set_cursor(const litehtml::tchar_t* cursor)
{
	
}

void NanoHTMLDocumentContainer::transform_text(litehtml::tstring& text, litehtml::text_transform tt)
{
	
}

void NanoHTMLDocumentContainer::import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl)
{
	
}

void NanoHTMLDocumentContainer::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y)
{
	
}

void NanoHTMLDocumentContainer::del_clip()
{
	
}

void NanoHTMLDocumentContainer::get_client_rect(litehtml::position& client)
{
	
}

litehtml::element* NanoHTMLDocumentContainer::create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, litehtml::document* doc)
{
	return NULL;
}

void NanoHTMLDocumentContainer::get_media_features(litehtml::media_features& media)
{
	
}
