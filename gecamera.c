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
#include <math.h>

static float PI_180 = 0.01745329;
static float PI_90 = 1.57079632;
ge_Camera* ge_current_camera = NULL;

float* geGetMatrix(int which);

extern short mouse_smooth_warp_x;
extern short mouse_smooth_warp_y;

ge_Camera* geCreateCamera(){
	PI_180 = M_PI / 180;
	PI_90 = 90 * M_PI / 180;
	ge_Camera* cam = (ge_Camera*)geMalloc(sizeof(ge_Camera));
	return cam;
}

void geCameraSetMaximumAngles(ge_Camera* cam, float min, float max, int what){
	if(what == GE_CAMERA_VERTICAL){
		cam->minRotV = min;
		cam->maxRotV = max;
		cam->maxV = true;
	}else
	if(what == GE_CAMERA_HORIZONTAL){
		cam->minRotH = min;
		cam->maxRotH = max;
		cam->maxH = true;
	}else
	if(what == 0){
		cam->maxH = false;
		cam->maxV = false;
	}
}

void geFreeCamera(ge_Camera* cam){
	geFree(cam);
}

void geCameraLook(ge_Camera* cam){
	geMatrixMode(GE_MATRIX_VIEW);
	geLoadIdentity();
	geLookAt(cam->x,cam->y,cam->z, cam->cX,cam->cY,cam->cZ);
	ge_current_camera = cam;
}

void geCameraLookAt(ge_Camera* cam, float x, float y, float z, float cX, float cY, float cZ){
	cam->x = x;
	cam->y = y;
	cam->z = z;
	cam->cX = cX;
	cam->cY = cY;
	cam->cZ = cZ;
}

void geCameraRotateWithMouse(ge_Camera* cam, float sens){
	int warpX=0, warpY=0;
	geCursorWarp(&warpX, &warpY);

//	warpX = mouse_smooth_warp_x;
//	warpY = mouse_smooth_warp_y;

	cam->RotH -= warpX * sens;
	cam->RotV -= warpY * sens;
	
	if(cam->maxV){
		if(cam->RotV > cam->maxRotV){
			cam->RotV = cam->maxRotV;
		}else if(cam->RotV < cam->minRotV){
			cam->RotV = cam->minRotV;
		}
	}
	if(cam->maxH){
		if(cam->RotH > cam->maxRotH){
			cam->RotH = cam->maxRotH;
		}else if(cam->RotH < cam->minRotH){
			cam->RotH = cam->minRotH;
		}
	}

	cam->RotH_rad = cam->RotH*PI_180;
	cam->RotV_rad = cam->RotV*PI_180;
	geCameraFromAngles(cam, cam->RotH_rad, cam->RotV_rad);
}

void geCameraFromAngles(ge_Camera* cam, float RotH, float RotV){
	double rtemp = cos(RotV);
	cam->cZ = cam->z + 10000000.0 * sinf(RotV);
	cam->cX = ((double)cam->x) + (((double)10000000.0) * rtemp) * ((double)cosf(RotH));
	cam->cY = ((double)cam->y) + (((double)10000000.0) * rtemp) * ((double)sinf(RotH));
}

void geCameraMoveForward(ge_Camera* cam, float sensibility){
	cam->x += cosf(cam->RotH_rad) / sensibility;
	cam->y += sinf(cam->RotH_rad) / sensibility;
	geCameraFromAngles(cam, cam->RotH_rad, cam->RotV_rad);
}

void geCameraMoveBack(ge_Camera* cam, float sensibility){
	cam->x -= cosf(cam->RotH_rad) / sensibility;
	cam->y -= sinf(cam->RotH_rad) / sensibility;
	geCameraFromAngles(cam, cam->RotH_rad, cam->RotV_rad);
}

void geCameraMoveLeft(ge_Camera* cam, float sensibility){
	/*
	cam->x -= cosf(cam->RotH_rad-PI_90) / sensibility;
	cam->y -= sinf(cam->RotH_rad-PI_90) / sensibility;
	*/
	float x = cosf(cam->RotH_rad);
	float y = sinf(cam->RotH_rad);
	float x2 = x * 0.0 - y * -1.0;
	float y2 = x * -1.0 - y * 0.0;
	cam->x -= x2 / sensibility;
	cam->y -= y2 / sensibility;
}

