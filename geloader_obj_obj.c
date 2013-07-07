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
int CheckNumSlash(const char* buffer);
char* strlastof(const char* buffer, char last_c);
bool findInList(const char** list, int count, const char* w);

typedef struct MAT {
	char name[32];
	float diffuse[4];
	float ambient[4];
	float specular[4];
	char texture[256];
	char bump_map[256];
	char dudv_map[256];
	float bump_force;
	int texture_filter;
} MAT;

typedef struct MTL {
	MAT* mats;
	int nMats;
} MTL;

MTL* LoadObjMtl(const char* file){
	gePrintDebug(0x100, "LoadObjMtl(\"%s\")\n", file);
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ|GE_FILE_MODE_BINARY);
	if(!fp){
		gePrintDebug(0x100, "\nERROR while opening file %s\n", file);
		geFileClose(fp);
		return NULL;
	}
	
	MTL* mtl = (MTL*)geMalloc(sizeof(MTL));
	memset(mtl, 0, sizeof(MTL));
	char buffer[256] = "";
	char last_buffer[256] = "";

	while(geFileGets(fp, buffer, 256)){
		if(strstr(buffer, "newmtl")){
			mtl->nMats++;
		}
	}
	geFileRewind(fp);

	mtl->mats = (MAT*)geMalloc(sizeof(MAT)*mtl->nMats);
	memset(mtl->mats, 0, sizeof(MAT)*mtl->nMats);

	geFileGets(fp, buffer, 256);
	int i = 0;
	float ftmp[3] = { 0.0 };
	char stmp[256] = "";
	do{
		if(buffer[0]=='#')continue;
		if(strstr(buffer, "newmtl") || strstr(last_buffer, "newmtl")){
			mtl->mats[i].texture_filter = GE_LINEAR;
			if(strstr(buffer, "newmtl")){
				geGetParamString(buffer, "newmtl", mtl->mats[i].name, 256);
			}
			if(strstr(last_buffer, "newmtl")){
				geGetParamString(last_buffer, "newmtl", mtl->mats[i].name, 256);
			}
			memset(last_buffer, 0, 256);
			float Tf[3] = { 1.0, 1.0, 1.0 };
			float dissolve = 1.0;
			while(geFileGets(fp, buffer, 256)){
				if(buffer[0]=='#')continue;
				if(strstr(buffer, "newmtl")){
					strcpy(last_buffer, buffer);
					break;
				}
				geGetParamFloatMulti(buffer, "Tf", Tf, 3);
				geGetParamFloat(buffer, "d", &dissolve);
				if(geGetParamFloat(buffer, "Tr", &ftmp[0])){
					mtl->mats[i].ambient[3] = 1.0-ftmp[0];
					mtl->mats[i].diffuse[3] = 1.0-ftmp[0];
				}
				if(geGetParamFloatMulti(buffer, "Ka", ftmp, 3)){
					mtl->mats[i].ambient[0] = ftmp[0];
					mtl->mats[i].ambient[1] = ftmp[1];
					mtl->mats[i].ambient[2] = ftmp[2];
				}
				if(geGetParamFloatMulti(buffer, "Kd", ftmp, 3)){
					mtl->mats[i].diffuse[0] = ftmp[0];
					mtl->mats[i].diffuse[1] = ftmp[1];
					mtl->mats[i].diffuse[2] = ftmp[2];
				}
				if(geGetParamFloatMulti(buffer, "Ks", ftmp, 3)){
					mtl->mats[i].specular[0] = ftmp[0];
					mtl->mats[i].specular[1] = ftmp[1];
					mtl->mats[i].specular[2] = ftmp[2];
				}
				if(geGetParamString(buffer, "map_Kd", stmp, 256)){
					if(!strncmp(stmp, "filter", 6)){
						geGetParamString(stmp, "filter", stmp, 256);
						if(strstr(stmp, "nearest")){
							mtl->mats[i].texture_filter = GE_NEAREST;
						}else
						if(strstr(stmp, "linear")){
							mtl->mats[i].texture_filter = GE_LINEAR;
						}
					}else{
						strcpy(mtl->mats[i].texture, stmp);
					}
				}
				if(geGetParamString(buffer, "map_dudv", stmp, 256)){
					strcpy(mtl->mats[i].dudv_map, stmp);
				}
				if(strstr(buffer, "map_bump")){
					if(strstr(buffer, "-bm")){
						geGetParamFloat(buffer, "-bm", &mtl->mats[i].bump_force);
						strcpy(mtl->mats[i].bump_map, strstr(buffer, "map_bump")+strlen("map_bump -bm 0.800000 "));
						if(mtl->mats[i].bump_map[strlen(mtl->mats[i].bump_map)-1] == '\n'){
							mtl->mats[i].bump_map[strlen(mtl->mats[i].bump_map)-1] = 0x0;
						}
					}else{
						geGetParamString(buffer, "map_bump", mtl->mats[i].bump_map, 256);
						mtl->mats[i].bump_force = 1.0;
					}
				}
			}
			
			mtl->mats[i].diffuse[0] *= Tf[0];
			mtl->mats[i].diffuse[1] *= Tf[1];
			mtl->mats[i].diffuse[2] *= Tf[2];
			mtl->mats[i].diffuse[3] *= dissolve;
			gePrintDebug(0x100, "Material [%d] :\n", i);
			gePrintDebug(0x100, "  name: \"%s\"\n", mtl->mats[i].name);
			gePrintDebug(0x100, "  ambient: 0x%08X\n", mtl->mats[i].ambient);
			gePrintDebug(0x100, "  diffuse: 0x%08X\n", mtl->mats[i].diffuse);
			gePrintDebug(0x100, "  specular: 0x%08X\n", mtl->mats[i].specular);
			gePrintDebug(0x100, "  texture: \"%s\"\n", mtl->mats[i].texture);
			gePrintDebug(0x100, "  bump_map: \"%s\"\n", mtl->mats[i].bump_map);
			gePrintDebug(0x100, "  bump_force: %f\n", mtl->mats[i].bump_force);
			gePrintDebug(0x100, "  dudv_map: \"%s\"\n", mtl->mats[i].dudv_map);

			i++;
		}
	}while(geFileGets(fp, buffer, 256));

	geFileClose(fp);
	gePrintDebug(0x100, "end\n");
	return mtl;
}

