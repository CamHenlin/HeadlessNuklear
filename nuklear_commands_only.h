/*
 * Nuklear - 1.32.0 - public domain
 * no warranty implied; use at your own risk.
 * based on allegro5 version authored from 2015-2016 by Micha Mettke
 * commands_only version camhenlin 2021
 * 
 * v1 intent:
 * - only default system font support
 * - no graphics/images support - commands_only has very limited support for this
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#ifndef NK_COMMANDS_ONLY_H_
#define NK_COMMANDS_ONLY_H_

typedef struct NkCommandsOnlyFont NkCommandsOnlyFont;
NK_API struct nk_context* nk_commands_only_init(unsigned int width, unsigned int height);
NK_API void nk_commands_only_shutdown(void);
NK_API void nk_commands_only_render(struct nk_context *ctx);

NK_API struct nk_image* nk_commands_only_create_image(const char* file_name);
NK_API void nk_commands_only_del_image(struct nk_image* image);
NK_API NkCommandsOnlyFont* nk_commands_only_font_create_from_file();

#endif

#ifdef RUNNING_UNDER_EMSCRIPTEN

EM_JS(void, pushCommandToOuterCommandList, (char *currentCommand), {

  global.storeCommandOnCommandList(UTF8ToString(currentCommand));

  return;
});

#endif

/*
 * ==============================================================
 *
 *                          IMPLEMENTATION
 *
 * ===============================================================
 */

#ifdef NK_COMMANDS_ONLY_IMPLEMENTATION
#ifndef NK_COMMANDS_ONLY_TEXT_MAX
#define NK_COMMANDS_ONLY_TEXT_MAX 256
#endif
#endif
struct NkCommandsOnlyFont {
    struct nk_user_font nk;
    char *font;
};

void *last;
void *buf;

// constant keyboard mappings for convenenience
// See Inside Macintosh: Text pg A-7, A-8
int homeKey = (int)0x01;
int enterKey = (int)0x03;
int endKey = (int)0x04;
int helpKey = (int)0x05;
int backspaceKey = (int)0x08;
int deleteKey = (int)0x7F;
int tabKey = (int)0x09;
int pageUpKey = (int)0x0B;
int pageDownKey = (int)0x0C;
int returnKey = (int)0x0D;
int rightArrowKey = (int)0x1D;
int leftArrowKey = (int)0x1C;
int downArrowKey = (int)0x1F;
int upArrowKey = (int)0x1E;
int eitherShiftKey = (int)0x0F;
int escapeKey = (int)0x1B;

// #define NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING
// #define NK_COMMANDS_ONLY_EVENTS_DEBUGGING

static struct nk_commands_only {
    unsigned int width;
    unsigned int height;
    struct nk_context nuklear_context;
    struct nk_buffer cmds;
} commands_only;

// TODO: maybe V2 - skipping images for first pass
NK_API struct nk_image* nk_commands_only_create_image(const char* file_name) {

    // TODO: just read the file to a char, we can draw it using commands_only
    // b&w bitmaps are pretty easy to draw...
    // for us to do this, we need to figure out the format, then figure out if we can make it in to a commands_only rect
    // and set the buffer to the image handle pointer in the image struct
    // i assume this gets consumed elsewhere in the code, thinking NK_COMMAND_IMAGE
    char* bitmap = ""; //al_load_bitmap(file_name); // TODO: https://www.allegro.cc/manual/5/al_load_bitmap, loads file to in-memory buffer understood by allegro

    if (bitmap == NULL) {

        fprintf(stdout, "Unable to load image file: %s", file_name);
        return NULL;
    }

    struct nk_image *image = (struct nk_image*)calloc(1, sizeof(struct nk_image));
    image->handle.ptr = bitmap;
    image->w = 0; // al_get_bitmap_width(bitmap); // TODO: this can be retrieved from a bmp file
    image->h = 0; // al_get_bitmap_height(bitmap); // TODO: this can be retrieved from a bmp file

    return image;
}

// TODO: maybe V2 - skipping images for first pass
NK_API void nk_commands_only_del_image(struct nk_image* image) {

    if (!image) {
        
        return;
    }

    free(image);
}

