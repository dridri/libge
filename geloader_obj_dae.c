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

//#define gePrintDebug(...) ;

bool findInList(const char** list, int count, const char* w);
#define CHECK_EXIT(name) if(strstr(buf, name)){ break; }

#define DAE_ID_LEN 128
#define DAE_NAME_LEN 64
#define DAE_SEMANTIC_LEN 64
#define DAE_SOURCE_LEN 128
#define DAE_TYPE_LEN 64
#define DAE_FILE_LEN 512

typedef struct DAE_array {
	char id[DAE_ID_LEN];
	int count;
	float* data;
} DAE_array;

typedef struct DAE_Name_array {
	char id[DAE_ID_LEN];
	int count;
	char** data;
} DAE_Name_array;

typedef struct DAE_input {
	char semantic[DAE_SEMANTIC_LEN];
	int offset;
	int set;
	char source[DAE_SOURCE_LEN];
} DAE_input;

typedef struct DAE_param {
	char name[DAE_NAME_LEN];
	char type[DAE_TYPE_LEN];
} DAE_param;

typedef struct DAE_accessor {
	char source[DAE_SOURCE_LEN];
	int count;
	int stride;
	int nParams;
	DAE_param* params;
} DAE_accessor;

typedef struct DAE_technique_common {
	DAE_accessor accessor;
} DAE_technique_common;

typedef struct DAE_source {
	char id[DAE_ID_LEN];
	DAE_array array;
	DAE_Name_array Name_array;
	DAE_technique_common technique;
} DAE_source;

typedef struct DAE_vertices {
	char id[DAE_ID_LEN];
	int nInputs;
	DAE_input* inputs;
} DAE_vertices;

typedef struct DAE_triangles {
	int count;
	char material[DAE_ID_LEN];
	int nInputs;
	DAE_input* inputs;
	int nP;
	int* p;
} DAE_triangles;

typedef struct DAE_geometry {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	int nSources;
	DAE_source* sources;
	int nVertices;
	DAE_vertices* vertices;
	int nTriangles;
	DAE_triangles* triangles;
} DAE_geometry;




typedef struct DAE_matrix {
	char sid[DAE_ID_LEN];
	float array[16];
} DAE_matrix;

typedef struct DAE_node DAE_node;
struct DAE_node {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	char instance[DAE_ID_LEN];
	int nMatrices;
	DAE_matrix* matrices;
	int nNodes;
	DAE_node* nodes;
};

typedef struct DAE_visual {
	int nNodes;
	DAE_node* nodes;
} DAE_visual;



typedef struct DAE_material {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	char instance[DAE_ID_LEN];
} DAE_material;




typedef struct DAE_float {
	char sid[DAE_ID_LEN];
	float value;
} DAE_float;

typedef struct DAE_color {
	char sid[DAE_ID_LEN];
	float value[4];
} DAE_color;

typedef struct DAE_texture {
	char texture[DAE_ID_LEN];
	char texcoord[DAE_ID_LEN];
} DAE_texture;

typedef struct DAE_phong {
	DAE_color color;
	DAE_float value;
	DAE_texture texture;
} DAE_phong;

typedef struct DAE_technique {
	char sid[DAE_ID_LEN];
	DAE_phong emission;
	DAE_phong ambient;
	DAE_phong diffuse;
	DAE_phong specular;
	DAE_phong shininess;
	DAE_phong reflective;
	DAE_phong reflectivity;
	DAE_phong transparent;
	DAE_phong transparency;
} DAE_technique;

typedef struct DAE_effect {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	DAE_technique technique;
} DAE_effect;




typedef struct DAE_image {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	char init_from[DAE_FILE_LEN];
	bool mipmaps;
} DAE_image;





typedef struct DAE_sampler {
	char id[DAE_ID_LEN];
	int nInputs;
	DAE_input* inputs;
} DAE_sampler;

typedef struct DAE_channel {
	char source[DAE_ID_LEN];
	char target[DAE_ID_LEN];
} DAE_channel;

typedef struct DAE_animation {
	char id[DAE_ID_LEN];
	char name[DAE_ID_LEN];
	int nSources;
	DAE_source* sources;
	DAE_sampler sampler;
	DAE_channel channel;
} DAE_animation;




typedef struct DAE {
	int nAnimations;
	DAE_animation* animations;
	int nImages;
	DAE_image* images;
	int nMaterials;
	DAE_material* materials;
	int nEffects;
	DAE_effect* effects;
	int nGeometries;
	DAE_geometry* geometries;
	int nVisual_scenes;
	DAE_visual* visual_scenes;
} DAE;


DAE_animation* DaeLoadAnimations(ge_File* fp, int* _nAnims);
void ComputeAnimations(DAE_animation* anims, int nAnims, ge_Animator* animator);



int GetXml(ge_File* fp, char* buf, int max){
	int ret = geFileGets(fp, buf, max);
	int ret0 = ret;
	while(buf[0] == ' ' || buf[0] == '\t'){
		strcpy(buf, &buf[1]);
		ret--;
	}
	int e = (strchr(buf, '>') - buf);
	int f = (strchr(&buf[1], '<') - buf);
	if(f < e){
		if(f > 0){
			if(ret0 == max-1){
				geFileSeek(fp, -(ret-e), GE_FILE_SEEK_CUR);
			}else{
				geFileSeek(fp, -(ret-e)+1, GE_FILE_SEEK_CUR);
			}
			strchr(buf, '<')[0] = 0x0;
			ret = strlen(buf);
		}
	}else
	if(e > 0 && strchr(buf, '>')){
		if(ret0 == max-1){
			geFileSeek(fp, -(ret-e), GE_FILE_SEEK_CUR);
		}else{
			geFileSeek(fp, -(ret-e)+1, GE_FILE_SEEK_CUR);
		}
		strchr(buf, '>')[1] = 0x0;
		ret = strlen(buf);
	}
	if(strchr(buf, '\n')){
		strchr(buf, '\n')[0] = 0x0;
	}
//	gePrintDebug(0x100, "    BUF: \"%s\"\n", buf);
	return ret;
}

