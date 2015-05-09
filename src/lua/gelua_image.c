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

static void alloc_img(lua_State *L, ge_Image* img);

static int Image_gc(lua_State *L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	printf("Image_gc\n");
	geFreeImage(dest);

	return 1;
}

static int Image_create(lua_State *L){
	int argc = lua_gettop(L); 

	if (argc != 2 && argc != 3){
		return luaL_error(L, "Argument error: geImage.Create(width, height, [background_color]) takes two or three arguments.");
	}
	ge_Image* img = geCreateSurface(luaL_checkint(L, 1), luaL_checkint(L, 2), argc==3 ? *toColor(L, 3) : 0x0);

	alloc_img(L, img);

	return 1;
/*
	int argc = lua_gettop(L); 
	if (argc != 2 && argc != 3){
		return luaL_error(L, "Argument error: geImage.Create(width, height, [background_color]) takes two or three arguments.");
	}
	ge_Image* img = geCreateSurface(luaL_checkint(L, 1), luaL_checkint(L, 2), argc==3 ? *toColor(L, 3) : 0x0);
	*pushNewImage(L) = img;
	return 1;
*/
}

static int Image_load(lua_State *L){
	printf("mylock\n");
//	lua_lock(L);
	printf("Image_load 1\n");
	int argc = lua_gettop(L); 
	if (argc != 1 && argc != 2){
		return luaL_error(L, "Argument error: geImage.Load(path, [flags]) takes one or two arguments.");
	}
	printf("Image_load 2\n");
// 	lua_gc(L, LUA_GCCOLLECT, 0);
	printf("Image_load 3\n");
	
	char file[2048] = "";
	char tmp[2048] = "";
	strncpy(tmp, luaL_checkstring(L, 1), 2048);
	ge_LuaScript* script = ge_ScriptFromState(L);
	sprintf(file, "%s%s%s", script->root, script->root[0] ? "/" : "", tmp);
	printf("Image_load 4\n");

	int flags = 0;
	if(argc == 2){
		flags = luaL_checkint(L, 2);
	}
	printf("Image_load 5\n");

	ge_Image* img = geLoadImageOptions(file, flags);
	printf("Image_load 6\n");

	if(!img){
		return luaL_error(L, "Error while loading image \"%s\"", file);
	}

	printf("Image_load 7\n");
//	lua_unlock(L);
	alloc_img(L, img);
	printf("Image_load 8\n");

	return 1;
}

static int Image_textureMode(lua_State* L){
	int argc = lua_gettop(L);

	if(argc != 2){
		return luaL_error(L, "Error: geImage:TextureMode(mode) must be with a colon and takes one argument");
	}

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	int mode = luaL_checkint(L, 2);

	geTextureMode(dest, mode);

	return 1;
}

static int Image_textureWrap(lua_State* L){
	int argc = lua_gettop(L);

	if(argc != 3){
		return luaL_error(L, "Error: geImage:TextureWrap(wrap_x, wrap_y) must be with a colon and takes two arguments");
	}

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	geTextureWrap(dest, luaL_checkint(L, 2), luaL_checkint(L, 3));

	return 1;
}

