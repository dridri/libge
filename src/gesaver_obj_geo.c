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

#include "ge_internal.h"

void geSaveGeoList(ge_Object** objs, int count, const char* file, int flags){
	int i, j, nTextures;
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_CLEAR_RW | GE_FILE_MODE_BINARY);
	if(!fp){
		return;
	}

	ge_geo_header header;
	memset(&header, 0x0, sizeof(ge_geo_header));
	header.magic = 0x4F4547FE;
	header.version_min = 0;
	header.version_maj = 1;
	header.nObjs = count;
	header.compressed = (flags & GE_COMPRESSED) >> 16;
	geFileWrite(fp, &header, sizeof(ge_geo_header));

	ge_geo_object* objects = (ge_geo_object*)geMalloc(sizeof(ge_geo_object)*count);
	ge_geo_texture** textures = (ge_geo_texture**)geMalloc(sizeof(ge_geo_texture*) * count);
	ge_geo_vertex** vertices = (ge_geo_vertex**)geMalloc(sizeof(ge_geo_vertex*) * count);


	for(i=0; i<count; i++){
		for(j=0, nTextures=0; j<8; j++){
			if(objs[i]->material.textures[j] != NULL){
				nTextures++;
			}
		}
		strncpy(objects[i].name, objs[i]->name, 64);
		memcpy(objects[i].matrix, objs[i]->matrix, sizeof(float)*16);
#ifdef U32_COLORS
		objects[i].diffuse = objs[i]->material.diffuse;
		objects[i].ambient = objs[i]->material.ambient;
		objects[i].specular = objs[i]->material.specular;
#else
		objects[i].diffuse = fRGBA(objs[i]->material.diffuse);
		objects[i].ambient = fRGBA(objs[i]->material.ambient);
		objects[i].specular = fRGBA(objs[i]->material.specular);
#endif
		objects[i].bump_force = objs[i]->material.bump_force;
		objects[i].nTextures = nTextures;
		objects[i].nVertices = objs[i]->nVerts;
		objects[i].texturesOfs = 0;
		objects[i].verticesOfs = 0;
		textures[i] = (ge_geo_texture*)geMalloc(sizeof(ge_geo_texture)*objects[i].nTextures);
		for(j=0, nTextures=0; j<8; j++){
			if(objs[i]->material.textures[j] != NULL){
				textures[i][nTextures].id = objs[i]->material.textures[nTextures]->id;
				textures[i][nTextures].flags = objs[i]->material.textures[nTextures]->flags;
				textures[i][nTextures].color = objs[i]->material.textures[nTextures]->color;
				textures[i][nTextures].width = objs[i]->material.textures[nTextures]->width;
				textures[i][nTextures].height = objs[i]->material.textures[nTextures]->height;
				ge_File* ftex = geFileOpen(objs[i]->material.textures_path[nTextures], GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
				geFileSeek(ftex, 0, GE_FILE_SEEK_END);
				textures[i][nTextures].datasize = geFileTell(ftex);
				geFileClose(ftex);
				nTextures++;
			}
		}

		vertices[i] = (ge_geo_vertex*)geMalloc(sizeof(ge_geo_vertex)*objects[i].nVertices);
		for(j=0; j<objects[i].nVertices; j++){
			vertices[i][j].u = objs[i]->verts[j].u;
			vertices[i][j].v = objs[i]->verts[j].v;
#ifndef NO_3D_TEXCOORDS
			vertices[i][j].w = objs[i]->verts[j].w;
#endif
#ifdef U32_COLORS
			SET_COLOR(vertices[i][j].color, objs[i]->verts[j].color);
#else
			vertices[i][j].color = fRGBA(objs[i]->verts[j].color);
#endif
			vertices[i][j].nx = objs[i]->verts[j].nx;
			vertices[i][j].ny = objs[i]->verts[j].ny;
			vertices[i][j].nz = objs[i]->verts[j].nz;
			vertices[i][j].x = objs[i]->verts[j].x;
			vertices[i][j].y = objs[i]->verts[j].y;
			vertices[i][j].z = objs[i]->verts[j].z;
		}
	}

	int ObjsOfs = geFileTell(fp);
	geFileWrite(fp, objects, sizeof(ge_geo_object)*count);

	for(i=0; i<count; i++){
		objects[i].texturesOfs = geFileTell(fp);
		for(j=0, nTextures=0; j<8; j++){
			if(objs[i]->material.textures[j] != NULL){
				textures[i][nTextures].flags = objs[i]->material.textures[nTextures]->flags;
				// objs[i]->material.textures_path[nTextures]
				geFileWrite(fp, &textures[i][nTextures], sizeof(ge_geo_texture));
				ge_File* ftex = geFileOpen(objs[i]->material.textures_path[nTextures], GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
				geFileSeek(ftex, 0, GE_FILE_SEEK_END);
				int bufsz = geFileTell(ftex);
				geFileRewind(ftex);
				void* buf = geMalloc(bufsz);
				geFileRead(ftex, buf, bufsz);
				geFileClose(ftex);
				geFileWrite(fp, buf, bufsz);
				geFree(buf);
				nTextures++;
			}
		}

		objects[i].verticesOfs = geFileTell(fp);
		if(header.compressed){
			ge_Buffer* vbuf = geCompress(vertices[i], sizeof(ge_geo_vertex)*objects[i].nVertices, GE_COMPRESSION_ZLIB);
			geFileWrite(fp, vbuf->ptr, vbuf->size);
			objects[i].verticesSize = vbuf->size;
			geFree(vbuf->ptr);
			geFree(vbuf);
		}else{
			geFileWrite(fp, vertices[i], sizeof(ge_geo_vertex)*objects[i].nVertices);
			objects[i].verticesSize = sizeof(ge_geo_vertex)*objects[i].nVertices;
		}
	}

	geFileSeek(fp, ObjsOfs, GE_FILE_SEEK_SET);
	geFileWrite(fp, objects, sizeof(ge_geo_object)*count);
	
	geFileClose(fp);

	for(i=0; i<count; i++){
		geFree(textures[i]);
		geFree(vertices[i]);
	}
	geFree(textures);
	geFree(vertices);
	geFree(objects);
}
