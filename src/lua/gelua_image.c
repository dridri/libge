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

#include "ge_lua.c"

UserdataStubs(Image, ge_Image*);

static int Image_create(lua_State *L){
	int argc = lua_gettop(L); 
	if (argc != 2 && argc != 3){
		return luaL_error(L, "Argument error: geImage.Create(width, height, [background_color]) takes two or three arguments.");
	}
	ge_Image* img = geCreateSurface(luaL_checkint(L, 1), luaL_checkint(L, 2), argc==3 ? *toColor(L, 3) : 0x0);
	*pushNewImage(L) = img;
	return 1;
}

static int Image_load(lua_State *L){
	int argc = lua_gettop(L); 
	if (argc != 1 && argc != 2){
		return luaL_error(L, "Argument error: geImage.Load(path, [flags]) takes one or two arguments.");
	}
	lua_gc(L, LUA_GCCOLLECT, 0);
	
	char file[2048] = "";
	char tmp[2048] = "";
	strncpy(tmp, luaL_checkstring(L, 1), 2048);
	ge_LuaScript* script = ge_ScriptFromState(L);
	sprintf(file, "%s/%s", script->root, tmp);

	int flags = 0;
	if(argc == 2){
		flags = luaL_checkint(L, 2);
	}

	ge_Image* img = geLoadImageOptions(file, flags);

	if(!img){
		return luaL_error(L, "Error while loading image \"%s\"", file);
	}
	*pushNewImage(L) = img;

	return 1;
}

static int Image_blit(lua_State* L){
	int argc = lua_gettop(L);
	ge_Image* dest = selfImage(L, &argc);

	if(argc < 3){
		return luaL_error(L, "Argument error: geImage.Blit(x, y, image, [sx, sy, width, height, flags]) takes at least 3 arguments.");
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	ge_Image* img = *toImage(L, 3);
	int sx = 0;
	int sy = 0;
	int width = img->width;
	int height = img->height;
	int flags = 0;

	if(argc > 3){
		sx = luaL_checkint(L, 4);
	}
	if(argc > 4){
		sy = luaL_checkint(L, 5);
	}
	if(argc > 5){
		width = luaL_checkint(L, 6);
	}
	if(argc > 6){
		height = luaL_checkint(L, 7);
	}
	if(argc > 7){
		flags = luaL_checkint(L, 8);
	}
	
	if(dest){
		geBlitImageToImage(x, y, img, sx, sy, width, height, dest);
	}else{
		geBlitImage(x, y, img, sx, sy, width, height, flags);
	}

	return 1;
}

static int Image_width(lua_State* L){
	ge_Image* dest = selfImage(L, NULL);
	if(!dest){
		return luaL_error(L, "Error: geImage.Width() must be with a colon");
	}
	lua_pushinteger(L, dest->width);
	return 1;
}

static int Image_height(lua_State* L){
	ge_Image* dest = selfImage(L, NULL);
	if(!dest){
		return luaL_error(L, "Error: geImage.Height() must be with a colon");
	}
	lua_pushinteger(L, dest->height);
	return 1;
}

static int Image_color(lua_State* L){
	int argc = lua_gettop(L);
	ge_Image* dest = selfImage(L, &argc);
	if(!dest){
		return luaL_error(L, "Error: geImage.Color([color]) must be with a colon");
	}

	if(argc > 0){
		u32 color = *toColor(L, 1);
		dest->color = color;
	}else{
		*pushNewColor(L) = dest->color;
	}

	return 1;
}

static int Image_pixel(lua_State* L){
	int argc = lua_gettop(L);
	ge_Image* dest = selfImage(L, &argc);
	if(!dest){
		return luaL_error(L, "Error: geImage.Pixel(x, y, [color]) must be with a colon");
	}
	if(argc < 2){
		return luaL_error(L, "Argument error: geImage.Pixel(x, y, [color] takes at least 2 arguments.");
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	if(x >= 0 && y >= 0 && x < dest->width && y < dest->height){
		if(argc == 3){
			dest->data[x + y*dest->textureWidth] = *toColor(L, 3);
		}else{
			*pushNewColor(L) = dest->data[x + y*dest->textureWidth];
		}
	}

	return 1;
}

static int Image_update(lua_State* L){
	ge_Image* dest = selfImage(L, NULL);
	if(!dest){
		return luaL_error(L, "Error: geImage.Update() must be with a colon");
	}

	geUpdateImage(dest);

	return 1;
}

static const luaL_Reg Image_methods[] = {
	{ "Create", Image_create },
	{ "Load", Image_load },
	{ "Blit", Image_blit },
	{ "Draw", Image_blit },
	{ "Width", Image_width },
	{ "Height", Image_height },
	{ "Color", Image_color },
	{ "Pixel", Image_pixel },
	{ "Update", Image_update },
	{ NULL, NULL }
};

static const luaL_Reg Image_meta[] = {
	{ NULL, NULL }
};

UserdataRegister(Image, Image_methods, Image_meta)

int geLuaInit_image(lua_State* L){
	Image_register(L);
	return 0;
}