DAE_image* DaeLoadImages(ge_File* fp, int* _nImages){
	char buf[4096] = "";

	int nImages = 0;
	DAE_image* images = NULL;

	while(GetXml(fp, buf, 4096)){
		gePrintDebug(0x100, "DaeLoadImages BUF \"%s\"\n", buf);
		CHECK_EXIT("</library_images>");
		if(strstr(buf, "<image")){
			images = (DAE_image*)geRealloc(images, sizeof(DAE_image)*(nImages+1));
			memset(&images[nImages], 0x0, sizeof(DAE_image));
			geGetParamString(buf, "id", images[nImages].id, DAE_ID_LEN);
			geGetParamString(buf, "name", images[nImages].name, DAE_ID_LEN);
			while(GetXml(fp, buf, 4096)){
				gePrintDebug(0x100, "DaeLoadImages BUF2 \"%s\"\n", buf);
				CHECK_EXIT("</image>");
				if(strstr(buf, "<init_from>")){
					GetXml(fp, buf, 4096);
					gePrintDebug(0x100, "DaeLoadImages BUF3 \"%s\"\n", buf);
					strncpy(images[nImages].init_from, buf, DAE_FILE_LEN);
				}
				if(strstr(buf, "<extra>")){
					while(GetXml(fp, buf, 4096)){
						CHECK_EXIT("</extra>");
						if(strstr(buf, "<generate_mipmap>")){
							GetXml(fp, buf, 6);
							if(!strncmp(buf, "true", 4)){
								images[nImages].mipmaps = true;
							}else if(!strncmp(buf, "false", 5)){
								images[nImages].mipmaps = false;
							}
						}
					}
				}
			}
			nImages++;
		}
	}

	*_nImages = nImages;
	return images;
}

DAE_material* DaeLoadMaterials(ge_File* fp, int* _nMaterials){
	char buf[4096] = "";

	int nMaterials = 0;
	DAE_material* materials = NULL;

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</library_materials>");
		if(strstr(buf, "<material")){
			materials = (DAE_material*)geRealloc(materials, sizeof(DAE_material)*(nMaterials+1));
			memset(&materials[nMaterials], 0x0, sizeof(DAE_material));
			geGetParamString(buf, "id", materials[nMaterials].id, DAE_ID_LEN);
			geGetParamString(buf, "name", materials[nMaterials].name, DAE_ID_LEN);
			gePrintDebug(0x100, "  Material %d id=\"%s\" name=\"%s\"\n", nMaterials, materials[nMaterials].id, materials[nMaterials].name);
			while(GetXml(fp, buf, 4096)){
				CHECK_EXIT("</material>");
				if(strstr(buf, "<instance_effect")){
					geGetParamString(buf, "url", materials[nMaterials].instance, DAE_ID_LEN);
					if(materials[nMaterials].instance[0] == '#'){
						strncpy(materials[nMaterials].instance, &materials[nMaterials].instance[1], DAE_SOURCE_LEN-1);
					}
					gePrintDebug(0x100, "    instance_effect url=\"%s\"\n", materials[nMaterials].instance);
				}
			}
			nMaterials++;
		}
	}

	*_nMaterials = nMaterials;
	return materials;
}

void LoadPhongEntry(ge_File* fp, char* buf, const char* entry, DAE_phong* phong){
	char start[64], end[64];
	sprintf(start, "<%s>", entry);
	sprintf(end, "</%s>", entry);
	if(!strstr(buf, start))return;
	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT(end);
		if(strstr(buf, "<color")){
			geGetParamString(buf, "sid", phong->color.sid, DAE_ID_LEN);
			geGetParamFloatMulti(buf, ">", phong->color.value, 4);
		}
		if(strstr(buf, "<float")){
			geGetParamString(buf, "sid", phong->value.sid, DAE_ID_LEN);
			geGetParamFloat(buf, ">", &phong->value.value);
		}
		if(strstr(buf, "<texture")){
			geGetParamString(strstr(buf, "<texture") + 8, "texture", phong->texture.texture, DAE_ID_LEN);
			geGetParamString(buf, "texcoord", phong->texture.texcoord, DAE_ID_LEN);
			gePrintDebug(0, "PhongEntry '%s' has texture '%s'\n", entry, phong->texture.texture);
		}
	}
}

DAE_effect* DaeLoadEffects(ge_File* fp, int* _nEffects){
	char buf[4096] = "";

	int nEffects = 0;
	DAE_effect* effects = NULL;

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</library_effects>");
		if(strstr(buf, "<effect")){
			effects = (DAE_effect*)geRealloc(effects, sizeof(DAE_effect)*(nEffects+1));
			memset(&effects[nEffects], 0x0, sizeof(DAE_effect));
			geGetParamString(buf, "id", effects[nEffects].id, DAE_ID_LEN);
			SET_COLOR_fRGBA(effects[nEffects].technique.emission.color.value, 1.0, 1.0, 1.0, 1.0);
			SET_COLOR_fRGBA(effects[nEffects].technique.ambient.color.value, 1.0, 1.0, 1.0, 1.0);
			SET_COLOR_fRGBA(effects[nEffects].technique.diffuse.color.value, 1.0, 1.0, 1.0, 1.0);
			SET_COLOR_fRGBA(effects[nEffects].technique.specular.color.value, 1.0, 1.0, 1.0, 1.0);
			effects[nEffects].technique.shininess.value.value = 1.0;
			SET_COLOR_fRGBA(effects[nEffects].technique.reflective.color.value, 1.0, 1.0, 1.0, 1.0);
			effects[nEffects].technique.reflectivity.value.value = 1.0;
			SET_COLOR_fRGBA(effects[nEffects].technique.transparent.color.value, 1.0, 1.0, 1.0, 1.0);
			effects[nEffects].technique.transparency.value.value = 1.0;
			while(GetXml(fp, buf, 4096)){
				CHECK_EXIT("</effect>");
				LoadPhongEntry(fp, buf, "emission", &effects[nEffects].technique.emission);
				LoadPhongEntry(fp, buf, "ambient", &effects[nEffects].technique.ambient);
				LoadPhongEntry(fp, buf, "diffuse", &effects[nEffects].technique.diffuse);
				LoadPhongEntry(fp, buf, "specular", &effects[nEffects].technique.specular);
				LoadPhongEntry(fp, buf, "shininess", &effects[nEffects].technique.shininess);
				LoadPhongEntry(fp, buf, "reflective", &effects[nEffects].technique.reflective);
				LoadPhongEntry(fp, buf, "reflectivity", &effects[nEffects].technique.reflectivity);
				LoadPhongEntry(fp, buf, "transparent", &effects[nEffects].technique.transparent);
				LoadPhongEntry(fp, buf, "transparency", &effects[nEffects].technique.transparency);
			}
			gePrintDebug(0x100, "  Effect %d :\n", nEffects);
#define print_array(text,v,n) { gePrintDebug(0x103, "%s",text); int _i; for(_i=0;_i<n;_i++){ gePrintDebug(0x103, " %f",v[_i]); } gePrintDebug(0x103, "\n"); }
			print_array("    emission :", effects[nEffects].technique.emission.color.value, 4);
			print_array("    ambient :", effects[nEffects].technique.ambient.color.value, 4);
			print_array("    diffuse :", effects[nEffects].technique.diffuse.color.value, 4);
			print_array("    specular :", effects[nEffects].technique.specular.color.value, 4);
			print_array("    shininess :", (&effects[nEffects].technique.shininess.value.value), 1);
			print_array("    reflective :", effects[nEffects].technique.reflective.color.value, 4);
			print_array("    reflectivity :", (&effects[nEffects].technique.reflectivity.value.value), 1);
			print_array("    transparent :", effects[nEffects].technique.transparent.color.value, 4);
			print_array("    transparency : ", (&effects[nEffects].technique.transparency.value.value), 1);
			nEffects++;
		}
	}

	*_nEffects = nEffects;
	return effects;
}

