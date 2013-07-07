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
static u32 tick = 0;

ge_GuiInputBox* geGuiCreateInputBox(int width, int height, const char* base_text, int maxlen){
	ge_GuiInputBox* box = (ge_GuiInputBox*)geMalloc(sizeof(ge_GuiInputBox));

	box->type = GE_GUI_INPUTBOX;
	box->width = width;
	box->height = height;
	box->back_color = 0xFFFFFFFF;
	box->textpos[0] = -1;
	box->textpos[1] = -1;
	box->maxlen = maxlen;
	box->text = (char*)geMalloc(box->maxlen);
	strncpy(box->text, base_text, strlen(base_text));

	return box;
}

void _geGui_RenderInputBox(ge_GuiAreaObject* object, ge_GuiStyle* style){
	int i = -1;
	ge_GuiInputBox* box = (ge_GuiInputBox*)object->object;
	if(!box->img){
		_geGui_MakeResizableObject((ge_GuiWidget*)box, style);
	}

	if(box->gotfocus){
		geKeyboardInit();
		geKeyboardOutput(box->text, box->maxlen, box->maxlines);
	}
	if(box->focused){
		geKeyboardUpdate();
		i = geKeyboardIndex();
	}
	if(box->lostfocus){
		geKeyboardFinished();
	}

	u32 foreground = RGBA(255-R(box->back_color), 255-G(box->back_color), 255-B(box->back_color), 255);
	geBlitImage(object->absx, object->absy, box->img, 0, 0, box->width, box->height, 0);
	geFontPrintScreen(object->absx+box->textpos[0], object->absy+(box->height/2-style->font->size/2), style->font, box->text, foreground);

	if(i >= 0){
		int w, h;
		char str[2048] = "";
		strncpy(str, box->text, i);
		geFontMeasureText(style->font, str, &w, &h);
		int x = object->absx+box->textpos[0]+w;
		int y = object->absy+(box->height/2-style->font->size/2);

		if(geGetTick() - tick < 500){
			geDrawLineScreen(x, y, x, y+style->font->size, foreground);
		}else if(geGetTick() - tick >= 1000){
			tick = geGetTick();
		}
	}
}
