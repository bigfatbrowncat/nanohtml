//
//  NanoHTMLDocumentContainer.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 31.05.15.
//
//

#include "nanohtml.h"
#include "NanoHTMLDocumentContainer.h"

#define ROBOTO_PATH "fonts/Roboto"

litehtml::uint_ptr NanoHTMLDocumentContainer::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm)
{
	char *fontFile, *fontFace;
	
	enum WeightTypes
	{
		wtThin, wtLight, wtRegular, wtMedium, wtBold, wtBlack
	} weightType;

	bool condensed;
	bool sans;


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
	
	// Creating the new font
	char fontNameFull[256];
	sprintf(fontNameFull, "%.240s-%d", fontFace, size);
	
	//std::string fontNameFull = std::string(fontFace) + size;
	Font* f = new Font(std::string(fontNameFull), size);
	nvgCreateFont(nvgContext, fontNameFull, fontFile);

	// Selecting the font we've just created
	nvgFontFace(nvgContext, f->fontFace.c_str());
	nvgFontSize(nvgContext, f->size);
	currentSelectedFont = f;

	// Measuring the selected font
	float ascender, descender, lineh;
	nvgTextMetrics(nvgContext, &ascender, &descender, &lineh);
	
	fm->ascent = ascender;
	fm->descent = descender;
	fm->height = lineh;

	return (litehtml::uint_ptr)f;
}

void NanoHTMLDocumentContainer::delete_font(litehtml::uint_ptr hFont)
{
	// TODO Do something here
}

int NanoHTMLDocumentContainer::text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont)
{
	const Font& f = *(Font*)hFont;
	if (currentSelectedFont != hFont)
	{
		nvgFontFace(nvgContext, f.fontFace.c_str());
		nvgFontSize(nvgContext, f.size);
		currentSelectedFont = hFont;
	}

	float bounds[4];
	nvgTextBounds(nvgContext, 0, 0, text, NULL, bounds);
	return (int)(bounds[2] - bounds[0]);
}

static bool operator==(const litehtml::web_color& c1, const litehtml::web_color& c2)
{
	if (c1.alpha == 0 && c2.alpha == 0)
	{
		return true;
	}
	else
	{
		return (c1.red == c2.red) && (c1.green == c2.green) && (c1.blue && c2.blue) && (c1.alpha == c2.alpha);
	}
}
static bool operator!=(const litehtml::web_color& c1, const litehtml::web_color& c2)
{
	return !operator==(c1, c2);
}


void NanoHTMLDocumentContainer::draw_text(litehtml::uint_ptr /*hdc*/, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos)
{
	const Font& f = *(Font*)hFont;
	if (currentSelectedFont != hFont)
	{
		nvgFontFace(nvgContext, f.fontFace.c_str());
		nvgFontSize(nvgContext, f.size);
		currentSelectedFont = hFont;
	}
	
	if (color != currentColor) {
		finishDrawing();
		// Setting the new color
		currentColor = color;
		nvgFillColor(nvgContext, nvgRGBA(currentColor.red, currentColor.green, currentColor.blue, currentColor.alpha));
	}
	nvgText(nvgContext, pos.left(), pos.top(), text, NULL);
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

void NanoHTMLDocumentContainer::finishDrawing()
{
	// The color has just changed
	if (drawingState == dsText) {
		// Filling in the previously written text
		nvgFill(nvgContext);
	}
}