MAT* MtlGetMat(MTL* mtl, const char* name){
	int i = 0;
	for(i=0; i<mtl->nMats; i++){
		if(!strcmp(mtl->mats[i].name, name)){
			return &mtl->mats[i];
		}
	}
	gePrintDebug(0x103, "MtlGetMat(\"%s\") returned NULL !!\n", name);
	return NULL;
}

void CloseObjMtl(MTL* mtl){
	if(mtl && mtl->mats)geFree(mtl->mats);
	if(mtl)geFree(mtl);
}

void getData(const char* buffer, ge_Vertex* obj, int index, ge_Vector3f* Verts, ge_Vector3f* TVerts, ge_Vector3f* NVerts, u32 colorI){
	int v1, v2, v3;
	int vt1, vt2, vt3;
	int vn1, vn2, vn3;
			int slash = CheckNumSlash(buffer);
		//	if((textures == false) && (normals == false)){
			if(slash == 0){
				sscanf(buffer, "f %d %d %d", &v1, &v2, &v3);
				v1--; v2--;v3--;
				obj[index].x = Verts[v1].x;	obj[index].y = Verts[v1].y;	obj[index].z = Verts[v1].z;
				obj[index+1].x = Verts[v2].x;	obj[index+1].y = Verts[v2].y;	obj[index+1].z = Verts[v2].z;
				obj[index+2].x = Verts[v3].x;	obj[index+2].y = Verts[v3].y;	obj[index+2].z = Verts[v3].z;
				
				obj[index].nx = 0;	 obj[index].ny = 0;	obj[index].nz = 0;
				obj[index+1].nx = 0;  obj[index+1].ny = 0;	obj[index+1].nz = 0;
				obj[index+2].nx = 0;  obj[index+2].ny = 0;	obj[index+2].nz = 0;
				
				obj[index].u = 0;	 obj[index].v = 0;
				obj[index+1].u = 0;  obj[index+1].v = 0;
				obj[index+2].u = 0;  obj[index+2].v = 0;
#ifndef NO_3D_TEXCOORDS
				obj[index+0].w = 0;
				obj[index+1].w = 0;
				obj[index+2].w = 0;
#endif
/*
				obj[index].color = RGBA( 255, 255, 255, 255 );
				obj[index+1].color = RGBA( 255, 255, 255, 255 );
				obj[index+2].color = RGBA( 255, 255, 255, 255 );
*/
				SET_COLOR_RGBA(obj[index].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+1].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+2].color, 255, 255, 255, 255);
		//	}else if((textures == true) && (normals == false)){
			}else if(slash == 3){
				sscanf(buffer, "f %d/%d %d/%d %d/%d", &v1, &vt1, &v2, &vt2, &v3, &vt3);
				v1--; v2--;v3--;
				vt1--; vt2--;vt3--;
				obj[index].x = Verts[v1].x;	obj[index].y = Verts[v1].y;	obj[index].z = Verts[v1].z;
				obj[index+1].x = Verts[v2].x;	obj[index+1].y = Verts[v2].y;	obj[index+1].z = Verts[v2].z;
				obj[index+2].x = Verts[v3].x;	obj[index+2].y = Verts[v3].y;	obj[index+2].z = Verts[v3].z;
				
				obj[index].nx = 0;	 obj[index].ny = 0;	obj[index].nz = 0;
				obj[index+1].nx = 0;  obj[index+1].ny = 0;	obj[index+1].nz = 0;
				obj[index+2].nx = 0;  obj[index+2].ny = 0;	obj[index+2].nz = 0;

				obj[index+1].u = TVerts[vt1].x;		obj[index+1].v = TVerts[vt1].y;
				obj[index+2].u = TVerts[vt2].x;		obj[index+2].v = TVerts[vt2].y;
				obj[index].u =	TVerts[vt3].x;		obj[index].v = TVerts[vt3].y;

#ifndef NO_3D_TEXCOORDS
				obj[index+1].w = TVerts[vt1].z;
				obj[index+2].w = TVerts[vt2].z;
				obj[index].w = TVerts[vt3].z;
#endif

				SET_COLOR_RGBA(obj[index].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+1].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+2].color, 255, 255, 255, 255);
				
				//	}else if((textures == false) && (normals == true)){
			}else if(slash == 6 && buffer[(t_ptr)strchr(buffer,'/')-(t_ptr)buffer+1]=='/'){
				sscanf(buffer, "f %d//%d %d//%d %d//%d", &v1, &vn1, &v2, &vn2, &v3, &vn3);
				v1--; v2--;v3--;
				vn1--; vn2--;vn3--;
				obj[index].x = Verts[v1].x;	obj[index].y = Verts[v1].y;	obj[index].z = Verts[v1].z;
				obj[index+1].x = Verts[v2].x;	obj[index+1].y = Verts[v2].y;	obj[index+1].z = Verts[v2].z;
				obj[index+2].x = Verts[v3].x;	obj[index+2].y = Verts[v3].y;	obj[index+2].z = Verts[v3].z;

				obj[index].nx = NVerts[vn1].x;	 obj[index].ny = NVerts[vn1].y;	obj[index].nz = NVerts[vn1].z;
				obj[index+1].nx = NVerts[vn2].x;  obj[index+1].ny = NVerts[vn2].y;	obj[index+1].nz = NVerts[vn2].z;
				obj[index+2].nx = NVerts[vn3].x;  obj[index+2].ny = NVerts[vn3].y;	obj[index+2].nz = NVerts[vn3].z;
				
				obj[index].u = 0;	 obj[index].v = 0;
				obj[index+1].u = 0;  obj[index+1].v = 0;
				obj[index+2].u = 0;  obj[index+2].v = 0;
#ifndef NO_3D_TEXCOORDS
				obj[index+0].w = 0;
				obj[index+1].w = 0;
				obj[index+2].w = 0;
#endif

				SET_COLOR_RGBA(obj[index].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+1].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+2].color, 255, 255, 255, 255);
				
				//	}else if((textures == true) && (normals == true)){
			}else if(slash == 6){
				sscanf(buffer, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
				v1--; v2--;v3--;
				vt1--; vt2--;vt3--;
				vn1--; vn2--;vn3--;

				obj[index].x = Verts[v1].x;	obj[index].y = Verts[v1].y;	obj[index].z = Verts[v1].z;
				obj[index+1].x = Verts[v2].x;	obj[index+1].y = Verts[v2].y;	obj[index+1].z = Verts[v2].z;
				obj[index+2].x = Verts[v3].x;	obj[index+2].y = Verts[v3].y;	obj[index+2].z = Verts[v3].z;

				obj[index].nx = NVerts[vn1].x;	 obj[index].ny = NVerts[vn1].y;	obj[index].nz = NVerts[vn1].z;
				obj[index+1].nx = NVerts[vn2].x;  obj[index+1].ny = NVerts[vn2].y;	obj[index+1].nz = NVerts[vn2].z;
				obj[index+2].nx = NVerts[vn3].x;  obj[index+2].ny = NVerts[vn3].y;	obj[index+2].nz = NVerts[vn3].z;

				obj[index].u = TVerts[vt1].x;
				obj[index].v = -TVerts[vt1].y;
				obj[index+1].u = TVerts[vt2].x;
				obj[index+1].v = -TVerts[vt2].y;
				obj[index+2].u = TVerts[vt3].x;
				obj[index+2].v = -TVerts[vt3].y;

#ifndef NO_3D_TEXCOORDS
			//	obj[index].w = -TVerts[vt1].z;
				obj[index].w = TVerts[vt1].z;
			//	obj[index+1].w = -TVerts[vt2].z;
				obj[index+1].w = TVerts[vt2].z;
			//	obj[index+2].w = -TVerts[vt3].z;
				obj[index+2].w = TVerts[vt3].z;
#endif
			}
			if(colorI){
				SET_COLOR(obj[index].color, colorI);
				SET_COLOR(obj[index+1].color, colorI);
				SET_COLOR(obj[index+2].color, colorI);
			}else{
				SET_COLOR_RGBA(obj[index].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+1].color, 255, 255, 255, 255);
				SET_COLOR_RGBA(obj[index+2].color, 255, 255, 255, 255);
			}
}

