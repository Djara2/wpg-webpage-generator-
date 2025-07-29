#ifndef wpglib_h
#define wpglib_h

enum PageType {
	PAGETYPE_NONE,
	PAGETYPE_GRID_LANDING,
	PAGETYPE_ARTICLE
};

struct AnchorTag {
	unsigned short href_length;
	unsigned short text_length;
	char *href;
	char *text;
};

struct GridPage {
	struct AnchorTag *grid_items;
	unsigned short grid_items_length; 
	unsigned short grid_items_capacity;
};

struct Page{
	enum PageType page_type;
	char *title;
	void *page_data;	// based on page_type
};

struct GridPage* grid_page_create();
void grid_page_destroy(struct *GridPage grid_page);

struct AnchorTag* anchor_tag_create(char *href, char *text);
void anchor_tag_destroy(struct AnchorTag *anchor_tag);

struct Page* page_create();
void page_destroy(struct Page *page);

#endif
