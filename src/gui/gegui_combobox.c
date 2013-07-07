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

ge_GuiComboBox* geGuiCreateComboBox(int width, int height){
	ge_GuiComboBox* box = (ge_GuiComboBox*)geMalloc(sizeof(ge_GuiComboBox));

	box->type = GE_GUI_COMBOBOX;
	box->width = width;
	box->height = height;
	box->back_color = 0xFF000000;
	box->textpos[0] = -1;
	box->textpos[1] = -1;
	box->state = 0;
	box->nItems = 0;
	box->currentIndex = -1;

	return box;
}

void geGuiComboBoxAddItem(ge_GuiComboBox* box, const char* text){
	box->items = (ge_GuiComboBoxItem*)geRealloc(box->items, (box->nItems+1)*sizeof(ge_GuiComboBoxItem));
	box->items[box->nItems].text = (char*)geMalloc(strlen(text)+1);
	strcpy(box->items[box->nItems].text, text);
	box->items[box->nItems].back_color = box->back_color;
	box->nItems++;
}

void _geGui_RenderComboBox(ge_GuiAreaObject* object, ge_GuiStyle* style){
	ge_GuiComboBox* box = (ge_GuiComboBox*)object->object;
	if(!box->img){
		_geGui_MakeResizableObject((ge_GuiWidget*)box, style);
	}
	if(box->currentIndex < 0){
		box->currentIndex = 0;
		box->currentItem = &box->items[0];
		box->text = box->items[0].text;
		int tw=0, th=0;
		geFontMeasureText(style->font, box->text, &tw, &th);
		box->textpos[0] = box->width/2 - tw/2;
		box->textpos[1] = box->height/2 - th/2;
	}

	int mx = libge_context->mouse_x;
	int my = libge_context->mouse_y;

	if(box->pressed){
		geBlitImage(object->absx, object->absy, box->img, 0, box->height*2, box->width, box->height, 0);
		int i;
		for(i=0; i<box->nItems; i++){
			int y = i * style->font->size * 1.5;
			if((mx>object->absx) && (mx<=object->absx+box->width) && (my>object->absy+box->height+y) && (my<=object->absy+box->height+y+style->font->size*1.5)){
				geBlitImageStretched(object->absx, object->absy + box->height + y, style->comboitem, 0, style->comboitem->height*1/3, style->comboitem->width, style->comboitem->height/3, box->width, style->font->size*1.5, 0);
				if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
					box->currentIndex = i;
					box->currentItem = &box->items[i];
					box->text = box->items[i].text;
					int tw=0, th=0;
					geFontMeasureText(style->font, box->text, &tw, &th);
					box->textpos[0] = box->width/2 - tw/2;
					box->textpos[1] = box->height/2 - th/2;
				}
			}else{
				geBlitImageStretched(object->absx, object->absy + box->height + y, style->comboitem, 0, style->comboitem->height*0/3, style->comboitem->width, style->comboitem->height/3, box->width, style->font->size*1.5, 0);
			}
			geFontPrintScreen(object->absx + 2, object->absy + box->height + y, style->font, box->items[i].text, 0xFFFFFFFF);
		}
	}else{
		geBlitImage(object->absx, object->absy, box->img, 0, box->height*box->state, box->width, box->height, 0);
	}

	if(box->text != NULL){
		geFontPrintScreen(object->absx+box->textpos[0], object->absy+box->textpos[1]+1, style->font, box->text, 0xFFFFFFFF);
	}



	if(box->state != 2){
		box->state = 0;
	}
	if((mx>object->absx) && (mx<object->absx+box->width) && (my>object->absy) && (my<object->absy+box->height)){
		if(box->state == 2 && !libge_context->ge_keys->pressed[GEK_LBUTTON]){
			if(!box->pressed){
				box->pressed = true;
			}
		}
		box->state = 1;
		if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
			if(!box->pressed){
				box->state = 2;
			}else{
				box->state = 0;
				box->pressed = false;
			}
		}
	}else{
		if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
			box->state = 0;
			box->pressed = false;
		}
	}
}