ge_Object* geLoadObj(const char* file, const char* obj_name){
	ge_Vertex* obj;

	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ|GE_FILE_MODE_BINARY);
	if(!fp){
		return NULL;
	}
	char pth[2048] = "";
	if(strstr(file, "/")){
		strncpy(pth, file, strlen(file));
		int A = 0;
		for(A=strlen(pth); A>=0; A--){
			if(pth[A]=='/')break;
			pth[A] = 0x0;
		}
	}
	char tmp[2048] = "";
	char mtlFile[2048] = "";
//	char texFile[2048] = "";
	char buffer[256] = "";
	int nVerts = 0;
	int nTVerts = 0;
	int nNVerts = 0;
	int nTriangles = 0;
	
//	bool normals = false;
//	bool textures = false;
	bool materials = false;
	MTL* mtl = NULL;
	
	while(geFileGets(fp, buffer, 256)){
		if(strstr(buffer, "mtllib")){
			memcpy(tmp, buffer, strlen(buffer));
			sprintf(mtlFile, "%s", strstr(tmp, "mtllib")+7);
			*(strlastof(mtlFile,'.')+4) = 0x0;
			sprintf(tmp, "%s%s", pth, mtlFile);
			sprintf(mtlFile, "%s", tmp);
			mtl = LoadObjMtl(mtlFile);
			materials = true;
		}
		if(buffer[0] == 'v'){
			if(buffer[1] == ' '){
				nVerts++;
			}else if(buffer[1] == 't'){
				nTVerts++;
			//	textures = true;
			}else if(buffer[1] == 'n'){
				nNVerts++;
			//	normals = true;
			}
		}else if(buffer[0] == 'f'){
			nTriangles++;
		}
	}
	
	gePrintDebug(0x100, "plop 1\n");
	if((nVerts <= 0) || (nTriangles <= 0)){
		return NULL;
	}
	
	int size = sizeof(ge_Vertex) * 3 * nTriangles;
	obj = (ge_Vertex*)geMalloc(size);
	memset(obj, 0, size);
	gePrintDebug(0x100, "plop 2\n");
	
	ge_Vector3f* Verts = (ge_Vector3f*) geMalloc(sizeof(ge_Vector3f) * nVerts);
	ge_Vector3f* TVerts = (ge_Vector3f*) geMalloc(sizeof(ge_Vector3f) * nTVerts);
	ge_Vector3f* NVerts = (ge_Vector3f*) geMalloc(sizeof(ge_Vector3f) * nNVerts);
	
	gePrintDebug(0x100, "plop 3\n");
	int v = 0;
	int vn = 0;
	int vt = 0;
	geFileRewind(fp);
	while(geFileGets(fp, buffer, 256)){
		if(buffer[0] == 'v'){
			if(buffer[1] == ' '){
				sscanf(buffer, "v %f %f %f", &Verts[v].x, &Verts[v].y, &Verts[v].z);
				v++;
			}else if(buffer[1] == 't'){
				sscanf(buffer, "vt %f %f %f", &TVerts[vt].x, &TVerts[vt].y, &TVerts[vt].z);
				vt++;
				
			}else if(buffer[1] == 'n'){
				sscanf(buffer, "vn %f %f %f", &NVerts[vn].x, &NVerts[vn].y, &NVerts[vn].z);
				vn++;
			}
		}
	}
	gePrintDebug(0x100, "plop 4\n");

	char mtl_buf[256] = "";
