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
	
	
	void draw()
	{
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		
		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;
		
		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
		glClearColor(1.f, 1.f, 1.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
		nvgBeginFrame(nvgContext, winWidth, winHeight, pxRatio);
		
		double t1 = glfwGetTime();
		
		doc->render(winWidth);
		
		// Neutralizing overscroll
		
		scrollDivHeight = scrollPtr->height();
		
		double yScrollVel = 0;
		if (yScrollPos < -scrollDivHeight + winHeight) {
			double delta = yScrollPos - (-scrollDivHeight + winHeight);
			yScrollVel = -delta / 2;
		}
		if (yScrollPos > 0) {
			yScrollVel = -yScrollPos / 2 + 0.1;
		}
		
		yScrollPos += yScrollVel / 2;
		
		// Setting scroll position
		{
			std::stringstream ss; ss << "position: relative; top: " << yScrollPos << "px;";
			scrollPtr->set_attr("style", ss.str().c_str());
			scrollPtr->parse_styles();
		}
		
		// Seting scrollbar size and position
		{
			if (scrollDivHeight > winHeight)
			{
				int sbHeight = winHeight * winHeight / scrollDivHeight - 4;
				int sbPos = (winHeight - sbHeight - 4) * (1.0 - yScrollPos / (scrollDivHeight - winHeight)) - winHeight + sbHeight + 6;
				std::stringstream ss; ss << "visibility: visible; border-radius: 3.5px; position: absolute; right: 2px; top: " << sbPos << "px; width: 7px; height: " << sbHeight << "px; background-color: rgba(0, 0, 0, 0.5);";
				scrollbarPtr->set_attr("style", ss.str().c_str());
				scrollbarPtr->parse_styles();
			} else {
				std::stringstream ss; ss << "visibility: hidden;";
				scrollbarPtr->set_attr("style", ss.str().c_str());
				scrollbarPtr->parse_styles();
			}
		}
		
		litehtml::position* ppos = NULL;
		doc->draw((litehtml::uint_ptr)NULL, 0, 0, ppos);
		
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

	void cursorPos(double x, double y)
	{
		litehtml::position::vector v;
		
		cursorX = x;
		cursorY = y;
		
		if (x > 0 && y > 0 && x < winWidth && y < winHeight) {
			doc->on_mouse_over(x, y, x, y, v);
		} else {
			doc->on_mouse_leave(v);
		}
		
		// Dragging the scrollbar
		if (lmbIsDown && lmbDownElement == scrollbarPtr) {
			scrollbarCursorX = lmbDownX - scrollbarPtr->left();
			scrollbarCursorY = lmbDownY - scrollbarPtr->top();
			
			scrollbarCursorDeltaX = cursorX - lmbDownX;
			scrollbarCursorDeltaY = cursorY - lmbDownY;
			
			// Calculate the new scrollbar position here
			printf("gotcha! %lf %lf\n", scrollbarCursorX, scrollbarCursorY);
		}
	}
	
	void mouseBtn(int button, int action, int mods)
	{
		litehtml::position::vector v;
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			doc->on_lbutton_down(cursorX, cursorY, cursorX, cursorY, v);
			lmbIsDown = true;
			lmbDownX = cursorX;
			lmbDownY = cursorY;
			lmbDownElement = doc->root()->get_element_by_point(cursorX, cursorY, cursorX, cursorY);
		} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
			doc->on_lbutton_up(cursorX, cursorY, cursorX, cursorY, v);
			lmbIsDown = false;
		}

	}
	
	void scroll(double xoffset, double yoffset)
	{
		// Neutralizing overscroll
		if ((yScrollPos < -scrollDivHeight + winHeight) && yoffset < 0) {
			yoffset = 0;
		}
		if (yScrollPos > 0 && yoffset > 0) {
			yoffset = 0;
		}
		
		yScrollPos += yoffset * SCROLL_VEL;
		
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		cursorPosition(window, xpos, ypos);
	}
	
	static void errorcb(int error, const char* desc)
	{
		printf("GLFW error %d: %s\n", error, desc);
	}
	
	static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		NVG_NOTUSED(scancode);
		NVG_NOTUSED(mods);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
	
	static void windowSize(GLFWwindow* window, int width, int height)
	{
		UIHandler* handler = handlersForWindows[window];
		handler->draw();
	}
	
	static void cursorPosition(GLFWwindow* window, double x, double y)
	{
		UIHandler* handler = handlersForWindows[window];
		handler->cursorPos(x, y);
	}
	
	static void mouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		UIHandler* handler = handlersForWindows[window];
		handler->mouseBtn(button, action, mods);
	}
	
	static void scroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		UIHandler* handler = handlersForWindows[window];
		handler->scroll(xoffset, yoffset);
	}
	
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
	void loadFonts();
	
public:
	UIHandler();
	virtual ~UIHandler();
	void setDoc(litehtml::document* doc);
	void loop();
};

#endif /* defined(__nanohtml__UIHandler__) */
