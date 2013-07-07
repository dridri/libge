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

ge_GuiButton* geGuiCreateButton(const char* text, int width, int height){
	ge_GuiButton* btn = (ge_GuiButton*)geMalloc(sizeof(ge_GuiButton));

	btn->type = GE_GUI_BUTTON;
	btn->width = width;
	btn->height = height;
	btn->back_color = 0xFFFFFFFF;
	btn->textpos[0] = -1;
	btn->textpos[1] = -1;
    btn->pressed = 0;
	btn->state = 0;
	btn->text = (char*)geMalloc(strlen(text)+2);
	strcpy(btn->text, text);

	return btn;
}

void _geGui_RenderButton(ge_GuiAreaObject* object, ge_GuiStyle* style){
	ge_GuiButton* btn = (ge_GuiButton*)object->object;
	
	int mx = libge_context->mouse_x;
	int my = libge_context->mouse_y;
	btn->pressed = false;
	if((mx>object->absx) && (mx<object->absx+btn->width) && (my>object->absy) && (my<object->absy+btn->height)){
		if(btn->state == 2 && !libge_context->ge_keys->pressed[GEK_LBUTTON]){
			btn->pressed = true;
		}
		btn->state = 1;
		if(libge_context->ge_keys->pressed[GEK_LBUTTON]){
			btn->state = 2;
		}
	}else{
		btn->state = 0;
	}
	
	if(!btn->img){
		if(btn->textpos[0] == -1){
			int tw=0, th=0;
			geFontMeasureText(style->font, btn->text, &tw, &th);
			btn->textpos[0] = btn->width/2 - tw/2;
			btn->textpos[1] = btn->height/2 - th/2;
		}
		_geGui_MakeResizableObject((ge_GuiWidget*)btn, style);
		
#ifdef PSP
		if(btn->img->flags & GE_IMAGE_SWIZZLED){
			geUnswizzle(btn->img);
		}
#endif
		
		int i;
		for(i=0; i<btn->height*3; i+=btn->height){
			geFontPrintImage(btn->textpos[0], i+btn->textpos[1]+1, style->font, btn->text, 0xFFFFFFFF, btn->img);
		}

		geUpdateImage(btn->img);
	}

	geBlitImage(object->absx, object->absy, btn->img, 0, btn->height*btn->state, btn->width, btn->height, 0);
	if(btn->fore_image){
		geBlitImageStretched(object->absx+2, object->absy+2, btn->fore_image, 0, 0, btn->fore_image->width, btn->fore_image->height, btn->width-4, btn->height-4, 0);
	}
}
