#include "md.h"

//hoedown
#include "document.h"
#include "html.h"

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

struct option_data {
	const char *basename;
	int done;

	/* time reporting */
	int show_time;

	/* I/O */
	size_t iunit;
	size_t ounit;
	const char *filename;

	/* renderer */
	enum renderer_type renderer;
	int toc_level;
	hoedown_html_flags html_flags;

	/* parsing */
	hoedown_extensions extensions;
	size_t max_nesting;
};

namespace md{

std::string render_md_to_html(const std::string& md_file_path){
	std::string testeeer;
	struct option_data data;
	FILE *file = stdin;
	hoedown_buffer *ib, *ob;
	hoedown_renderer *renderer = NULL;
	void (*renderer_free)(hoedown_renderer *) = NULL;
	hoedown_document *document;

	/* Parse options */
	data.basename = NULL;
	data.done = 0;
	data.show_time = 0;
	data.iunit = DEF_IUNIT;
	data.ounit = DEF_OUNIT;
	data.filename = md_file_path.c_str();
	data.renderer = RENDERER_HTML;
	data.toc_level = 0;
	data.html_flags = hoedown_html_flags(0);
	data.extensions = HOEDOWN_EXT_FENCED_CODE;
	data.max_nesting = DEF_MAX_NESTING;

	fprintf(stderr, "hello dawg, parsing %s\n", md_file_path.c_str());
	file = fopen(md_file_path.c_str(), "r");
	if (!file) {
		fprintf(stderr, "Unable to open input file \"%s\": %s\n", data.filename, strerror(errno));
		return "";
	}

	/* Read everything */
	ib = hoedown_buffer_new(data.iunit);

	if (hoedown_buffer_putf(ib, file)) {
		fprintf(stderr, "I/O errors found while reading input.\n");
		return "";
	}

	if (file != stdin) fclose(file);

	/* Create the renderer */
	switch (data.renderer) {
		case RENDERER_HTML:
			renderer = hoedown_html_renderer_new(data.html_flags, data.toc_level);
			renderer_free = hoedown_html_renderer_free;
			break;
		case RENDERER_HTML_TOC:
			renderer = hoedown_html_toc_renderer_new(data.toc_level);
			renderer_free = hoedown_html_renderer_free;
			break;
	};

	/* Perform Markdown rendering */
	ob = hoedown_buffer_new(data.ounit);
	document = hoedown_document_new(renderer, data.extensions, data.max_nesting);

	hoedown_document_render(document, ob, ib->data, ib->size);

	/* Cleanup */
	hoedown_buffer_free(ib);
	hoedown_document_free(document);
	renderer_free(renderer);

	/* Write the result to stdout */
	// (void)fwrite(ob->data, 1, ob->size, stdout);

	std::string contentString((const char*)ob->data, ob->size);
	hoedown_buffer_free(ob);
	return contentString;
}

}