int DaeReadArrayFloat(ge_File* fp, const char* start, const char* end, float* data, int nData){
	int len = 128 + nData * strlen("0000000000.0000000000 ") + strlen(end);
	char* buf2 = (char*)geMalloc(len);
	len = geFileRead(fp, buf2, len);
	gePrintDebug(0x100, "BUF2 [%s]: \n%s\n\n", start, buf2);
	nData = geGetParamFloatMulti(buf2, start, data, nData);
	gePrintDebug(0x100, "Ret : %d\n", nData);
	int pos = strstr(buf2, end) - buf2;
	geFileSeek(fp, -(len-pos)+strlen(end), GE_FILE_SEEK_CUR);
	geFree(buf2);
/*
	gePrintDebug(0x100, "Float ARRAY [%d]: %s", nData, start);
	int i;
	for(i=0; i<(nData-1); i++){
		gePrintDebug(0x100, "%f ", data[i]);
	}
	gePrintDebug(0x100, "%f%s", data[nData-1], end);
*/
	return nData;
}

int DaeReadArrayInt(ge_File* fp, const char* start, const char* end, int* data, int nData){
	int len = nData * strlen("00000000 ") + strlen(end);
	char* buf2 = (char*)geMalloc(len);
	len = geFileRead(fp, buf2, len);
	nData = geGetParamIntMulti(buf2, start, data, nData);
	int pos = strstr(buf2, end) - buf2;
	geFileSeek(fp, -(len-pos)+strlen(end), GE_FILE_SEEK_CUR);
	geFree(buf2);
/*
	gePrintDebug(0x100, "Integer ARRAY: %s", start);
	int i;
	for(i=0; i<(nData-1); i++){
		gePrintDebug(0x100, "%d ", data[i]);
	}
	gePrintDebug(0x100, "%d%s\n", data[nData-1], end);
*/
	return nData;
}

int DaeReadArrayName(ge_File* fp, const char* start, const char* end, char** data, int nData){
	int len = nData * strlen("01234567890123456789 ") + strlen(end);
	char* buf2 = (char*)geMalloc(len);
	len = geFileRead(fp, buf2, len);
	char* start2 = strstr(buf2, start);
	start2 += strlen(start);
	while((*start2==' ' || *start2=='\n') && *start2!=0x0)start2++;
	nData = geGetParamStringMulti(start2, " \n<", data, 64, nData);
	int pos = strstr(buf2, end) - buf2;
	geFileSeek(fp, -(len-pos)+strlen(end), GE_FILE_SEEK_CUR);
	geFree(buf2);
/*
	gePrintDebug(0x100, "Name ARRAY: %s", start);
	int i;
	for(i=0; i<(nData-1); i++){
		gePrintDebug(0x100, "\"%s\" ", data[i]);
	}
	gePrintDebug(0x100, "\"%s\"%s\n", data[nData-1], end);
*/
	return nData;
}

void DaeLoadInputs(ge_File* fp, const char* out, DAE_input** _inputs, int* _nInputs){
	char buf[4096] = "";
	int nInputs = 0;
	DAE_input* inputs = NULL;

	while(GetXml(fp, buf, 4096)){
	//	gePrintDebug(0x100, "DaeLoadInputs BUF : \"%s\"\n", buf);
		if(strstr(buf, out)){
			break;
		}
		if(strstr(buf, "<input")){
			inputs = (DAE_input*)geRealloc(inputs, sizeof(DAE_input)*(nInputs+1));
			memset(&inputs[nInputs], 0x0, sizeof(DAE_input));
			geGetParamString(buf, "semantic", inputs[nInputs].semantic, DAE_SEMANTIC_LEN);
			geGetParamInt(buf, "offset", &inputs[nInputs].offset);
			geGetParamInt(buf, " set", &inputs[nInputs].set);
			geGetParamString(buf, "source", inputs[nInputs].source, DAE_SOURCE_LEN);
			if(inputs[nInputs].source[0] == '#'){
				strncpy(inputs[nInputs].source, &inputs[nInputs].source[1], DAE_SOURCE_LEN-1);
			}
			gePrintDebug(0x100, "      Input semantic=\"%s\" offset=%d set=%d source=\"%s\"\n", inputs[nInputs].semantic, inputs[nInputs].offset, inputs[nInputs].set, inputs[nInputs].source);
			nInputs++;
		}
	}
	*_nInputs = nInputs;
	*_inputs = inputs;
	geFileSeek(fp, -(strlen(buf)), GE_FILE_SEEK_CUR);
}

