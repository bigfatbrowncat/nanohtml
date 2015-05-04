#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include <GL3/gl3w.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

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

void drawClock(NVGcontext* vg, int screenWidth, int screenHeight) {
	int clockRadius = (fmin(screenWidth, screenHeight) - 10) / 2;

	NVGcolor baseColor = nvgRGB(145, 100, 0);
	NVGcolor lighterColor = nvgRGB(200, 140, 0);
	NVGcolor dblLighterColor = nvgRGB(255, 177, 0);
	NVGcolor lightestColor = nvgRGB(255, 195, 60);

	int widthLight = fmax(clockRadius * 0.0075, 1);
	int widthMedium = fmax(clockRadius * 0.013, 1);
	int widthHeavy = fmax(clockRadius * 0.025, 1);


	time_t t = time(NULL);
	struct tm localTime = *localtime(&t);

	nvgTranslate(vg, screenWidth / 2, screenHeight / 2);
	nvgStrokeColor(vg, baseColor);

	// Frame
	nvgSave(vg);
	nvgBeginPath(vg);
	nvgCircle(vg, 0, 0, clockRadius);
	nvgStrokeWidth(vg, widthMedium);
	nvgStroke(vg);
	nvgBeginPath(vg);
	nvgStrokeColor(vg, lighterColor);
	for (int i = 0; i < 12; i++) {
		nvgMoveTo(vg, 0, -clockRadius * 0.98);
		nvgLineTo(vg, 0, -clockRadius * 0.90);
		for (int j = 0; j < 5; j++) {
			nvgRotate(vg, 2 * M_PI / 12 / 5);
			nvgMoveTo(vg, 0, -clockRadius * 0.98);
			nvgLineTo(vg, 0, -clockRadius * 0.95);
		}
	}
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgFontFace(vg, "bold");

	int digitsRadius = clockRadius * 0.79;
	int digitLargeSize = clockRadius * 0.29;
	int digitSmallSize = clockRadius * 0.2;

	int clockTitleSize = clockRadius * 0.06;

	nvgFillColor(vg, dblLighterColor);

	char* nineTitle = "9";
	nvgFontSize(vg, digitLargeSize);
	nvgTranslate(vg, -digitsRadius, 0);
	nvgRotate(vg, - 3 * 2 * M_PI / 12);
	//nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, nineTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, lighterColor);

	char* tenTitle = "10";
	nvgFontSize(vg, digitSmallSize);
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, tenTitle, 0, 0);
	nvgFill(vg);

	char* elevenTitle = "11";
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, elevenTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, dblLighterColor);

	char* twelveTitle = "12";
	nvgFontSize(vg, digitLargeSize);
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, twelveTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, lighterColor);

	char* oneTitle = "1";
	nvgFontSize(vg, digitSmallSize);
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, oneTitle, 0, 0);
	nvgFill(vg);

	char* twoTitle = "2";
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, twoTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, dblLighterColor);

	char* threeTitle = "3";
	nvgFontSize(vg, digitLargeSize);
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, -digitsRadius);
	drawTextCenter(vg, threeTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, lighterColor);

	char* fourTitle = "4";
	nvgFontSize(vg, digitSmallSize);
	nvgTranslate(vg, 0, digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12 - M_PI);
	nvgTranslate(vg, 0, digitsRadius);
	drawTextCenter(vg, fourTitle, 0, 0);
	nvgFill(vg);

	char* fiveTitle = "5";
	nvgTranslate(vg, 0, -digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, digitsRadius);
	drawTextCenter(vg, fiveTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, dblLighterColor);

	char* sixTitle = "6";
	nvgFontSize(vg, digitLargeSize);
	nvgTranslate(vg, 0, -digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, digitsRadius);
	drawTextCenter(vg, sixTitle, 0, 0);
	nvgFill(vg);

	nvgFillColor(vg, lighterColor);

	char* sevenTitle = "7";
	nvgFontSize(vg, digitSmallSize);
	nvgTranslate(vg, 0, -digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, digitsRadius);
	drawTextCenter(vg, sevenTitle, 0, 0);
	nvgFill(vg);

	char* eightTitle = "8";
	nvgTranslate(vg, 0, -digitsRadius);
	nvgRotate(vg, 2 * M_PI / 12);
	nvgTranslate(vg, 0, digitsRadius);
	drawTextCenter(vg, eightTitle, 0, 0);
	nvgFill(vg);

	nvgRestore(vg);

	nvgSave(vg);
	nvgFontFace(vg, "black");
	char* clockTitle = "ALPHA";
	nvgFontSize(vg, clockTitleSize);
	nvgFillColor(vg, baseColor);
	nvgTranslate(vg, 0, clockTitleSize);
	nvgTextLetterSpacing(vg, clockTitleSize);
	drawTextCenter(vg, clockTitle, 0, -2 * clockTitleSize);
	nvgFill(vg);
	nvgRestore(vg);

	double secAngle = 2 * M_PI / 60 * localTime.tm_sec;
	double minAngle = 2 * M_PI / 60 * localTime.tm_min + secAngle / 60;
	double hrAngle = 2 * M_PI / 12 * localTime.tm_hour + minAngle / 60;

	// Hour hand
	nvgStrokeColor(vg, lighterColor);
	nvgSave(vg);
	nvgRotate(vg, hrAngle);
	nvgBeginPath(vg);
	nvgMoveTo(vg, 0, clockRadius * 0.02);
	nvgLineTo(vg, 0, -clockRadius * 0.5);
	nvgStrokeWidth(vg, widthHeavy);
	nvgStroke(vg);
	nvgRestore(vg);

	// Minute hand
	nvgStrokeColor(vg, lighterColor);
	nvgSave(vg);
	nvgRotate(vg, minAngle);
	nvgBeginPath(vg);
	nvgMoveTo(vg, 0, clockRadius * 0.04);
	nvgLineTo(vg, 0, -clockRadius * 0.8);
	nvgStrokeWidth(vg, widthMedium);
	nvgStroke(vg);
	nvgRestore(vg);

	// Second hand
	nvgStrokeColor(vg, lightestColor);
	nvgSave(vg);
	nvgRotate(vg, secAngle);
	nvgBeginPath(vg);
	nvgMoveTo(vg, 0, clockRadius * 0.05);
	nvgLineTo(vg, 0, -clockRadius * 0.9);
	nvgStrokeWidth(vg, widthLight);
	nvgStroke(vg);
	nvgRestore(vg);
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

static void draw()
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
	drawClock(vg, winWidth, winHeight);
	double t2 = glfwGetTime();

	usleep(fmax((1.0 / fps - (t2 - t1)) * 1000000, 0));
	nvgEndFrame(vg);

	glfwSwapBuffers(window);
}

static void windowSize(GLFWwindow* window, int width, int height)
{
	draw();
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

	window = glfwCreateWindow(600, 400, "Alpha clock", NULL, NULL);
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

	nvgCreateFont(vg, "bold", "fonts/Walkway_Bold.ttf");
	nvgCreateFont(vg, "black", "fonts/Walkway_Black.ttf");

	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;

		glfwGetCursorPos(window, &mx, &my);

		draw();
		
		glfwPollEvents();
	}

	nvgDeleteGL3(vg);

	glfwTerminate();
	return 0;
}