static int Image_blit(lua_State* L){
	int argc = lua_gettop(L);
/*
	lua_pushstring(L, "img");
	lua_gettable(L, -2);
	ge_Image* dest = selfImage(L, &argc);
	printf("dest = %p\n", dest);
*/
	ge_Image* dest = 0;

	if(argc < 3){
		return luaL_error(L, "Argument error: geImage.Blit(x, y, image, [sx, sy, width, height, flags]) takes at least 3 arguments.");
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	lua_getfield(L, 3, "img");
	ge_Image* img = *toImage(L, -1);
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

static int Image_stretched(lua_State* L){
	int argc = lua_gettop(L);

	ge_Image* dest = 0;

	if(argc < 3){
		return luaL_error(L, "Argument error: geImage.stretch(x, y, width, height, image, [sx, sy, w, h, flags]) takes at least 5 arguments.");
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	int w = luaL_checkint(L, 3);
	int h = luaL_checkint(L, 4);
	lua_getfield(L, 5, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 5);
	ge_Image* img = *toImage(L, -1);
	int sx = 0;
	int sy = 0;
	int width = img->width;
	int height = img->height;
	int flags = 0;

	if(argc > 5){
		sx = luaL_checkint(L, 6);
	}
	if(argc > 6){
		sy = luaL_checkint(L, 7);
	}
	if(argc > 7){
		width = luaL_checkint(L, 8);
	}
	if(argc > 8){
		height = luaL_checkint(L, 9);
	}
	if(argc > 9){
		flags = luaL_checkint(L, 10);
	}
	
	if(dest){
	//	geBlitImageToImage(x, y, img, sx, sy, width, height, dest);
	}else{
		geBlitImageStretched(x, y, img, sx, sy, width, height, w, h, flags);
	}

	return 1;
}

static int Image_stretchedRotated(lua_State* L){
	int argc = lua_gettop(L);

	ge_Image* dest = 0;

	if(argc < 3){
		return luaL_error(L, "Argument error: geImage.stretch(x, y, width, height, image, angle, [sx, sy, w, h, flags]) takes at least 6 arguments.");
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	int w = luaL_checkint(L, 3);
	int h = luaL_checkint(L, 4);
	double angle = luaL_checknumber(L, 6);
	lua_getfield(L, 5, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 5);
	ge_Image* img = *toImage(L, -1);
	int sx = 0;
	int sy = 0;
	int ex = img->width;
	int ey = img->height;
	int flags = 0;

	if(argc > 6){
		sx = luaL_checkint(L, 7);
	}
	if(argc > 7){
		sy = luaL_checkint(L, 8);
	}
	if(argc > 8){
		ex = luaL_checkint(L, 9);
	}
	if(argc > 9){
		ey = luaL_checkint(L, 10);
	}
	if(argc > 10){
		flags = luaL_checkint(L, 11);
	}
	
	if(dest){
	//	geBlitImageToImage(x, y, img, sx, sy, width, height, dest);
	}else{
		geBlitImageStretchedRotated(x, y, img, sx, sy, ex, ey, w, h, angle, flags | GE_BLIT_CENTERED);
	}

	return 1;
}

static int Image_width(lua_State* L){
	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	if(!dest){
		return luaL_error(L, "Error: geImage.Width() must be with a colon");
	}
	lua_pushinteger(L, dest->width);
	return 1;
}

static int Image_height(lua_State* L){
	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	if(!dest){
		return luaL_error(L, "Error: geImage.Height() must be with a colon");
	}
	lua_pushinteger(L, dest->height);
	return 1;
}
/*
static int Image_color(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	if(!dest){
		return luaL_error(L, "Error: geImage.setColor([color]) must be with a colon");
	}

	if(argc > 0){
		u32 color = *toColor(L, 2);
		dest->color = color;
	}else{
		*pushNewColor(L) = dest->color;
	}

	return 1;
}
*/
static int Image_pixel(lua_State* L){
/*TODO
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
*/
	return 1;
}

static int Image_update(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	if(!dest){
		return luaL_error(L, "Error: geImage.update() must be with a colon");
	}

	geUpdateImage(dest);

	return 1;
}

static int Image_animate(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	if(!dest || argc < 3){
		return luaL_error(L, "Error: geImage.animate(nFrames, frameTime) must be with a colon and takes two arguments");
	}

	ge_Image* img = geAnimateImage(dest, luaL_checkint(L, 2), luaL_checknumber(L, 3));
	geFree(dest);

	*pushNewImage(L) = img;
	lua_setfield(L, 1, "img");

	return 1;
}

static int Image_index(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	const char* key = luaL_checkstring(L, 2);
//	gePrintDebug(0x100, "LUA::Image_index key = \"%s\"\n", key);
	if(!strcmp(key, "width")){
		lua_pushinteger(L, dest->width);
	}else if(!strcmp(key, "height")){
		lua_pushinteger(L, dest->height);
	}else if(!strcmp(key, "textureWidth")){
		lua_pushinteger(L, dest->textureWidth);
	}else if(!strcmp(key, "textureHeight")){
		lua_pushinteger(L, dest->textureHeight);
	}else if(!strcmp(key, "color")){
		*pushNewColor(L) = dest->color;
	}else if(!strcmp(key, "flags")){
		lua_pushinteger(L, dest->flags);
	}else{
		lua_pushvalue(L, 2);
		lua_rawget(L, 1);
	}

	return 1;
}

static int Image_newIndex(lua_State* L){
	int argc = lua_gettop(L);

	lua_getfield(L, 1, "img");
	lua_pushstring(L, "img");
	lua_gettable(L, 1);
	ge_Image* dest = *toImage(L, -1);

	const char* key = luaL_checkstring(L, 2);
//	gePrintDebug(0x100, "LUA::Image_newIndex key = \"%s\"\n", key);

	if(!strcmp(key, "width")){
		dest->width = luaL_checkint(L, 3);
	}else if(!strcmp(key, "height")){
		dest->height = luaL_checkint(L, 3);
	}else if(!strcmp(key, "color")){
		dest->color = *toColor(L, 3);
	}else if(!strcmp(key, "flags")){
		dest->flags = luaL_checkint(L, 3);
	}else if(!strcmp(key, "anim") && dest->flags & GE_IMAGE_ANIMATED){
		((_ge_ImageAnimated*)dest)->_ge_n = luaL_checkint(L, 3);
	}else{
		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
	//	lua_setfield(L, -2, key);
		lua_rawset(L, 1);
	}

	return 1;
}


static const luaL_Reg Image_functions[] = {
	{ "create", Image_create },
	{ "load", Image_load },
	{ "draw", Image_blit },
	{ "blit", Image_blit },
	{ "blitStretched", Image_stretched },
	{ "blitStretchedRotated", Image_stretchedRotated },
	{ NULL, NULL }
};

static const luaL_Reg Image_meta_base[] = {
	{ "__index", Image_index },
	{ "__newindex", Image_newIndex },
	{ NULL, NULL }
};

UserdataRegister(Image, Image_functions, Image_meta_base)


int geLuaInit_image(lua_State* L){
	Image_register(L);
	return 0;
}




static const luaL_Reg Image_methods[] = {
//	{ "width", Image_width },
//	{ "height", Image_height },
	{ "animate", Image_animate },
//	{ "setColor", Image_color },
	{ "Pixel", Image_pixel },
	{ "Update", Image_update },
	{ "TextureMode", Image_textureMode },
	{ "TextureWrap", Image_textureWrap },
	{ NULL, NULL }
};

static const luaL_Reg Image_meta[] = {
	{ "__index", Image_index },
	{ "__newindex", Image_newIndex },
// 	{ "__gc", Image_gc },
	{ NULL, NULL }
};

static void alloc_img(lua_State *L, ge_Image* img){
	printf("alloc_img 1\n");

	lua_createtable(L, 0, 0);
	printf("alloc_img 2\n");
	luaL_setfuncs(L, Image_methods, 0);
	printf("alloc_img 3\n");
/*
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Image_meta, 0);
//	lua_pushcclosure(L, Image_index, 0);
//	lua_setfield(L, -2, "__index");
//	lua_pushcclosure(L, Image_newIndex, 0);
//	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);
*/

	*pushNewImage(L) = img;
	lua_setfield(L, -2, "img");

	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "x");

	lua_pushinteger(L, 0);
	lua_setfield(L, -2, "y");

	lua_pushnumber(L, 1.0);
	lua_setfield(L, -2, "scale");
/*
	luaL_getmetatable(L, "geImage");
	lua_setmetatable(L, -2);
*/
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, Image_meta, 0);
	lua_setmetatable(L, -2);
}
