#include "nanohtml.h"

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#include "NanoHTMLDocumentContainer.h"

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

int blowup = 0;
int screenshot = 0;
int premult = 0;

GLFWwindow* window;
NVGcontext* vg = NULL;
int fbWidth, fbHeight;
int winWidth, winHeight;
float pxRatio;
double fps = 25.0;

void drawTextCenter(NVGcontext* vg, char* text, int x, int y) {
	float bounds[4];
	nvgTextBounds(vg, 0, 0, text, NULL, bounds);
	int dx = x - (bounds[2] - bounds[0]) / 2, dy = y - (bounds[3] + bounds[1]) / 2;
	nvgText(vg, dx, dy, text, NULL);
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
}

static void draw(litehtml::document::ptr doc)
{
	glfwGetWindowSize(window, &winWidth, &winHeight);
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

	// Calculate pixel ration for hi-dpi devices.
	pxRatio = (float)fbWidth / (float)winWidth;

	// Update and render
	glViewport(0, 0, fbWidth, fbHeight);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

	double t1 = glfwGetTime();
	doc->render(fbWidth);
	doc->draw(NULL, 0, 0, NULL);
	
	//drawClock(vg, winWidth, winHeight);
	double t2 = glfwGetTime();

	usleep(fmax((1.0 / fps - (t2 - t1)) * 1000000, 0));
	nvgEndFrame(vg);

	glfwSwapBuffers(window);
}

