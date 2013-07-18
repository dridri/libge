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

#ifndef __GE_TYPES_HEADER__
#define __GE_TYPES_HEADER__

#include <stdint.h>
#include <stdbool.h>

#if(defined(PSP) && !defined(U32_COLORS))
#define U32_COLORS
#endif

#if (!defined(WIN32) && (defined(_Windows) || \
	defined(_WINDOWS) || defined(_WIN32) || defined(__WIN32__)))
	#define WIN32
#endif

#ifdef WIN32
	#if (defined(LIBGE_STATIC) || defined(LIBGE_LIB))
		#define LIBGE_API __declspec(dllexport)
	#else
		#define LIBGE_API __declspec(dllimport)
	#endif
#else
	#ifndef LIBGE_API
		#define LIBGE_API
	#endif
#endif

#ifndef s64
	#define s64 int64_t
#endif
#ifndef s32
	#define s32 int32_t
#endif
#ifndef s16
	#define s16 int16_t
#endif
#ifndef s8
	#define s8 int8_t
#endif

#ifndef u64
	#define u64 uint64_t
#endif
#ifndef u32
	#define u32 uint32_t
#endif
#ifndef u16
	#define u16 uint16_t
#endif
#ifndef u8
	#define u8 uint8_t
#endif
#if(ARCH==64)
	#define t_ptr u64
#else
	#define t_ptr u32
#endif


typedef struct ge_Thread {
	t_ptr id;
	char name[128];

	s32 priority;
	s32 flags;
	void* func;

	s32 state;
	s32 exit_code;
	s32 returned;

	s32 paused;
	t_ptr handle;
	u32 stackSize;
	t_ptr graphic_ctx;
} ge_Thread;

typedef struct ge_Timer {
	s32 hours;
	s32 minuts;
	s32 seconds;
	u32 ellapsed;

	char name[64];
	s32 id;
	bool running;

	u32 current_time;
	u32 start_time;
	u32 pause_time;
	u32 pause_start;
	u32 pause_end;
} ge_Timer;

//2D structures
typedef struct ge_Image {
	s32 flags;
	u32 id;
	s32 width, height;
	s32 textureWidth, textureHeight;
	float u, v;
	u32* data;
	u32 color;
} ge_Image;

typedef struct ge_Image3D {
	s32 flags;
	u32 id;
	s32 width, height, depth;
	s32 textureWidth, textureHeight, textureDepth;
	float u, v, w;
	u32* data;
	u32 color;
} ge_Image3D;

typedef struct TTF_AbsPos {
	s32 x, y, w, h;
	s32 advX, posY;
} TTF_AbsPos;

typedef struct ge_Font {
	void* face;
	u8* data;
	s32 size;
	s32 type;
	
	ge_Image* texture;
	TTF_AbsPos positions[256];
} ge_Font;


typedef struct ge_Vector3i {
	int x, y, z;
} ge_Vector3i;

typedef struct ge_Vector2f {
	float x, y;
} ge_Vector2f;

typedef struct ge_Vector3f {
	float x, y, z;
} ge_Vector3f;

typedef struct ge_Vector4f {
	float x, y, z, w;
} ge_Vector4f;

typedef struct ge_Matrix4f {
	ge_Vector4f x, y, z, w;
} ge_Matrix4f;

typedef struct ge_Vector3d {
	double x, y, z;
} ge_Vector3d;

typedef struct ge_Vector4d {
	double x, y, z, w;
} ge_Vector4d;

#ifdef PSP
typedef struct ge_Vertex {
	float u, v;
	u32 color;
	float nx, ny, nz;
	float x, y, z;
} ge_Vertex;
#else
typedef struct ge_Vertex {
	float u, v, w;
	float color[4];
	float nx, ny, nz;
	float x, y, z;
	int align[3];
} ge_Vertex;
#endif


typedef struct ge_Gl_Loc_Light {
	s32 loc_flags;
	s32 loc_position;
	s32 loc_target;
	s32 loc_ambient;
	s32 loc_diffuse;
	s32 loc_specular;
	s32 loc_spotCutoff;
	s32 loc_spotCosCutoff;
	s32 loc_spotExponent;
	s32 loc_attenuation;
	s32 loc_shadow;
	s32 loc_vector;
	s32 loc_targetVector;
	s32 loc_CosOuterConeAngle;
	s32 loc_CosInnerMinusOuterAngle;
} ge_Gl_Loc_Light;

