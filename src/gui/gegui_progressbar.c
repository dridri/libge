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

ge_GuiProgressBar* geGuiCreateProgressBar(int width, int height, int max){
	ge_GuiProgressBar* bar = (ge_GuiProgressBar*)geMalloc(sizeof(ge_GuiProgressBar));

	bar->type = GE_GUI_PROGRESSBAR;
	bar->back_color = 0xFF000000;
	bar->max = max;
	bar->status = 0;
	bar->width = width;
	bar->height = height;
	bar->textpos[0] = -1;
	bar->textpos[1] = -1;
	bar->text = (char*)geMalloc(128);
	bar->textmode = GE_GUI_PROGRESS_PERCENT;

	return bar;
}

void _geGui_RenderProgressBar(ge_GuiAreaObject* object, ge_GuiStyle* style){
	ge_GuiProgressBar* bar = (ge_GuiProgressBar*)object->object;
	if(!bar->img){
		_geGui_MakeResizableObject((ge_GuiWidget*)bar, style);
	}

	geBlitImage(object->absx, object->absy, bar->img, 0, 0, bar->width, bar->height, 0);

	int left = (int)(floor( (float)style->box->width/2 - (float)style->progressbar->width/2) );
	int top =  (int)(floor( (float)style->box->height/2 -  (float)style->progressbar->height/2) );

	int block_w = 0;
	int block_h = 0;
	if(bar->width < style->progressbar->width){
		block_w = bar->width / 2;
	}else{
		block_w = style->progressbar->width / 2;
	}
	if(bar->height > style->progressbar->height){
		block_h = bar->height;
	}else{
		block_h = style->progressbar->height;
	}
	block_h -= top;

	if(bar->status > bar->max){
		bar->status = bar->max;
	}
	int wdt = bar->width * bar->status / bar->max;
	
	if(wdt < style->progressbar->width){
		geBlitImageStretched(object->absx + left, object->absy + top, style->progressbar, 0, 0, style->progressbar->width, style->progressbar->height, wdt, block_h-top, 0);
	}else{
		geBlitImageStretched(object->absx + left, object->absy + top, style->progressbar, 0, 0, block_w, style->progressbar->height, block_w, block_h-top, 0);
		geBlitImageStretched(object->absx + left + wdt - style->progressbar->width/2, object->absy + top, style->progressbar, style->progressbar->width/2, 0, style->progressbar->width/2, style->progressbar->height, block_w, block_h-top, 0);
		geBlitImageStretched(object->absx + left + block_w, object->absy + top, style->progressbar, style->progressbar->width / 2, 0, style->progressbar->width-block_w*2, style->progressbar->height, wdt-block_w*2, block_h-top, 0);
	}

	if(bar->textmode == GE_GUI_PROGRESS_PERCENT){
		int tw, th;
		sprintf(bar->text, "%d %%", 100 * bar->status / bar->max);
		u32 foreground = RGBA(255-R(bar->back_color), 255-G(bar->back_color), 255-B(bar->back_color), 255);
		geFontMeasureText(style->font, bar->text, &tw, &th);
		geFontPrintScreen(object->absx + bar->width / 2 - tw / 2, object->absy + bar->height / 2 - th / 2, style->font, bar->text, foreground);
	}
}