void geCameraMoveRight(ge_Camera* cam, float sensibility){
	/*
	cam->x += cosf(cam->RotH_rad-PI_90) / sensibility;
	cam->y += sinf(cam->RotH_rad-PI_90) / sensibility;
	*/
	float x = cosf(cam->RotH_rad);
	float y = sinf(cam->RotH_rad);
	float x2 = x * 0.0 - y * -1.0;
	float y2 = x * -1.0 - y * 0.0;
	cam->x += x2 / sensibility;
	cam->y += y2 / sensibility;
}

void geCameraMoveForwardToPoint(ge_Camera* cam, float sensibility){
	float r_temp = cosf(cam->RotV_rad);
	cam->z += sinf(cam->RotV_rad) / sensibility;
	cam->x += r_temp* cosf(cam->RotH_rad) / sensibility;
	cam->y += r_temp* sinf(cam->RotH_rad) / sensibility;
/*	cam->x += cosf(cam->RotH_rad) / sensibility;
	cam->y += sinf(cam->RotH_rad) / sensibility;
	cam->z += (cosf(cam->RotV_rad)*sinf(cam->RotV_rad)) / sensibility;
*/
}

void geCameraMoveBackFromPoint(ge_Camera* cam, float sensibility){
	float r_temp = cosf(cam->RotV_rad);
	cam->z -= sinf(cam->RotV_rad) / sensibility;
	cam->x -= r_temp* cosf(cam->RotH_rad) / sensibility;
	cam->y -= r_temp* sinf(cam->RotH_rad) / sensibility;
/*	cam->x -= cosf(cam->RotH_rad) / sensibility;
	cam->y -= sinf(cam->RotH_rad) / sensibility;
	cam->z -= (cosf(cam->RotV_rad)*sinf(cam->RotV_rad)) / sensibility;
*/
}

void geCameraFollow(ge_Camera* cam, float x, float y, float z, float up, float camz, float min_distance, float speed){
	//	CameraPos = (CameraPosDesirée - CameraPos) * COEFF * DT
	float wanted_pos[3];

	cam->cX = x;
	cam->cY = y;
	cam->cZ = z + up;

	wanted_pos[0] = x;
	wanted_pos[1] = y;
	wanted_pos[2] = z;
	if(geDistance3D(cam->x, cam->y, cam->z, wanted_pos[0], wanted_pos[1], wanted_pos[2]) >= min_distance){
		cam->x += (wanted_pos[0] - cam->x) * speed;
		cam->y += (wanted_pos[1] - cam->y) * speed;
		if(camz == 0.0){
			cam->z += (wanted_pos[2] - cam->z) * speed;
		}else{
			cam->z = wanted_pos[2] + camz;
		}
	}

//	cam->z = z + camz;
}

void geCameraFollowMatrix(ge_Camera* cam, float* m, float up, float camz, float min_distance, float speed){
	float wanted_pos[3];

	cam->cX = m[12];
	cam->cY = m[13];
	cam->cZ = m[14] + up;

	wanted_pos[0] = m[12];
	wanted_pos[1] = m[13];
	wanted_pos[2] = m[14];
	if(geDistance3D(cam->x, cam->y, cam->z, wanted_pos[0], wanted_pos[1], wanted_pos[2]) >= min_distance){
		cam->x += (wanted_pos[0] - cam->x) * speed;
		cam->y += (wanted_pos[1] - cam->y) * speed;
		cam->z += (wanted_pos[2] - cam->z) * speed;
	}

//	cam->z = m[14] + camz;
}