void DaeLoadSource(ge_File* fp, char* buf, DAE_source* source){
	int i = 0;
	geGetParamString(buf, "id", source->id, DAE_ID_LEN);
	gePrintDebug(0x100, "    Source id=\"%s\"\n", source->id);
	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</source>");
		if(strstr(buf, "<float_array")){
			geGetParamString(buf, "id", source->array.id, DAE_ID_LEN);
			geGetParamInt(buf, "count", &source->array.count);
			gePrintDebug(0x100, "      Array id=\"%s\" count=%d\n", source->array.id, source->array.count);
				gePrintDebug(0x100, "plop 1\n");
				gePrintDebug(0x100, "plop 1.1 : %d\n", source->array.count);
			if(source->array.count > 0){
				gePrintDebug(0x100, "plop 2\n");
				source->array.data = (float*)geMalloc(sizeof(float)*source->array.count);
				geFileSeek(fp, -(strlen(buf)), GE_FILE_SEEK_CUR);
				DaeReadArrayFloat(fp, ">", "</float_array>", source->array.data, source->array.count);
			}
				gePrintDebug(0x100, "plop 3\n");
			gePrintDebug(0x100, "Array ok\n");
		}
		if(strstr(buf, "<Name_array")){
			geGetParamString(buf, "id", source->Name_array.id, DAE_ID_LEN);
			geGetParamInt(buf, "count", &source->Name_array.count);
			gePrintDebug(0x100, "      Array id=\"%s\" count=%d\n", source->Name_array.id, source->Name_array.count);
				gePrintDebug(0x100, "plop 1\n");
			if(source->array.count > 0){
				source->Name_array.data = (char**)geMalloc(sizeof(char*)*source->Name_array.count);
				for(i=0; i<source->Name_array.count; i++){
					source->Name_array.data[i] = (char*)geMalloc(sizeof(char)*DAE_SOURCE_LEN);
				}
					
				geFileSeek(fp, -(strlen(buf)), GE_FILE_SEEK_CUR);
				DaeReadArrayName(fp, ">", "</Name_array>", source->Name_array.data, source->Name_array.count);
			}
		}
		if(strstr(buf, "<technique_common")){
			gePrintDebug(0x100, "      Technique Common\n");
			while(GetXml(fp, buf, 4096)){
				CHECK_EXIT("</technique_common>");
				if(strstr(buf, "<accessor")){
					source->technique.accessor.params = NULL;
					source->technique.accessor.count = 0;
					source->technique.accessor.stride = 0;
					source->technique.accessor.nParams = 0;
					geGetParamString(buf, "source", source->technique.accessor.source, DAE_SOURCE_LEN);
					geGetParamInt(buf, "count", &source->technique.accessor.count);
					geGetParamInt(buf, "stride", &source->technique.accessor.stride);
					if(source->technique.accessor.source[0] == '#'){
						strcpy(source->technique.accessor.source, &source->technique.accessor.source[1]);
					}
					gePrintDebug(0x100, "        Accessor source=\"%s\" count=%d stride=%d\n", source->technique.accessor.source, source->technique.accessor.count, source->technique.accessor.stride);
					while(GetXml(fp, buf, 4096)){
						CHECK_EXIT("</accessor>");
						if(strstr(buf, "<param")){
							source->technique.accessor.params = (DAE_param*)geRealloc(source->technique.accessor.params, sizeof(DAE_param)*(source->technique.accessor.nParams+1));
							memset(&source->technique.accessor.params[source->technique.accessor.nParams], 0x0, sizeof(DAE_param));
							if(strstr(buf, "name=")){
								geGetParamString(buf, "name", source->technique.accessor.params[source->technique.accessor.nParams].name, DAE_NAME_LEN);
							}
							if(strstr(buf, "type=")){
								geGetParamString(buf, "type", source->technique.accessor.params[source->technique.accessor.nParams].type, DAE_TYPE_LEN);
							}
							gePrintDebug(0x100, "          Param name=\"%s\" type=\"%s\"\n", source->technique.accessor.params[source->technique.accessor.nParams].name, source->technique.accessor.params[source->technique.accessor.nParams].type);
							source->technique.accessor.nParams++;
						}
					}
				}
			}
		}
	}
/*
	if(!strcmp(source->id, "EscalierEnU01-lib-UV0")){
		while(1){
			geSleep(1);
		}
	}
*/
	gePrintDebug(0x100, "Load Source OK\n");
}

void DaeLoadGeometry(ge_File* fp, DAE_geometry* geom){
	char buf[4096] = "";
	int  i = 0;

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</geometry>");
		if(strstr(buf, "<source")){
			geom->sources = (DAE_source*)geRealloc(geom->sources, sizeof(DAE_source)*(geom->nSources+1));
			memset(&geom->sources[geom->nSources], 0x0, sizeof(DAE_source));
			DaeLoadSource(fp, buf, &geom->sources[geom->nSources]);
			geom->nSources++;
		}
		if(strstr(buf, "<vertices")){
			geom->vertices = (DAE_vertices*)geRealloc(geom->vertices, sizeof(DAE_vertices)*(geom->nVertices+1));
			memset(&geom->vertices[geom->nVertices], 0x0, sizeof(DAE_vertices));
			geGetParamString(buf, "id", geom->vertices[geom->nVertices].id, DAE_ID_LEN);
			gePrintDebug(0x100, "    Vertices %d id=\"%s\"\n", geom->nVertices, geom->vertices[geom->nVertices].id);
			DaeLoadInputs(fp, "</vertices>", &geom->vertices[geom->nVertices].inputs, &geom->vertices[geom->nVertices].nInputs);
			geom->nVertices++;
		}
		if(strstr(buf, "<triangles") || strstr(buf, "<polygons")){
			geom->triangles = (DAE_triangles*)geRealloc(geom->triangles, sizeof(DAE_triangles)*(geom->nTriangles+1));
			memset(&geom->triangles[geom->nTriangles], 0x0, sizeof(DAE_triangles));
		//	geGetParamString(buf, "count", tmp, 256);
			geGetParamInt(buf, "count", &geom->triangles[geom->nTriangles].count);
			geGetParamString(buf, "material", geom->triangles[geom->nTriangles].material, DAE_ID_LEN);
			gePrintDebug(0x100, "    Triangles count=%d material=\"%s\"\n", geom->triangles[geom->nTriangles].count, geom->triangles[geom->nTriangles].material);
			DaeLoadInputs(fp, "<p>", &geom->triangles[geom->nTriangles].inputs, &geom->triangles[geom->nTriangles].nInputs);
			geom->triangles[geom->nTriangles].nP = 3 * geom->triangles[geom->nTriangles].count * geom->triangles[geom->nTriangles].nInputs;
			geom->triangles[geom->nTriangles].p = (int*)geMalloc(sizeof(int) * geom->triangles[geom->nTriangles].nP);
			i = 0;
			while(GetXml(fp, buf, 4096)){
				CHECK_EXIT("</triangles>");
				CHECK_EXIT("</polygons>");
				if(strstr(buf, "<p>") && i < geom->triangles[geom->nTriangles].nP){
					geFileSeek(fp, -(strlen(buf)), GE_FILE_SEEK_CUR);
					i += DaeReadArrayInt(fp, "<p>", "</p>", &geom->triangles[geom->nTriangles].p[i], geom->triangles[geom->nTriangles].nP);
					/*
					for(i=0; i<geom->triangles[geom->nTriangles].nP; i+=9){
						gePrintDebug(0x100, "        %02d %02d %02d  %02d %02d %02d  %02d %02d %02d\n"
							, geom->triangles[geom->nTriangles].p[i+0], geom->triangles[geom->nTriangles].p[i+1], geom->triangles[geom->nTriangles].p[i+2]
							, geom->triangles[geom->nTriangles].p[i+3], geom->triangles[geom->nTriangles].p[i+4], geom->triangles[geom->nTriangles].p[i+5]
							, geom->triangles[geom->nTriangles].p[i+6], geom->triangles[geom->nTriangles].p[i+7], geom->triangles[geom->nTriangles].p[i+8]);
					}
					*/
				}
			}
			geom->nTriangles++;
		}
	}
}

