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

void geLoadGeoListMulti(const char* file, const char** obj_list, int count, ge_Object** objs){
	int i, j, k;
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(!fp){
		return;
	}

	ge_geo_header header;
	geFileRead(fp, &header, sizeof(ge_geo_header));
	if(header.magic != 0x4F4547FE){
		geFileClose(fp);
		gePrintDebug(0x102, "geLoadGeoListMulti: Incorrect magic (0x%08X)\n", header.magic);
		return;
	}
	if(count > header.nObjs){
		geFileClose(fp);
		gePrintDebug(0x102, "geLoadGeoListMulti: List bigger than objects in file (%d/%d)\n", count, header.nObjs);
		return;
	}
	gePrintDebug(0x100, "geLoadGeoListMulti: 0x%08X %d %d %d\n", header.magic, header.version_min, header.version_maj, header.nObjs);
	gePrintBuffer((void*)&header, sizeof(ge_geo_header));

	if(count <= 0){
		count = header.nObjs;
	}

	ge_geo_object* objects = (ge_geo_object*)geMalloc(sizeof(ge_geo_object) * count);
	ge_geo_texture** textures = (ge_geo_texture**)geMalloc(sizeof(ge_geo_texture*) * count);
	ge_geo_vertex** vertices = (ge_geo_vertex**)geMalloc(sizeof(ge_geo_vertex*) * count);
	
	gePrintDebug(0x100, "geLoadGeoListMulti: Loading %d objects\n", count);

	int tex_i = 0;
	int tex_ids[128] = { -1 };
	ge_Image* texes[128] = { NULL };

	geFileRead(fp, objects, sizeof(ge_geo_object)*count);
	for(i=0; i<count; i++){
		strncpy(objs[i]->name, objects[i].name, 64);
		memcpy(objs[i]->matrix, objects[i].matrix, sizeof(float)*16);
		objs[i]->matrix_used = ge_IsIdentity(objs[i]->matrix);
		SET_COLOR(objs[i]->material.diffuse, objects[i].diffuse);
		SET_COLOR(objs[i]->material.ambient, objects[i].ambient);
		SET_COLOR(objs[i]->material.specular, objects[i].specular);
		objs[i]->material.bump_force = objects[i].bump_force;
		objs[i]->nVerts = objects[i].nVertices;

		geFileSeek(fp, objects[i].texturesOfs, GE_FILE_SEEK_SET);
		textures[i] = (ge_geo_texture*)geMalloc(sizeof(ge_geo_texture)*objects[i].nTextures);
		for(j=0; j<objects[i].nTextures; j++){
			geFileRead(fp, &textures[i][j], sizeof(ge_geo_texture));
			for(k=0; k<tex_i; k++){
				if(textures[i][j].id == tex_ids[k]){
					break;
				}
			}
			if(k < tex_i){
				objs[i]->material.textures[j] = texes[k];
			}else{
				ge_File* ftex = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
				((ge_file_desc*)ftex->desc)->fake_offset = geFileTell(fp);
				((ge_file_desc*)ftex->desc)->size = textures[i][j].datasize;
				geFileRewind(ftex);
				objs[i]->material.textures[j] = geLoadImage(gePathFromFile(ftex));
			//	objs[i]->material.textures[j]->flags = textures[i][j].flags;
				objs[i]->material.textures[j]->color = textures[i][j].color;
				objs[i]->material.textures[j] = geAddTexture(objs[i]->material.textures[j]);
				texes[tex_i] = objs[i]->material.textures[j];
				tex_ids[tex_i] = textures[i][j].id;
				tex_i++;
			}
			geFileSeek(fp, textures[i][j].datasize, GE_FILE_SEEK_CUR);
		}
		if(textures[i]){
			geFree(textures[i]);
		}

		geFileSeek(fp, objects[i].verticesOfs, GE_FILE_SEEK_SET);
		vertices[i] = (ge_geo_vertex*)geMalloc(objects[i].verticesSize);
		geFileRead(fp, vertices[i], objects[i].verticesSize);
		if(header.compressed){
			ge_Buffer* vbuf = geDecompress(vertices[i], objects[i].verticesSize, GE_COMPRESSION_ZLIB);
			geFree(vertices[i]);
			vertices[i] = (ge_geo_vertex*)vbuf->ptr;
			geFree(vbuf);
		}
		objs[i]->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*objects[i].nVertices);
		for(j=0; j<objects[i].nVertices; j++){
			objs[i]->verts[j].u = vertices[i][j].u;
			objs[i]->verts[j].v = vertices[i][j].v;
#ifndef NO_3D_TEXCOORDS
			objs[i]->verts[j].w = vertices[i][j].w;
#endif
			SET_COLOR(objs[i]->verts[j].color, vertices[i][j].color);
			objs[i]->verts[j].nx = vertices[i][j].nx;
			objs[i]->verts[j].ny = vertices[i][j].ny;
			objs[i]->verts[j].nz = vertices[i][j].nz;
			objs[i]->verts[j].x = vertices[i][j].x;
			objs[i]->verts[j].y = vertices[i][j].y;
			objs[i]->verts[j].z = vertices[i][j].z;
		}
		geFree(vertices[i]);
	}

	geFree(vertices);
	geFree(textures);
	geFree(objects);
	geFileClose(fp);
	gePrintDebug(0x100, "geLoadGeoListMulti: Ok for %d objects\n", count);
}

ge_Object* geLoadGeo(const char* file, const char* obj_name){
	ge_Object* obj = (ge_Object*)geMalloc(sizeof(ge_Object));
	if(obj_name && obj_name[0] != 0x0){
		geLoadGeoListMulti(file, &obj_name, 1, &obj);
	}else{
		geLoadGeoListMulti(file, NULL, 1, &obj);
	}
	return obj;
}

int geGeoCountInFile(const char* file){
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(!fp){
		return 0;
	}

	ge_geo_header header;
	geFileRead(fp, &header, sizeof(ge_geo_header));
	if(header.magic != 0x4F4547FE){
		header.nObjs = 0;
	}

	geFileClose(fp);
	return header.nObjs;
}
