#include <stdio.h>
#include "vc.h"

/*int main(){
    IVC* image[5];

    image[0] = vc_read_image("P2/img2.ppm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_read_image():\n\tFile not found!\n");
		return 0;
	}

	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	if (image[1] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tFail to create file!\n");
		return 0;
	}

    image[2] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	if (image[2] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tFail to create file!\n");
		return 0;
	}
    image[3] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	if (image[3] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tFail to create file!\n");
		return 0;
	}
    image[4] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	if (image[4] == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tFail to create file!\n");
		return 0;
	}

    //vc_gray_to_binary_midpoint(image[0], image[1], 30); //120
    vc_gray_to_binary(image[0], image[1], 70);
    vc_binary_erode(image[1], image[2], 11);
    vc_binary_dilate(image[2], image[3], 13);
    vc_binary_close(image[3], image[4], 9, 11);

    vc_binary_to_original(image[4], image[0]);

    vc_write_image("result.pgm", image[1]);
	vc_write_image("result2.pgm", image[2]);
    vc_write_image("result3.pgm", image[3]);
    vc_write_image("result4.pgm", image[4]);
    vc_write_image("result5.pgm", image[0]);
    vc_image_free(image[0]);
	vc_image_free(image[1]);
    vc_image_free(image[2]);
    vc_image_free(image[3]);
    vc_image_free(image[4]);
    system("cmd /c start FilterGear P2/img2.ppm");
    system("cmd /c start FilterGear result.pgm");
    system("cmd /c start FilterGear result2.pgm");
    system("cmd /c start FilterGear result3.pgm");
    system("cmd /c start FilterGear result4.pgm");
    system("cmd /c start FilterGear result5.pgm");
}*/