static void windowSize(GLFWwindow* window, int width, int height)
{
	//draw();
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

/*	nvgCreateFont(vg, "bold", "fonts/Walkway_Bold.ttf");
	nvgCreateFont(vg, "black", "fonts/Walkway_Black.ttf");*/
	
	NanoHTMLDocumentContainer dc(vg);
	litehtml::context ctx;
	ctx.load_master_stylesheet("html { "
		"    display: block; "
		"    position: relative; "
		"} "
		
		"head { "
		"    display: none "
		"} "
		
		"meta { "
		"    display: none "
		"} "
		
		"title { "
		"    display: none "
		"} "
		
		"link { "
		"    display: none "
		"} "
		
		"style { "
		"    display: none "
		"} "
		
		"script { "
		"    display: none "
		"} "
		
		"body { "
		"	display:block;  "
		"	margin:8px;  "
		"} "
		
		"p { "
		"	display:block;  "
		"	margin-top:1em;  "
		"	margin-bottom:1em; "
		"} "
		
		"b, strong { "
		"	display:inline;  "
		"	font-weight:bold; "
		"} "
		
		"i, em { "
		"	display:inline;  "
		"	font-style:italic; "
		"} "
		
		"center  "
		"{ "
		"	text-align:center; "
		"	display:block; "
		"} "
		
		"a:link "
		"{ "
		"	text-decoration: underline; "
		"	color: #00f; "
		"	cursor: pointer; "
		"} "
		
		"h1, h2, h3, h4, h5, h6, div { "
		"	display:block; "
		"} "
		
		"h1 { "
		"	font-weight:bold;  "
		"	margin-top:0.67em;  "
		"	margin-bottom:0.67em;  "
		"	font-size: 2em; "
		"} "
		
		"h2 { "
		"	font-weight:bold;  "
		"	margin-top:0.83em;  "
		"	margin-bottom:0.83em;  "
		"	font-size: 1.5em; "
		"} "
		
		"h3 { "
		"	font-weight:bold;  "
		"	margin-top:1em;  "
		"	margin-bottom:1em;  "
		"	font-size:1.17em; "
		"} "
		
		"h4 { "
		"	font-weight:bold;  "
		"	margin-top:1.33em;  "
		"	margin-bottom:1.33em "
		"} "
		
		"h5 { "
		"	font-weight:bold;  "
		"	margin-top:1.67em;  "
		"	margin-bottom:1.67em; "
		"	font-size:.83em; "
		"} "
		
		"h6 { "
		"	font-weight:bold;  "
		"	margin-top:2.33em;  "
		"	margin-bottom:2.33em; "
		"	font-size:.67em; "
		"}  "
		
		"br { "
		"	display:inline-block; "
		"} "
		
		"span { "
		"	display:inline "
		"} "
		
		"img { "
		"	display: inline-block; "
		"} "
		
		"img[align=\"right\"] "
		"{ "
		"	float: right; "
		"} "
		
		"img[align=\"left\"] "
		"{ "
		"	float: left; "
		"} "
		
		"hr { "
		"    display: block; "
		"    margin-top: 0.5em; "
		"    margin-bottom: 0.5em; "
		"    margin-left: auto; "
		"    margin-right: auto; "
		"    border-style: inset; "
		"    border-width: 1px "
		"} "
		
		
		"/***************** TABLES ********************/ "
		
		"table { "
		"    display: table; "
		"    border-collapse: separate; "
		"    border-spacing: 2px; "
		"    border-top-color:gray; "
		"    border-left-color:gray; "
		"    border-bottom-color:black; "
		"    border-right-color:black; "
		"} "
		
		"tbody, tfoot, thead { "
		"	display:table-row-group; "
		"	vertical-align:middle; "
		"} "
		
		"tr { "
		"    display: table-row; "
		"    vertical-align: inherit; "
		"    border-color: inherit; "
		"} "
		
		"td, th { "
		"    display: table-cell; "
		"    vertical-align: inherit; "
		"    border-width:1px; "
		"    padding:1px; "
		"} "
		
		"th { "
		"	font-weight: bold; "
		"} "
		
		"table[border] { "
		"    border-style:solid; "
		"} "
		
		"table[border|=0] { "
		"    border-style:none; "
		"} "
		
		"table[border] td, table[border] th { "
		"    border-style:solid; "
		"    border-top-color:black; "
		"    border-left-color:black; "
		"    border-bottom-color:gray; "
		"    border-right-color:gray; "
		"} "
		
		"table[border|=0] td, table[border|=0] th { "
		"    border-style:none; "
		"} "
		
		"caption { "
		"	display: table-caption; "
		"} "
		
		"td[nowrap], th[nowrap] { "
		"	white-space:nowrap; "
		"} "
		
		"tt, code, kbd, samp { "
		"    font-family: monospace "
		"} "
		
		"pre, xmp, plaintext, listing { "
		"    display: block; "
		"    font-family: monospace; "
		"    white-space: pre; "
		"    margin: 1em 0 "
		"} "
		
		"/***************** LISTS ********************/ "
		
		"ul, menu, dir { "
		"    display: block; "
		"    list-style-type: disc; "
		"    margin-top: 1em; "
		"    margin-bottom: 1em; "
		"    margin-left: 0; "
		"    margin-right: 0; "
		"    padding-left: 40px "
		"} "
		
		"ol { "
		"    display: block; "
		"    list-style-type: decimal; "
		"    margin-top: 1em; "
		"    margin-bottom: 1em; "
		"    margin-left: 0; "
		"    margin-right: 0; "
		"    padding-left: 40px "
		"} "
		
		"li { "
		"    display: list-item; "
		"} "
		
		"ul ul, ol ul { "
		"    list-style-type: circle; "
		"} "
		
		"ol ol ul, ol ul ul, ul ol ul, ul ul ul { "
		"    list-style-type: square; "
		"} "
		
		"dd { "
		"    display: block; "
		"    margin-left: 40px; "
		"} "
		
		"dl { "
		"    display: block; "
		"    margin-top: 1em; "
		"    margin-bottom: 1em; "
		"    margin-left: 0; "
		"    margin-right: 0; "
		"} "
		
		"dt { "
		"    display: block; "
		"} "
		
		"ol ul, ul ol, ul ul, ol ol { "
		"    margin-top: 0; "
		"    margin-bottom: 0 "
		"} "
		
		"blockquote { "
		"	display: block; "
		"	margin-top: 1em; "
		"	margin-bottom: 1em; "
		"	margin-left: 40px; "
		"	margin-left: 40px; "
		"} "
		
		"/*********** FORM ELEMENTS ************/ "
		
		"form { "
		"	display: block; "
		"	margin-top: 0em; "
		"} "
		
		"option { "
		"	display: none; "
		"} "
		
		"input, textarea, keygen, select, button, isindex { "
		"	margin: 0em; "
		"	color: initial; "
		"	line-height: normal; "
		"	text-transform: none; "
		"	text-indent: 0; "
		"	text-shadow: none; "
		"	display: inline-block; "
		"} "
		"input[type=\"hidden\"] { "
		"	display: none; "
		"} "
		
		"article, aside, footer, header, hgroup, nav, section  "
		"{ "
		"	display: block; "
		"} ");
	
	litehtml::document::ptr doc = litehtml::document::createFromUTF8("<html><head><title>the title</title></head><body>the text</body></html>", &dc, &ctx);

	while (!glfwWindowShouldClose(window))
	{
		double mx, my;

		glfwGetCursorPos(window, &mx, &my);

		draw(doc);
		
		glfwPollEvents();
	}

	nvgDeleteGL3(vg);

	glfwTerminate();
	return 0;
}