int widthFor12ptFont[128] = {
    0,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    8,
    10,
    10,
    10,
    0,
    10,
    10,
    10,
    11,
    11,
    9,
    11,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    10,
    4,
    6,
    7,
    10,
    7,
    11,
    10,
    3,
    5,
    5,
    7,
    7,
    4,
    7,
    4,
    7,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    8,
    4,
    4,
    6,
    8,
    6,
    8,
    11,
    8,
    8,
    8,
    8,
    7,
    7,
    8,
    8,
    6,
    7,
    9,
    7,
    12,
    9,
    8,
    8,
    8,
    8,
    7,
    6,
    8,
    8,
    12,
    8,
    8,
    8,
    5,
    7,
    5,
    8,
    8,
    6,
    8,
    8,
    7,
    8,
    8,
    6,
    8,
    8,
    4,
    6,
    8,
    4,
    12,
    8,
    8,
    8,
    8,
    6,
    7,
    6,
    8,
    8,
    12,
    8,
    8,
    8,
    5,
    5,
    5,
    8,
    8
};

// obviously we're consuming this with a quickdraw back end on the other side of a serial port, so let's do the color
// calculation for the poor old mac
static int nk_color_to_quickdraw_bw_color(struct nk_color color) {

    // TODO: since we are operating under a b&w display - we need to convert these colors to black and white
    // look up a simple algorithm for taking RGBA values and making the call on black or white and try it out here
    // as a future upgrade, we could support color quickdraw
    // using an algorithm from https://stackoverflow.com/questions/3942878/how-to-decide-font-color-in-white-or-black-depending-on-background-color
    // if (red*0.299 + green*0.587 + blue*0.114) > 186 use #000000 else use #ffffff
    // return al_map_rgba((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
   
    float magicColorNumber = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;
   
    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

       char stringMagicColorNumber[255];
       sprintf(stringMagicColorNumber, "stringMagicColorNumber: %f", magicColorNumber);
       writeSerialPort(boutRefNum, stringMagicColorNumber);
    #endif
   
   if (magicColorNumber > 37) {
       
       return 0;//blackColor; note! this are not actual quickdraw integers, just short values for sending across a serial port
   }
   
   return 1;//whiteColor; note! this are not actual quickdraw integers, just short values for sending across a serial port
}

// i split this in to a 2nd routine because we can use the various shades of gray when filling rectangles and whatnot
static int nk_color_to_quickdraw_color(const struct nk_color *color) {

    // as a future upgrade, we could support color quickdraw
    // using an algorithm from https://stackoverflow.com/questions/3942878/how-to-decide-font-color-in-white-or-black-depending-on-background-color
    // if (red*0.299 + green*0.587 + blue*0.114) > 186 use #000000 else use #ffffff
    uint8_t red;
    uint8_t blue;
    uint8_t green;
    
    red = color->r;
    blue = color->b;
    green = color->g;
    
    float magicColorNumber = (uint8_t)red * 0.299 + (uint8_t)green * 0.587 + (uint8_t)blue * 0.114;

    if (magicColorNumber > 150) {

        return 0;// qd.black;
    } else if (magicColorNumber > 100) {

        return 1;// qd.dkGray;
    } else if (magicColorNumber > 75) {

        return 2;// qd.gray;
    } else if (magicColorNumber > 49) {

        return 3;// qd.ltGray;
    }

    return 4;// qd.white;
}

float width;

// doing this in a "fast" way by using a precomputed table for the system 6 12pt font
// you should pre-calculate the widths that you need for your font of choice
static float nk_commands_only_font_get_text_width(nk_handle handle, float height, const char *text, int len) {

    width = 0.0f;

    if (!text || len == 0) {

        return width;
    }


    for (int i = 0; i < len; i++) {

        width += (float)widthFor12ptFont[(int)text[i]];
    }

    return width;
}

// set up a dummy 12pt font, equivalent to the system 6 font
NK_API NkCommandsOnlyFont* nk_commands_only_font_create_from_file() {

    NkCommandsOnlyFont *font = (NkCommandsOnlyFont*)calloc(1, sizeof(NkCommandsOnlyFont));

    font->font = calloc(1, 1024);
    font->nk.userdata = nk_handle_ptr(font);
    font->nk.height = (int)12;
    font->nk.width = nk_commands_only_font_get_text_width;

    return font;
}

