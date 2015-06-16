//
//  UIHandler.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 16.06.15.
//
//

#include <string>
#include <sstream>
#include <vector>

#include "nanohtml.h"
#include "UIHandler.h"

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#define FONTS_PATH "fonts/"

enum WeightType
{
	wtThin = 0,
	wtLight = 1,
	wtRegular = 2,
	wtMedium = 3,
	wtBold = 4,
	wtBlack = 5
};
static std::string weightNames[] = { "Thin", "Light", "Regular", "Medium", "Bold", "Black" };

char* cutToken(char* str, char* delims)
{
	while ((strchr(delims, *str) == NULL) && (*str != 0))
	{
		str++;
	}
	if (*str == 0)
	{
		return NULL;
	}
	else
	{
		*str = 0;
		str++;
	}
	
	while ((strchr(delims, *str) != NULL) && (*str != 0))
	{
		str++;
	}
	if (*str == 0)
	{
		return NULL;
	}
	return str;
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

std::map<GLFWwindow*, UIHandler*> UIHandler::handlersForWindows;


litehtml::uint_ptr UIHandler::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm)
{
	bool condensed = false;
	bool sans = false;
	
	std::vector<char*> faceNameTokens;
	char* faceNameDup = strdup(faceName);
	char* token = faceNameDup;
	do
	{
		char* tokenNew = cutToken(token, (char*)" \t\n\r");
		faceNameTokens.push_back(token);
		token = tokenNew;
	} while (token != NULL);
	
	for (std::vector<char*>::iterator iter = faceNameTokens.begin(); iter != faceNameTokens.end(); iter++)
	{
		if (strcmp(*iter, "sans") == 0) sans = true;
		if (strcmp(*iter, "Roboto") == 0) sans = true;
		if (strcmp(*iter, "condensed") == 0) condensed = true;
	}
	free(faceNameDup);
	
	WeightType weightType;
	
	if (weight < 200) {
		weightType = wtThin;
	} else if (weight < 350) {
		weightType = wtLight;
	} else if (weight < 450) {
		weightType = wtRegular;
	} else if (weight < 600) {
		weightType = wtMedium;
	} else if (weight < 800) {
		weightType = wtBold;
	}
	
	std::string fontFile, fontFace;
	
	if (sans == true) {
		std::string base = FONTS_PATH "Roboto";
		if (condensed) {
			base += "Condensed";
		}
		
		if (weightType == wtRegular) {
			if (italic == litehtml::font_style::fontStyleItalic) {
				fontFile = base + "-Italic";
			} else {
				fontFile = base + "-Regular";
			}
		} else {
			
			fontFile = base + "-" + weightNames[weightType];
			
			if (italic == litehtml::font_style::fontStyleItalic)
			{
				fontFile += "Italic";
			}
		}
	} else {
		// TODO
	}
	
	fontFile += ".ttf";
	
	printf("face: %s - loading font: %s\n", faceName, fontFile.c_str());
	
	nvgCreateFont(nvgContext, fontFile.c_str(), fontFile.c_str());
	
	// Selecting the font we've just created
	nvgFontFace(nvgContext, fontFile.c_str());
	nvgFontSize(nvgContext, size);
	
	// Measuring the selected font
	float ascender, descender, lineh;
	nvgTextMetrics(nvgContext, &ascender, &descender, &lineh);
	
	fm->ascent = ascender;
	fm->descent = descender;
	fm->height = lineh;
	
	Font* f = new Font(fontFile, size, +ascender+descender-lineh);
	currentSelectedFont = f;
	
	return (litehtml::uint_ptr)f;
}

void UIHandler::delete_font(litehtml::uint_ptr hFont)
{
	delete (Font*)hFont;
}

int UIHandler::text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont)
{
	Font& f = *(Font*)hFont;
	if (currentSelectedFont != &f)
	{
		nvgFontFace(nvgContext, f.fontFace.c_str());
		nvgFontSize(nvgContext, f.size);
		currentSelectedFont = &f;
	}
	
	float bounds[4];
	return (int)nvgTextBounds(nvgContext, 0, 0, text, NULL, bounds);
}

void UIHandler::draw_text(litehtml::uint_ptr /*hdc*/, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos)
{
	if (drawingState != dsText && drawingState != dsNone) {
		finishDrawing();
	}
	
	if (drawingState == dsNone) {
		nvgBeginPath(nvgContext);
		drawingState = dsText;
	}
	
	Font& f = *(Font*)hFont;
	if (currentSelectedFont != &f)
	{
		nvgFontFace(nvgContext, f.fontFace.c_str());
		nvgFontSize(nvgContext, f.size);
		currentSelectedFont = &f;
	}
	
	nvgFillColor(nvgContext, nvgRGBA(color.red, color.green, color.blue, color.alpha));
	nvgText(nvgContext, pos.left(), pos.top() - f.deltaY, text, NULL);
}

