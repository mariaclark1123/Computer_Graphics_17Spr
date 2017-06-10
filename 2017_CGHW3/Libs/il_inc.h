#pragma comment (lib, "DevIL.lib")

#include "IL/il.h"

inline void il_init(){
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_FILE_OVERWRITE);
}

// RGBA, unsigned byte, wchar_t* L"??"
inline void il_readImg(const wchar_t* name, void** data, int* w, int* h){
	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ILboolean r = ilLoadImage(name);
	*h = ilGetInteger(IL_IMAGE_HEIGHT);
	*w = ilGetInteger(IL_IMAGE_WIDTH);
	*data = new unsigned char[*w * *h * 4];
	ILuint n = ilCopyPixels(0,0,0,*w,*h,1,IL_RGBA,IL_UNSIGNED_BYTE,*data);
	ilDeleteImage(img);
	ILenum error = ilGetError();
}

// RGBA, unsigned byte, wchar_t* L"??"
inline void il_saveImg(const wchar_t* name, void* data, int w, int h){
	ILuint img;
	ilGenImages(1, &img);
	ilBindImage(img);
	ILboolean r = ilTexImage(w,h,1,4,IL_RGBA,IL_UNSIGNED_BYTE,data);
	r = ilSaveImage(name);
	ilDeleteImage(img);
	ILenum error = ilGetError();
}

// depth float
inline void il_saveImgDep(const wchar_t* name, float* data, int w, int h)
{
	unsigned char rgba[4] = {0xff,0xff,0xff,0xff};
	for(int i=0; i<w*h; ++i){
		rgba[0]=rgba[1]=rgba[2] = unsigned char(data[i]*255);
		*((int*)(&data[i])) = *((int*)rgba);
	}
	il_saveImg(name,data,w,h);
}