NK_API void nk_commands_only_render(struct nk_context *ctx) {

    void *cmds = nk_buffer_memory(&ctx->memory);

    // do not render if the buffer did not change from the previous rendering run
    // we will need this to return some sort of empty response
    if (!memcmp(cmds, last, ctx->memory.allocated)) {

        #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

            printf("NK_COMMAND_NOP");
        #endif

        return;
    }

    memcpy(last, cmds, ctx->memory.allocated);

    const struct nk_command *cmd = 0;

    nk_foreach(cmd, ctx) {

        int color; // Color CommandsOnly colors are integers - see Retro68/InterfacesAndLibraries/Interfaces&Libraries/Interfaces/CIncludes/Quickdraw.h:122 for more info

        switch (cmd->type) {

            case NK_COMMAND_NOP:
                
                #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                    printf("NK_COMMAND_NOP");
                #endif
                

                break;
            case NK_COMMAND_SCISSOR: {
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_SCISSOR");
                    #endif

                    const struct nk_command_scissor *s =(const struct nk_command_scissor*)cmd;
                   //  // al_set_clipping_rectangle((int)s->x, (int)s->y, (int)s->w, (int)s->h); // TODO: https://www.allegro.cc/manual/5/al_set_clipping_rectangle
                   //  // this essentially just sets the region of the screen that we are going to write to
                   //  // initially, i thought that this would be SetClip, but now believe this should be ClipRect, see:
                   //  // Inside Macintosh: Imaging with Quickdraw pages 2-48 and 2-49 for more info
                   //  // additionally, see page 2-61 for a data structure example for the rectangle OR 
                   //  // http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-60.html
                   //  // for usage example
                   // Rect quickDrawRectangle;
                   // quickDrawRectangle.top = (int)s->y;
                   // quickDrawRectangle.left = (int)s->x;
                   // quickDrawRectangle.bottom = (int)s->y + (int)s->h;
                   // quickDrawRectangle.right = (int)s->x + (int)s->w;
                   
                   // ClipRect(&quickDrawRectangle);
                   // completely ignoring clipping commands as it does not seem to have any perf difference on classic macs
                }

                break;
            case NK_COMMAND_LINE: {

                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_LINE");
                    #endif

                    const struct nk_command_line *l = (const struct nk_command_line *)cmd;
                    color = nk_color_to_quickdraw_bw_color(l->color);
                    // great reference: http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-60.html
                    // al_draw_line((int)l->begin.x, (int)l->begin.y, (int)l->end.x, (int)l->end.y, color, (int)l->line_thickness); // TODO: look up and convert al_draw_line
                    // ForeColor(color);
                    // PenSize((int)l->line_thickness, (int)l->line_thickness);
                    // MoveTo((int)l->begin.x, (int)l->begin.y);
                    // LineTo((int)l->end.x, (int)l->end.y);
                    char output[255];
                    sprintf(output, "L%d,%d,%d,%d,%d,%d", color, (int)l->line_thickness, (int)l->begin.x, (int)l->begin.y, (int)l->end.x, (int)l->end.y);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_RECT: {
                
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_RECT");
                    #endif

                    // http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-102.html#MARKER-9-372
                    // http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-103.html#HEADING103-0
                    const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
                    color = nk_color_to_quickdraw_bw_color(r->color);
                    
                    // ForeColor(color);
                    // PenSize((int)r->line_thickness, (int)r->line_thickness);

                    // Rect quickDrawRectangle;
                    // quickDrawRectangle.top = (int)r->y;
                    // quickDrawRectangle.left = (int)r->x;
                    // quickDrawRectangle.bottom = (int)r->y + (int)r->h;
                    // quickDrawRectangle.right = (int)r->x + (int)r->w;

                    // FrameRect(&quickDrawRectangle);//, (int)r->rounding, (int)r->rounding);
                    char output[255];
                    sprintf(output, "R%d,%d,%d,%d,%d,%d,%d", color, (int)r->line_thickness, (int)r->x, (int)r->y, (int)r->y + (int)r->h, (int)r->x + (int)r->w, (int)r->rounding);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_RECT_FILLED: {
                
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_RECT_FILLED");
                    #endif

                    const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled *)cmd;

                    color = nk_color_to_quickdraw_bw_color(r->color);
                    
                    // ForeColor(color);
                    int colorPattern = nk_color_to_quickdraw_color(&r->color);

                    // // BackPat(&colorPattern); // inside macintosh: imaging with commands_only 3-48
                    // PenSize(1.0, 1.0); // no member line thickness on this struct so assume we want a thin line
                    // // might actually need to build this with SetRect, search inside macintosh: imaging with commands_only
                    // Rect quickDrawRectangle;
                    // quickDrawRectangle.top = (int)r->y;
                    // quickDrawRectangle.left = (int)r->x;
                    // quickDrawRectangle.bottom = (int)r->y + (int)r->h;
                    // quickDrawRectangle.right = (int)r->x + (int)r->w;

                    // FillRect(&quickDrawRectangle, &colorPattern);
                    // FrameRect(&quickDrawRectangle);//, (int)r->rounding, (int)r->rounding); // http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-105.html#HEADING105-0
                    char output[255];
                    sprintf(output, "RF%d,%d,%d,%d,%d,%d,%d", color, colorPattern, (int)r->x, (int)r->y, (int)r->y + (int)r->h, (int)r->x + (int)r->w, (int)r->rounding);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_CIRCLE: {
                
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_CIRCLE");
                    #endif

                    const struct nk_command_circle *c = (const struct nk_command_circle *)cmd;
                    color = nk_color_to_quickdraw_bw_color(c->color);

                    // ForeColor(color);  
                    
                    // Rect quickDrawRectangle;
                    // quickDrawRectangle.top = (int)c->y;
                    // quickDrawRectangle.left = (int)c->x;
                    // quickDrawRectangle.bottom = (int)c->y + (int)c->h;
                    // quickDrawRectangle.right = (int)c->x + (int)c->w;

                    // FrameOval(&quickDrawRectangle); // An oval is a circular or elliptical shape defined by the bounding rectangle that encloses it. inside macintosh: imaging with commands_only 3-25
                    char output[255];
                    sprintf(output, "C%d,%d,%d,%d,%d", color, (int)c->x, (int)c->y, (int)c->y + (int)c->h, (int)c->x + (int)c->w);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_CIRCLE_FILLED: {
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_CIRCLE_FILLED");
                    #endif

                    const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
                    
                    color = nk_color_to_quickdraw_bw_color(c->color);
                    
                    // ForeColor(color);
                    int colorPattern = nk_color_to_quickdraw_color(&c->color);
                    // BackPat(&colorPattern); // inside macintosh: imaging with commands_only 3-48
                    // PenSize(1.0, 1.0);
                    // Rect quickDrawRectangle;
                    // quickDrawRectangle.top = (int)c->y;
                    // quickDrawRectangle.left = (int)c->x;
                    // quickDrawRectangle.bottom = (int)c->y + (int)c->h;
                    // quickDrawRectangle.right = (int)c->x + (int)c->w;

                    // FillOval(&quickDrawRectangle, &colorPattern); 
                    // FrameOval(&quickDrawRectangle);// An oval is a circular or elliptical shape defined by the bounding rectangle that encloses it. inside macintosh: imaging with commands_only 3-25
                    // http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-111.html#HEADING111-0
                    char output[255];
                    sprintf(output, "CF%d,%d,%d,%d,%d,%d", color, colorPattern, (int)c->x, (int)c->y, (int)c->y + (int)c->h, (int)c->x + (int)c->w);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_TRIANGLE: {
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_TRIANGLE");
                    #endif

                    const struct nk_command_triangle *t = (const struct nk_command_triangle*)cmd;
                    color = nk_color_to_quickdraw_bw_color(t->color);
                    
                    // ForeColor(color);
                    // PenSize((int)t->line_thickness, (int)t->line_thickness);

                    // MoveTo((int)t->a.x, (int)t->a.y);
                    // LineTo((int)t->b.x, (int)t->b.y);
                    // LineTo((int)t->c.x, (int)t->c.y);
                    // LineTo((int)t->a.x, (int)t->a.y);
                    char output[255];
                    sprintf(output, "T%d,%d,%d,%d,%d,%d,%d,%d", color, (int)t->line_thickness, (int)t->a.x, (int)t->a.y, (int)t->b.x, (int)t->b.y, (int)t->c.x, (int)t->c.y);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_TRIANGLE_FILLED: {
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_TRIANGLE_FILLED");
                    #endif

                    const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled *)cmd;
                    int colorPattern = nk_color_to_quickdraw_color(&t->color);
                    color = nk_color_to_quickdraw_bw_color(t->color);
                    // PenSize(1.0, 1.0);
                    // // BackPat(&colorPattern); // inside macintosh: imaging with commands_only 3-48
                    // ForeColor(color);

                    // PolyHandle trianglePolygon = OpenPoly(); 
                    // MoveTo((int)t->a.x, (int)t->a.y);
                    // LineTo((int)t->b.x, (int)t->b.y);
                    // LineTo((int)t->c.x, (int)t->c.y);
                    // LineTo((int)t->a.x, (int)t->a.y);
                    // ClosePoly();

                    // FillPoly(trianglePolygon, &colorPattern);
                    // KillPoly(trianglePolygon);
                    char output[255];
                    sprintf(output, "TF%d,%d,%d,%d,%d,%d,%d,%d", color, colorPattern, (int)t->a.x, (int)t->a.y, (int)t->b.x, (int)t->b.y, (int)t->c.x, (int)t->c.y);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_POLYGON: {

                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_POLYGON");
                    #endif

                    // const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;

                    // color = nk_color_to_quickdraw_bw_color(p->color);

                    // int i;

                    // for (i = 0; i < p->point_count; i++) {
                        
                    //     if (i == 0) {
                            
                    //         MoveTo(p->points[i].x, p->points[i].y);
                    //     }
                        
                    //     LineTo(p->points[i].x, p->points[i].y);
                        
                    //     if (i == p->point_count - 1) {

                    //         LineTo(p->points[0].x, p->points[0].y);
                    //     }
                    // }
                }
                
                break;
            case NK_COMMAND_POLYGON_FILLED: {
                    
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_POLYGON_FILLED");
                    #endif

                    // const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;

                    // int colorPattern = nk_color_to_quickdraw_color(&p->color);
                    // color = nk_color_to_quickdraw_bw_color(p->color);
                    // // BackPat(&colorPattern); // inside macintosh: imaging with commands_only 3-48 -- but might actually need PenPat -- look into this
                    // ForeColor(color);

                    // int i;

                    // PolyHandle trianglePolygon = OpenPoly(); 
                    // for (i = 0; i < p->point_count; i++) {
                        
                    //     if (i == 0) {
                            
                    //         MoveTo(p->points[i].x, p->points[i].y);
                    //     }
                        
                    //     LineTo(p->points[i].x, p->points[i].y);
                        
                    //     if (i == p->point_count - 1) {
                            
                            
                    //         LineTo(p->points[0].x, p->points[0].y);
                    //     }
                    // }
                    
                    // ClosePoly();

                    // FillPoly(trianglePolygon, &colorPattern);
                    // KillPoly(trianglePolygon);
                }
                
                break;
            case NK_COMMAND_POLYLINE: {

                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_POLYLINE");
                    #endif

                    // // this is similar to polygons except the polygon does not get closed to the 0th point
                    // // check out the slight difference in the for loop
                    // const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;

                    // color = nk_color_to_quickdraw_bw_color(p->color);
                    // ForeColor(color);

                    // int i;

                    // for (i = 0; i < p->point_count; i++) {
                        
                    //     if (i == 0) {
                            
                    //         MoveTo(p->points[i].x, p->points[i].y);
                    //     }
                        
                    //     LineTo(p->points[i].x, p->points[i].y);
                    // }
                }

                break;
            case NK_COMMAND_TEXT: {

                    const struct nk_command_text *t = (const struct nk_command_text*)cmd;
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_TEXT");
                        char log[255];
                        sprintf(log, "%f: %s, %d", (int)t->height, &t->string, (int)t->length);
                        writeSerialPort(boutRefNum, log);
                    #endif

                    color = nk_color_to_quickdraw_bw_color(t->foreground);
                    // ForeColor(color);
                    // MoveTo((int)t->x, (int)t->y + (int)t->height);
                    // TextSize((int)t->height);

                    // DrawText((const char*)t->string, 0, (int)t->length);
                    char output[255];
                    sprintf(output, "S%d,%d,%d,%d,%d,%s", color, (int)t->height, (int)t->x, (int)t->y, (int)t->length, (char*)t->string);

                    #ifdef RUNNING_UNDER_EMSCRIPTEN

                        pushCommandToOuterCommandList(output);

                    #else

                        puts(output);

                    #endif
                }

                break;
            case NK_COMMAND_CURVE: {
                    
                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_CURVE");
                    #endif

                    // const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
                    // color = nk_color_to_quickdraw_bw_color(q->color);
                    // ForeColor(color);
                    // Point p1 = { (int)q->begin.x, (int)q->begin.y};
                    // Point p2 = { (int)q->ctrl[0].x, (int)q->ctrl[0].y};
                    // Point p3 = { (int)q->ctrl[1].x, (int)q->ctrl[1].y};
                    // Point p4 = { (int)q->end.x, (int)q->end.y};

                    // BezierCurve(p1, p2, p3, p4);
                }

                break;
            case NK_COMMAND_ARC: {

                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_ARC");
                    #endif

                    // const struct nk_command_arc *a = (const struct nk_command_arc *)cmd;
                    // color = nk_color_to_quickdraw_bw_color(a->color);
                    // ForeColor(color);

                    // Rect arcBoundingBoxRectangle;
                    // // this is kind of silly because the cx is at the center of the arc and we need to create a rectangle around it 
                    // // http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/CommandsOnly/CommandsOnly-60.html#MARKER-2-116
                    // int x1 = (int)a->cx - (int)a->r;
                    // int y1 = (int)a->cy - (int)a->r;
                    // int x2 = (int)a->cx + (int)a->r;
                    // int y2 = (int)a->cy + (int)a->r;
                    // SetRect(&arcBoundingBoxRectangle, x1, y1, x2, y2);
                    // // SetRect(secondRect,90,20,140,70);

                    // FrameArc(&arcBoundingBoxRectangle, a->a[0], a->a[1]);
                }

                break;
            case NK_COMMAND_IMAGE: {

                    #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                        printf("NK_COMMAND_IMAGE");  
                    #endif

                    const struct nk_command_image *i = (const struct nk_command_image *)cmd;
                    // al_draw_bitmap_region(i->img.handle.ptr, 0, 0, i->w, i->h, i->x, i->y, 0); // TODO: look up and convert al_draw_bitmap_region
                    // TODO: consider implementing a bitmap drawing routine. we could iterate pixel by pixel and draw
                    // here is some super naive code that could work, used for another project that i was working on with a custom format but would be
                    // easy to modify for standard bitmap files (just need to know how many bytes represent each pixel and iterate from there):
                    // 
                    // for (int i = 0; i < strlen(string); i++) {
                    //     printf("char: %c", string[i]);
                    //     char pixel[1];
                    //     memcpy(pixel, &string[i], 1);
                    //     if (strcmp(pixel, "0") == 0) { // white pixel
                    //         MoveTo(++x, y);
                    //      } else if (strcmp(pixel, "1") == 0) { // black pixel
                    //          // advance the pen and draw a 1px x 1px "line"
                    //          MoveTo(++x, y);
                    //          LineTo(x, y);
                    //      } else if (strcmp(pixel, "|") == 0) { // next line
                    //          x = 1;
                    //          MoveTo(x, ++y);
                    //      } else if (strcmp(pixel, "/") == 0) { // end
                    //      }
                    //  }
                }
                
                break;
                
            // why are these cases not implemented?
            case NK_COMMAND_RECT_MULTI_COLOR:
            case NK_COMMAND_ARC_FILLED:
            default:
            
                #ifdef NK_COMMANDS_ONLY_GRAPHICS_DEBUGGING

                    printf("NK_COMMAND_RECT_MULTI_COLOR/NK_COMMAND_ARC_FILLED/default");
                #endif
                break;
        }
    }
}

NK_INTERN void nk_commands_only_clipboard_paste(nk_handle usr, struct nk_text_edit *edit) {    

    // TODO
}

NK_INTERN void nk_commands_only_clipboard_copy(nk_handle usr, const char *text, int len) {

    // TODO
}

// it us up to our "main" function to call this code
NK_API struct nk_context* nk_commands_only_init(unsigned int width, unsigned int height) {

    NkCommandsOnlyFont *commands_onlyfont = nk_commands_only_font_create_from_file();
    struct nk_user_font *font = &commands_onlyfont->nk;

    // these may need to be bigger depending on the app.
    // i believe this is not enough for the nuklear "overview.c" example
    last = calloc(1, 64 * 1024);
    buf = calloc(1, 64 * 1024);
    nk_init_fixed(&commands_only.nuklear_context, buf, 64 * 1024, font);
    
    // nk_init_default(&commands_only.nuklear_context, font);
    nk_style_push_font(&commands_only.nuklear_context, font);

    commands_only.nuklear_context.clip.copy = nk_commands_only_clipboard_copy;
    commands_only.nuklear_context.clip.paste = nk_commands_only_clipboard_paste;
    commands_only.nuklear_context.clip.userdata = nk_handle_ptr(0);

    // fix styles to be more "mac-like"
    struct nk_style *style;
    struct nk_style_toggle *toggle;
    style = &commands_only.nuklear_context.style;

    /* checkbox toggle */
    toggle = &style->checkbox;
    nk_zero_struct(*toggle);
    toggle->normal          = nk_style_item_color(nk_rgba(45, 45, 45, 255));
    toggle->hover           = nk_style_item_color(nk_rgba(80, 80, 80, 255)); // this is the "background" hover state regardless of checked status - we want light gray
    toggle->active          = nk_style_item_color(nk_rgba(255, 255, 255, 255)); // i can't tell what this does yet
    toggle->cursor_normal   = nk_style_item_color(nk_rgba(255, 255, 255, 255)); // this is the "checked" box itself - we want "black"
    toggle->cursor_hover    = nk_style_item_color(nk_rgba(255, 255, 255, 255)); // this is the hover state of a "checked" box - anything lighter than black is ok
    toggle->userdata        = nk_handle_ptr(0);
    toggle->text_background = nk_rgba(255, 255, 255, 255);
    toggle->text_normal     = nk_rgba(70, 70, 70, 255);
    toggle->text_hover      = nk_rgba(70, 70, 70, 255);
    toggle->text_active     = nk_rgba(70, 70, 70, 255);
    toggle->padding         = nk_vec2(3.0f, 3.0f);
    toggle->touch_padding   = nk_vec2(0,0);
    toggle->border_color    = nk_rgba(0,0,0,0);
    toggle->border          = 0.0f;
    toggle->spacing         = 5;

    /* option toggle */
    toggle = &style->option;
    nk_zero_struct(*toggle);
    toggle->normal          = nk_style_item_color(nk_rgba(45, 45, 45, 255));
    toggle->hover           = nk_style_item_color(nk_rgba(80, 80, 80, 255)); // this is the "background" hover state regardless of checked status - we want light gray
    toggle->active          = nk_style_item_color(nk_rgba(255, 255, 255, 255)); // i can't tell what this does yet
    toggle->cursor_normal   = nk_style_item_color(nk_rgba(255, 255, 255, 255)); // this is the "checked" box itself - we want "black"
    toggle->cursor_hover    = nk_style_item_color(nk_rgba(255, 255, 255, 255)); // this is the hover state of a "checked" box - anything lighter than black is ok
    toggle->userdata        = nk_handle_ptr(0);
    toggle->text_background = nk_rgba(255, 255, 255, 255);
    toggle->text_normal     = nk_rgba(70, 70, 70, 255);
    toggle->text_hover      = nk_rgba(70, 70, 70, 255);
    toggle->text_active     = nk_rgba(70, 70, 70, 255);
    toggle->padding         = nk_vec2(3.0f, 3.0f);
    toggle->touch_padding   = nk_vec2(0,0);
    toggle->border_color    = nk_rgba(0,0,0,0);
    toggle->border          = 0.0f;
    toggle->spacing         = 5;

    return &commands_only.nuklear_context;
}

NK_API void nk_commands_only_shutdown(void) {

    nk_free(&commands_only.nuklear_context);
    memset(&commands_only, 0, sizeof(commands_only));
}
        

        
