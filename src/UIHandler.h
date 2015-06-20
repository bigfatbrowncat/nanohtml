//
//  UIHandler.h
//  nanohtml
//
//  Created by Ilya Mizus on 16.06.15.
//
//

#ifndef __nanohtml__UIHandler__
#define __nanohtml__UIHandler__

#include <sstream>
#include <map>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

#include <GL3/gl3w.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include "gl_tools.h"
#include "litehtml.h"
#include "../src/el_div.h"

#include <nanovg.h>

	
#ifdef WIN32
#	define SCROLL_VEL		25
#else
#	define SCROLL_VEL		5
#endif

class UIHandler : public litehtml::document_container
{
private:
	enum DrawingState
	{
		dsNone, dsText, dsBackground
	};
	
	struct Font
	{
		const std::string fontFace;
		const int size;
		const int deltaY;
		Font(const std::string& fontFace, int size, int deltaY): fontFace(fontFace), size(size), deltaY(deltaY) { }
	};
	
	static std::map<GLFWwindow*, UIHandler*> handlersForWindows;
	static std::map<std::string, int> handlersForImages;
	
	GLFWwindow* window;
	NVGcontext* nvgContext;
	int fbWidth, fbHeight;
	int winWidth, winHeight;
	float pxRatio;
	double fps = 25.0;
	
	int frameCounter = 0;
	double framesTime = 0.0;
	
	double yScrollPos = 0;
	int scrollDivHeight = 0;
	
	litehtml::document* doc;
	litehtml::el_div::ptr scrollbarPtr;
	litehtml::element::ptr scrollPtr;
	
	double cursorX = 0, cursorY = 0;
	
	bool lmbIsDown = false;
	double lmbDownX = 0, lmbDownY = 0;
	litehtml::element::ptr lmbDownElement = NULL;
	
	double scrollbarCursorX = 0, scrollbarCursorY = 0;
	double scrollbarCursorDeltaX = 0, scrollbarCursorDeltaY = 0;
	
	DrawingState drawingState;
	Font* currentSelectedFont;
	
	void updateFrame();
	void draw();
	void cursorPos(double x, double y);
	void mouseBtn(int button, int action, int mods);
	void scroll(double xoffset, double yoffset);
	
	static void errorcb(int error, const char* desc);
	static void key(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void windowSize(GLFWwindow* window, int width, int height);
	static void cursorPosition(GLFWwindow* window, double x, double y);
	static void mouseButton(GLFWwindow* window, int button, int action, int mods);
	static void scroll(GLFWwindow* window, double xoffset, double yoffset);
	
	virtual litehtml::uint_ptr create_font(const litehtml::tchar_t* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm);
	virtual void delete_font(litehtml::uint_ptr hFont);
	virtual int	text_width(const litehtml::tchar_t* text, litehtml::uint_ptr hFont);
	virtual void draw_text(litehtml::uint_ptr hdc, const litehtml::tchar_t* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos);
	virtual int pt_to_px(int pt);
	virtual int get_default_font_size() const;
	virtual const litehtml::tchar_t* get_default_font_name() const;
	virtual void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker);
	virtual void load_image(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, bool redraw_on_ready);
	virtual void get_image_size(const litehtml::tchar_t* src, const litehtml::tchar_t* baseurl, litehtml::size& sz);
	virtual void draw_background(litehtml::uint_ptr hdc, const litehtml::background_paint& bg);
	virtual void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root);
	
	virtual	void set_caption(const litehtml::tchar_t* caption);
	virtual	void set_base_url(const litehtml::tchar_t* base_url);
	virtual void link(litehtml::document* doc, litehtml::element::ptr el);
	virtual void on_anchor_click(const litehtml::tchar_t* url, litehtml::element::ptr el);
	virtual	void set_cursor(const litehtml::tchar_t* cursor);
	virtual	void transform_text(litehtml::tstring& text, litehtml::text_transform tt);
	virtual void import_css(litehtml::tstring& text, const litehtml::tstring& url, litehtml::tstring& baseurl);
	virtual void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius, bool valid_x, bool valid_y);
	virtual void del_clip();
	virtual void get_client_rect(litehtml::position& client);
	virtual litehtml::element* create_element(const litehtml::tchar_t* tag_name, const litehtml::string_map& attributes, litehtml::document* doc);
	virtual void get_media_features(litehtml::media_features& media);
	
	void finishDrawing();
	
public:
	UIHandler();
	virtual ~UIHandler();
	void setDoc(litehtml::document* doc);
	void loop();
};

#endif /* defined(__nanohtml__UIHandler__) */
