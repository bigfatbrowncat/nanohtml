#include "nanohtml.h"

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#include <sstream>

#include <stdio.h>
#include <math.h>

#include "default_style.h"
#include "NanoHTMLDocumentContainer.h"

#include "../src/el_div.h"

using namespace litehtml;

void getGlVersion(int *major, int *minor)
{
    const char *verstr = (const char *) glGetString(GL_VERSION);
    if ((verstr == NULL) || (sscanf(verstr,"%d.%d", major, minor) != 2))
    {
        *major = *minor = 0;
        fprintf(stderr, "Invalid GL_VERSION format!!!\n");
    }
}

void getGlslVersion(int *major, int *minor)
{
    int gl_major, gl_minor;
    getGlVersion(&gl_major, &gl_minor);

    *major = *minor = 0;
    if (gl_major == 1)
    {
        /* GL v1.x can only provide GLSL v1.00 as an extension */
        const char *extstr = (const char *) glGetString(GL_EXTENSIONS);
        if ((extstr != NULL) &&
            (strstr(extstr, "GL_ARB_shading_language_100") != NULL))
        {
            *major = 1;
            *minor = 0;
        }
    }
    else if (gl_major >= 2)
    {
        /* GL v2.0 and greater must parse the version string */
        const char *verstr =
            (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);

        if((verstr == NULL) ||
            (sscanf(verstr, "%d.%d", major, minor) != 2))
        {
            *major = *minor = 0;
            fprintf(stderr,
                "Invalid GL_SHADING_LANGUAGE_VERSION format!!!\n");
        }
    }
}

bool glInit()
{
    if (gl3wInit())
    {
        printf("Problem initializing OpenGL\n");
        return false;
    }

    int maj, min, slmaj, slmin;
    getGlVersion(&maj, &min);
    getGlslVersion(&slmaj, &slmin);

    printf("OpenGL version: %d.%d\n", maj, min);
    printf("GLSL version: %d.%d\n", slmaj, slmin);

    return true;
}

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

GLFWwindow* window;
NVGcontext* vg = NULL;
int fbWidth, fbHeight;
int winWidth, winHeight;
float pxRatio;
double fps = 25.0;

litehtml::document* doc;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int frameCounter = 0;
double framesTime = 0.0;

double yScrollPos = 0;
int scrollDivHeight = 0;

static void draw()
{
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

	// Calculate pixel ration for hi-dpi devices.
	pxRatio = (float)fbWidth / (float)winWidth;

	// Update and render
	glViewport(0, 0, fbWidth, fbHeight);
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

	double t1 = glfwGetTime();

	doc->render(winWidth);

	// Neutralizing overscroll
	
	litehtml::element::ptr scrollPtr = doc->root()->select_one("#scroll");
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
		litehtml::element::ptr scrollbarPtr = doc->root()->select_one("#scrollbar");
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
	//

	doc->draw(NULL, 0, 0, NULL);

	((NanoHTMLDocumentContainer*)doc->container())->finishDrawing();
	
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
	nvgEndFrame(vg);

	glfwSwapBuffers(window);
}

static void windowSize(GLFWwindow* window, int width, int height)
{
	draw();
}

int cursorX = 0, cursorY = 0;

static void cursorPosition(GLFWwindow* window, double x, double y)
{
	position::vector v;

	cursorX = x;
	cursorY = y;
	
	if (x > 0 && y > 0 && x < winWidth && y < winHeight) {
		doc->on_mouse_over(x, y, x, y, v);
	} else {
		doc->on_mouse_leave(v);
	}
	

	/*element::ptr el = doc->root()->get_element_by_point(x, y, x, y);
	if (el != NULL)
	{
		std::string str;
		el->get_text(str);
		if (str.length() > 20) {
			str = str.substr(0, 20) + "...";
		}
		
		printf("Element under cursor is <%s> (\"%s\")\n", el->get_tagName(), str.c_str());
	}*/
}

static void mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	position::vector v;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		doc->on_lbutton_down(cursorX, cursorY, cursorX, cursorY, v);
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		doc->on_lbutton_up(cursorX, cursorY, cursorX, cursorY, v);
	}
}

static void scroll(GLFWwindow* window, double xoffset, double yoffset)
{
	// Neutralizing overscroll
	if ((yScrollPos < -scrollDivHeight + winHeight) && yoffset < 0) {
		yoffset = 0;
	}
	if (yScrollPos > 0 && yoffset > 0) {
		yoffset = 0;
	}

	yScrollPos += yoffset * 5;
	
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	cursorPosition(window, xpos, ypos);
}


int main()
{

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
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
		return -1;
	}

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
		return -1;

    }
	
