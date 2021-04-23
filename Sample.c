

#define WINDOW_WIDTH 510
#define WINDOW_HEIGHT 302

#define NK_ZERO_COMMAND_MEMORY
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_QUICKDRAW_IMPLEMENTATION

#include <string.h>
#include "nuklear.h"
#include "nuklear_commands_only.h"

// #define EMSCRIPTEN_NUKLEAR_DEBUGGING

void EventLoop(struct nk_context *ctx);
void DoEvent(char *event, struct nk_context *ctx);
void Initialize(void);

static void boxTest(struct nk_context *ctx) {

    if (nk_begin(ctx, "POC", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT), NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE)) {

		nk_layout_row_dynamic(ctx, 200, 1);
		static char box_buffer[512];
		static int box_len;

		nk_edit_string(ctx, NK_EDIT_BOX, box_buffer, &box_len, 512, nk_filter_default);

    	nk_end(ctx);
	}
}

int main() {

    struct nk_context *ctx;

    #ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING
    
    	printf("call nk_init");
    #endif

    ctx = nk_commands_only_init(WINDOW_WIDTH, WINDOW_HEIGHT);

    #ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING

	    printf("call into event loop");
	#endif

	EventLoop(ctx);
}

int x;
int y;

char xPlaceHolder[5];
char yPlaceHolder[5];
char commandType;

void getMousePosition(char *command) {

	short haveSetX = 0;
	int xMark = 0;
	int xPosCounter = 0;
	int yPosCounter = 0;
	strcpy(xPlaceHolder, "    ");
	strcpy(yPlaceHolder, "    ");

	for (int i = 1; i < strlen(command); i++) {

		if (command[i] == ',') {

			if (!haveSetX) {

				x = atoi(xPlaceHolder);
				xMark = i;

				haveSetX = 1;
			}

			continue;
		}

		if (!haveSetX) {

			xPlaceHolder[xPosCounter++] = command[i];
		} else {

			yPlaceHolder[yPosCounter++] = command[i];
		}
	}

	y = atoi(yPlaceHolder);

	return;
}

void handleKey (struct nk_context *ctx, char *charKey, char *modifierKey) {

	int key = (int)charKey;
	int modifierKeyInt = (int)modifierKey;

	#ifdef NK_COMMANDS_ONLY_EVENTS_DEBUGGING

	    printf("keyDown/autoKey");

	    char logy[255];
	    sprintf(logy, "key pressed: key: '%c', 02x: '%02X', return: '%02X', %d == %d ??", key, key, returnKey, (int)(key), (int)(returnKey));
	    printf(logy);
	#endif

	short isKeyDown = 1;

	if (modifierKeyInt == 'C') { // calling this the "cmd" key for now
	    
	    if (key == 'c') {
	        
	        nk_input_key(ctx, NK_KEY_COPY, 1);
	    } else if (key == 'v') {
	        
	        nk_input_key(ctx, NK_KEY_PASTE, 1);
	    } else if (key == 'x') {
	        
	        nk_input_key(ctx, NK_KEY_CUT, 1);
	    } else if (key == 'z') {
	        
	        nk_input_key(ctx, NK_KEY_TEXT_UNDO, 1);
	    } else if (key == 'r') {
	        
	        nk_input_key(ctx, NK_KEY_TEXT_REDO, 1);
	    } 
	} else if (key == eitherShiftKey) {
	    
	    nk_input_key(ctx, NK_KEY_SHIFT, isKeyDown);
	} else if (key == deleteKey) {
	    
	    nk_input_key(ctx, NK_KEY_DEL, isKeyDown);
	} else if (key == enterKey) {
	    
	    nk_input_key(ctx, NK_KEY_ENTER, isKeyDown);
	} else if (key == returnKey) {
	    
	    nk_input_key(ctx, NK_KEY_ENTER, isKeyDown);
	} else if (key == tabKey) {
	    
	    nk_input_key(ctx, NK_KEY_TAB, isKeyDown);
	} else if (key == leftArrowKey) {
	    
	    nk_input_key(ctx, NK_KEY_LEFT, isKeyDown);
	} else if (key == rightArrowKey) {
	    
	    nk_input_key(ctx, NK_KEY_RIGHT, isKeyDown);
	} else if (key == upArrowKey) {
	    
	    nk_input_key(ctx, NK_KEY_UP, isKeyDown);
	} else if (key == downArrowKey) {
	    
	    nk_input_key(ctx, NK_KEY_DOWN, isKeyDown);
	} else if (key == backspaceKey) {
	    
	    nk_input_key(ctx, NK_KEY_BACKSPACE, isKeyDown);
	} else if (key == escapeKey) {
	    
	    // nk_input_key(ctx, NK_KEY_TEXT_RESET_MODE, isKeyDown);
	} else if (key == pageUpKey) {
	 
	    nk_input_key(ctx, NK_KEY_SCROLL_UP, isKeyDown);
	} else if (key == pageDownKey) {
	    
	    nk_input_key(ctx, NK_KEY_SCROLL_DOWN, isKeyDown);
	} else if (key == homeKey) {

	    // nk_input_key(ctx, NK_KEY_TEXT_START, isKeyDown);
	    nk_input_key(ctx, NK_KEY_SCROLL_START, isKeyDown);
	} else if (key == endKey) {

	    // nk_input_key(ctx, NK_KEY_TEXT_END, isKeyDown);
	    nk_input_key(ctx, NK_KEY_SCROLL_END, isKeyDown);
	} else {

	    #ifdef NK_COMMANDS_ONLY_EVENTS_DEBUGGING

	        printf("default keydown/autokey event");
	    #endif
	    
	    nk_input_unicode(ctx, (nk_rune)charKey);
	}
}

void EventLoop(struct nk_context *ctx) {

	char command[255];

	// strcpy(command, "MX ");

	do {

		#ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING

	        printf("nk_input_begin");
	    #endif

		nk_input_begin(ctx);

        #ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING

        	printf("nk_input_begin complete");
        #endif

		scanf("%s", command);
		commandType = command[0];

		switch (commandType) {

			case 'M':

				getMousePosition(command);

				nk_input_motion(ctx, x, y);

			break;

			case 'D':

				getMousePosition(command);

				nk_input_button(ctx, NK_BUTTON_LEFT, x, y, 1); // MOUSE DOWN

			break;

			case 'U':

				getMousePosition(command);

				nk_input_button(ctx, NK_BUTTON_LEFT, x, y, 0); // MOUSE UP

			break;

			case 'K':

				handleKey(ctx, &command[1], &command[2]); // KEY HANDLING

			break;
		}

		#ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING

        	printf("nk_input_end");
        #endif

    	nk_input_end(ctx);

        #ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING
        	
        	printf("nk_input_end complete");
        #endif


        #ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING

	        printf("nk_quickdraw_render");
	    #endif

    	boxTest(ctx);

    	// this should dump all the drawing commands to standard output
    	nk_commands_only_render(ctx);

		nk_clear(ctx);

    	#ifdef EMSCRIPTEN_NUKLEAR_DEBUGGING

        	printf("nk_input_render complete");
        #endif
	} while (1);
}

int mouseMove = 0;
int mouseUp = 1;
int mouseDown = 2;
int keyDown = 3;

