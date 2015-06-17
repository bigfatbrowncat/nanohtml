#include "nanohtml.h"
#include "gl_tools.h"


#include <stdio.h>
#include <math.h>

#include "default_style.h"
#include "UIHandler.h"

#include "../src/el_div.h"

using namespace litehtml;

int main()
{
	UIHandler uiHandler;
	litehtml::context ctx;
	ctx.load_master_stylesheet(DEFAULT_STYLESHEET);
	
	litehtml::document::ptr theNewDoc = litehtml::document::createFromUTF8(
		"<html>"
		"<head>"
		"	<title>Harry Potter and the Methods of Rationality</title>"
		"	<style> p:hover { color: #444444; } body {margin: 0;} h1 { color: #337711 } div.title { background-color:rgba(128, 255, 128, 0.3); padding-left: 20pt; padding-right: 20pt; padding-top: 20pt; padding-bottom: 10pt; }</style>"
		"</head>"
		"<body id=\"scroll\">"
			"<img src=\"images/tux.png\" style=\"float: right; margin: 5pt; border-radius: 64px; width: 128px; height: 128px; background-color: rgba(128, 128, 255, 0.4);\"/>"
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
		"</body>"
		"</html>", &uiHandler, &ctx);

	uiHandler.setDoc(theNewDoc);
	uiHandler.loop();

	return 0;
}