typedef struct ge_Shader {
	s32 programId;
	s32 vShaderId;
	s32 gShaderId;
	s32 tcShaderId;
	s32 teShaderId;
	s32 fShaderId;


	s32 loc_fog_density;
	s32 loc_fog_color;
	s32 loc_fog_start;
	s32 loc_fog_end;

	s32 loc_time;
	s32 loc_ratio;
	s32 loc_HasTexture;

	//Matrices
	s32 loc_mvp;
	s32 loc_model;
	s32 loc_view;
	s32 loc_projection;
	s32 loc_normal;
	s32 loc_modelview;
	s32 loc_texture0;
	s32 loc_texture1;
	s32 loc_texture2;
	s32 loc_texture3;
	s32 loc_texture4;
	s32 loc_texture5;
	s32 loc_texture6;
	s32 loc_texture7;

	s32 loc_submodel;
	s32 loc_clipplane;
	//Material
	s32 loc_front_ambient;
	s32 loc_front_diffuse;
	s32 loc_front_specular;
	//Camera
	s32 loc_camera;
	//Shadow
	s32 loc_shadow;
	//Lights
	s32 loc_lights_s_count;
	s32 loc_lights_d_count;
	ge_Gl_Loc_Light* loc_lights;
} ge_Shader;

typedef struct ge_Framebuffer {
	s32 id;
	s32 ms_id;
	s32 samples;
	ge_Image* texture;
	ge_Image* depth;
} ge_Framebuffer;

typedef struct ge_Fog {
	s32 mode;
#ifdef U32_COLORS
	u32 color;
#else
	float color[4];
#endif
	float density;
	float start;
	float end;
} ge_Fog;

typedef struct ge_Camera {
	float x, y, z;
	float cX, cY, cZ;
	float RotH, RotV;
	float RotH_rad, RotV_rad;
	float minRotV, maxRotV;
	float minRotH, maxRotH;
	bool maxV, maxH;
} ge_Camera;

typedef struct ge_Material {
#ifdef U32_COLORS
	u32 diffuse;
	u32 ambient;
	u32 specular;
#else
	float diffuse[4];
	float ambient[4];
	float specular[4];
#endif
	ge_Image* textures[8];
	char* textures_path[8];
	float bump_force;
} ge_Material;

typedef struct ge_Object {
	float matrix[16];
	s32 matrix_used;
	s32 vert_start;
	ge_Vertex* verts;
	s32 nVerts;
	ge_Material material;
	char name[64];
} ge_Object;

typedef struct ge_Light {
	bool used;
	s32 flags;
	ge_Vector4f position;
	ge_Vector4f target;
#ifdef U32_COLORS
	u32 ambient;
	u32 diffuse;
	u32 specular;
#else
	float ambient[4];
	float diffuse[4];
	float specular[4];
#endif
	float spot_cutoff;
	float spot_coscutoff;
	float spot_exponent;
	float attenuation;
	s32 type;

	ge_Image* shadow;
	ge_Framebuffer* shadow_fbo;
	ge_Shader* shadow_shader;
	int iShadow;
	int shadow_depth;
	float shadow_factor;

	bool isDynamic;
	int i_loc;

	// For static lights
	ge_Vector3f vector;
	ge_Vector3f target_vector;
	float CosOuterConeAngle;
	float CosInnerMinusOuterAngle;
} ge_Light;

typedef struct ge_Cloud {
	float x, y, z;
	s32 size;
	s32 parts;
	float* parts_x;
	float* parts_y;
	float* parts_z;
	float* parts_a;
	s32 ready;
	float alpha;
	float matrix[16];
	float* parts_matrix;
} ge_Cloud;

typedef struct ge_CloudsGenerator {
	u32 vbo;
	u32 vao;
	ge_Shader* shader;
	s32 loc_VertexTexcoord;
	s32 loc_VertexColor;
	s32 loc_VertexNormal;
	s32 loc_VertexPosition;
	ge_Vertex* verts;
	s32 nVerts;
	ge_Object** objs;
	s32 nObjs;
	u32 draw_mode;
	u32 memory_mode;

	s32 size_min[3];
	s32 size_max[3];
	s32 parts_min[3];
	s32 parts_max[3];
	s32 map_size_x;
	s32 map_size_y;
	s32 n_clouds[3];
	u8* types;
	ge_Cloud* clouds;
	s32 loc_tex_decal;
	s32 loc_cloud_pos;
} ge_CloudsGenerator;

