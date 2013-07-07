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

void _geGui_RenderButton(ge_GuiAreaObject* object, ge_GuiStyle* style);
void _geGui_RenderCheckBox(ge_GuiAreaObject* object, ge_GuiStyle* style);
void _geGui_RenderProgressBar(ge_GuiAreaObject* object, ge_GuiStyle* style);
void _geGui_RenderInputBox(ge_GuiAreaObject* object, ge_GuiStyle* style);
void _geGui_RenderComboBox(ge_GuiAreaObject* object, ge_GuiStyle* style);
void _geGui_RenderListWidget(ge_GuiAreaObject* object, ge_GuiStyle* style);

static int _ge_gui_cursor_last_x = 0;
static int _ge_gui_cursor_last_y = 0;
static ge_GuiWidget* _ge_gui_focused_widget = NULL;

void geGuiStyleFont(ge_GuiStyle* style, ge_Font* font, int size){
	style->font = font;
	geFontSize(font, size);
}

void _geGui_MakeResizableObject(ge_GuiWidget* obj, ge_GuiStyle* style){
	ge_Image* src = NULL;
	int n_tex = 1;
	bool stretch = true;

	switch(obj->type){
		case GE_GUI_BUTTON :
			src = geDuplicateImage(style->button);
			n_tex = 3;
			break;
		case GE_GUI_CHECKBOX :
			src = geDuplicateImage(style->checkbox);
			n_tex = 4;
			stretch = false;
			break;
		case GE_GUI_PROGRESSBAR :
			src = geDuplicateImage(style->box);
			break;
		case GE_GUI_INPUTBOX :
			src = geDuplicateImage(style->box);
			break;
		case GE_GUI_COMBOBOX :
			src = geDuplicateImage(style->button);
			n_tex = 3;
			break;
		case GE_GUI_LISTWIDGET :
			src = geDuplicateImage(style->box);
			break;
		default:
			break;
	}
	if(!src){
		return;
	}
#ifdef PSP
	if(src->flags & GE_IMAGE_SWIZZLED){
		geUnswizzle(src);
	}
#endif

	int block_w = 0;
	int block_h = 0;
	int tex_s = 0;
	if(obj->width < src->width){
		block_w = obj->width / 2;
	}else{
		block_w = src->width / 2;
	}
	if(obj->height < src->height/n_tex){
		block_h = obj->height / 2;
		tex_s = obj->height / n_tex;
	}else{
		block_h = src->height / 2 / n_tex;
		tex_s = src->height / n_tex;
	}

	if(obj->img){
		geFreeImage(obj->img);
	}
	obj->img = geCreateSurface(obj->width, obj->height*n_tex, 0x00000000);
	int n, i, tex_i;
	for(n=0, i=0, tex_i=0; i<obj->height*n_tex; n++, i+=obj->height, tex_i+=tex_s){
		if(!stretch){
			geBlitImageToImage(0, i + obj->height/2 - src->height/n_tex/2, src, 0, n*(src->height/n_tex), src->width, src->height/n_tex, obj->img);
		}else{
			geBlitImageToImage(0, i, src, 0, tex_i, block_w, block_h, obj->img);
			geBlitImageToImage(obj->width-block_w, i, src, src->width-block_w, tex_i, block_w, block_h, obj->img);
			geBlitImageToImage(0, i+obj->height-block_h, src, 0, tex_i+src->height/n_tex-block_h, block_w, block_h, obj->img);
			geBlitImageToImage(obj->width-block_w, i+obj->height-block_h, src, src->width-block_w, tex_i+src->height/n_tex-block_h, block_w, block_h, obj->img);
		
			geBlitImageToImageStretched(block_w, i, src, block_w, tex_i, 1, block_h, obj->width-2*block_w, block_h, obj->img);
			geBlitImageToImageStretched(block_w, i+obj->height-block_h, src, block_w, tex_i+src->height/n_tex-block_h, 1, block_h, obj->width-2*block_w, block_h, obj->img);
			geBlitImageToImageStretched(0, i+block_h, src, 0, tex_i+src->height/n_tex-block_h, block_w, 1, block_w, obj->height-2*block_h, obj->img);
			geBlitImageToImageStretched(obj->width-block_w, i+block_h, src, src->width-block_w, tex_i+src->height/n_tex-block_h, block_w, 1, block_w, obj->height-2*block_h, obj->img);
			geBlitImageToImageStretched(block_w, i+block_h, src, block_w, tex_i+src->height/n_tex-block_h, 1, 1, obj->width-2*block_w, obj->height-2*block_h, obj->img);
		}
	}

	int x, y;
	for(y=0; y<obj->img->height; y++){
		for(x=0; x<obj->img->width; x++){
			if((obj->img->data[x + y*obj->img->textureWidth] & 0x00FFFFFF) == 0x0000FF00){
				obj->img->data[x + y*obj->img->textureWidth] = obj->back_color;
				if(obj->type == GE_GUI_INPUTBOX && obj->textpos[0] == -1){
					obj->textpos[0] = x + 2;
				}
			}
		}
	}
	geUpdateImage(obj->img);
	geFreeImage(src);
}