int UIHandler::pt_to_px(int pt)
{
	// TODO Calculate correctly (int)((float)pt * 96 / dpi);
	return pt;
}

int UIHandler::get_default_font_size() const
{
	return 17;
}

const litehtml::tchar_t* UIHandler::get_default_font_name() const
{
	return "sans";
}

void UIHandler::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
	
}

void UIHandler::load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready)
{
	
}

void UIHandler::get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz)
{
	
}

void UIHandler::draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg)
{
	if (drawingState != dsNone) {
		finishDrawing();
	}
	nvgBeginPath(nvgContext);
	nvgFillColor(nvgContext, nvgRGBA(bg.color.red, bg.color.green, bg.color.blue, bg.color.alpha));
	
	if (bg.border_radius.top_left_x > 0) {
		nvgRoundedRect(nvgContext, bg.border_box.left(), bg.border_box.top(), bg.border_box.width, bg.border_box.height, bg.border_radius.top_left_x);
	} else {
		nvgRect(nvgContext, bg.border_box.left(), bg.border_box.top(), bg.border_box.width, bg.border_box.height);
	}
	
	nvgFill(nvgContext);
}

void UIHandler::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
	
}

void UIHandler::set_caption(const litehtml::tchar_t* caption)
{
	
}

void UIHandler::set_base_url(const litehtml::tchar_t* base_url)
{
	
}

void UIHandler::link(litehtml::document* doc, litehtml::element::ptr el)
{
	
}

void UIHandler::on_anchor_click(const litehtml::tchar_t* url, litehtml::element::ptr el)
{
	printf("Click at: %s", el->get_tagName());
}

void UIHandler::set_cursor(const litehtml::tchar_t* cursor)
{
	
}

void UIHandler::transform_text(litehtml::tstring& text, litehtml::text_transform tt)
{
	
}

void UIHandler::import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl)
{
	
}

void UIHandler::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y)
{
	
}

void UIHandler::del_clip()
{
	
}

void UIHandler::get_client_rect(litehtml::position& client)
{
	
}

litehtml::element* UIHandler::create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, litehtml::document* doc)
{
	return NULL;
}

void UIHandler::get_media_features(litehtml::media_features& media)
{
	
}

void UIHandler::finishDrawing()
{
	// The color has just changed
	if (drawingState == dsText) {
		// Filling in the previously written text
		nvgFill(nvgContext);
		currentSelectedFont = NULL;
		drawingState = dsNone;
	}
}

void UIHandler::loadFonts()
{
	
}

UIHandler::UIHandler() : drawingState(dsNone), currentSelectedFont(NULL)
{
	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		throw -1;
	}
	
	glfwSetErrorCallback(errorcb);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
	
	
#ifdef DEMO_MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif
	
	window = glfwCreateWindow(600, 400, "nanohtml", NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw -1;
	}
	
	handlersForWindows.insert(std::pair<GLFWwindow*, UIHandler*>(window, this));
	
	glfwSetKeyCallback(window, key);
	glfwSetWindowSizeCallback(window, windowSize);
	glfwSetCursorPosCallback(window, cursorPosition);
	glfwSetMouseButtonCallback(window, mouseButton);
	glfwSetScrollCallback(window, scroll);
	
	glfwMakeContextCurrent(window);
	if (!glInit())
	{
		printf("Error: can't initialize GL3 API\n");
		glfwTerminate();
		throw -1;
	}
	
#ifdef DEMO_MSAA
	nvgContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (nvgContext == NULL) {
		printf("Could not init nanovg.\n");
		throw -1;
	}
	
	glfwSwapInterval(0);
	glfwSetTime(0);
	
}

UIHandler::~UIHandler()
{
	nvgDeleteGL3(nvgContext);
	
	glfwTerminate();
	
	handlersForWindows.erase(window);
}

void UIHandler::setDoc(litehtml::document* doc)
{
	this->doc = doc;
	
	scrollPtr = doc->root()->select_one("#scroll");
	
	// Adding a scrollbar to <body>
	scrollbarPtr = new litehtml::el_div(doc);
	scrollbarPtr->set_attr("id", "scrollbar");
	scrollPtr->parent()->appendChild(scrollbarPtr);
	
}

void UIHandler::loop()
{
	while (!glfwWindowShouldClose(window))
	{
		double mx, my;
		
		glfwGetCursorPos(window, &mx, &my);
		
		draw();
		
		glfwPollEvents();
	}
}
