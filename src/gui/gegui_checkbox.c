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

ge_GuiCheckBox* geGuiCreateCheckBox(const char* text, int width, int height){
	ge_GuiCheckBox* btn = (ge_GuiCheckBox*)geMalloc(sizeof(ge_GuiCheckBox));

	btn->type = GE_GUI_CHECKBOX;
	btn->width = width;
	btn->height = height;
	btn->back_color = 0xFFFFFFFF;
	btn->textpos[0] = -1;
	btn->textpos[1] = -1;
	btn->checked = false;
	btn->state = 0;
	btn->text = (char*)geMalloc(strlen(text)+2);
	strcpy(btn->text, text);

	return btn;
}

void _geGui_RenderCheckBox(ge_GuiAreaObject* object, ge_GuiStyle* style){
	ge_GuiCheckBox* checkbox = (ge_GuiCheckBox*)object->object;
	
	int mx = libge_context->mouse_x;
	int my = libge_context->mouse_y;
	if((mx>object->absx) && (mx<object->absx+checkbox->width) && (my>object->absy) && (my<object->absy+checkbox->height)){
		if(((!checkbox->checked && checkbox->state == 2) || (checkbox->checked && checkbox->state == 0)) && !libge_context->ge_keys->pressed[GEK_LBUTTON]){
			checkbox->checked = !checkbox->checked;
		}
		checkbox->state = 1 + (checkbox->checked?2:0);
		if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
			if(checkbox->checked){
				checkbox->state = 0;
			}else{
				checkbox->state = 2;
			}
		}
	}else{
		checkbox->state = (checkbox->checked?2:0);
	}
	
	if(!checkbox->img){
		if(checkbox->textpos[0] == -1){
			int tw=0, th=0;
			geFontMeasureText(style->font, checkbox->text, &tw, &th);
			checkbox->textpos[0] = style->checkbox->width * 1.2;
			checkbox->textpos[1] = checkbox->height/2 - th/2;
		}
		_geGui_MakeResizableObject((ge_GuiWidget*)checkbox, style);
		int i;
		for(i=0; i<checkbox->height*4; i+=checkbox->height){
			geFontPrintImage(checkbox->textpos[0], i+checkbox->textpos[1]+1, style->font, checkbox->text, 0xFFFFFFFF, checkbox->img);
		}
		geUpdateImage(checkbox->img);
	}

	geBlitImage(object->absx, object->absy, checkbox->img, 0, checkbox->height*checkbox->state, checkbox->width, checkbox->height, 0);
}
