#include "nanohtml.h"
#include "gl_tools.h"

#include <stdio.h>
#include <math.h>

#include "default_style.h"
#include "Window.h"

#include "ScrollbarExpansion.h"
#include "TeletypeExpansion.h"

#include "../src/el_div.h"

using namespace litehtml;

char* allocBufferAndReadFile(char* fileName) {
	FILE *fp;
	long lSize;
	char *buffer;

	fp = fopen (fileName, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Can't open file %s\n", fileName);
		return NULL;
	}

	// Checking file size
	fseek(fp, 0L, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	// Allocating buffer memory
	buffer = (char*)malloc(lSize + 1);
	if (!buffer) {
		fclose(fp);
		fprintf(stderr, "Can't allocate memory for reading %s\n", fileName);
		return NULL;
	}

	// Reading the file into buffer
	if (1 != fread(buffer, lSize, 1, fp)) {
		fclose(fp);
		free(buffer);
		fprintf(stderr, "Can't read %s into memory\n", fileName);
		return NULL;
	}

	fclose(fp);

	// Null-terminating
	buffer[lSize] = 0;

	return buffer;
}

void freeFileBuffer(char* buffer) {
	free(buffer);
}

int main()
{
	ScrollbarExpansion scrollbarExpansion;
	TeletypeExpansion teletypeExpansion(glfwGetTime());
	
	Window windowHandler;
	windowHandler.addExpansion(scrollbarExpansion);
	windowHandler.addExpansion(teletypeExpansion);

	char* htmlFile = allocBufferAndReadFile("demo.html");
	if (htmlFile != NULL) {
		windowHandler.loadDocument(htmlFile);
		freeFileBuffer(htmlFile);

		windowHandler.loop();
	} else {
		fprintf(stderr, "Can't load the HTML file to show\n");
		return 1;
	}
	return 0;
}