void geCameraFollowAround(ge_Camera* cam, float cX, float cY, float cZ, float distance, float rotH, float rotV, float speed){
	cam->RotH = rotH;
	cam->RotV = rotV;
	cam->RotH_rad = cam->RotH*PI_180;
	cam->RotV_rad = cam->RotV*PI_180;
	
	cam->cX = cX;
	cam->cY = cY;
	cam->cZ = cZ;
	float r_temp = distance * cosf(cam->RotV_rad);
	cam->z = cZ + sinf(cam->RotV_rad);

	float wanted_pos[2];
	wanted_pos[0] = cX + r_temp* cosf(cam->RotH_rad);
	wanted_pos[1] = cY + r_temp* sinf(cam->RotH_rad);
//	if(geDistance2D(cam->x, cam->y, wanted_pos[0], wanted_pos[1]) >= distance){
		cam->x += (wanted_pos[0] - cam->x) * speed;
		cam->y += (wanted_pos[1] - cam->y) * speed;
//	}
/*
	cam->RotH = rotH;
	cam->RotV = rotV;
	cam->RotH_rad = cam->RotH*PI_180;
	cam->RotV_rad = cam->RotV*PI_180;

	float r_temp = distance * cosf(cam->RotV_rad);
	cam->z = cZ + sinf(cam->RotV_rad);
	cam->x = cX + r_temp* cosf(cam->RotH_rad);
	cam->y = cY + r_temp* sinf(cam->RotH_rad);
	cam->cX = cX;
	cam->cY = cY;
	cam->cZ = cZ;
*/
}










/* Follower test

		go_front = false;
		go_back = false;
		go_left = false;
		go_right = false;
		anim_run = false;

		float dist = geDistance2D(fcam->x, fcam->y, fcam->cX+0.1*geCos(guy_rotH/180.0*3.14), fcam->cY+0.1*geSin(guy_rotH/180.0*3.14));
		float dist2 = geDistance2D(fcam->x, fcam->y, fcam->cX+0.1*geCos((guy_rotH+180.0)/180.0*3.14), fcam->cY+0.1*geSin((guy_rotH+180.0)/180.0*3.14));
		gePrintDebug(0x100, "dist: %f\n", dist);

		if(keys->pressed[GEVK_UP]){
			ge_Translate(guy->default_matrix, 0.05, 0.0, 0.0);
			if(keys->pressed[GEVK_LEFT]){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, 1.5/180.0*3.14);
				guy_rotH += 1.5;
			}
			if(keys->pressed[GEVK_RIGHT]){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, -1.5/180.0*3.14);
				guy_rotH -= 1.5;
			}
			go_front = true;
			anim_run = true;
		}else
		if(keys->pressed[GEVK_DOWN]){
			ge_Translate(guy->default_matrix, 0.05, 0.0, 0.0);
			if(keys->pressed[GEVK_LEFT]){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, 1.5/180.0*3.14);
				guy_rotH += 1.5;
			}
			if(keys->pressed[GEVK_RIGHT]){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, -1.5/180.0*3.14);
				guy_rotH -= 1.5;
			}
			if(geKeysToggled(keys, GEVK_DOWN) && !go_back && dist < dist2){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, 180.0/180.0*3.14);
				guy_rotH += 180.0;
			}
			go_back = true;
			anim_run = true;
		}else
		if(keys->pressed[GEVK_LEFT]){
			if(geKeysToggled(keys, GEVK_LEFT)){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, 90.0/180.0*3.14);
				guy_rotH += 90.0;
			}else{
			//	ge_Rotate(guy->default_matrix, 0.0, 0.0, 1.5/180.0*3.14);
			//	guy_rotH += 1.5;
			}
			ge_Translate(guy->default_matrix, 0.05, 0.0, 0.0);
			go_left = true;
			anim_run = true;
		}else
		if(keys->pressed[GEVK_RIGHT]){
			if(geKeysToggled(keys, GEVK_RIGHT)){
				ge_Rotate(guy->default_matrix, 0.0, 0.0, -90.0/180.0*3.14);
				guy_rotH -= 90.0;
			}else{
			//	ge_Rotate(guy->default_matrix, 0.0, 0.0, -1.5/180.0*3.14);
			//	guy_rotH -= 1.5;
			}
			ge_Translate(guy->default_matrix, 0.05, 0.0, 0.0);
			go_right = true;
			anim_run = true;
		}
		
		if(anim_run){
			geAnimationStart(cd);
			geAnimationStart(jd);
			geAnimationStart(pd);
			geAnimationStart(bd);
			geAnimationStart(bg);
			geAnimationStart(cg);
			geAnimationStart(jg);
			geAnimationStart(pg);
		}else{
			geAnimationStop(cd);
			geAnimationStop(jd);
			geAnimationStop(pd);
			geAnimationStop(bd);
			geAnimationStop(bg);
			geAnimationStop(cg);
			geAnimationStop(jg);
			geAnimationStop(pg);
		}

		if(keys->pressed[GEVK_ACTION5]){
			ge_Translate(guy->default_matrix, 0.0, 0.0, 0.02);
		}
		if(keys->pressed[GEVK_ACTION6]){
			ge_Translate(guy->default_matrix, 0.0, 0.0, 0.02);
		}
		if(keys->pressed[GEVK_BACK]){
			break;
		}

		if(go_front){
			geCameraFollowMatrix(fcam, guy->default_matrix, 1.0, 2.8, 3.0, 0.008);
		}
		if(go_back){
			geCameraLookAt(fcam, fcam->x+(guy->default_matrix[12]-last_x), fcam->y+(guy->default_matrix[13]-last_y), fcam->z+(guy->default_matrix[14]-last_z), guy->default_matrix[12], guy->default_matrix[13], guy->default_matrix[14]+1.0);
		}
		if(go_left || go_right){
			geCameraLookAt(fcam, fcam->x+(guy->default_matrix[12]-last_x), fcam->y+(guy->default_matrix[13]-last_y), fcam->z+(guy->default_matrix[14]-last_z), guy->default_matrix[12], guy->default_matrix[13], guy->default_matrix[14]+1.0);
		}
		last_x = guy->default_matrix[12];
		last_y = guy->default_matrix[13];
		last_z = guy->default_matrix[14];
*/