DAE_geometry* DaeLoadGeometries(ge_File* fp, int* nGeoms){
	DAE_geometry* geoms = NULL;
	char buf[4096] = "";

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</library_geometries>");
		if(strstr(buf, "<geometry")){
			geoms = (DAE_geometry*)geRealloc(geoms, sizeof(DAE_geometry)*(*nGeoms+1));
			memset(&geoms[(*nGeoms)], 0x0, sizeof(DAE_geometry));
			geGetParamString(buf, "id", geoms[*nGeoms].id, DAE_ID_LEN);
			geGetParamString(buf, "name", geoms[*nGeoms].name, DAE_ID_LEN);
			gePrintDebug(0x100, "  Geometry %d id=\"%s\", name=\"%s\"\n", *nGeoms, geoms[*nGeoms].id, geoms[*nGeoms].name);
			DaeLoadGeometry(fp, &geoms[*nGeoms]);
			(*nGeoms)++;
		}
	}

	return geoms;
}

void DaeLoadNode(ge_File* fp, DAE_node* node){
	char buf[4096] = "";
	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</node>");
		if(strstr(buf, "<instance_geometry")){
			geGetParamString(buf, "url", node->instance, DAE_SEMANTIC_LEN);
			if(node->instance[0] == '#'){
				strncpy(node->instance, &node->instance[1], DAE_ID_LEN-1);
			}
			gePrintDebug(0x100, "    Instance url=\"%s\"\n", node->instance);
		}
		if(strstr(buf, "<matrix")){
			node->matrices = (DAE_matrix*)geRealloc(node->matrices, sizeof(DAE_matrix)*(node->nMatrices+1));
			memset(&node->matrices[node->nMatrices], 0x0, sizeof(DAE_matrix));
			geGetParamString(buf, "sid", node->matrices[node->nMatrices].sid, DAE_ID_LEN);
			geFileSeek(fp, -(strlen(buf)), GE_FILE_SEEK_CUR);
			DaeReadArrayFloat(fp, ">", "</matrix>", node->matrices[node->nMatrices].array, 16);
			gePrintDebug(0x100, "    Matrix %d sid=\"%s\"\n", node->nMatrices, node->matrices[node->nMatrices].sid);
			float tmp_mat[16];
			memcpy(tmp_mat, node->matrices[node->nMatrices].array, sizeof(float)*16);
			geMatrixTranspose(node->matrices[node->nMatrices].array, tmp_mat, 4);
			
			int i;
			for(i=0; i<16; i+=4){
				gePrintDebug(0x100, "      %f %f %f %f\n", node->matrices[node->nMatrices].array[i+0], node->matrices[node->nMatrices].array[i+1], node->matrices[node->nMatrices].array[i+2], node->matrices[node->nMatrices].array[i+3]);
			}
			
			node->nMatrices++;
		}
		if(strstr(buf, "<node")){
			node->nodes = (DAE_node*)geRealloc(node->nodes, sizeof(DAE_node)*(node->nNodes+1));
			memset(&node->nodes[node->nNodes], 0x0, sizeof(DAE_node));
			geGetParamString(buf, "id", node->nodes[node->nNodes].id, DAE_SEMANTIC_LEN);
			geGetParamString(buf, "name", node->nodes[node->nNodes].name, DAE_SEMANTIC_LEN);
			gePrintDebug(0x100, "  Node %d id=\"%s\", name=\"%s\"\n", node->nNodes, node->nodes[node->nNodes].id, node->nodes[node->nNodes].name);
			DaeLoadNode(fp, &node->nodes[node->nNodes]);
			node->nNodes++;
		}
	}
}

void DaeLoadVisualScene(ge_File* fp, DAE_visual* visual_scene){
	char buf[4096] = "";

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</visual_scene>");
		if(strstr(buf, "<node")){
			visual_scene->nodes = (DAE_node*)geRealloc(visual_scene->nodes, sizeof(DAE_node)*(visual_scene->nNodes+1));
			memset(&visual_scene->nodes[visual_scene->nNodes], 0x0, sizeof(DAE_node));
			geGetParamString(buf, "id", visual_scene->nodes[visual_scene->nNodes].id, DAE_SEMANTIC_LEN);
			geGetParamString(buf, "name", visual_scene->nodes[visual_scene->nNodes].name, DAE_SEMANTIC_LEN);
			gePrintDebug(0x100, "  Node %d id=\"%s\", name=\"%s\"\n", visual_scene->nNodes, visual_scene->nodes[visual_scene->nNodes].id, visual_scene->nodes[visual_scene->nNodes].name);
			DaeLoadNode(fp, &visual_scene->nodes[visual_scene->nNodes]);
			visual_scene->nNodes++;
		}
	}
}

DAE_visual* DaeLoadVisualScenes(ge_File* fp, int* _nScenes){
	char buf[4096] = "";

	int nScenes = 0;
	DAE_visual* visual_scenes = NULL;

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</library_visual_scenes>");
		if(strstr(buf, "<visual_scene")){
			visual_scenes = (DAE_visual*)geRealloc(visual_scenes, sizeof(DAE_visual)*(nScenes+1));
			memset(&visual_scenes[nScenes], 0x0, sizeof(DAE_visual));
			DaeLoadVisualScene(fp, &visual_scenes[nScenes]);
			nScenes++;
		}
	}

	*_nScenes = nScenes;
	return visual_scenes;
}

DAE* DaeLoad(ge_File* fp){
	u32 first_tick = geGetTick();
	DAE* dae = (DAE*)geMalloc(sizeof(DAE));
	char buf[4096] = "";

	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</COLLADA>");
		if(strstr(buf, "<library_animations>")){
			gePrintDebug(0x100, "\n\n");
			dae->animations = DaeLoadAnimations(fp, &dae->nAnimations);
			gePrintDebug(0x100, "\n\n");
		}
		if(strstr(buf, "<library_images>")){
			gePrintDebug(0x100, "\n\n");
			dae->images = DaeLoadImages(fp, &dae->nImages);
			gePrintDebug(0x100, "\n\n");
		}
		if(strstr(buf, "<library_materials>")){
			gePrintDebug(0x100, "\n\n");
			dae->materials = DaeLoadMaterials(fp, &dae->nMaterials);
			gePrintDebug(0x100, "\n\n");
		}
		if(strstr(buf, "<library_effects>")){
			gePrintDebug(0x100, "\n\n");
			dae->effects = DaeLoadEffects(fp, &dae->nEffects);
			gePrintDebug(0x100, "\n\n");
		}
		if(strstr(buf, "<library_geometries>")){
			gePrintDebug(0x100, "\n\n");
			dae->geometries = DaeLoadGeometries(fp, &dae->nGeometries);
			gePrintDebug(0x100, "\n\n");
		}
		if(strstr(buf, "<library_visual_scenes>")){
			gePrintDebug(0x100, "\n\n");
			dae->visual_scenes = DaeLoadVisualScenes(fp, &dae->nVisual_scenes);
			gePrintDebug(0x100, "\n\n");
		}
	}
	
	printf("DaeLoad end\n");
	gePrintDebug(0x100, "Load time : %d ms\n", geGetTick()-first_tick);
	(void)first_tick;
	return dae;
}

