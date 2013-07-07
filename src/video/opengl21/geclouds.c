#include "../../ge_internal.h"
#include <math.h>

void ge_LoadIdentity(float* m);
void ge_Translate(float* m, float x, float y, float z);
void ge_Rotate(float* m, float x, float y, float z);
void ge_Scale(float* m, float x, float y, float z);
void geSendMatrix(int type, float* m);

void GenerateCloud(ge_Scene* scene, ge_Cloud* cloud, int j);

extern ge_Camera* ge_current_camera;
static int nClouds = 0;

int ge_clouds_thread(int args, void* argp){
	ge_Scene* scene = (ge_Scene*)argp;
	nClouds = scene->cloudsGenerator->n_clouds[0] + scene->cloudsGenerator->n_clouds[1] + scene->cloudsGenerator->n_clouds[2];
	float map_size_x = scene->cloudsGenerator->map_size_x;

	while(1){
		if(!ge_current_camera){
			geSleep(1000);
			continue;
		}

		int c=0;
		for(c=0; c<nClouds; c++){
			if(!ge_current_camera)break;
			int type = scene->cloudsGenerator->types[c];
			ge_Cloud* cloud = &scene->cloudsGenerator->clouds[c];

			float scale = 1.5;
			if(type == GE_CLOUD_TYPE_HIGH_LEVEL){
				scale = 5.0;
			}else
			if(type == GE_CLOUD_TYPE_MID_LEVEL){
				scale = 10.0;
			}else{
				scale = 20.0;
			}

		//	if(cloud->x > map_size_x/2.0){
			if(cloud->z <= 0.0 && cloud->x > 0.0){
				srand(0);
				int first_rand = rand()*geGetTick();
				srand(first_rand);
				GenerateCloud(scene, cloud, c);
			//	cloud->x = -map_size_x/2.0;
				float D = -4 * (-1.0/scene->cloudsGenerator->map_size_x) * (scene->cloudsGenerator->map_size_x / 2.0);
				cloud->x = sqrt(D) / (-2.0 / scene->cloudsGenerator->map_size_x);
			}

			cloud->alpha = 1.0;
			if(abs(cloud->x) > map_size_x/2.0*0.6){
			//	cloud->alpha = 1.0 - (abs(cloud->x) - map_size_x/2.0*0.6) / (map_size_x/2.0*0.4);
			}


			float dx = (ge_current_camera->x - cloud->x);
			float dy = (ge_current_camera->y - cloud->y);
			float dz = (ge_current_camera->z - cloud->z);
			float rotX = atanf(dx/dz);
			float rotY = atanf(dy/dz);
		//	float rotZ = atanf(dx/-dy);

			ge_LoadIdentity(cloud->matrix);
			ge_Scale(cloud->matrix, scale, scale, scale);
			ge_Rotate(cloud->matrix, -rotY, rotX, 0.0);
		}
		geSleep(100);
	}

	return 0;
}

ge_Object* ref = NULL;
void DrawClouds(ge_Scene* scene){
	glEnable(GL_BLEND);
	glDepthFunc(GL_ALWAYS);
//	glDepthFunc(GL_LEQUAL);
	geShaderUse(scene->cloudsGenerator->shader);

	glBindBuffer(GL_ARRAY_BUFFER, scene->cloudsGenerator->vbo);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
#ifdef U32_COLORS
	glVertexPointer(3, GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(28)); //3*4 + 1*4 + 3*4 => size(u,v,w,color,nx,ny,nz)
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ge_Vertex), BUFFER_OFFSET(12)); //3*4 => size(u,v,w)
	glNormalPointer(GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(16)); //3*4 + 1*4 => size(u,v,w,color)
	glTexCoordPointer(3, GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(0));
#else
	glVertexPointer(3, GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(40)); //3*4 + 4*4 + 3*4 => size(u,v,w,color[4],nx,ny,nz)
	glColorPointer(4, GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(12)); //3*4 => size(u,v,w)
	glNormalPointer(GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(28)); //3*4 + 4*4 => size(u,v,w,color[4])
	glTexCoordPointer(3, GL_FLOAT, sizeof(ge_Vertex), BUFFER_OFFSET(0));
