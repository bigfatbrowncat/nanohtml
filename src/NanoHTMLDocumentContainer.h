//
//  NanoHTMLDocumentContainer.h
//  nanohtml
//
//  Created by Ilya Mizus on 31.05.15.
//
//

#ifndef __nanohtml__NanoHTMLDocumentContainer__
#define __nanohtml__NanoHTMLDocumentContainer__

#include <string>

#include <litehtml.h>

class NanoHTMLDocumentContainer : public litehtml::document_container
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
	
	NVGcontext* nvgContext;

	DrawingState drawingState;
	litehtml::uint_ptr currentSelectedFont;
	
public:
	NanoHTMLDocumentContainer(NVGcontext* nvgContext):
		nvgContext(nvgContext), drawingState(dsNone),
		currentSelectedFont(NULL)
	{
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
};

#endif /* defined(__nanohtml__NanoHTMLDocumentContainer__) */