DAE_image* DaeFindImage(DAE* dae, const char* id){
	int i = 0;
	for(i=0; i<dae->nImages; i++){
		if(!strcmp(id, dae->images[i].id)){
			return &dae->images[i];
		}
	}
	gePrintDebug(0x102, "DaeFindImage error on \"%s\"\n", id);
	return NULL;
}

DAE_material* DaeFindMaterial(DAE* dae, const char* id){
	int i = 0;
	for(i=0; i<dae->nMaterials; i++){
		if(!strcmp(id, dae->materials[i].id)){
			return &dae->materials[i];
		}
	}
	gePrintDebug(0x102, "DaeFindMaterial error on \"%s\"\n", id);
	return NULL;
}

DAE_effect* DaeFindEffect(DAE* dae, const char* id){
	gePrintDebug(0x100, "DaeFindEffect(\"%s\")\n", id);
	int i = 0;
	for(i=0; i<dae->nEffects; i++){
		if(!strcmp(id, dae->effects[i].id)){
			gePrintDebug(0x100, "  Found\n");
			return &dae->effects[i];
		}
	}
	gePrintDebug(0x102, "DaeFindEffect error on \"%s\"\n", id);
	return NULL;
}

DAE_node* DaeFindNode(DAE* dae, const char* name){
	int i = 0;
	int j = 0;
	for(j=0; j<dae->nVisual_scenes; j++){
		for(i=0; i<dae->visual_scenes[j].nNodes; i++){
			if(!strcmp(name, dae->visual_scenes[j].nodes[i].name)){
				return &dae->visual_scenes[j].nodes[i];
			}
		}
	}
	gePrintDebug(0x102, "DaeFindNode error on \"%s\"\n", name);
	return NULL;
}

DAE_matrix* DaeNodeFindMatrix(DAE_node* node, const char* sid){
	int i = 0;
	for(i=0; i<node->nMatrices; i++){
		if(!strcmp(sid, node->matrices[i].sid)){
			return &node->matrices[i];
		}
	}
	gePrintDebug(0x102, "DaeNodeFindMatrix error on \"%s\"\n", sid);
	return NULL;
}

DAE_geometry* DaeFindGeometry(DAE* dae, const char* id){
	gePrintDebug(0x100, "DaeFindGeometry(\"%s\")\n", id);
	int i = 0;
	for(i=0; i<dae->nGeometries; i++){
		if(!strcmp(id, dae->geometries[i].id)){
			return &dae->geometries[i];
		}
	}
	gePrintDebug(0x101, "DaeFindGeometry error on \"%s\"\n", id);
	return NULL;
}

DAE_vertices* DaeGeometryFindVertices(DAE_geometry* geom, const char* id){
	gePrintDebug(0x100, "DaeGeometryFindVertices(\"%s\", \"%s\")\n", geom->name, id);
	int i = 0;
	for(i=0; i<geom->nVertices; i++){
		gePrintDebug(0x100, "  \"%s\"\n", geom->vertices[i].id);
		if(!strcmp(id, geom->vertices[i].id)){
			return &geom->vertices[i];
		}
	}
	gePrintDebug(0x102, "DaeGeometryFindVertices error on \"%s\"\n", id);
	return NULL;
}

DAE_source* DaeGeometryFindSource(DAE_geometry* geom, const char* id){
	gePrintDebug(0x100, "DaeGeometryFindSource(\"%s\")\n", id);
	int i = 0;
	for(i=0; i<geom->nSources; i++){
		if(!strcmp(id, geom->sources[i].id)){
			return &geom->sources[i];
		}
	}
	gePrintDebug(0x102, "DaeGeometryFindSource error on \"%s\"\n", id);
	return NULL;
}

DAE_input* DaeFindInput(DAE_input* inputs, int nInputs, const char* semantic){
	int i = 0;
	for(i=0; i<nInputs; i++){
		if(!strcmp(semantic, inputs[i].semantic)){
			return &inputs[i];
		}
	}
	return NULL;
}

