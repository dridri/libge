 /*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2012  Aubry Adrien (dridri85)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../ge_internal.h"

static bool initialized = false;
static ge_GuiWindow* windows[8] = { NULL };
static int nWindows = 0;
//static ge_GuiStyle* style_default = NULL;
static ge_GuiStyle* style_black = NULL;
//static ge_GuiStyle* style_white = NULL;
//static ge_GuiStyle* style_gray = NULL;

extern unsigned char message_box_style_black[];

static ge_GuiStyle* CreateInternalWindowStyle(int st);
static void _geGui_AddWindow(ge_GuiWindow* box);
void _geGui_AreaRender(int x, int y, ge_GuiArea* area, ge_GuiStyle* style);

void _geGui_InitWindows(){
//	style_default = CreateInternalWindowStyle(GE_WINDOW_STYLE_DEFAULT);
	style_black = CreateInternalWindowStyle((u32)GE_WINDOW_STYLE_BLACK);
//	style_white = CreateInternalWindowStyle(GE_WINDOW_STYLE_WHITE);
//	style_gray = CreateInternalWindowStyle(GE_WINDOW_STYLE_GRAY);
}

static void _geGui_CreateBox(ge_GuiWindow* box){
	if(! (box->flags & GE_WINDOW_FULLSCREEN) ){
		box->winwidth = box->width+box->style->left->width+box->style->right->width;
		box->winheight = box->height+box->style->top->height+box->style->bottom->height;
	}else{
		box->winwidth = box->width;
		box->winheight = box->height;
	}
	if(box->flags & GE_WINDOW_FULLSCREEN && box->flags & GE_WINDOW_TRANSPARENT){
		return;
	}
	box->img = geCreateSurface(box->winwidth, box->winheight, 0x00000000);
	
	if(box->flags & GE_WINDOW_FULLSCREEN){
		geFillRectImage(0, 0, box->width, box->height, box->style->back_color, box->img);
	}else{
		//Back
		geFillRectImage(box->style->left->width, box->style->top->height, box->width, box->height, box->style->back_color, box->img);
		//Top left
		geBlitImageToImage(0, 0, box->style->top_left, 0, 0, box->style->top_left->width, box->style->top_left->height, box->img);
		//Top right
		geBlitImageToImage(box->winwidth-box->style->top_right->width, 0, box->style->top_right, 0, 0, box->style->top_right->width, box->style->top_right->height, box->img);
		//Bottom left
		geBlitImageToImage(0, box->winheight-box->style->bottom_left->height, box->style->bottom_left, 0, 0, box->style->bottom_left->width, box->style->bottom_left->height, box->img);
		//Bottom right
		geBlitImageToImage(box->winwidth-box->style->bottom_right->width, box->winheight-box->style->bottom_right->height, box->style->bottom_right, 0, 0, box->style->bottom_right->width, box->style->bottom_right->height, box->img);

		int i = 0;
		//Top
		for(i=box->style->top_left->width; i<(box->winwidth-box->style->top_left->width-box->style->top_right->width); i+=box->style->top->width){
			geBlitImageToImage(i, 0, box->style->top, 0, 0, box->style->top->width, box->style->top->height, box->img);
		}
		geBlitImageToImage(i, 0, box->style->top, 0, 0, ((box->winwidth-box->style->top_right->width)-i), box->style->top->height, box->img);

		//Bottom
		for(i=box->style->bottom_left->width; i<(box->winwidth-box->style->bottom_left->width-box->style->bottom_right->width); i+=box->style->bottom->width){
			geBlitImageToImage(i, box->winheight-box->style->bottom->height, box->style->bottom, 0, 0, box->style->bottom->width, box->style->bottom->height, box->img);
		}
		geBlitImageToImage(i, box->winheight-box->style->bottom->height, box->style->bottom, 0, 0, ((box->winwidth-box->style->bottom_right->width)-i), box->style->bottom->height, box->img);

		//Left
		for(i=box->style->top_left->height; i<(box->winheight-box->style->bottom_left->height-box->style->bottom_left->height); i+=box->style->left->height){
			geBlitImageToImage(0, i, box->style->left, 0, 0, box->style->left->width, box->style->left->height, box->img);
		}
		geBlitImageToImage(0, i, box->style->left, 0, 0, box->style->left->width, ((box->winheight-box->style->bottom_left->height)-i), box->img);

		//Right
		for(i=box->style->top_right->height; i<(box->winheight-box->style->bottom_right->height-box->style->bottom_right->height); i+=box->style->right->height){
			geBlitImageToImage(box->winwidth-box->style->right->width, i, box->style->right, 0, 0, box->style->right->width, box->style->right->height, box->img);
		}
		geBlitImageToImage(box->winwidth-box->style->right->width, i, box->style->right, 0, 0, box->style->right->width, ((box->winheight-box->style->bottom_right->height)-i), box->img);
	
		//Close button
		ge_Image* close = box->style->close;
#ifdef PSP
		if(close->flags & GE_IMAGE_SWIZZLED){
			close = geDuplicateImage(close);
			geUnswizzle(close);
		}
#endif
		geBlitImageToImage(box->winwidth-box->style->top_right->width-close->width, box->style->top->height/2-close->height/3/2, close, 0, 0, close->width, close->height/3, box->img);
		if(close != box->style->close){
			geFreeImage(close);
		}
	}

	geUpdateImage(box->img);
}

static void _geGui_AddWindow(ge_GuiWindow* box){
	int i = 0;
	for(i=0; i<8; i++){
		if(!windows[i]){
			windows[i] = box;
			nWindows++;
			break;
		}
	}
}

ge_GuiWindow* geGuiCreateWindow(const char* title, int width, int height, int flags){
	if(!initialized){
		_geGui_InitWindows();
		initialized = true;
	}
	ge_GuiWindow* box = (ge_GuiWindow*)geMalloc(sizeof(ge_GuiWindow));
	box->style = (ge_GuiStyle*)geMalloc(sizeof(ge_GuiStyle));

	box->type = GE_GUI_WINDOW;

	box->title = (char*)geMalloc(strlen(title)+1);
	if(box->title)strcpy(box->title, title);
	box->flags = flags;

	if(flags & GE_WINDOW_FULLSCREEN){
		box->x = 0;
		box->y = 0;
		box->width = libge_context->width;
		box->height = libge_context->height;
	}else{
		box->x = libge_context->width/2 - width/2;
		box->y = libge_context->height/2 - height/2;
		box->width = width;
		box->height = height;
	}

	ge_GuiStyle* style = NULL;
	switch(flags & 0xF){
		case GE_WINDOW_STYLE_DEFAULT :
			break;
		case GE_WINDOW_STYLE_BLACK :
			style = style_black;
			break;
		case GE_WINDOW_STYLE_WHITE :
			break;
		case GE_WINDOW_STYLE_GRAY :
			break;
		default:
			break;
	}
	if(!style)style=style_black;

	memcpy(box->style, style, sizeof(ge_GuiStyle));
	if(flags & GE_WINDOW_TRANSPARENT){
		box->style->back_color = 0x00000000;
	}

	box->area = (ge_GuiArea*)geMalloc(sizeof(ge_GuiArea));
	box->area->x = 0;
	box->area->y = 0;
	box->area->width = box->width;
	box->area->height = box->height;

	_geGui_CreateBox(box);
	_geGui_AddWindow(box);

	box->visible = true;

	return box;
}

void geGuiWindowPosition(ge_GuiWindow* win, int x, int y){
	win->x = x;
	win->y = y;
}

static bool round_mode = false;
static void _geGui_move(ge_GuiWindow* box){
	if(box->flags & GE_WINDOW_FULLSCREEN){
		return;
	}
	int x=0, y=0;
	int wX=0, wY=0;
//	geCursorPosition(&x, &y);
	x = libge_context->mouse_x;
	y = libge_context->mouse_y;
	geCursorWarp(&wX, &wY);
//	x = x - wX;
//	y = y - wY;
	if(geKeysToggled(libge_context->ge_keys, GEK_LBUTTON) && (x>box->x) && (x<box->x+box->width) && (y>box->y) && (y<box->y+box->style->top->height)){
		round_mode = libge_context->mouse_round;
		geCursorRoundMode(false);
		box->moving = true;
	}else
	if(geKeysUnToggled(libge_context->ge_keys, GEK_LBUTTON)){
		geCursorRoundMode(round_mode);
		box->moving = false;
	}

	if(box->moving){
		box->x += wX;
		box->y += wY;
	}
}

static int _geGui_DrawWindow(ge_GuiWindow* box){
	bool ret = true;
	if(!(box->flags & GE_WINDOW_FULLSCREEN) || !(box->flags & GE_WINDOW_TRANSPARENT)){
		geBlitImage(box->x, box->y, box->img, 0, 0, box->img->width, box->img->height, 0);
	}
	if(!(box->flags & GE_WINDOW_FULLSCREEN) ){
		geFontPrintScreen(box->x+box->style->top_left->width, box->y+box->style->top->height/2-box->style->font->size/2, box->style->font, box->title, 0xFFFFFFFF);
	}

	int x = libge_context->mouse_x;
	int y = libge_context->mouse_y;
	if(! (box->flags & GE_WINDOW_FULLSCREEN) ){
		if((x >= box->x+box->winwidth-box->style->top_right->width-box->style->close->width) && (x <= box->x+box->winwidth-box->style->top_right->width) && (y >= box->y+box->style->top->height/2-box->style->close->height/3/2) && (y <= box->y+box->style->top->height/2+box->style->close->height/3/2)){
			if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
				ret = false;
				geBlitImage(box->x+box->winwidth-box->style->top_right->width-box->style->close->width, box->y+box->style->top->height/2-box->style->close->height/3/2, box->style->close, 0, (box->style->close->height/3)*2, box->style->close->width, box->style->close->height/3, 0);
			}else
			if(geKeysUnToggled(libge_context->ge_keys, GEK_LBUTTON)){
				if(!box->moving){
					box->visible = false;
				}
			}else{
				ret = false;
				geBlitImage(box->x+box->winwidth-box->style->top_right->width-box->style->close->width, box->y+box->style->top->height/2-box->style->close->height/3/2, box->style->close, 0, box->style->close->height/3, box->style->close->width, box->style->close->height/3, 0);
			}
		}
	}

	if(box->area){
		if(box->flags & GE_WINDOW_FULLSCREEN){
			box->width = libge_context->width;
			box->height = libge_context->height;
			box->area->width = libge_context->width;
			box->area->height = libge_context->height;
			_geGui_AreaRender(box->x, box->y, box->area, box->style);
		}else{
			_geGui_AreaRender(box->x+box->style->left->width, box->y+box->style->top->height, box->area, box->style);
		}
	}

	return ret;
}

void WindowsRoutine(){
	int draw_mode = libge_context->drawing_mode;
	if(! (draw_mode & GE_DRAWING_MODE_2D) ){
		geDrawingMode(GE_DRAWING_MODE_2D);
	}
	int i, j;
	for(i=0, j=0; i<8 && j<nWindows; i++){
		if(windows[i] && windows[i]->visible){
			int can_move = _geGui_DrawWindow(windows[i]);
			if(can_move){
				_geGui_move(windows[i]);
			}
			j++;
		}
	}
	if(! (draw_mode & GE_DRAWING_MODE_2D) ){
		geDrawingMode(draw_mode);
	}
}

static ge_Image* ge_GuiLoadImage(ge_Image* src, u32* data){
	ge_Image* ret = geCreateSurface(src->width, src->height, 0);
	int x, y;
	for(y=0; y<src->height; y++){
		for(x=0; x<src->width; x++){
			ret->data[y*ret->textureWidth + x] = data[y*src->width + x];
		}
	}
	return ret;
}

static ge_GuiStyle* CreateInternalWindowStyle(int st){
	gePrintDebug(0x100, "CreateInternalWindowStyle 1\n");
	u8* buffer = NULL;
	switch(st){
		case (u32)GE_WINDOW_STYLE_DEFAULT :
		//	buffer = message_box_style_default;
			break;
		case (u32)GE_WINDOW_STYLE_BLACK :
			buffer = message_box_style_black;
			break;
		case (u32)GE_WINDOW_STYLE_WHITE :
		//	buffer = message_box_style_white;
			break;
		case (u32)GE_WINDOW_STYLE_GRAY :
		//	buffer = message_box_style_gray;
			break;
		default :
			break;
	}
	if(!buffer)return NULL;

	ge_GuiStyle* msg_style = (ge_GuiStyle*)geMalloc(sizeof(ge_GuiStyle));
	gePrintDebug(0x100, "CreateInternalWindowStyle 2\n");

	msg_style->back_color = RGBA(128, 128, 128, 255);
	gePrintDebug(0x100, "CreateInternalWindowStyle 3\n");


	
	u32 size = 0;
	msg_style->top = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->top = ge_GuiLoadImage(msg_style->top, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->top->width*msg_style->top->height;


	msg_style->top_left = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->top_left = ge_GuiLoadImage(msg_style->top_left, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->top_left->width*msg_style->top_left->height;


	msg_style->top_right = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->top_right = ge_GuiLoadImage(msg_style->top_right, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->top_right->width*msg_style->top_right->height;


	msg_style->bottom = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->bottom = ge_GuiLoadImage(msg_style->bottom, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->bottom->width*msg_style->bottom->height;


	msg_style->bottom_left = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->bottom_left = ge_GuiLoadImage(msg_style->bottom_left, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->bottom_left->width*msg_style->bottom_left->height;


	msg_style->bottom_right = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->bottom_right = ge_GuiLoadImage(msg_style->bottom_right, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->bottom_right->width*msg_style->bottom_right->height;


	msg_style->left = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->left = ge_GuiLoadImage(msg_style->left, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->left->width*msg_style->left->height;


	msg_style->right = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->right = ge_GuiLoadImage(msg_style->right, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->right->width*msg_style->right->height;


	msg_style->close = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->close = ge_GuiLoadImage(msg_style->close, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->close->width*msg_style->close->height;


	msg_style->button = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->button = ge_GuiLoadImage(msg_style->button, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->button->width*msg_style->button->height;


	msg_style->checkbox = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->checkbox = ge_GuiLoadImage(msg_style->checkbox, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->checkbox->width*msg_style->checkbox->height;


	msg_style->box = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->box = ge_GuiLoadImage(msg_style->box, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->box->width*msg_style->box->height;


	msg_style->progressbar = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->progressbar = ge_GuiLoadImage(msg_style->progressbar, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->progressbar->width*msg_style->progressbar->height;


	msg_style->comboitem = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->comboitem = ge_GuiLoadImage(msg_style->comboitem, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->comboitem->width*msg_style->comboitem->height;


	msg_style->listitem = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->listitem = ge_GuiLoadImage(msg_style->listitem, (u32*)&buffer[size]);
	size += sizeof(u32)*msg_style->listitem->width*msg_style->listitem->height;
	/*
	u32 size = 0;
	msg_style->top = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->top->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->top->textureWidth*msg_style->top->textureHeight;


	msg_style->top_left = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->top_left->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->top_left->textureWidth*msg_style->top_left->textureHeight;


	msg_style->top_right = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->top_right->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->top_right->textureWidth*msg_style->top_right->textureHeight;


	msg_style->bottom = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->bottom->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->bottom->textureWidth*msg_style->bottom->textureHeight;


	msg_style->bottom_left = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->bottom_left->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->bottom_left->textureWidth*msg_style->bottom_left->textureHeight;


	msg_style->bottom_right = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->bottom_right->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->bottom_right->textureWidth*msg_style->bottom_right->textureHeight;


	msg_style->left = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->left->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->left->textureWidth*msg_style->left->textureHeight;


	msg_style->right = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->right->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->right->textureWidth*msg_style->right->textureHeight;


	msg_style->close = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->close->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->close->textureWidth*msg_style->close->textureHeight;


	msg_style->button = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->button->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->button->textureWidth*msg_style->button->textureHeight;


	msg_style->checkbox = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->checkbox->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->checkbox->textureWidth*msg_style->checkbox->textureHeight;


	msg_style->box = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->box->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->box->textureWidth*msg_style->box->textureHeight;


	msg_style->progressbar = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->progressbar->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->progressbar->textureWidth*msg_style->progressbar->textureHeight;


	msg_style->comboitem = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->comboitem->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->comboitem->textureWidth*msg_style->comboitem->textureHeight;


	msg_style->listitem = (ge_Image*)&buffer[size];
	size += sizeof(ge_Image);
	msg_style->listitem->data = (u32*)&buffer[size];
	size += sizeof(u32)*msg_style->listitem->textureWidth*msg_style->listitem->textureHeight;
	*/
	
	gePrintDebug(0x100, "CreateInternalWindowStyle 4\n");
	msg_style->top_left->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->top_right->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->top->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->bottom_left->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->bottom_right->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->bottom->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->left->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->right->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->close->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->button->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->checkbox->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->box->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->progressbar->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->comboitem->flags |= GE_IMAGE_NO_MIPMAPS;
	msg_style->listitem->flags |= GE_IMAGE_NO_MIPMAPS;
	/*
#ifdef PSP
	msg_style->top_left->textureHeight = msg_style->top_left->height;
	msg_style->top_right->textureHeight = msg_style->top_right->height;
	msg_style->top->textureHeight = msg_style->top->height;
	msg_style->bottom_left->textureHeight = msg_style->bottom_left->height;
	msg_style->bottom_right->textureHeight = msg_style->bottom_right->height;
	msg_style->bottom->textureHeight = msg_style->bottom->height;
	msg_style->left->textureHeight = msg_style->left->height;
	msg_style->right->textureHeight = msg_style->right->height;
	msg_style->close->textureHeight = msg_style->close->height;
	msg_style->button->textureHeight = msg_style->button->height;
	msg_style->checkbox->textureHeight = msg_style->checkbox->height;
	msg_style->box->textureHeight = msg_style->box->height;
	msg_style->progressbar->textureHeight = msg_style->progressbar->height;
	msg_style->comboitem->textureHeight = msg_style->comboitem->height;
	msg_style->listitem->textureHeight = msg_style->listitem->height;
#endif
	*/

	geUpdateImage(msg_style->close);
	geUpdateImage(msg_style->button);
	geUpdateImage(msg_style->checkbox);
	geUpdateImage(msg_style->box);
	geUpdateImage(msg_style->progressbar);
	geUpdateImage(msg_style->comboitem);
	geUpdateImage(msg_style->listitem);

	return msg_style;
}

static ge_Image* __LoadImage(const char* path, const char* file){
	char tmppath[512];
	sprintf(tmppath,"%s/%s",path, file);
	return geLoadImage(tmppath);
}
void _ge_CreateBox(const char* path, const char* out){
	char* __list[] = {
		"top.png",
		"top_left.png",
		"top_right.png",
		"bottom.png",
		"bottom_left.png",
		"bottom_right.png",
		"left.png",
		"right.png",
		"close_button.png",
		"button.png",
		"checkbox.png",
		"box.png",
		"progressbar.png",
		"comboitem.png",
		"listitem.png",
		NULL
	};

	char** list = __list;

	ge_File* fp = geFileOpen(out, GE_FILE_MODE_WRITE | GE_FILE_MODE_BINARY);
	while(*list != NULL){
		ge_Image* img = __LoadImage(path, *list);
		img->id = 0;
		geFileWrite(fp, img, sizeof(ge_Image));
		geFileWrite(fp, img->data, sizeof(u32)*img->textureWidth*img->textureHeight);
		geFreeImage(img);
		list++;
	}

	geFileClose(fp);
}
