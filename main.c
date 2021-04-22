#include <stdio.h>
#include <stdlib.h>
#include "vc.h"

/*int main(){
    
	IVC* image[4];

	// Leitura da Imagem original
	image[0] = vc_read_image("P1/img1.pgm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}
	
	// Criação de uma imagem copia da original
	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	image[2] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	//image[3] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);

	//Verificacao de erros
	if (image[1] == NULL || image[2] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}

	// Binarizacao por thresholding
	vc_gray_to_binary(image[0],image[1],70);

	// Erosao
	vc_binary_erode(image[1], image[2], 3);
	vc_binary_erode(image[2], image[1], 3);
	vc_binary_erode(image[1], image[2], 3);
	vc_binary_erode(image[2], image[1], 3);
	vc_binary_erode(image[1], image[2], 3);

	// Dilatacao
	vc_binary_dilate(image[2], image[1], 11);
	vc_binary_dilate(image[1], image[2], 5);
	
	///vc_binary_close(image[2], image[1], 5, 7); //FAZER O CLOSE?

	//Calculo da area, perimetro e centro de massa atraves de blobs e labelling
	int labels, i;
	OVC* blobs;
	blobs = vc_binary_blob_labelling(image[2], image[1], &labels);
	vc_binary_blob_info(image[1], blobs, labels);
	if (blobs != NULL)
	{
		printf("\nObjeto: Cerebro");
		for (i = 0; i < labels; i++)
		{
			printf("-> Area %d\n", blobs[i].area);
			printf("-> Perimetro %d\n", blobs[i].perimeter);
			printf("-> Centro de Massa (%d,%d)\n", blobs[i].xc, blobs[i].yc);  //A REVER //funcao vc_binary_blob_info tinha a parte do centro de gravidade comentada
		}
		free(blobs);
	}

	// Voltar para a Imagem original apenas com o cerebro
	vc_binary_to_original(image[2], image[0]);

	//vc_draw_bouding_box(image[0], image[3], blobs, labels);
	
	vc_write_image("result.pgm", image[0]);
	vc_image_free(image[0]);
	vc_image_free(image[1]);
	vc_image_free(image[2]);

	//vc_image_free(image[3]);

	system("cmd /c start FilterGear P1/img1.pgm");
    system("cmd /c start FilterGear result.pgm");

	//system("cmd /c start FilterGear result2.pgm");

	return 1;
}*/

int main(){
    
	IVC* image[4];

	// Leitura da Imagem original
	image[0] = vc_read_image("P2/img2.ppm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}
	
	// Criação de uma imagem copia da original
	image[1] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	image[2] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	image[3] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels); //3

	//Verificacao de erros
	if (image[1] == NULL || image[2] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}

	vc_rgb_get_blue_gray(image[0]);
	vc_rgb_to_gray(image[0], image[1]);
	vc_gray_to_binary(image[1],image[2], 50); //1
	//vc_gray_to_binary_global_mean(image[1],image[2]);
	vc_binary_close(image[2],image[1], 3, 3);


	//Calculo da area, perimetro e centro de massa atraves de blobs e labelling (30)
	int labels, i;
	OVC* blobs;
	blobs = vc_binary_blob_labelling(image[1], image[2], &labels);
	vc_binary_blob_info(image[2], blobs, labels);
	if (blobs != NULL)
	{
		printf("\nNumero de nucleos: %d\n", labels);
		for (i = 0; i < labels; i++)
		{
			printf("-> Area Nucleo %d: %d\n", i+1, blobs[i].area);
		}
	}

	vc_draw_center_mass(image[2], image[1], blobs, labels);
	vc_draw_bouding_box(image[1], image[2], blobs, labels);

	vc_write_image("result.pgm", image[2]);

	vc_image_free(image[0]);
	vc_image_free(image[1]);
	vc_image_free(image[2]);

	system("cmd /c start FilterGear P2/img2.ppm");
    system("cmd /c start FilterGear result.pgm");

	return 1;
}