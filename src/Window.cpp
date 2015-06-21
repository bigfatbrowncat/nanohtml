//
//  Window.cpp
//  nanohtml
//
//  Created by Ilya Mizus on 16.06.15.
//
//

#include <string>
#include <sstream>
#include <vector>

#include "default_style.h"

#include "Window.h"

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#define FONTS_PATH		"fonts/"
#define FONT_SCALE		1.2

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

std::map<GLFWwindow*, Window*> Window::handlersForWindows;
std::map<std::string, int> Window::handlersForImages;
bool Window::initialized = false;
litehtml::context Window::liteHTMLContext;

void Window::initialize()
{
	if (!initialized)
	{
		// Initializing LiteHTML
		liteHTMLContext.load_master_stylesheet(DEFAULT_STYLESHEET);

		// Initializing GLFW and creating a window
		
		if (!glfwInit()) {
			printf("Failed to init GLFW.");
			throw -1;
		}
		glfwSetErrorCallback(errorcb);
		initialized = true;
	}
}

void Window::errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

void Window::updateFrameSize()
{
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	
	// Calculate pixel ratio for hi-dpi devices.
	pxRatio = (float)fbWidth / (float)winWidth;
	
}

void Window::draw()
{
	glfwMakeContextCurrent(window);
	
	// Update and render
	glViewport(0, 0, fbWidth, fbHeight);
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	
	nvgBeginFrame(nvgContext, winWidth, winHeight, pxRatio);
	
	double t1 = glfwGetTime();
	

	for (auto iter = expansions.begin(); iter != expansions.end(); iter++)
	{
		(*iter)->render(*this);
	}
	document->render(winWidth);

	litehtml::position pos;
	pos.x = 0;
	pos.y = 0;
	pos.width = winWidth;
	pos.height = winHeight;
	
	for (auto iter = expansions.begin(); iter != expansions.end(); iter++)
	{
		(*iter)->draw(*this);
	}
	document->draw((litehtml::uint_ptr)NULL, 0, 0, &pos);
	
	finishDrawing();
	
	double t2 = glfwGetTime();
	
	double d = ((t2 - t1)) * 1000;
	framesTime += d;
	frameCounter ++;
	
	if (frameCounter == (int)fps * 3) {
		printf("%lf msec\n", framesTime / frameCounter);
		framesTime = 0;
		frameCounter = 0;
	}
	usleep(fmax((1.0 / fps - (t2 - t1)) * 1000000, 0));
	nvgEndFrame(nvgContext);
	
	glfwSwapBuffers(window);
}

void Window::key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void Window::windowSize(GLFWwindow* window, int width, int height)
{
	Window* handler = handlersForWindows[window];

	handler->updateFrameSize();
	handler->draw();
}

void Window::cursorPosition(GLFWwindow* window, double x, double y)
{
	Window* handler = handlersForWindows[window];
	
	handler->cursorX = x; handler->cursorY = y;
	
	litehtml::position::vector v;
	if (x > 0 && y > 0 && x < handler->getWidth() && y < handler->getHeight())
	{
		handler->document->on_mouse_over(x, y, x, y, v);
	}
	else
	{
		handler->document->on_mouse_leave(v);
	}

	for (auto iter = handler->expansions.begin(); iter != handler->expansions.end(); iter++)
	{
		(*iter)->cursorPosition(*handler, x, y);
	}
}

void Window::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	Window* handler = handlersForWindows[window];

	litehtml::position::vector v;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		handler->document->on_lbutton_down(handler->cursorX, handler->cursorY, handler->cursorX, handler->cursorY, v);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		handler->document->on_lbutton_up(handler->cursorX, handler->cursorY, handler->cursorX, handler->cursorY, v);
	}
	
	for (auto iter = handler->expansions.begin(); iter != handler->expansions.end(); iter++)
	{
		(*iter)->mouseButton(*handler, button, action, mods);
	}
}

void Window::scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	Window* handler = handlersForWindows[window];
	for (auto iter = handler->expansions.begin(); iter != handler->expansions.end(); iter++)
	{
		(*iter)->scroll(*handler, xoffset, yoffset);
	}

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	cursorPosition(window, xpos, ypos);
}

litehtml::uint_ptr Window::create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm)
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
		if (strcmp(*iter, "sans-serif") == 0) sans = true;
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
		std::string base = FONTS_PATH "Roboto/Roboto";
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
		std::string base = FONTS_PATH "RobotoSlab/RobotoSlab";
		if (weightType == wtRegular) {
			fontFile = base + "-Regular";
		} else {
			fontFile = base + "-" + weightNames[weightType];
		}
	}
	
	fontFile += ".ttf";
	
	printf("face: %s - loading font: %s\n", faceName, fontFile.c_str());
	
	nvgCreateFont(nvgContext, fontFile.c_str(), fontFile.c_str());
	
	// Selecting the font we've just created
	nvgFontFace(nvgContext, fontFile.c_str());
	nvgFontSize(nvgContext, size * FONT_SCALE);
	
	// Measuring the selected font
	float ascender, descender, lineh;
	nvgTextMetrics(nvgContext, &ascender, &descender, &lineh);
	
	fm->ascent = ascender;
	fm->descent = descender;
	fm->height = lineh;
	
	Font* f = new Font(fontFile, size, +ascender+descender-lineh);
	
	return (litehtml::uint_ptr)f;
}

void Window::delete_font(litehtml::uint_ptr hFont)
{
	delete (Font*)hFont;
}

int Window::text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont)
{
	Font& f = *(Font*)hFont;
	nvgFontFace(nvgContext, f.fontFace.c_str());
	nvgFontSize(nvgContext, f.size * FONT_SCALE);
	
	float bounds[4];
	return (int)nvgTextBounds(nvgContext, 0, 0, text, NULL, bounds);
}

