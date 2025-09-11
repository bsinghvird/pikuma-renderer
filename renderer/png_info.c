#include <stdlib.h>
#include "png_info.h"


void free_png_resources(png_info_t png_info)
{
	free(png_info.png_image);

}