#ifdef DEMO_MSAA
	vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	glfwSwapInterval(0);

	glfwSetTime(0);
	
	NanoHTMLDocumentContainer dc(vg);
	litehtml::context ctx;
	ctx.load_master_stylesheet(DEFAULT_STYLESHEET);
	
	litehtml::document::ptr theNewDoc = litehtml::document::createFromUTF8(
		"<html>"
		"<head>"
		"	<title>Harry Potter and the Methods of Rationality</title>"
		"	<style> p:hover { color: #444444; } body {margin: 0;} h1 { color: #337711 } div.title { background-color:rgba(128, 255, 128, 0.3); padding-left: 20pt; padding-right: 20pt; padding-top: 20pt; padding-bottom: 10pt; }</style>"
		"</head>"
		"<body id=\"scroll\">"
//			"<div id=\"scroll\" style=\"position: relative; top: 0px;\">"
			"<div class=\"title\" style=\"position: relative; top: -100px; padding-top: 100px; margin-bottom: -100pt;\"><h1>Chapter 122</h1><h2 style=\"margin-top: 0;\">Something to Protect: Hermione Granger</h2></div>"
			"<div style=\"margin: 20pt\">"
			"<p>And it was evening and it was morning, the last day. June 15th, 1992.</p>"
			"<p>The beginning light of morning, the pre-dawn before sunrise, was barely brightening the sky. To the east of Hogwarts, where the Sun would rise, that faintest tinge of grey made barely visible the hilly horizon beyond the Quidditch stands.</p>"
			"<p>The stone terrace-platform where Harry now sat would be high enough to see the dawn beyond the hills below; he'd asked for that, when he was describing his new office.</p>"
			"<p>Harry was currently sitting cross-legged on a cushion, chilly pre-morning breezes stirring over his exposed hands and face. He'd ordered the house-elves to bring up the hand-glittered throne from his previous office as General Chaos... and then he'd told the elves to put it back, once it had occurred to Harry to start worrying about where his taste in decorations had come from and whether Voldemort had once possessed a similar throne. Which, itself, wasn't a knockdown argument - it wasn't like sitting on a glittery throne to survey the lands below Hogwarts was unethical in any way Harry's moral philosophy could make out - but Harry had decided that he needed to take time and think it through. Meanwhile, simple cushions would do well enough.</p>"
			"<p>In the room below, connected to the rooftop by a simple wooden ladder, was Harry's new office inside Hogwarts. A wide room, surrounded by full-wall windows on four sides for sunlight; currently bare of furnishings but for four chairs and a desk. Harry had told Headmistress McGonagall what he was looking for, and Headmistress McGonagall had put on the Sorting Hat and then told Harry the series of twists and turns that would take him where he wanted to be. High enough in Hogwarts that the castle shouldn't have been that tall, high enough in Hogwarts that nobody looking from the outside would see a piece of castle corresponding to where Harry now sat. It seemed like an elementary precaution against snipers that there was no reason not to take.</p>"
			"<p>Though, on the flip side, Harry had no idea where he currently was in any real sense. If his office couldn't be seen from the lands below, then how was Harry seeing the lands, how were photons making it from the landscape to him? On the western side of the horizon, stars still glittered, clear in the pre-dawn air. Were those photons the actual photons that had been emitted by huge plasma furnaces in the unimaginable distance? Or did Harry now sit within some dreaming vision of the Hogwarts castle? Or was it all, without any further explanation, 'just magic'? He needed to get electricity to work better around magic so he could experiment with shining lasers downward and upward.</p>"
			"<p>And yes, Harry had his own office on Hogwarts now. He didn't have any official title yet, but the Boy-Who-Lived was now a true fixture of the Hogwarts School of Witchcraft and Wizardry, the soon-to-be-home of the Philosopher's Stone and the world's only wizarding institution of genuinely higher education. It wasn't fully secured, but Professor Vector had put up some preliminary Charms and Runes to screen the office and its rooftop against eavesdropping.</p>"
			"</div>"
//			"</div>"
//			"<div id=\"scrollbar\"></div>"
		"</body>"
		"</html>", &dc, &ctx);

	doc = theNewDoc;
	
	// Adding a scrollbar to <body>
	el_div::ptr scrollbarPtr = new el_div(doc);
	scrollbarPtr->set_attr("id", "scrollbar");
	doc->root()->select_one("#scroll")->parent()->appendChild(scrollbarPtr);

	while (!glfwWindowShouldClose(window))
	{
		double mx, my;

		glfwGetCursorPos(window, &mx, &my);

		draw();
		
		glfwPollEvents();
	}

	nvgDeleteGL3(vg);

	glfwTerminate();
	return 0;
}