void DaeComputeVertices(DAE* dae, DAE_geometry* geom, ge_Object* obj, const char* pth){
	gePrintDebug(0x100, "DaeComputeVertices(dae, \"%s\", \"%s\")\n", geom->id, obj->name);
	int i = 0;
	int j = 0;
	int s = 0;
	int v = 0;

	ge_Vertex* verts = obj->verts;

	DAE_effect* effect = NULL;
	DAE_source* vertices = NULL;
	DAE_source* normals = NULL;
	DAE_source* texcoords = NULL;
	int ofs_vertices = -1;
	int ofs_normals = -1;
	int ofs_texcoords = -1;

	for(i=0; i<geom->nTriangles; i++){
		gePrintDebug(0x100, "  Computing Triangle: %d/%d - material: \"%s\"\n", i+1, geom->nTriangles, geom->triangles[i].material);

		vertices = NULL;
		normals = NULL;
		texcoords = NULL;
		ofs_vertices = -1;
		ofs_normals = -1;
		ofs_texcoords = -1;
		DAE_material* material = DaeFindMaterial(dae, geom->triangles[i].material);
		if(material){
			gePrintDebug(0x100, "    Material \"%s\" found\n", geom->triangles[i].material);
			effect = DaeFindEffect(dae, material->instance);
		}
		if(effect){
			gePrintDebug(0x100, "    texture = \"%s\"\n", effect->technique.diffuse.texture.texture);
			if(!obj->material.textures[0] && effect->technique.diffuse.texture.texture && effect->technique.diffuse.texture.texture[0]){
				DAE_image* image = DaeFindImage(dae, effect->technique.diffuse.texture.texture);
				gePrintDebug(0x100, "      init_from = \"%s\"\n", image->init_from);
				char tmp[2048];
				if(!strncmp(image->init_from, "file://", 7)){
					strcpy(tmp, &image->init_from[7]);
				}else{
					sprintf(tmp, "%s/%s", pth, image->init_from);
				}
				gePrintDebug(0x100, "      path = \"%s\"\n", tmp);
				obj->material.textures[0] = geAddTexture(geLoadImageOptions(tmp, image->mipmaps == false ? GE_IMAGE_NO_MIPMAPS : 0));
				obj->material.textures_path[0] = (char*)malloc(strlen(tmp) + 1);
				strcpy(obj->material.textures_path[0], tmp);
			}
		}else{
			obj->material.textures[0] = NULL;
		}
		
		for(s=0; s<geom->triangles[i].nInputs; s++){
			gePrintDebug(0x100, "input %d: semantic: \"%s\", source: \"%s\"\n", s, geom->triangles[i].inputs[s].semantic, geom->triangles[i].inputs[s].source);
		}
		for(s=0; s<geom->triangles[i].nInputs; s++){
			if(!strcmp(geom->triangles[i].inputs[s].semantic, "VERTEX")){
				DAE_vertices* in_vertices = DaeGeometryFindVertices(geom, geom->triangles[i].inputs[s].source);
				DAE_input* in_positions = DaeFindInput(in_vertices->inputs, in_vertices->nInputs, "POSITION");
				vertices = DaeGeometryFindSource(geom, in_positions->source);
				ofs_vertices = geom->triangles[i].inputs[s].offset;
			}
			if(!strcmp(geom->triangles[i].inputs[s].semantic, "NORMAL")){
				normals = DaeGeometryFindSource(geom, geom->triangles[i].inputs[s].source);
				ofs_normals = geom->triangles[i].inputs[s].offset;
			}
			if(!strcmp(geom->triangles[i].inputs[s].semantic, "TEXCOORD")){
				texcoords = DaeGeometryFindSource(geom, geom->triangles[i].inputs[s].source);
				ofs_texcoords = geom->triangles[i].inputs[s].offset;
			}
		}
		gePrintDebug(0x100, "    vertices: 0x%08lX [ofs=%d]\n", (unsigned long)vertices, ofs_vertices);
		gePrintDebug(0x100, "    normals: 0x%08lX [ofs=%d]\n", (unsigned long)normals, ofs_normals);
		gePrintDebug(0x100, "    texcoords: 0x%08lX [ofs=%d]\n", (unsigned long)texcoords, ofs_texcoords);
		for(j=0; j< 3 * geom->triangles[i].count * geom->triangles[i].nInputs; j += geom->triangles[i].nInputs){
			if(effect){
				SET_COLOR_RGBAf(verts[v].color, effect->technique.diffuse.color.value[0], effect->technique.diffuse.color.value[1], effect->technique.diffuse.color.value[2], effect->technique.diffuse.color.value[3]);
			}else{
				SET_COLOR_RGBA(verts[v].color, 255, 255, 255, 255);
			}
			for(s=0; s<geom->triangles[i].nInputs; s++){
				int index = geom->triangles[i].p[j+s];
				if(s == ofs_vertices){
					verts[v].x = vertices->array.data[index * vertices->technique.accessor.stride + 0];
					verts[v].y = vertices->array.data[index * vertices->technique.accessor.stride + 1];
					verts[v].z = vertices->array.data[index * vertices->technique.accessor.stride + 2];
				}
				if(s == ofs_normals){
					verts[v].nx = normals->array.data[index * normals->technique.accessor.stride + 0];
					verts[v].ny = normals->array.data[index * normals->technique.accessor.stride + 1];
					verts[v].nz = normals->array.data[index * normals->technique.accessor.stride + 2];
				}
				if(s == ofs_texcoords){
					verts[v].u = texcoords->array.data[index * texcoords->technique.accessor.stride + 0];
					verts[v].v = 1.0 - texcoords->array.data[index * texcoords->technique.accessor.stride + 1];
					if(texcoords->technique.accessor.stride == 3){
#ifndef NO_3D_TEXCOORDS
						verts[v].w = texcoords->array.data[index * texcoords->technique.accessor.stride + 2];
#endif
					}
				}
			}
		//	gePrintDebug(0x100, "      vertex[%d]: { %3.4f, %3.4f,  %3.4f, %3.4f, %3.4f,  %3.4f, %3.4f, %3.4f }\n", v, verts[v].u, verts[v].v, verts[v].nx, verts[v].ny, verts[v].nz, verts[v].x, verts[v].y, verts[v].z);
			v++;
		}
	}
}

void ComputeNode(const char* pth, DAE* dae, DAE_node* node, ge_Object* obj){
	int i = 0;
	ge_Object tmp_obj;
	DAE_geometry* geom = DaeFindGeometry(dae, node->instance);
//	DAE_matrix* matrix = DaeNodeFindMatrix(node, "matrix");

	tmp_obj.nVerts = 0;
	for(i=0; i<geom->nTriangles; i++){
		tmp_obj.nVerts += geom->triangles[i].count * 3;
	}
	tmp_obj.verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*tmp_obj.nVerts);
	tmp_obj.material.textures[0] = NULL;
	DaeComputeVertices(dae, geom, &tmp_obj, pth);

	obj->verts = (ge_Vertex*)geRealloc(obj->verts, sizeof(ge_Vertex)*(obj->nVerts+tmp_obj.nVerts));
	memcpy(&obj->verts[obj->nVerts], tmp_obj.verts, sizeof(ge_Vertex)*tmp_obj.nVerts);
	obj->nVerts += tmp_obj.nVerts;
	if(tmp_obj.material.textures[0]){
		obj->material.textures[0] = tmp_obj.material.textures[0];
	}

	geFree(tmp_obj.verts);

	for(i=0; i<node->nNodes; i++){
		ComputeNode(pth, dae, &node->nodes[i], obj);
	}
}

void ComputeNodeObjs(const char* pth, DAE* dae, DAE_node* node, ge_Object** objs, int* _cObj, ge_AnimNode* anim){
	gePrintDebug(0x100, "ComputeNodeObjs(\"%s\", 0x%08lX, \"%s\", 0x%08lX, %d)\n", pth, (unsigned long)dae, node->name, (unsigned long)objs, *_cObj);
	int i = 0;
	int cObj = *_cObj;
	DAE_geometry* geom = DaeFindGeometry(dae, node->instance);
	gePrintDebug(0x100, "  GEOMETRY: \"%s\"\n", geom->id);
	DAE_matrix* matrix = DaeNodeFindMatrix(node, "matrix");

	anim->nChilds = 0;
	strncpy(anim->name, node->name, 64);
	if(geom){
		anim->target = objs[cObj];
	}

	memcpy(anim->default_matrix, matrix->array, sizeof(float)*16);
	memcpy(anim->matrix, matrix->array, sizeof(float)*16);

	if(geom){
		ge_Object* obj = objs[cObj];
		strcpy(obj->name, node->name);
		memcpy(obj->matrix, matrix->array, sizeof(float)*16);
	//	PrintMatrix("  matrix", obj->matrix, 4);
		obj->matrix_used = true;
		obj->vert_start = 0;
		obj->nVerts = 0;
		for(i=0; i<geom->nTriangles; i++){
			obj->nVerts += geom->triangles[i].count * 3;
		}
		obj->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*obj->nVerts);
		DaeComputeVertices(dae, geom, obj, pth);

		cObj++;
	}
	
	gePrintDebug(0x100, "GROUUUUUUUUUUUMMM   => \"%s\"->nNodes = %d\n", node->name, node->nNodes);
	for(i=0; i<node->nNodes; i++){
		anim->childs = (ge_AnimNode*)geRealloc(anim->childs, sizeof(ge_AnimNode)*(anim->nChilds+1));
		memset(&anim->childs[anim->nChilds], 0x0, sizeof(ge_AnimNode));
		anim->childs[anim->nChilds].parent = anim;
		ComputeNodeObjs(pth, dae, &node->nodes[i], objs, &cObj, &anim->childs[anim->nChilds]);
		anim->nChilds++;
	}

	*_cObj = cObj;
}