/* Follower test 2

		bool anim_run = false;
		if(keys->pressed[GEVK_UP]){
			float rot = 0.0;
			rot += (cam_rotH - guy_rotH);
			if(geKeysToggled(keys, GEVK_LEFT)){
				rot += 45.0;
			}
			if(geKeysToggled(keys, GEVK_RIGHT)){
				rot -= 45.0;
			}
			if(geKeysUnToggled(keys, GEVK_LEFT)){
				rot -= 45.0;
			}
			if(geKeysUnToggled(keys, GEVK_RIGHT)){
				rot += 45.0;
			}
			ge_Rotate(guy->default_matrix, 0.0, 0.0, rot/180.0*3.14);
			ge_Translate(guy->default_matrix, 0.05, 0.0, 0.0);
			guy_rotH = cam_rotH;
			anim_run = true;
		}
		if(anim_run){
			geAnimationStart(cd);
			geAnimationStart(jd);
			geAnimationStart(pd);
			geAnimationStart(bd);
			geAnimationStart(bg);
			geAnimationStart(cg);
			geAnimationStart(jg);
			geAnimationStart(pg);
		}else{
			geAnimationStop(cd);
			geAnimationStop(jd);
			geAnimationStop(pd);
			geAnimationStop(bd);
			geAnimationStop(bg);
			geAnimationStop(cg);
			geAnimationStop(jg);
			geAnimationStop(pg);
		}


		if(keys->pressed[GEVK_ACTION2]){
			cam_rotH -= 0.4;
		}
		if(keys->pressed[GEVK_ACTION4]){
			cam_rotH += 0.4;
		}

		if(keys->pressed[GEVK_ACTION5]){
			ge_Translate(guy->default_matrix, 0.0, 0.0, 0.02);
		}
		if(keys->pressed[GEVK_ACTION6]){
			ge_Translate(guy->default_matrix, 0.0, 0.0, 0.02);
		}
		if(keys->pressed[GEVK_BACK]){
			break;
		}

		geCameraFollowAround(fcam, guy->default_matrix[12], guy->default_matrix[13], guy->default_matrix[14]+1.0, 4.0, cam_rotH, 30.0, 0.06);
*/