void _geGui_AreaRender(int x, int y, ge_GuiArea* area, ge_GuiStyle* style){
	int mx, my;
	int i = 0;
	geCursorPosition(&mx, &my);

	ge_GuiKeyEvent key_e;
	memcpy(key_e.pressed, libge_context->ge_keys->pressed, sizeof(u8)*(GE_KEYS_COUNT+32));
	
	for(i=0; i<area->nObjects; i++){
		area->objs[i].absx = x + area->objs[i].x + area->x;
		area->objs[i].absy = y + area->objs[i].y + area->y;
		if(area->objs[i].flags & GE_GUI_ALIGNX_CENTER){
			area->objs[i].absx += area->width / 2;
		}
		if(area->objs[i].flags & GE_GUI_ALIGNY_CENTER){
			area->objs[i].absy += area->height / 2;
		}
		if(area->objs[i].flags & GE_GUI_ALIGNX_RIGHT){
			area->objs[i].absx += area->width;
		}
		if(area->objs[i].flags & GE_GUI_ALIGNY_BOTTOM){
			area->objs[i].absy += area->height;
		}

		ge_GuiWidget* widget = (ge_GuiWidget*)area->objs[i].object;
		if(!widget || !widget->visible || !area->objs[i].render){
			continue;
		}
		if(!widget->enabled && widget->img){
			widget->img->color = RGBA(190, 190, 190, 255);
		}else if(widget->img){
			widget->img->color = RGBA(255, 255, 255, 255);
		}
		if(widget->enabled && mx >= area->objs[i].absx && my >= area->objs[i].absy && mx <= (area->objs[i].absx+widget->width) && my <= (area->objs[i].absy+widget->height)){
			if(geKeysToggled(libge_context->ge_keys, GEK_LBUTTON)){
				widget->focused = true;
				widget->gotfocus = true;
				_ge_gui_focused_widget = widget;
			}
		}else if(widget->focused == true && widget->gotfocus == false){
			if(geKeysToggled(libge_context->ge_keys, GEK_LBUTTON)){
				widget->lostfocus = true;
				widget->focused = false;
			}
		}
		
		if(widget->focused && widget->KeyEventFunc){
			widget->KeyEventFunc(widget, &key_e);
		}
		if(widget->focused && widget->CursorEventFunc && geKeysToggled(libge_context->ge_keys, GEK_LBUTTON)){
			if(!widget->cursor_event){
				widget->cursor_event = (ge_GuiCursorEvent*)geMalloc(sizeof(ge_GuiCursorEvent));
				widget->last_cursor_event = (ge_GuiCursorEvent*)geMalloc(sizeof(ge_GuiCursorEvent));
			}
			widget->cursor_event->x = mx - widget->x;
			widget->cursor_event->y = my - widget->y;
			widget->cursor_event->last_x = _ge_gui_cursor_last_x - widget->x;
			widget->cursor_event->last_y = _ge_gui_cursor_last_y - widget->y;
			widget->cursor_event->warp_x = (widget->cursor_event->last_x - widget->cursor_event->x) - widget->x;
			widget->cursor_event->warp_y = (widget->cursor_event->last_y - widget->cursor_event->y) - widget->y;
			widget->cursor_event->ticks = geGetTick();
			widget->cursor_event->last = widget->last_cursor_event;
			widget->CursorEventFunc(widget, widget->cursor_event);
			memcpy(widget->last_cursor_event, widget->cursor_event, sizeof(ge_GuiCursorEvent));
		}

		area->objs[i].render(&area->objs[i], style);

		widget->gotfocus = false;
		widget->lostfocus = false;
	}

	_ge_gui_cursor_last_x = mx;
	_ge_gui_cursor_last_y = my;
}

