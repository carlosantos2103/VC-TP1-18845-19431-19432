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

#include <stdio.h>
#include <stdlib.h>
#include "vc.h"


int main(){
    
	IVC* image[4];
	int label;

	// Leitura da Imagem 
	image[0] = vc_read_image("P1/img1.pgm");
	image[3] = vc_read_image("P1/img1.pgm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}
	
	// Criação de uma imagem copia da original
	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	image[2] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);

	if (image[1] == NULL || image[2] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}

	// Binarizacao por thresholding
	vc_gray_to_binary(image[0],image[1],70);

	// Dilatacao
	vc_binary_dilate(image[1], image[2], 3);
	vc_binary_dilate(image[2], image[1], 3);
	vc_binary_dilate(image[1], image[2], 3);
	vc_binary_dilate(image[2], image[1], 3);
	vc_binary_dilate(image[1], image[2], 3);


	// Erosao
	vc_binary_erode(image[2], image[1], 11);
	vc_binary_erode(image[1], image[2], 5);
	
	//vc_gray_binary_close(image[1], image[2], 7);

	// Meter Imagem original
	vc_binary_to_original(image[3], image[2]);
	
	vc_write_image("P1/testeimg1.pgm", image[2]);
	vc_image_free(image[0]);
	vc_image_free(image[1]);
	vc_image_free(image[2]);
	vc_image_free(image[3]);

	
	//system("cmd /c start FilterGear TP1/img1.pgm");
	//system("FilterGear TP1teste.pgm");
	
    system("cmd /c start FilterGear P1/img1.pgm");
	system("cmd /c start FilterGear P1/testeimg1.ppm");
	return 1;
}

//int main(void)
//{
//	IVC* image[4];
//	int label;
//
//	// Leitura da Imagem 
//	image[0] = vc_read_image("TP1/img1.pgm");
//	image[3] = vc_read_image("TP1/img1.pgm");
//	if (image[0] == NULL)
//	{
//		printf("ERROR -> vc_readimage() \n File not found!\n");
//		getchar();
//		return 0;
//	}
//	
//	// Criação de uma imagem copia da original
//	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
//	image[2] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
//
//	if (image[1] == NULL || image[2] == NULL)
//	{
//		printf("ERROR -> vc_readimage() \n File not found!\n");
//		getchar();
//		return 0;
//	}
//
//	// Binarizacao por thresholding
//	vc_gray_to_binary(image[0],70);
//
//	// Dilatacao
//	vc_gray_to_binary_dilate(image[0], image[1], 3);
//	vc_gray_to_binary_dilate(image[1], image[2], 3);
//	vc_gray_to_binary_dilate(image[2], image[1], 3);
//	vc_gray_to_binary_dilate(image[1], image[2], 3);
//	vc_gray_to_binary_dilate(image[2], image[1], 3);
//
//
//	// Erosao
//	vc_gray_to_binary_erode(image[1], image[2], 11);
//	vc_gray_to_binary_erode(image[2], image[1], 5);
//	
//	//vc_gray_binary_close(image[1], image[2], 7);
//
//	// Meter Imagem original
//	vc_original(image[3], image[1]);
//	
//	vc_write_image("TP1teste.pgm", image[1]);
//	vc_image_free(image[0]);
//	vc_image_free(image[1]);
//	vc_image_free(image[2]);
//	vc_image_free(image[3]);
//
//	
//	system("cmd /c start FilterGear TP1/img1.pgm");
//	system("FilterGear TP1teste.pgm");
//	
//	return 1;
//}