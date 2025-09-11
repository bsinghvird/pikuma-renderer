#ifndef PNG_INFO_H
#define PNG_INFO_H



typedef struct {
	unsigned texture_width;
	unsigned texture_height;

	unsigned char* png_image;


} png_info_t;


void free_png_resources(png_info_t* png_info);



#endif // !PNG_INFO_H