ge_GuiWidget* geGuiCreateWidget(int width, int height){
	ge_GuiWidget* widget = (ge_GuiWidget*)geMalloc(sizeof(ge_GuiWidget));
	widget->width = widget->width;
	widget->height = widget->height;

	widget->area = (ge_GuiArea*)geMalloc(sizeof(ge_GuiArea));
	widget->area->x = 0;
	widget->area->y = 0;
	widget->area->width = widget->width;
	widget->area->height = widget->height;

	return widget;
}

void geGuiWidgetForegroundImage(ge_GuiWidget* widget, ge_Image* img){
	widget->fore_image = img;
}

void geGuiAreaLinkObject(ge_GuiArea* area, int x, int y, void* object, int flags){
	if(object == NULL || (t_ptr)object == 0xDEADBEEF || (t_ptr)object == 0xBAADF00D){
		return;
	}
	ge_GuiWidget* w = (ge_GuiWidget*)object;
	int type = w->type;
	w->x = x;
	w->y = y;
	w->enabled = true;
	w->visible = true;

	area->objs = (ge_GuiAreaObject*)geRealloc(area->objs, sizeof(ge_GuiAreaObject)*(area->nObjects+1));
	area->objs[area->nObjects].x = x;
	area->objs[area->nObjects].y = y;
	area->objs[area->nObjects].type = type;
	area->objs[area->nObjects].object = object;
	area->objs[area->nObjects].flags = flags;
	switch(type){
		case GE_GUI_BUTTON :
			area->objs[area->nObjects].render = _geGui_RenderButton;
			break;
		case GE_GUI_CHECKBOX :
			area->objs[area->nObjects].render = _geGui_RenderCheckBox;
			break;
		case GE_GUI_PROGRESSBAR :
			area->objs[area->nObjects].render = _geGui_RenderProgressBar;
			break;
		case GE_GUI_INPUTBOX :
			area->objs[area->nObjects].render = _geGui_RenderInputBox;
			break;
		case GE_GUI_COMBOBOX :
			area->objs[area->nObjects].render = _geGui_RenderComboBox;
			break;
		case GE_GUI_LISTWIDGET :
			area->objs[area->nObjects].render = _geGui_RenderListWidget;
			break;
		default:
			break;
	}
	area->nObjects++;
}

void geGuiWidgetLinkObject(ge_GuiWidget* parent, int x, int y, void* object, int flags){
	geGuiAreaLinkObject(parent->area, x, y, object, flags);
	((ge_GuiWidget*)object)->parent = (ge_GuiWidget*)parent;
}

void geGuiWindowLinkObject(ge_GuiWindow* win, int x, int y, void* object, int flags){
	geGuiAreaLinkObject(win->area, x, y, object, flags);
	((ge_GuiWidget*)object)->parent = (ge_GuiWidget*)win;
}

void geGuiWidgetSetName(void* widget, const char* name){
	if(!((ge_GuiWidget*)widget)->name){
		((ge_GuiWidget*)widget)->name = (char*)geMalloc(64);
	}
	strncpy(((ge_GuiWidget*)widget)->name, name, 64);
}

void* geGuiAreaFindObject(ge_GuiArea* area, const char* name){
	int i;
	for(i=0; i<area->nObjects; i++){
		if(((ge_GuiWidget*)area->objs[i].object)->name && !strcmp(((ge_GuiWidget*)area->objs[i].object)->name, name)){
			return area->objs[i].object;
		}
	}
	return NULL;
}

void* geGuiWindowFindObject(ge_GuiWindow* win, const char* name){
	return geGuiAreaFindObject(win->area, name);
}

void geGuiGiveFocus(void* widget){
	if(_ge_gui_focused_widget){
		_ge_gui_focused_widget->focused = false;
		_ge_gui_focused_widget->lostfocus = true;
	}
	_ge_gui_focused_widget = (ge_GuiWidget*)widget;
	_ge_gui_focused_widget->focused = true;
	_ge_gui_focused_widget->gotfocus = true;
}