void Window::draw_text(litehtml::uint_ptr /*hdc*/, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos)
{
	if (drawingState != dsText && drawingState != dsNone) {
		finishDrawing();
	}
	
	if (drawingState == dsNone) {
		nvgBeginPath(nvgContext);
		drawingState = dsText;
	}
	
	Font& f = *(Font*)hFont;
	nvgFontFace(nvgContext, f.fontFace.c_str());
	nvgFontSize(nvgContext, f.size * FONT_SCALE);
	
	nvgFillColor(nvgContext, nvgRGBA(color.red, color.green, color.blue, color.alpha));
	nvgText(nvgContext, pos.left(), pos.top() - f.deltaY, text, NULL);
}

static int pt_to_px(float pxRatio, int pt) {
	int res = (int)((float)pt / 0.75f * pxRatio);
	return res;
}

int Window::pt_to_px(int pt)
{
	return ::pt_to_px(pxRatio, pt);
}

int Window::get_default_font_size() const
{
	return ::pt_to_px(pxRatio, 12);
}

const litehtml::tchar_t* Window::get_default_font_name() const
{
	return "serif";
}

void Window::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker)
{
	
}

void Window::load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready)
{
	if (handlersForImages.find(src) == handlersForImages.end()) {
		printf("Loading image %s from %s...\n", src, baseurl);
		int handler = nvgCreateImage(nvgContext, src, 0);
		if (handler != 0) {
			handlersForImages.insert(std::pair<std::string, int>(std::string(src), handler));
		} else {
			printf("ERROR: Can't load image %s\n", src);
		}
	}
}

void Window::get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz)
{
	nvgImageSize(nvgContext, handlersForImages[src], &sz.width, &sz.height);
}

void Window::draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg)
{
	if (drawingState != dsNone) {
		finishDrawing();
	}
	nvgBeginPath(nvgContext);

	nvgFillColor(nvgContext, nvgRGBA(bg.color.red, bg.color.green, bg.color.blue, bg.color.alpha));
	if (bg.image != "") {
		int imgIndex = handlersForImages[bg.image];
		NVGpaint img = nvgImagePattern(nvgContext, bg.position_x, bg.position_y, bg.image_size.width, bg.image_size.height, 0.f, imgIndex, 1.f);
		nvgFillPaint(nvgContext, img);
	}

	// TODO Add support for different corner radiuses
	if (bg.border_radius.top_left_x > 0) {
		nvgRoundedRect(nvgContext, bg.border_box.left(), bg.border_box.top(), bg.border_box.width, bg.border_box.height, bg.border_radius.top_left_x);
	} else {
		nvgRect(nvgContext, bg.border_box.left(), bg.border_box.top(), bg.border_box.width, bg.border_box.height);
	}

	nvgFill(nvgContext);
}

void Window::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root)
{
	
}

void Window::set_caption(const litehtml::tchar_t* caption)
{
	glfwSetWindowTitle(window, caption);
}

void Window::set_base_url(const litehtml::tchar_t* base_url)
{
    printf("Set base url: %s\n", base_url);
}

void Window::link(litehtml::document* doc, litehtml::element::ptr el)
{
	printf("Link: %s\n", el->get_tagName());
}

void Window::on_anchor_click(const litehtml::tchar_t* url, litehtml::element::ptr el)
{
	printf("Click on: %s\n", el->get_tagName());
}

void Window::set_cursor(const litehtml::tchar_t* cursor)
{
	
}

void Window::transform_text(litehtml::tstring& text, litehtml::text_transform tt)
{
	
}

void Window::import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl)
{
	
}

void Window::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y)
{
	
}

void Window::del_clip()
{
	
}

void Window::get_client_rect(litehtml::position& client)
{
    client.x = 0;
    client.y = 0;
    client.width = winWidth;
    client.height = winHeight;
}

litehtml::element* Window::create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, litehtml::document* doc)
{
	return NULL;
}

void Window::get_media_features(litehtml::media_features& media)
{
    media.type = litehtml::media_type_screen;
    media.width = winWidth;
    media.height = winHeight;
    media.device_width = fbWidth;
    media.device_height = fbHeight;
    media.color = 32;
    //media.resolution = ??
}

void Window::finishDrawing()
{
	// The color has just changed
	if (drawingState == dsText) {
		// Filling in the previously written text
		nvgFill(nvgContext);
		drawingState = dsNone;
	}
}

Window::Window() : drawingState(dsNone)
{
	initialize();
	
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
#else
	glfwWindowHint(GLFW_SAMPLES, 2);
#endif

	window = glfwCreateWindow(600, 400, "nanohtml", NULL, NULL);
	if (!window) {
		glfwTerminate();
		throw -1;
	}
	
	handlersForWindows.insert(std::pair<GLFWwindow*, Window*>(window, this));
	
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
	
	updateFrameSize();
	
}

void Window::loadDocument(std::string htmlText)
{
	litehtml::document::ptr theNewDoc = litehtml::document::createFromUTF8(htmlText.c_str(), this, &liteHTMLContext);
	
	this->document = theNewDoc;
	
	for (auto iter = expansions.begin(); iter != expansions.end(); iter++)
	{
		(*iter)->documentLoaded(*this);
	}
	
}

void Window::loop()
{
	while (!glfwWindowShouldClose(window))
	{
		draw();
		glfwPollEvents();
	}
}

Window::~Window()
{
	nvgDeleteGL3(nvgContext);
	
	glfwTerminate();
	
	handlersForWindows.erase(window);
}