//	bool usemtl = false;
	int index = 0;
	u32 colorI = RGBA(255, 255, 255, 255);
//	MTL* mtl = NULL;
	
	geFileRewind(fp);
	if(obj_name[0]){
		while(geFileGets(fp, buffer, 256)){
			if(buffer[0]=='g' || buffer[0]=='o'){
				if(strstr(buffer, obj_name))break;
			}
		}
	}
	gePrintDebug(0x100, "plop 5\n");

	ge_Object* OBJ = (ge_Object*)geMalloc(sizeof(ge_Object));
	memset(OBJ, 0, sizeof(ge_Object));
	strcpy(OBJ->name, obj_name);
	OBJ->verts = obj;
	OBJ->nVerts = nTriangles*3;

	char texFile[2048];
	while(geFileGets(fp, buffer, 256)){
		if((materials == true) && (strstr(buffer, "usemtl"))){
		//	usemtl = true;
			geGetParamString(buffer, "usemtl", mtl_buf, 256);
			MAT* mat = MtlGetMat(mtl, mtl_buf);
			colorI = fRGBA(mat->diffuse);
			if(mat->texture[0] && !OBJ->material.textures[0]){
				sprintf(texFile, "%s%s", pth, mat->texture);
				gePrintDebug(0x100, "texture: \"%s\"\n", texFile);
				OBJ->material.textures[0] = geAddTexture(geLoadImageOptions(texFile, 0));
				OBJ->material.textures_path[0] = (char*)malloc(strlen(texFile) + 1);
				strcpy(OBJ->material.textures_path[0], texFile);
				if(mat->texture_filter == GE_NEAREST){
					geTextureMode(OBJ->material.textures[0], GE_NEAREST);
				}
				gePrintDebug(0x100, "id: %d\n", OBJ->material.textures[0]->id);
			}
			if(mat->bump_map[0] && !OBJ->material.textures[1]){
				sprintf(texFile, "%s%s", pth, mat->bump_map);
				gePrintDebug(0x100, "bump_map: \"%s\" [\"%s\"]\n", texFile, mat->bump_map);
				OBJ->material.textures[1] = geAddTexture(geLoadImageOptions(texFile, GE_IMAGE_BUMP));
				OBJ->material.bump_force = mat->bump_force;
				OBJ->material.textures_path[1] = (char*)malloc(strlen(texFile) + 1);
				strcpy(OBJ->material.textures_path[1], texFile);
				gePrintDebug(0x100, "id: %d\n", OBJ->material.textures[1]->id);
			}
			if(mat->dudv_map[0] && !OBJ->material.textures[2]){
				sprintf(texFile, "%s%s", pth, mat->dudv_map);
				gePrintDebug(0x100, "dudv_map: \"%s\" [\"%s\"]\n", texFile, mat->dudv_map);
				OBJ->material.textures[2] = geAddTexture(geLoadImageOptions(texFile, GE_IMAGE_NO_MIPMAPS));
				OBJ->material.textures_path[2] = (char*)malloc(strlen(texFile) + 1);
				strcpy(OBJ->material.textures_path[2], texFile);
				gePrintDebug(0x100, "id: %d\n", OBJ->material.textures[2]->id);
			}
		}else
		if(buffer[0] == 'f'){
			getData(buffer, obj, index, Verts, TVerts, NVerts, colorI);
			index += 3;
		}else if((obj_name[0]) && (index>0)){
			break;
		}
	}
	gePrintDebug(0x100, "plop 6\n");
	geFree(Verts);
	geFree(TVerts);
	geFree(NVerts);
	geFileClose(fp);
	gePrintDebug(0x100, "plop 7\n");
