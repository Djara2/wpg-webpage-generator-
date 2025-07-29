#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct GridPage* grid_page_create() {
	struct GridPage *new_grid_page = malloc(sizeof(struct GridPage));
	if (new_grid_page == NULL) {
		fprintf(stderr, "[grid_page_create] Tried to allocate memory for a GridPage, but malloc returned NULL\n");
		return NULL;
	}

	new_grid_page->grid_items = malloc(sizeof(struct AnchorTag) * 5);
	if (new_grid_page->grid_items == NULL) {
		fprintf(stderr, "[grid_page_create] Failed to allocate memory for default amount of grid items (5) within the grid page.\n");
		free(new_grid_page);
		return NULL;
	}
	new_grid_page->grid_items_length = 0;
	new_grid_page->grid_items_capacity = 5;	
}

void grid_page_destroy(struct *GridPage grid_page);

struct AnchorTag* anchor_tag_create() { 
	struct AnchorTag *new_anchor_tag = malloc(sizeof(struct AnchorTag));
	if (new_anchor_tag == NULL) { 
		fprintf(stderr, "[anchor_tag_create] Failed to allocate memroy for a new AnchorTag on the heap.\n");
		return NULL;
	}
	
	// Allocate 64 bytes for the href attribute.
	new_anchor_tag->href_capacity = 64; 
	new_anchor_tag->href = create_string( &(new_anchor_tag->href_length), new_anchor_tag->href_capacity ); 
	if (new_anchor_tag->href == NULL) { 
		fprintf(stderr, "[anchor_tag_create] Failed to allocate memory for a new char buffer for the href attribute.\n");
		free(new_anchor_tag);
		return NULL;
	}
	
	// Allocate 64 bytes for the text attribute.
	new_anchor_tag->text_capacity = 64; 
	new_anchor_tag->text = create_string( &(new_anchor_tag->text_length), new_anchor_tag->text_capacity ); 
	if (new_anchor_tag->text == NULL) { 
		fprintf(stderr, "[anchor_tag_create] Failed to allocate memory for a new char buffer for the text attribute.\n");
		free(new_anchor_tag->href);
		free(new_anchor_tag);
		return NULL;
	}

	return new_anchor_tag;
} 

void anchor_tag_destroy(struct AnchorTag *anchor_tag) {
	if (anchor_tag == NULL) {
		fprintf(stderr, "[anchor_tag_destroy] Cannot free the memory of an AnchorTag using a pointer that points to NULL.\n");
		return;
	}

	if (anchor_tag->href != NULL) free(anchor_tag->href);
	if (anchor_tag->text != NULL) free(anchor_tag->text);

	free(anchor_tag);
	return;
}

struct Page* page_create();
void page_destroy(struct Page *page);