#endif

	geUpdateMatrix();
	geMatrixMode(GE_MATRIX_SUBMODEL);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	int last_tex_id = -1;
	int c=0, i=0; 
	for(c=0; c<nClouds; c++){
		int type = scene->cloudsGenerator->types[c];
		ge_Object* obj = scene->cloudsGenerator->objs[type];
		ge_Cloud* cloud = &scene->cloudsGenerator->clouds[c];
		if(!cloud->ready)continue;
		if(obj->material.textures[0]->id != last_tex_id){
			last_tex_id = scene->cloudsGenerator->objs[type]->material.textures[0]->id;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, last_tex_id);
		}
		
		cloud->x += 10.0;
		float cloud_z = 1.0;
/*
		if(abs(cloud->x) > scene->cloudsGenerator->map_size_x/2.0*0.5){
			cloud_z = (1.0 - (abs(cloud->x) - scene->cloudsGenerator->map_size_x/2.0*0.5) / (scene->cloudsGenerator->map_size_x/2.0*0.5));
		}
			cloud_z *=  (1.0 - (abs(cloud->y) - scene->cloudsGenerator->map_size_y/2.0*0.5) / (scene->cloudsGenerator->map_size_y/2.0*0.5));
			cloud_z = cloud->z * cloud_z;
*/
		float _x = cloud->x;// + scene->cloudsGenerator->map_size_x/2;
		float z_x = -(1.0/(scene->cloudsGenerator->map_size_x))*_x*_x + scene->cloudsGenerator->map_size_x/2;
		float z_y = -(1.0/(scene->cloudsGenerator->map_size_y))*cloud->y*cloud->y + scene->cloudsGenerator->map_size_y/2;
	//	cloud_z = z_x + z_y;
	//	cloud_z /= 4.0;
		cloud->z = cloud_z;
		cloud_z = cloud->z = (z_x * 0.5 + scene->cloudsGenerator->map_size_x/2) * (z_y / scene->cloudsGenerator->map_size_x/2);

		glUniformMatrix4fv(scene->cloudsGenerator->shader->loc_model, 1, GL_FALSE, cloud->matrix);
		glUniform3f(scene->cloudsGenerator->loc_cloud_pos, cloud->x, cloud->y, cloud_z);
		
		for(i=0; i<scene->cloudsGenerator->clouds[c].parts; i++){
			float tex_decal_x=0.0, tex_decal_y=0.0;
			if(((u32)cloud->parts_a[i] & 0xFF) > 0x7F){
				tex_decal_x = 0.5;
			}
			if((((u32)cloud->parts_a[i] >> 8) & 0xFF) > 0x7F){
				tex_decal_y = 0.5;
			}
			geLoadIdentity();
			geTranslate(cloud->x+cloud->parts_x[i], cloud->y+cloud->parts_y[i], cloud_z+cloud->parts_z[i]);
		//	geTranslate(cloud->x, cloud->y, cloud_z);
			geUpdateCurrentMatrix();
			glUniform4f(scene->cloudsGenerator->shader->loc_front_diffuse, cloud->parts_a[i], 1.0, 1.0, cloud->alpha);
			glUniform2f(scene->cloudsGenerator->loc_tex_decal, tex_decal_x, tex_decal_y);
			glDrawArrays(GL_TRIANGLES, obj->vert_start, obj->nVerts);
		//	break;
		}

	//	break;
	}
	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);
}