/*
//	OBJ->lighted = normals_obj;
	OBJ->material.textures[0] = NULL;
	if(usemtl){
		//Ambient (non-lighted)
		OBJ->material.ambient = colorByMTL(mtlFile, mtl_buf, "Ka");
		//diffuse (lighted)
		OBJ->material.diffuse = colorByMTL(mtlFile, mtl_buf, "Kd");
		//specular (metal effect)
		OBJ->material.specular = colorByMTL(mtlFile, mtl_buf, "Ks");
	}

	char path[1024] = "";
	if(texFile[0]){
		strcpy(path, file);
		int A = 0;
		for(A=strlen(path); A>=0; A--){
			if(path[A]=='/')break;
			path[A] = 0x0;
		}
		sprintf(path, "%s%s", path, texFile);
		OBJ->material.textures[0] = geLoadImage(path);
		if(OBJ->material.textures[0]){
			OBJ->material.textures[0] = geAddTexture(OBJ->material.textures[0]);
		}
	}
*/
	CloseObjMtl(mtl);
	return OBJ;
}

int geObjsCountInFile(const char* file){
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ | GE_FILE_MODE_BINARY);
	if(!fp)return 0;

	int nObjs = 0;
	char buffer[1024] = "";

	while(geFileGets(fp, buffer, 1024)){
		if(buffer[0]=='g' || buffer[0]=='o'){
			nObjs++;
		}
	}

	geFileClose(fp);
	return nObjs;
}

