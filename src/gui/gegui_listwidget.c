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

void _geGui_MakeResizableObject(ge_GuiWidget* obj, ge_GuiStyle* style);

ge_GuiListWidget* geGuiCreateListWidget(int width, int height){
	ge_GuiListWidget* box = (ge_GuiListWidget*)geMalloc(sizeof(ge_GuiListWidget));

	box->type = GE_GUI_LISTWIDGET;
	box->width = width;
	box->height = height;
	box->back_color = 0xFF000000;
	box->textpos[0] = -1;
	box->textpos[1] = -1;
	box->nItems = 0;
	box->currentIndex = -1;

	return box;
}

ge_GuiListWidgetItem* geGuiListWidgetAddItem(ge_GuiListWidget* box, const char* text){
	box->items = (ge_GuiListWidgetItem*)geRealloc(box->items, (box->nItems+1)*sizeof(ge_GuiListWidgetItem));
	box->items[box->nItems].text = (char*)geMalloc(strlen(text)+1);
	strcpy(box->items[box->nItems].text, text);
	box->items[box->nItems].front_color = 0xFFFFFFFF;
	box->items[box->nItems].back_color = box->back_color;
	box->nItems++;
	return &box->items[box->nItems-1];
}

void geGuiListWidgetClear(ge_GuiListWidget* list){
	int i;
	if(list && list->items){
		for(i=0; i<list->nItems; i++){
			if(list->items[i].image){
				geFree(list->items[i].image);
			}
		}
		geFree(list->items);
	}
	list->firstVisible = 0;
	list->items = NULL;
	list->textpos[0] = -1;
	list->nItems = 0;
	list->currentIndex = -1;
	list->currentItem = NULL;
}

void _geGui_RenderListWidget(ge_GuiAreaObject* object, ge_GuiStyle* style){
	ge_GuiListWidget* list = (ge_GuiListWidget*)object->object;
	if(!list->img){
		_geGui_MakeResizableObject((ge_GuiWidget*)list, style);
	}
	if(list->textpos[0] == -1){
		list->textpos[0] = 5;
		u32 foreground = RGBA(255-R(list->back_color), 255-G(list->back_color), 255-B(list->back_color), 255);
		int i;
		for(i=0; i<list->nItems; i++){
			list->items[i].image = geCreateSurface(list->width, style->font->size * 1.5, 0);
			geFontPrintImage(list->textpos[0], 0, style->font, list->items[i].text, foreground, list->items[i].image);
			geUpdateImage(list->items[i].image);
			list->items[i].image->color = list->items[i].front_color;
		}
	}

	geBlitImage(object->absx, object->absy, list->img, 0, list->height, list->width, list->height, 0);

	int mx = libge_context->mouse_x;
	int my = libge_context->mouse_y;
	int i, j;

	if(mx > object->absx && mx <= object->absx + list->width && my > object->absy && my <= object->absy + list->height){
		if(libge_context->ge_keys->pressed[GEK_MWHEELUP] && list->firstVisible > 0){
			list->firstVisible--;
		}
		if(libge_context->ge_keys->pressed[GEK_MWHEELDOWN] && list->firstVisible < list->nItems - 1){
			list->firstVisible++;
		}
	}

	for(i=0, j=list->firstVisible; (i+1)*style->font->size*1.5<list->height && j<list->nItems; i++, j++){
		int y = style->font->size * 0.5 + i * style->font->size * 1.5;
		int w = list->width;
		if(list->nItems*style->font->size*1.5 > list->height){
			w -= 20;
		}
		geBlitImage(object->absx+2, object->absy+y, list->items[j].image, 0, 0, w-10, list->items[j].image->height, 0);
		if((mx>object->absx) && (mx<=object->absx+w) && (my>object->absy+y) && (my<=object->absy+y+style->font->size*1.5)){
			if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
				list->currentIndex = j;
				list->currentItem = &list->items[j];
			}
			geBlitImageStretched(object->absx+2, object->absy+y, style->listitem, 0, 0, style->listitem->width/2, style->listitem->height/2, style->listitem->width/2, style->font->size * 1.5, 0);
			geBlitImageStretched(object->absx+w-style->listitem->width/2-4, object->absy+y, style->listitem, style->listitem->width/2, 0, style->listitem->width/2, style->listitem->height/2, style->listitem->width/2, style->font->size * 1.5, 0);
			geBlitImageStretched(object->absx+style->listitem->width/2+2, object->absy+y, style->listitem, style->listitem->width/2, 0, 1, style->listitem->height/2, w-style->listitem->width-6, style->font->size * 1.5, 0);
		}
		if(j == list->currentIndex){
			geBlitImageStretched(object->absx+2, object->absy+y, style->listitem, 0, style->listitem->height/2, style->listitem->width/2, style->listitem->height/2, style->listitem->width/2, style->font->size * 1.5, 0);
			geBlitImageStretched(object->absx+w-style->listitem->width/2-4, object->absy+y, style->listitem, style->listitem->width/2, style->listitem->height/2, style->listitem->width/2, style->listitem->height/2, style->listitem->width/2, style->font->size * 1.5, 0);
			geBlitImageStretched(object->absx+style->listitem->width/2+2, object->absy+y, style->listitem, style->listitem->width/2, style->listitem->height/2, 1, style->listitem->height/2, w-style->listitem->width-6, style->font->size * 1.5, 0);
		}
	}
}