typedef struct ge_AnimNode ge_AnimNode;
struct ge_AnimNode {
	char name[64];
	ge_AnimNode* parent;
	float default_matrix[16];
	float matrix[16];
	ge_Object* target;
	s32 nChilds;
	ge_AnimNode* childs;
};

typedef struct ge_AnimSampler {
	s32 mode;
	float speed;
	ge_Timer timer;
	s32 nTimes;
	float* times;
	s32 nMatrices;
	float* matrices;
	s32 nInterpolations;
	s32* interpolations;
	ge_AnimNode* target;
} ge_AnimSampler;

typedef struct ge_Animator {
	ge_AnimNode root;
	s32 nSamplers;
	ge_AnimSampler* samplers;
} ge_Animator;

typedef struct ge_Water {
	s32 type;
	s32 loc_normalMapPos;
	s32 loc_random;
	float normalMapPos[3];
	ge_Image* tex0;
	ge_Image* tex3;
	int initialized;
	float z;
} ge_Water;

typedef struct ge_CustomVertex {
	int size;
	int vertex_offset;
	int vertex_type;
	int vertex_count;
	int color_offset;
	int color_type;
	int color_count;
	int normal_offset;
	int normal_type;
	int texture_offset;
	int texture_type;
	int texture_count;
} ge_CustomVertex;

typedef struct ge_Renderer {
	u32 vbo;
	u32 vao;
	ge_Shader* shader;
	s32 loc_VertexTexcoord;
	s32 loc_VertexColor;
	s32 loc_VertexNormal;
	s32 loc_VertexPosition;
	ge_Vertex* verts;
	s32 nVerts;
	ge_Object** objs;
	s32 nObjs;
	u32 draw_mode;
	u32 memory_mode;

	char name[64];
	
	ge_Light** lights;
	s32 nLights;
	s32 nDynamicLights;
	ge_Animator* animator;

	float projection_matrix[16];
	s32 matrix_used;
	s32 tesselated;

	s32 depth_enabled;
	s32 depth_mask;
	s32 blend_enabled;
	s32 enabled;
	void (*ext_func)(void*, s32); // The 'void*' = this
	void (*callback)(void*, s32); // The 'void*' = this

	ge_CustomVertex* customVert;
	void* extension;
} ge_Renderer;

typedef struct ge_Scene {
#ifdef U32_COLORS
	u32 clear_color;
	u32 material_ambient;
	u32 material_diffuse;
#else
	float clear_color[4];
	float material_ambient[4];
	float material_diffuse[4];
#endif

	ge_Light* lights;
	s32 nLights;
	ge_Light* dynamicLights; //unused
	s32 nDynamicLights;

	ge_Renderer* renderers;
	s32 nRenderers;

	ge_Renderer sky;
	ge_CloudsGenerator* cloudsGenerator;
	ge_Fog* fog;
	bool fogEnabled;
} ge_Scene;

typedef struct ge_Smoke {
#ifdef U32_COLORS
	u32 color;
#else
	float color[4];
#endif
	ge_Vector3f position;
	ge_Vector3f direction;
	ge_Image* texture;
	bool activated;
} ge_Smoke;

typedef struct ge_Particle {
	ge_Vector3f pos;
	ge_Vector3f dpos;
	float life;
	float life_base;
} ge_Particle;

typedef struct ge_Particles {
	bool loop_mode;
	ge_Vector3f origin;
	ge_Vector3f direction;
	float velocity;
	float life_time;
	float ut;
	float dt;

	float radius;
	ge_Vector2f size;
	ge_Image* texture;
	int nParticles;
	ge_Particle* particles;
	int visible_parts;

	ge_Renderer* render;
	ge_Shader* shader;
	int loc_size;
	int loc_pos;
	int loc_rpos;
	int loc_life;
	int loc_textured;
} ge_Particles;


#endif // __GE_TYPES_HEADER__
