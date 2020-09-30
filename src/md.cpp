#include "md.h"
//hoedown
#include "document.h"
#include "html.h"

#include <cstring>

enum renderer_type {
	RENDERER_HTML,
	RENDERER_HTML_TOC
};

struct extension_category_info {
	unsigned int flags;
	const char *option_name;
	const char *label;
};

struct extension_info {
	unsigned int flag;
	const char *option_name;
	const char *description;
};

struct html_flag_info {
	unsigned int flag;
	const char *option_name;
	const char *description;
};

#define DEF_IUNIT 1024
#define DEF_OUNIT 64
#define DEF_MAX_NESTING 16

namespace md{

std::string render_md_to_html(const std::string& md_str){
	hoedown_buffer* ob;
	hoedown_renderer *renderer = NULL;
	void (*renderer_free)(hoedown_renderer *) = NULL;
	hoedown_document *document;

	/* Parse options */
	renderer_type renderer_t = RENDERER_HTML;
	int toc_level = 0;
	hoedown_html_flags html_flags = hoedown_html_flags(0);
	hoedown_extensions extensions = HOEDOWN_EXT_FENCED_CODE;
	size_t max_nesting = DEF_MAX_NESTING;

	/* Create the renderer */
	switch (renderer_t) {
		case RENDERER_HTML:
			renderer = hoedown_html_renderer_new(html_flags, toc_level);
			renderer_free = hoedown_html_renderer_free;
			break;
		case RENDERER_HTML_TOC:
			renderer = hoedown_html_toc_renderer_new(toc_level);
			renderer_free = hoedown_html_renderer_free;
			break;
	};

	/* Perform Markdown rendering */
	ob = hoedown_buffer_new(DEF_OUNIT);
	document = hoedown_document_new(renderer, extensions, max_nesting);

	hoedown_document_render(document, ob, (uint8_t*)md_str.c_str(), md_str.size());

	/* Cleanup */
	hoedown_document_free(document);
	renderer_free(renderer);

	std::string contentString((const char*)ob->data, ob->size);
	hoedown_buffer_free(ob);
	return contentString;
}

}