void GenerateCloud(ge_Scene* scene, ge_Cloud* cloud, int j){
	bool was_ready = cloud->ready;
	cloud->ready = false;
	srand(j*42+geGetTick()*rand());
	srand(j*42*rand()*rand() - j*42+geGetTick()*rand());

	int type = -1;
	if(j < scene->cloudsGenerator->n_clouds[0]){
		type = GE_CLOUD_TYPE_HIGH_LEVEL;
	}else
	if(j < scene->cloudsGenerator->n_clouds[0]+scene->cloudsGenerator->n_clouds[1]){
		type = GE_CLOUD_TYPE_MID_LEVEL;
	}else
	if(j < scene->cloudsGenerator->n_clouds[0]+scene->cloudsGenerator->n_clouds[1]+scene->cloudsGenerator->n_clouds[2]){
		type = GE_CLOUD_TYPE_LOW_LEVEL;
	}else{
		type = GE_CLOUD_TYPE_LOW_LEVEL; // Assume default is low-level
	}
	scene->cloudsGenerator->types[j] = (u8)type;
	int size_range = scene->cloudsGenerator->size_max[type] - scene->cloudsGenerator->size_min[type];
	int parts_range = scene->cloudsGenerator->parts_max[type] - scene->cloudsGenerator->parts_min[type];
	int size = rand() % size_range + scene->cloudsGenerator->size_min[type];
	int parts = rand() % parts_range + scene->cloudsGenerator->parts_min[type];
	parts = size * scene->cloudsGenerator->parts_max[type] / scene->cloudsGenerator->size_max[type];
//	parts = (size-scene->cloudsGenerator->size_min)*parts_range/size_range;
	float a = 0.0;
	a = rand() % 360;
	scene->cloudsGenerator->clouds[j].x = rand() % scene->cloudsGenerator->map_size_x - (scene->cloudsGenerator->map_size_x/2);
	scene->cloudsGenerator->clouds[j].y = rand() % scene->cloudsGenerator->map_size_y - (scene->cloudsGenerator->map_size_y/2);
	float r = geCos(rand()) * (scene->cloudsGenerator->map_size_x/1.0);// + (scene->cloudsGenerator->map_size_x/8);
	float r2 = geSin(rand()) * (scene->cloudsGenerator->map_size_y/1.0);// + (scene->cloudsGenerator->map_size_x/8);
	printf("r,r2 = %f, %f\n", r, r2);
	scene->cloudsGenerator->clouds[j].x = r * geCos(a/**scene->cloudsGenerator->clouds[j].x*/);
	scene->cloudsGenerator->clouds[j].y = r2 * geSin(a/**scene->cloudsGenerator->clouds[j].y*/);
	if(type == GE_CLOUD_TYPE_HIGH_LEVEL){
		scene->cloudsGenerator->clouds[j].z = rand() % 3000 + 11000;
	}else
	if(type == GE_CLOUD_TYPE_MID_LEVEL){
		scene->cloudsGenerator->clouds[j].z = rand() % 3000 + 7000;
	}else
	if(type == GE_CLOUD_TYPE_LOW_LEVEL){
		scene->cloudsGenerator->clouds[j].z = rand() % 2000 + 4000;
	}
	scene->cloudsGenerator->clouds[j].size = size;
	scene->cloudsGenerator->clouds[j].parts = parts;
	if(was_ready){
		free(scene->cloudsGenerator->clouds[j].parts_x);
		free(scene->cloudsGenerator->clouds[j].parts_y);
		free(scene->cloudsGenerator->clouds[j].parts_z);
		free(scene->cloudsGenerator->clouds[j].parts_a);
		free(scene->cloudsGenerator->clouds[j].parts_matrix);
	}
	scene->cloudsGenerator->clouds[j].parts_x = (float*)geMalloc(sizeof(float)*parts);
	scene->cloudsGenerator->clouds[j].parts_y = (float*)geMalloc(sizeof(float)*parts);
	scene->cloudsGenerator->clouds[j].parts_z = (float*)geMalloc(sizeof(float)*parts);
	scene->cloudsGenerator->clouds[j].parts_a = (float*)geMalloc(sizeof(float)*parts);
	scene->cloudsGenerator->clouds[j].parts_matrix = (float*)geMalloc(sizeof(float)*16*parts);
	memset(scene->cloudsGenerator->clouds[j].parts_x, 0x0, sizeof(float)*parts);
	memset(scene->cloudsGenerator->clouds[j].parts_y, 0x0, sizeof(float)*parts);
	memset(scene->cloudsGenerator->clouds[j].parts_z, 0x0, sizeof(float)*parts);
	memset(scene->cloudsGenerator->clouds[j].parts_a, 0x0, sizeof(float)*parts);
	memset(scene->cloudsGenerator->clouds[j].parts_matrix, 0x0, sizeof(float)*16*parts);
/*
	int k = 0;
	for(k=0; k<parts; k++){
		srand(k*2+geGetTick()*rand());
		srand((k*4+geGetTick())*rand());
		float a=0.0, x=0.0, y=0.0, z=0.0;
		a = rand() % 360;
		x = ((rand() % (int)(size*2.0)) - size) * geCos(a);
		if(type == GE_CLOUD_TYPE_HIGH_LEVEL){
			y = ((rand() % (size/2)) - (size/4)) * geSin(a);
			z = 0.0;
		}else{
			y = ((rand() % (int)(size*1.5)) - (size*0.75)) * geSin(a);
			z = ((rand() % size/10) - size/20) * geSin(rand() % 360);
		}
		scene->cloudsGenerator->clouds[j].parts_x[k] = x;
		scene->cloudsGenerator->clouds[j].parts_y[k] = y;
		scene->cloudsGenerator->clouds[j].parts_z[k] = z;
		scene->cloudsGenerator->clouds[j].parts_a[k] = 1.0;
		if(z < 0.0){
#define absf(a) (a>0.0?a:-a)
			scene->cloudsGenerator->clouds[j].parts_a[k] = 1.0 - ((absf(z) / (size/20)) * (1.0-(absf(x) / size)) * (1.0-(absf(y) / (size*0.75))));
			printf("%f => %f\n", z, scene->cloudsGenerator->clouds[j].parts_a[k]);
		}
	}
*/
	int k = 0;
	for(k=0; k<parts; k++){
		srand(k*2+geGetTick()*rand());
	//	srand((k*4+geGetTick())*rand());
		float a=0.0, x=0.0, y=0.0, z=0.0, zr=0.0;
		a = rand() % 360;
		x = ((rand() % size) - (size/2.0)) * geCos(a);
		y = ((rand() % size) - (size/2.0)) * geSin(a);
		if(type == GE_CLOUD_TYPE_HIGH_LEVEL){
			z = 0.0;
			zr = 0.0;
		}else
		if(type == GE_CLOUD_TYPE_MID_LEVEL){
			z = ((rand() % (int)(size/512)) - (size/1024)) * geSin(rand() % 360);
			zr = 1024;
		}else
		if(type == GE_CLOUD_TYPE_LOW_LEVEL){
			z = ((rand() % (int)(size/16)) - (size/32)) * geSin(rand() % 360);
			zr = 97;
		}
		scene->cloudsGenerator->clouds[j].parts_x[k] = x;
		scene->cloudsGenerator->clouds[j].parts_y[k] = y;
		scene->cloudsGenerator->clouds[j].parts_z[k] = z;
		scene->cloudsGenerator->clouds[j].parts_a[k] = 1.0;

		if(z < 0.0){
#define absf(x) (x<0.0?-x:x)
			scene->cloudsGenerator->clouds[j].parts_a[k] = 1.0 - ((absf(z) / (size/zr)) * (1.0-(absf(x) / (size/2.0))) * (1.0-(absf(y) / (size/2.0))));
		}
	}

	cloud->ready = true;
}

void GenerateClouds(ge_Scene* scene){
	int first_rand = rand()*geGetTick();
	srand(first_rand);
	int nClouds = scene->cloudsGenerator->n_clouds[0] + scene->cloudsGenerator->n_clouds[1] + scene->cloudsGenerator->n_clouds[2];
	scene->cloudsGenerator->clouds = (ge_Cloud*)geMalloc(sizeof(ge_Cloud)*nClouds);
	scene->cloudsGenerator->types = (u8*)geMalloc(sizeof(u8)*nClouds);

	int j = 0;
	for(j=0; j<nClouds; j++){
		GenerateCloud(scene, &scene->cloudsGenerator->clouds[j], j);
		scene->cloudsGenerator->clouds[j].ready = true;
	}
	ge_Thread* cloudsThread = geCreateThread("GE_clouds_thread", ge_clouds_thread, GE_THREAD_PRIORITY_LOWEST/*GE_THREAD_PRIORITY_NORMAL*/);
	geThreadStart(cloudsThread, sizeof(ge_Scene*), scene);
}
