//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITECNICO DO CAVADO E DO AVE
//                          2020/2021
//             ENGENHARIA DE SISTEMAS INFORMATICOS
//                    VISAO POR COMPUTADOR
//					   TRABALHO PRATICO 1
//						    PROBLEMA 2
//
//                [  JOAO AZEVEDO   - 18845  ]
//                [  JOAO RODRIGUES - 19431  ]
//                [  CARLOS SANTOS  - 19432  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include <stdio.h>
#include <stdlib.h>
#include "vc.h"

// Declaracao de constante com numero de imagens a usar
#define P2_IMAGES 5

//PROBLEMA 2
int main(){
	//Declaracao de variaveis
	IVC* image[P2_IMAGES];
	int labels, i;
	OVC* blobs;
	int aux;

	// Leitura da Imagem original para 2 imagens em memoria
	image[0] = vc_read_image("P2/img2.ppm");
	image[1] = vc_read_image("P2/img2.ppm");

	//Verificacao de erros na copia de imagens
	if (image[0] == NULL || image[1] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}

	// Criacao de imagens auxilares com 1 ou 3 canais
	image[2] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	image[3] = vc_image_new(image[0]->width, image[0]->height, 1, image[0]->levels);
	image[4] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);

	//Verificacao de erros na criacao de imagens
	if (image[2] == NULL || image[3] == NULL || image[4] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}

	//Extracao de uma imagem em tons de cinzentos com destaque para os azuis a partir da imagem original
	aux = vc_rgb_get_blue_gray(image[0]);
	if (aux==0)
	{
		printf("ERROR -> vc_rgb_get_blue_gray() \n");
		getchar();
		return 0;
	}

	//Transformacao de uma imagem em tons de cizentos de 3 canais para uma imagem de 1 canal
	aux = vc_rgb_to_gray(image[0], image[2]);
	if (aux==0)
	{
		printf("ERROR -> vc_rgb_to_gray() \n");
		getchar();
		return 0;
	}

	// Binarizacao por thresholding a partir da imagem de um canal obtida anteriormente
	aux = vc_gray_to_binary(image[2],image[3], 50);
	if (aux==0)
	{
		printf("ERROR -> vc_gray_to_binary() \n");
		getchar();
		return 0;
	}

	// Fechamento de modo a corrigir espaços recortados pela binarizacao por treshold
	aux = vc_binary_close(image[3],image[2], 3, 3);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_close() \n");
		getchar();
		return 0;
	}

	//Calculo da area, perimetro e centro de massa atraves de blobs e labelling de cada nucleo
	blobs = vc_binary_blob_labelling(image[2], image[3], &labels);
	if (blobs == NULL)
	{
		printf("ERROR -> vc_binary_blob_labelling() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_blob_info(image[3], blobs, labels);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_blob_info() \n");
		getchar();
		return 0;
	}

	printf("\nNumero de nucleos: %d\n", labels);
	for (i = 0; i < labels; i++)
		printf("-> Area Nucleo %d: %d pixeis\n", i+1, blobs[i].area);

	// Desenhar bounding boxes 
	aux = vc_draw_bouding_box(image[3], image[1], blobs, labels);
	if (aux==0)
	{
		printf("ERROR -> vc_draw_bouding_box() \n");
		getchar();
		return 0;
	}
	aux = vc_draw_center_mass(image[3], image[1], blobs, labels);
	if (aux==0)
	{
		printf("ERROR -> vc_draw_center_mass() \n");
		getchar();
		return 0;
	}

	//Escrita da imagem final em memória para ficheiro
	aux = vc_write_image("resultP2.ppm", image[1]);
	if (aux==0)
	{
		printf("ERROR -> vc_write_image() \n");
		getchar();
		return 0;
	}

	system("cmd /c start FilterGear resultP2.ppm");

	//Espaço de memória ocupado pelas imagens libertado
	for(i=0; i<P2_IMAGES; i++)
		vc_image_free(image[i]);
	
	return 1;
}