void geLoadObjListMulti(const char* file, const char** obj_list, int count, ge_Object** objs){
	char pth[2048] = "";
	if(strstr(file, "/")){
		strncpy(pth, file, strlen(file));
		int A = 0;
		for(A=strlen(pth); A>=0; A--){
			if(pth[A]=='/')break;
			pth[A] = 0x0;
		}
	}
	gePrintDebug(0x100, "pth: \"%s\"\n", pth);

	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ|GE_FILE_MODE_BINARY);
	if(!fp)return;

	char buffer[2048] = "";
	char tmp[2048] = "";
	char name[2048] = "";
	char mtlFile[2048] = "";
	char texFile[2048] = "";
	int i = 0;
	int start = 0;
	int new_obj = 0;
	int nVerts = 0;
	int nTVerts = 0;
	int nNVerts = 0;
	int nTriangles = 0;
	
//	bool normals = false;
//	bool textures = false;
//	bool materials = false;
	MTL* mtl = NULL;
	
	while(geFileGets(fp, buffer, 256)){
		if(strstr(buffer, "mtllib")){
			memcpy(tmp, buffer, strlen(buffer));
			sprintf(mtlFile, "%s", strstr(tmp, "mtllib")+7);
			*(strlastof(mtlFile,'.')+4) = 0x0;
			sprintf(tmp, "%s%s", pth, mtlFile);
			sprintf(mtlFile, "%s", tmp);
			mtl = LoadObjMtl(mtlFile);
		//	materials = true;
		}
		if(buffer[0] == 'v'){
			if(buffer[1] == ' '){
				nVerts++;
			}else if(buffer[1] == 't'){
				nTVerts++;
			//	textures = true;
			}else if(buffer[1] == 'n'){
				nNVerts++;
			//	normals = true;
			}
		}else if(buffer[0] == 'f'){
			nTriangles++;
		}
	}
	
	if((nVerts <= 0) || (nTriangles <= 0)){
		return;
	}
	
	int size = sizeof(ge_Vertex) * 3 * nTriangles;
	ge_Vertex* obj = (ge_Vertex*)geMalloc(size);
	memset(obj, 0, size);
	
	ge_Vector3f* Verts = (ge_Vector3f*) geMalloc(sizeof(ge_Vector3f) * nVerts);
	ge_Vector3f* TVerts = (ge_Vector3f*) geMalloc(sizeof(ge_Vector3f) * nTVerts);
	ge_Vector3f* NVerts = (ge_Vector3f*) geMalloc(sizeof(ge_Vector3f) * nNVerts);
	
	nVerts = 0;
	u32 colorI = 0xFFFFFFFF;
	int index = 0;
	int v = 0;
	int vn = 0;
	int vt = 0;
	geFileRewind(fp);
	while(geFileGets(fp, buffer, 256)){
		if(buffer[0] == 'v'){
			if(buffer[1] == ' '){
				sscanf(buffer, "v %f %f %f", &Verts[v].x, &Verts[v].y, &Verts[v].z);
				v++;
			}else if(buffer[1] == 't'){
				sscanf(buffer, "vt %f %f %f", &TVerts[vt].x, &TVerts[vt].y, &TVerts[vt].z);
				vt++;
				
			}else if(buffer[1] == 'n'){
				sscanf(buffer, "vn %f %f %f", &NVerts[vn].x, &NVerts[vn].y, &NVerts[vn].z);
				vn++;
			}
		}
	}
	geFileRewind(fp);
	while(geFileGets(fp, buffer, 2048)){
loop:
		new_obj = 0;
		if(buffer[0]=='g' || buffer[0]=='o'){
			char o[2] = { buffer[0], 0x0 };
			geGetParamString(buffer, o, name, 2048);
			printf("===============>\"%s\"\n", buffer);
			if( (count && findInList(obj_list,count,name)) || (count==0) ){
				gePrintDebug(0x100, "object: \"%s\" (%d)\n", name, i);
				memset(objs[i], 0, sizeof(ge_Object));
				strncpy(objs[i]->name, name, 64);
				objs[i]->vert_start = start;
				geObjectMatrixIdentity(objs[i]);
			
				nVerts = 0;
				colorI = 0xFFFFFFFF;
				while(geFileGets(fp, buffer, 1024)){
					if(buffer[0]=='g' || buffer[0]=='o'){
						new_obj = 1;
						break;
					}
					if(strstr(buffer, "usemtl")){
						memset(tmp, 0, 2048);
						geGetParamString(buffer, "usemtl", tmp, 2048);
						MAT* mat = MtlGetMat(mtl, tmp);
						colorI = fRGBA(mat->diffuse);
						SET_COLOR(objs[i]->material.ambient, fRGBA(mat->ambient));
						SET_COLOR(objs[i]->material.diffuse, fRGBA(mat->diffuse));
						SET_COLOR(objs[i]->material.specular, fRGBA(mat->specular));
						if(mat->texture[0] && !objs[i]->material.textures[0]){
							sprintf(texFile, "%s%s", pth, mat->texture);
							gePrintDebug(0x100, "texture: \"%s\"\n", texFile);
							objs[i]->material.textures[0] = geAddTexture(geLoadImageOptions(texFile, 0));
							objs[i]->material.textures_path[0] = (char*)malloc(strlen(texFile) + 1);
							strcpy(objs[i]->material.textures_path[0], texFile);
							gePrintDebug(0x100, "id: %d\n", objs[i]->material.textures[0]->id);
						}
						if(mat->bump_map[0] && !objs[i]->material.textures[1]){
							sprintf(texFile, "%s%s", pth, mat->bump_map);
							gePrintDebug(0x100, "bump_map: \"%s\" [\"%s\"]\n", texFile, mat->bump_map);
							objs[i]->material.textures[1] = geAddTexture(geLoadImageOptions(texFile, GE_IMAGE_BUMP));
							objs[i]->material.bump_force = mat->bump_force;
							objs[i]->material.textures_path[1] = (char*)malloc(strlen(texFile) + 1);
							strcpy(objs[i]->material.textures_path[1], texFile);
							gePrintDebug(0x100, "id: %d\n", objs[i]->material.textures[1]->id);
						}
						if(mat->dudv_map[0] && !objs[i]->material.textures[2]){
							sprintf(texFile, "%s%s", pth, mat->dudv_map);
							gePrintDebug(0x100, "dudv_map: \"%s\" [\"%s\"]\n", texFile, mat->dudv_map);
							objs[i]->material.textures[2] = geAddTexture(geLoadImageOptions(texFile, GE_IMAGE_NO_MIPMAPS));
							objs[i]->material.textures_path[2] = (char*)malloc(strlen(texFile) + 1);
							strcpy(objs[i]->material.textures_path[2], texFile);
							gePrintDebug(0x100, "id: %d\n", objs[i]->material.textures[2]->id);
						}
					}
					if(buffer[0] == 'f'){
						getData(buffer, obj, index, Verts, TVerts, NVerts, colorI);
						index += 3;
					}
					if(buffer[0] == 'f'){
						nVerts += 3;
					}
				}
//				gePrintDebug(0x100, "start: %d\t", start);
//				gePrintDebug(0x100, "nVerts: %d\n\n", nVerts);
				objs[i]->nVerts = nVerts;
				objs[i]->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*objs[i]->nVerts);
				memcpy(objs[i]->verts, &obj[start], sizeof(ge_Vertex)*objs[i]->nVerts);
				i++;

				start += nVerts;
			}
		}
		if(new_obj){
			goto loop;
		}
	}
	
	gePrintDebug(0x100, "geLoadObjListMulti 2\n");
	CloseObjMtl(mtl);
	gePrintDebug(0x100, "geLoadObjListMulti 3\n");
	geFree(Verts);
	geFree(TVerts);
	geFree(NVerts);
	gePrintDebug(0x100, "geLoadObjListMulti 4\n");
	geFree(obj);
	gePrintDebug(0x100, "geLoadObjListMulti 5\n");
	geFileClose(fp);
	gePrintDebug(0x100, "Ok for %d objects\n", i);
}

int CheckNumSlash(const char* buffer){
	int i=0, j=0;
	for(i=0;i<strlen(buffer);i++){
		if(buffer[i]=='/')j++;
	}
	return j;
}

char* strlastof(const char* buffer, char last_c){
	int i = 0;
	int len = strlen(buffer);
	int last = 0;
	for(i=0; i<len; i++){
		if(buffer[i]==last_c)last=i;
	}
	return (char*)buffer+last;
}
