//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITECNICO DO CAVADO E DO AVE
//                          2020/2021
//             ENGENHARIA DE SISTEMAS INFORMATICOS
//                    VISAO POR COMPUTADOR
//					   TRABALHO PRATICO 1
//                          PROBLEMA 1
//
//                [  JOAO AZEVEDO   - 18845  ]
//                [  JOAO RODRIGUES - 19431  ]
//                [  CARLOS SANTOS  - 19432  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include <stdio.h>
#include <stdlib.h>
#include "vc.h"

// Declaracao de constante com numero de imagens a usar
#define P1_IMAGES 3

//PROBLEMA 1
int main(){
    //Declaracao de variaveis
	IVC* image[P1_IMAGES];
	int labels, i;
	OVC* blobs;
	int aux;

	// Leitura da Imagem original
	image[0] = vc_read_image("P1/img1.pgm");
	if (image[0] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}
	
	// Criação de imagens auxiliares copias da imagem original
	image[1] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);
	image[2] = vc_image_new(image[0]->width, image[0]->height, image[0]->channels, image[0]->levels);

	//Verificacao de erros
	if (image[1] == NULL || image[2] == NULL)
	{
		printf("ERROR -> vc_readimage() \n File not found!\n");
		getchar();
		return 0;
	}

	// Binarizacao por thresholding
	aux = vc_gray_to_binary(image[0],image[1],70);
	if (aux==0)
	{
		printf("ERROR -> vc_gray_to_binary() \n");
		getchar();
		return 0;
	}

	//Erosão seguida de dilatacao de forma a obter apenas a area do cerebro retirando o cranio
	// Erosao
	aux = vc_binary_erode(image[1], image[2], 3);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_erode() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_erode(image[2], image[1], 3);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_erode() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_erode(image[1], image[2], 3);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_erode() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_erode(image[2], image[1], 3);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_erode() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_erode(image[1], image[2], 3);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_erode() \n");
		getchar();
		return 0;
	}

	// Dilatacao
	aux = vc_binary_dilate(image[2], image[1], 11);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_dilate() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_dilate(image[1], image[2], 5);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_dilate() \n");
		getchar();
		return 0;
	}
	
	//Calculo da area, perimetro e centro de massa do cerebro atraves de blobs e labelling
	blobs = vc_binary_blob_labelling(image[2], image[1], &labels);
	if (blobs == NULL)
	{
		printf("ERROR -> vc_binary_blob_labelling() \n");
		getchar();
		return 0;
	}
	aux = vc_binary_blob_info(image[1], blobs, labels);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_blob_info() \n");
		getchar();
		return 0;
	}

	printf("\nObjeto: Cerebro\n");
	for (i = 0; i < labels; i++)
	{
		printf("-> Area %d\n", blobs[i].area);
		printf("-> Perimetro %d\n", blobs[i].perimeter);
		printf("-> Centro de Massa (%d,%d)\n", blobs[i].xc, blobs[i].yc);  //A REVER //funcao vc_binary_blob_info tinha a parte do centro de gravidade comentada
	}
	free(blobs);

	// Obter a Imagem original apenas com o cerebro a partir da imagem binaria
	aux = vc_binary_to_original(image[2], image[0]);
	if (aux==0)
	{
		printf("ERROR -> vc_binary_to_original() \n");
		getchar();
		return 0;
	}
	
	//Escrita da imagem final em memória para ficheiro
	aux = vc_write_image("resultP1.pgm", image[0]);
	if (aux==0)
	{
		printf("ERROR -> vc_write_image() \n");
		getchar();
		return 0;
	}
	
	system("cmd /c start FilterGear resultP1.pgm");

	//Espaço de memória ocupado pelas imagens libertado
	for(i=0; i<P1_IMAGES; i++)
		vc_image_free(image[i]);

	return 1;
}