ge_Object* geLoadDae(const char* file, const char* obj_name){
	gePrintDebug(0x100, "geLoadDae(\"%s\", \"%s\")\n", file, obj_name);
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
	gePrintDebug(0x100, "pth: \"%s\"\n", pth);

	int i = 0;
	DAE* dae = DaeLoad(fp);
	gePrintDebug(0x100, "\n\n");

	ge_Object* obj = (ge_Object*)geMalloc(sizeof(ge_Object));

	if(obj_name && obj_name[0]){
		DAE_node* node = DaeFindNode(dae, obj_name);
		DAE_geometry* geom = DaeFindGeometry(dae, node->instance);
		DAE_matrix* matrix = DaeNodeFindMatrix(node, "matrix");

		strcpy(obj->name, obj_name);
		memcpy(obj->matrix, matrix->array, sizeof(float)*16);
		obj->matrix_used = true;
		obj->vert_start = 0;
		obj->nVerts = 0;
		for(i=0; i<geom->nTriangles; i++){
			obj->nVerts += geom->triangles[i].count * 3;
		}
		obj->verts = (ge_Vertex*)geMalloc(sizeof(ge_Vertex)*obj->nVerts);
		DaeComputeVertices(dae, geom, obj, pth);
	}else{
		strcpy(obj->name, file);
		ge_LoadIdentity(obj->matrix);
		obj->matrix_used = false;
		obj->vert_start = 0;
		obj->nVerts = 0;
		int j = 0;
		int k = 0;
	//	ge_Object tmp_obj;
		for(j=0; j<dae->nVisual_scenes; j++){
			for(k=0; k<dae->visual_scenes[j].nNodes; k++){
				DAE_node* node = &dae->visual_scenes[j].nodes[k];
				ComputeNode(pth, dae, node, obj);
			}
		}
	}

	return obj;
}

void geLoadDaeListMulti(const char* file, const char** obj_list, int count, ge_Object** objs, ge_Animator* animator){
	gePrintDebug(0x100, "geLoadDaeList(\"%s\", ..., %d)\n", file, count);
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ|GE_FILE_MODE_BINARY);
	if(!fp){
		return;
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
	gePrintDebug(0x100, "pth: \"%s\"\n", pth);

//	int i = 0;
	DAE* dae = DaeLoad(fp);
	gePrintDebug(0x100, "\n\n");
	gePrintDebug(0x100, "geLoadDaeListMulti 1\n");
	
	if(animator){
		animator->root.nChilds = 0;
	}
	int cObj = 0;
	int j = 0;
	int k = 0;
	gePrintDebug(0x100, "geLoadDaeListMulti 2\n");
	for(j=0; j<dae->nVisual_scenes; j++){
		for(k=0; k<dae->visual_scenes[j].nNodes; k++){
			DAE_node* node = &dae->visual_scenes[j].nodes[k];
			if(count != 0 && !findInList(obj_list, count, node->name)){
				continue;
			}
			if(animator){
				animator->root.childs = (ge_AnimNode*)geRealloc(animator->root.childs, sizeof(ge_AnimNode)*(animator->root.nChilds+1));
				memset(&animator->root.childs[animator->root.nChilds], 0x0, sizeof(ge_AnimNode));
				ComputeNodeObjs(pth, dae, node, objs, &cObj, &animator->root.childs[animator->root.nChilds]);
				animator->root.nChilds++;
			}
		}
	}
	
	gePrintDebug(0x100, "geLoadDaeListMulti 3\n");
	gePrintDebug(0x100, "\n\n");
	if(animator){
		ComputeAnimations(dae->animations, dae->nAnimations, animator);
	}
	gePrintDebug(0x100, "\n\n");
	
	gePrintDebug(0x100, "geLoadDaeListMulti 4\n");
	geFileClose(fp);
	gePrintDebug(0x100, "geLoadDaeListMulti 5\n");
}

int geDaeCountInFile(const char* file){
	gePrintDebug(0x100, "geDaeCountInFile(\"%s\")\n", file);
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ|GE_FILE_MODE_BINARY);
	if(!fp){
		return 0;
	}

	int nObjs = 0;
	char buf[4096] = "";

	while(GetXml(fp, buf, 4096)){
		if(strstr(buf, "<library_visual_scenes>")){
			while(GetXml(fp, buf, 4096)){
				CHECK_EXIT("</library_visual_scenes>");
					if(strstr(buf, "<node")){
						while(GetXml(fp, buf, 4096)){
							CHECK_EXIT("</node>");
							if(strstr(buf, "<instance_geometry")){
								nObjs++;
							}
						}
					}
			}
			break;
		}
	}
	
	gePrintDebug(0x100, "END\n");
	geFileClose(fp);
	return nObjs;
}

int CountSubNodes(ge_File* fp, char* buf, const char** obj_list, int count){
	int nObjs = 0;
	while(GetXml(fp, buf, 4096)){
		CHECK_EXIT("</node>");
		if(strstr(buf, "<node")){
			nObjs += CountSubNodes(fp, buf, obj_list, count);
		}
		if(strstr(buf, "<instance_geometry")){
			nObjs++;
		}
	}
	return nObjs;
}

int geDaeCountInFileSubs(const char* file, const char** obj_list, int count){
	ge_File* fp = geFileOpen(file, GE_FILE_MODE_READ|GE_FILE_MODE_BINARY);
	if(!fp){
		return 0;
	}

	int nObjs = 0;
	char buf[4096] = "";

	while(GetXml(fp, buf, 4096)){
		if(strstr(buf, "<library_visual_scenes>")){
			while(GetXml(fp, buf, 4096)){
				CHECK_EXIT("</library_visual_scenes>");
					if(strstr(buf, "<node")){
						char name[DAE_ID_LEN];
						geGetParamString(buf, "name", name, DAE_ID_LEN);
						if(findInList(obj_list, count, name)){
							nObjs += CountSubNodes(fp, buf, obj_list, count);
						}
					}
			}
			break;
		}
	}

	
	geFileClose(fp);
	return nObjs;
}
