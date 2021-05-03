//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITECNICO DO CAVADO E DO AVE
//                          2020/2021
//             ENGENHARIA DE SISTEMAS INFORMATICOS
//                    VISAO POR COMPUTADOR
//
//                [  JOAO AZEVEDO   - 18845  ]
//                [  JOAO RODRIGUES - 19431  ]
//                [  CARLOS SANTOS  - 19432  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Desabilita (no MSVC++) warnings de funcies nao seguras (fopen, sscanf, etc...)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "vc.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            FUNCOES: ALOCAR E LIBERTAR UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Alocar memoria para uma imagem
IVC* vc_image_new(int width, int height, int channels, int levels)
{
	IVC* image = (IVC*)malloc(sizeof(IVC));

	if (image == NULL) return NULL;
	if ((levels <= 0) || (levels > 255)) return NULL;

	image->width = width;
	image->height = height;
	image->channels = channels;
	image->levels = levels;
	image->bytesperline = image->width * image->channels;
	//Alocar menória para uma imagem;
	image->data = (unsigned char*)malloc(image->width * image->height * image->channels * sizeof(char));

	//Se a imagem a imagem não existir liberta a sua memória
	if (image->data == NULL)
	{
		return vc_image_free(image);
	}

	return image;
}


// Libertar memoria de uma imagem
IVC* vc_image_free(IVC* image)
{
	if (image != NULL)
	{
		if (image->data != NULL)
		{
			free(image->data);
			image->data = NULL;
		}

		free(image);
		image = NULL;
	}

	return image;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    FUNÇOES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


char* netpbm_get_token(FILE* file, char* tok, int len)
{
	char* t;
	int c;

	for (;;)
	{
		while (isspace(c = getc(file)));
		if (c != '#') break;
		do c = getc(file);
		while ((c != '\n') && (c != EOF));
		if (c == EOF) break;
	}

	t = tok;

	if (c != EOF)
	{
		do
		{
			*t++ = c;
			c = getc(file);
		} while ((!isspace(c)) && (c != '#') && (c != EOF) && (t - tok < len - 1));

		if (c == '#') ungetc(c, file);
	}

	*t = 0;

	return tok;
}

long int unsigned_char_to_bit(unsigned char* datauchar, unsigned char* databit, int width, int height)
{
	int x, y;
	int countbits;
	long int pos, counttotalbytes;
	unsigned char* p = databit;

	*p = 0;
	countbits = 1;
	counttotalbytes = 0;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//*p |= (datauchar[pos] != 0) << (8 - countbits);

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				*p |= (datauchar[pos] == 0) << (8 - countbits);

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				*p = 0;
				countbits = 1;
				counttotalbytes++;
			}
		}
	}

	return counttotalbytes;
}

void bit_to_unsigned_char(unsigned char* databit, unsigned char* datauchar, int width, int height)
{
	int x, y;
	int countbits;
	long int pos;
	unsigned char* p = databit;

	countbits = 1;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = width * y + x;

			if (countbits <= 8)
			{
				// Numa imagem PBM:
				// 1 = Preto
				// 0 = Branco
				//datauchar[pos] = (*p & (1 << (8 - countbits))) ? 1 : 0;

				// Na nossa imagem:
				// 1 = Branco
				// 0 = Preto
				datauchar[pos] = (*p & (1 << (8 - countbits))) ? 0 : 1;

				countbits++;
			}
			if ((countbits > 8) || (x == width - 1))
			{
				p++;
				countbits = 1;
			}
		}
	}
}

IVC* vc_read_image(char* filename)
{
	FILE* file = NULL;
	IVC* image = NULL;
	unsigned char* tmp;
	char tok[20];
	long int size, sizeofbinarydata;
	int width, height, channels;
	int levels = 255;
	int v;

	// Abre o ficheiro
	if ((file = fopen(filename, "rb")) != NULL)
	{
		// Efectua a leitura do header
		netpbm_get_token(file, tok, sizeof(tok));

		if (strcmp(tok, "P4") == 0) { channels = 1; levels = 1; }	// Se PBM (Binary [0,1])
		else if (strcmp(tok, "P5") == 0) channels = 1;				// Se PGM (Gray [0,MAX(level,255)])
		else if (strcmp(tok, "P6") == 0) channels = 3;				// Se PPM (RGB [0,MAX(level,255)])
		else
		{
#ifdef VC_DEBUG
			printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM, PGM or PPM file.\n\tBad magic number!\n");
#endif

			fclose(file);
			return NULL;
		}

		if (levels == 1) // PBM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PBM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL) return NULL;

			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height;
			tmp = (unsigned char*)malloc(sizeofbinarydata);
			if (tmp == NULL) return 0;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			if ((v = fread(tmp, sizeof(unsigned char), sizeofbinarydata, file)) != sizeofbinarydata)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				free(tmp);
				return NULL;
			}

			bit_to_unsigned_char(tmp, image->data, image->width, image->height);

			free(tmp);
		}
		else // PGM ou PPM
		{
			if (sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &width) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &height) != 1 ||
				sscanf(netpbm_get_token(file, tok, sizeof(tok)), "%d", &levels) != 1 || levels <= 0 || levels > 255)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tFile is not a valid PGM or PPM file.\n\tBad size!\n");
#endif

				fclose(file);
				return NULL;
			}

			// Aloca mem�ria para imagem
			image = vc_image_new(width, height, channels, levels);
			if (image == NULL) return NULL;

#ifdef VC_DEBUG
			printf("\nchannels=%d w=%d h=%d levels=%d\n", image->channels, image->width, image->height, levels);
#endif

			size = image->width * image->height * image->channels;

			if ((v = fread(image->data, sizeof(unsigned char), size, file)) != size)
			{
#ifdef VC_DEBUG
				printf("ERROR -> vc_read_image():\n\tPremature EOF on file.\n");
#endif

				vc_image_free(image);
				fclose(file);
				return NULL;
			}
		}

		fclose(file);
	}
	else
	{
#ifdef VC_DEBUG
		printf("ERROR -> vc_read_image():\n\tFile not found.\n");
#endif
	}

	return image;
}

int vc_write_image(char* filename, IVC* image)
{
	FILE* file = NULL;
	unsigned char* tmp;
	long int totalbytes, sizeofbinarydata;

	if (image == NULL) return 0;

	if ((file = fopen(filename, "wb")) != NULL)
	{
		if (image->levels == 1)
		{
			sizeofbinarydata = (image->width / 8 + ((image->width % 8) ? 1 : 0)) * image->height + 1;
			tmp = (unsigned char*)malloc(sizeofbinarydata);
			if (tmp == NULL) return 0;

			fprintf(file, "%s %d %d\n", "P4", image->width, image->height);

			totalbytes = unsigned_char_to_bit(image->data, tmp, image->width, image->height);
			printf("Total = %ld\n", totalbytes);
			if (fwrite(tmp, sizeof(unsigned char), totalbytes, file) != totalbytes)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				free(tmp);
				return 0;
			}

			free(tmp);
		}
		else
		{
			fprintf(file, "%s %d %d 255\n", (image->channels == 1) ? "P5" : "P6", image->width, image->height);

			if (fwrite(image->data, image->bytesperline, image->height, file) != image->height)
			{
#ifdef VC_DEBUG
				fprintf(stderr, "ERROR -> vc_read_image():\n\tError writing PBM, PGM or PPM file.\n");
#endif

				fclose(file);
				return 0;
			}
		}

		fclose(file);

		return 1;
	}

	return 0;
}

//Extraçao do componente Blue da imagem RGB para Gray
int vc_rgb_get_blue_gray(IVC* srcdst)
{
	unsigned char* data = (unsigned char*)srcdst->data;
	int width = srcdst->width;
	int height = srcdst->height;
	int bytesperline = srcdst->width * srcdst->channels;
	int channels = srcdst->channels;
	int x, y;
	long int pos;

	//Verifica��o de erros
	if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
	if (channels != 3) return 0;

	//Inverter a imagem RGB
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels;

			data[pos] = (unsigned char)data[pos + 2];
			data[pos + 1] = (unsigned char)data[pos + 2];
		}
	}
	return 1;
}

//Converter RGB para Gray (1 canal)
int vc_rgb_to_gray(IVC* src, IVC* dst)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	int bytesperline_src = src->width * src->channels;
	int channels_src = src->channels;
	unsigned char* datadst = (unsigned char*)dst->data;
	int bytesperline_dst = dst->width * dst->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int x, y;
	long int pos_src, pos_dst;
	float rf, gf, bf;

	//Verificacao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height)) return 0;
	if ((src->channels != 3) || (dst->channels != 1)) return 0;

	//Inverter a imagem RGB
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos_src = y * bytesperline_src + x * channels_src;
			pos_dst = y * bytesperline_dst + x * channels_dst;

			rf = (float)datasrc[pos_src];
			gf = (float)datasrc[pos_src + 1];
			bf = (float)datasrc[pos_src + 2];

			datadst[pos_dst] = (unsigned char)((rf * 0.299) + (gf * 0.587) + (bf * 0.114));
		}
	}
	return 1;
}

//Imagem binaria a partir do treshold 
int vc_gray_to_binary(IVC* src, IVC* dst, int treshold)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int bytesperline = src->width * src->channels;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	long int pos;
	int x, y;

	// Verifica��o de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL)) return 0;
	if (width != dst->width || height != dst->height) return 0;
	if (channels_src != 1 || channels_dst != 1) return 0;

	//Percorrer todos os pixeis da imagem fonte
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			pos = y * bytesperline + x * channels_src;

			if (datasrc[pos] > treshold) {
				datadst[pos] = 255;
			}
			else {
				datadst[pos] = 0;
			}
		}
	}
	return 1;
}

//Dilatacao de imagem binaria
int vc_binary_dilate(IVC* src, IVC* dst, int kernel)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int bytesperline = src->width * src->channels;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int offset = (kernel - 1) / 2;
	long int pos;
	int x, y, x2, y2, aux;

	// Verificacao de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL)) return 0;
	if (width != dst->width || height != dst->height) return 0;
	if (channels_src != 1 || channels_dst != 1) return 0;

	//Calculo media da vizinhanca de cada pixel
	//Percorre todos os pixeis da imagem
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			//Redefine o valor da variavel auxiliar a 0
			aux = 0;
			//Percorre o offset(vizinhanca) em y
			for (y2 = y - offset; y2 <= y + offset; y2++)
			{
				//Verifica se o pixel em y existe
				if (y2 > -1 && y2 < height)
					//Percorre o offset(vizinhanca) em x
					for (x2 = x - offset; x2 <= x + offset; x2++)
					{
						//Verifica se o pixel em x existe
						if (x2 > -1 && x2 < width) {
							pos = y2 * bytesperline + x2 * channels_src;
							//Verifica o plano do pixel vizinho
							if (datasrc[pos] != 0) {
								aux = 1;
								break;
							}
						}
					}
			}
			pos = y * bytesperline + x * channels_src;
			//Caso o pixel em questao tenha um pixel vizinho do primeiro plano torna esse pixel do primeiro plano
			if (aux == 1)
				datadst[pos] = 255;
			else
				datadst[pos] = 0;
		}
	}
	return 1;
}

//Erosao de imagem binaria
int vc_binary_erode(IVC* src, IVC* dst, int kernel)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int bytesperline = src->width * src->channels;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int offset = (kernel - 1) / 2;
	long int pos;
	int x, y, x2, y2, aux;

	// Verificaçao de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL)) return 0;
	if (width != dst->width || height != dst->height) return 0;
	if (channels_src != 1 || channels_dst != 1) return 0;

	//Calculo media da vizinhanca de cada pixel
	//Percorre todos os pixeis da imagem
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			//Redefine o valor da variavel auxiliar a 0
			aux = 0;
			//Percorre o offset(vizinhanca) em y
			for (y2 = y - offset; y2 <= y + offset; y2++)
			{
				//Verifica se o pixel em y existe
				if (y2 > -1 && y2 < height)
					//Percorre o offset(vizinhanca) em x
					for (x2 = x - offset; x2 <= x + offset; x2++)
					{
						//Verifica se o pixel em x existe
						if (x2 > -1 && x2 < width) {
							pos = y2 * bytesperline + x2 * channels_src;
							//Verifica o plano do pixel vizinho
							if (datasrc[pos] == 0) {
								aux = 1;
								break;
							}
						}
					}
			}
			pos = y * bytesperline + x * channels_src;

			//Caso o pixel em questao tenha um pixel vizinho do plano de fundo torna esse pixel do plano de fundo
			if (aux == 1)
				datadst[pos] = 0;
			else
				datadst[pos] = 255;
		}
	}
	return 1;
}

//close de imagem binaria
int vc_binary_close(IVC* src, IVC* dst, int kernel, int kernel2)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	int channels_src = src->channels;
	int channels_dst = dst->channels;
	int width = src->width;
	int height = src->height;
	int ret = 1;

	// Verificacao de erros
	if ((width <= 0) || (height <= 0) || (datasrc == NULL)) return 0;
	if (width != dst->width || height != dst->height) return 0;
	if (channels_src != 1 || channels_dst != 1) return 0;

	// Cria uma imagem binaria
	IVC* aux = vc_image_new(width, height, channels_src, src->levels);
	// Verificacao de erros
	if (aux == NULL)
	{
		printf("ERROR -> vc_image_new():\n\tFail to create file!\n");
		return 0;
	}

	//Faz a dilatacao da imagem src para a imagem auxiliar com o primeiro kernel
	ret &= vc_binary_dilate(src, aux, kernel);
	//Faz a erosao da imagem auxilar para a imagem destino com o segundo kernel
	ret &= vc_binary_erode(aux, dst, kernel2);

	//Libertacao de memoria da imagem auxiliar
	vc_image_free(aux);

	return ret;
}

// Etiquetagem de blobs
// src		: Imagem binaria de entrada
// dst		: Imagem grayscale (ira conter as etiquetas)
// nlabels	: Endereço de memoria de uma variavel, onde sera armazenado o numero de etiquetas encontradas.
// OVC*		: Retorna um array de estruturas de blobs (objectos), com respectivas etiquetas. E necessario libertar posteriormente esta memoria.
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels)
{
	unsigned char* datasrc = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, a, b;
	long int i, size;
	long int posX, posA, posB, posC, posD;
	int labeltable[256] = { 0 };
	int labelarea[256] = { 0 };
	int label = 1; // Etiqueta inicial.
	int num, tmplabel;
	OVC* blobs; // Apontador para array de blobs (objectos) que sera retornado desta funcao.

	// Verificacao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if ((src->width != dst->width) || (src->height != dst->height) || (src->channels != dst->channels)) return NULL;
	if (channels != 1) return NULL;

	// Copia dados da imagem binaria para imagem grayscale
	memcpy(datadst, datasrc, bytesperline * height);

	// Todos os pixeis de plano de fundo devem obrigatoriamente ter valor 0
	// Todos os pixeis de primeiro plano devem obrigatoriamente ter valor 255
	// Serao atribuidas etiquetas no intervalo [1,254]
	// Este algoritmo esta assim limitado a 255 labels
	for (i = 0, size = bytesperline * height; i < size; i++)
	{
		if (datadst[i] != 0) datadst[i] = 255;
	}

	// Limpa os rebordos da imagem binaria
	for (y = 0; y < height; y++)
	{
		datadst[y * bytesperline + 0 * channels] = 0;
		datadst[y * bytesperline + (width - 1) * channels] = 0;
	}
	for (x = 0; x < width; x++)
	{
		datadst[0 * bytesperline + x * channels] = 0;
		datadst[(height - 1) * bytesperline + x * channels] = 0;
	}

	// Efectua a etiquetagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			// Kernel:
			// A B C
			// D X

			posA = (y - 1) * bytesperline + (x - 1) * channels; // A
			posB = (y - 1) * bytesperline + x * channels; // B
			posC = (y - 1) * bytesperline + (x + 1) * channels; // C
			posD = y * bytesperline + (x - 1) * channels; // D
			posX = y * bytesperline + x * channels; // X

			// Se o pixel foi marcado
			if (datadst[posX] != 0)
			{
				if ((datadst[posA] == 0) && (datadst[posB] == 0) && (datadst[posC] == 0) && (datadst[posD] == 0))
				{
					datadst[posX] = label;
					labeltable[label] = label;
					label++;
				}
				else
				{
					num = 255;

					// Se A esta marcado
					if (datadst[posA] != 0) num = labeltable[datadst[posA]];
					// Se B esta marcado, e e menor que a etiqueta "num"
					if ((datadst[posB] != 0) && (labeltable[datadst[posB]] < num)) num = labeltable[datadst[posB]];
					// Se C esta marcado, e e menor que a etiqueta "num"
					if ((datadst[posC] != 0) && (labeltable[datadst[posC]] < num)) num = labeltable[datadst[posC]];
					// Se D esta marcado, e e menor que a etiqueta "num"
					if ((datadst[posD] != 0) && (labeltable[datadst[posD]] < num)) num = labeltable[datadst[posD]];

					// Atribui a etiqueta ao pixel
					datadst[posX] = num;
					labeltable[num] = num;

					// Actualiza a tabela de etiquetas
					if (datadst[posA] != 0)
					{
						if (labeltable[datadst[posA]] != num)
						{
							for (tmplabel = labeltable[datadst[posA]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posB] != 0)
					{
						if (labeltable[datadst[posB]] != num)
						{
							for (tmplabel = labeltable[datadst[posB]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posC] != 0)
					{
						if (labeltable[datadst[posC]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
					if (datadst[posD] != 0)
					{
						if (labeltable[datadst[posD]] != num)
						{
							for (tmplabel = labeltable[datadst[posC]], a = 1; a < label; a++)
							{
								if (labeltable[a] == tmplabel)
								{
									labeltable[a] = num;
								}
							}
						}
					}
				}
			}
		}
	}

	// Volta a etiquetar a imagem
	for (y = 1; y < height - 1; y++)
	{
		for (x = 1; x < width - 1; x++)
		{
			posX = y * bytesperline + x * channels; // X

			if (datadst[posX] != 0)
			{
				datadst[posX] = labeltable[datadst[posX]];
			}
		}
	}

	//printf("\nMax Label = %d\n", label);

	// Contagem do numero de blobs
	// Passo 1: Eliminar, da tabela, etiquetas repetidas
	for (a = 1; a < label - 1; a++)
	{
		for (b = a + 1; b < label; b++)
		{
			if (labeltable[a] == labeltable[b]) labeltable[b] = 0;
		}
	}
	// Passo 2: Conta etiquetas e organiza a tabela de etiquetas, para que nao hajam valores vazios (zero) entre etiquetas
	*nlabels = 0;
	for (a = 1; a < label; a++)
	{
		if (labeltable[a] != 0)
		{
			labeltable[*nlabels] = labeltable[a]; // Organiza tabela de etiquetas
			(*nlabels)++; // Conta etiquetas
		}
	}

	// Se nao ha blobs
	if (*nlabels == 0) return NULL;

	// Cria lista de blobs (objectos) e preenche a etiqueta
	blobs = (OVC*)calloc((*nlabels), sizeof(OVC));
	if (blobs != NULL)
	{
		for (a = 0; a < (*nlabels); a++) blobs[a].label = labeltable[a];
	}
	else return NULL;

	return blobs;
}

// Calcular a Area, Perimetro de cada objeto
int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs)
{
	unsigned char* data = (unsigned char*)src->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->bytesperline;
	int channels = src->channels;
	int x, y, i;
	long int pos;
	int xmin, ymin, xmax, ymax;
	long int sumx, sumy;

	// Verificaçao de erros
	if ((src->width <= 0) || (src->height <= 0) || (src->data == NULL)) return 0;
	if (channels != 1) return 0;

	// Conta area de cada blob
	for (i = 0; i < nblobs; i++)
	{
		xmin = width - 1;
		ymin = height - 1;
		xmax = 0;
		ymax = 0;

		sumx = 0;
		sumy = 0;

		blobs[i].area = 0;

		for (y = 1; y < height - 1; y++)
		{
			for (x = 1; x < width - 1; x++)
			{
				pos = y * bytesperline + x * channels;

				if (data[pos] == blobs[i].label)
				{
					// Area
					blobs[i].area++;

					// Centro de Gravidade
					sumx += x;
					sumy += y;

					// Bounding Box
					if (xmin > x) xmin = x;
					if (ymin > y) ymin = y;
					if (xmax < x) xmax = x;
					if (ymax < y) ymax = y;

					// Perimetro
					// Se pelo menos um dos quatro vizinhos nao pertence ao mesmo label, entao e um pixel de contorno
					if ((data[pos - 1] != blobs[i].label) || (data[pos + 1] != blobs[i].label) || (data[pos - bytesperline] != blobs[i].label) || (data[pos + bytesperline] != blobs[i].label))
					{
						blobs[i].perimeter++;
					}
				}
			}
		}

		// Bounding Box
		blobs[i].x = xmin;
		blobs[i].y = ymin;
		blobs[i].width = (xmax - xmin) + 1;
		blobs[i].height = (ymax - ymin) + 1;

		// Centro de Gravidade
		/*blobs[i].xc = (xmax - xmin) / 2;
		blobs[i].yc = (ymax - ymin) / 2;*/
		blobs[i].xc = sumx / MAX(blobs[i].area, 1);
		blobs[i].yc = sumy / MAX(blobs[i].area, 1);
	}

	return 1;
}

// Converte uma imagem binaria na imagem original igualmente binaria
int vc_binary_to_original(IVC* src, IVC* dst)
{
	unsigned char* data = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->width * src->channels;
	int channels = src->channels;
	int x, y;
	long int pos;
	int npixels = width * height;

	//Verificaçao de erros
	if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
	if (channels != 1) return 0;

	// Percorre todos os pixeis da imagem
	for (pos = 0; pos < npixels; pos++)
		// Caso o pixel dessa imagem seja preto a imagem de destino tb fica com o pixel preto, senao nao muda a cor do pixel de destino
		if (data[pos]==0) datadst[pos]=0;

	return 1;
}

// Desenha uma caixa delimitadora em cada um dos objetos
int vc_draw_bouding_box(IVC* src, IVC* dst, OVC* blobs, int labels)
{
	unsigned char* data = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->width * src->channels;
	int channels = src->channels;
	int channels_dst = dst->channels;
	int x, y, i;
	long int pos, pos_dst;
	int npixels = width * height;

	//Verificacao de erros
	if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
	if (channels != 1 || channels_dst != 3) return 0;
	//Verifica se existe blobs
	if (blobs != NULL)
	{
		// Percorre todos os objetos existentes
		for (i = 0; i < labels; i++)
		{
			// Percorre todos os pixeis da imagem
			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					// Calculo da posicao da imagem cinzenta, com 1 channel
					pos = y * bytesperline + x * channels;
					// Calculo da posicao da imagem RGB, com 3 channel
					pos_dst = y * dst->bytesperline + x * dst->channels;
					// Caso o y e x estejam compreendidos entre os parametros dos blobs desenha-se a caixa delimitadora
					if (y >= blobs[i].y && y <= blobs[i].y+blobs[i].height && x >= blobs[i].x && x <= blobs[i].x+blobs[i].width)
						if (x == blobs[i].x || x == blobs[i].x+blobs[i].width){
							datadst[pos_dst] = 255;
							datadst[pos_dst+1] = 255;
							datadst[pos_dst+2] = 255;
						}
						else if (y == blobs[i].y || y == blobs[i].y+blobs[i].height){
							datadst[pos_dst] = 255;
							datadst[pos_dst+1] = 255;
							datadst[pos_dst+2] = 255;
						}
				}
			}
		}
	}

	return 1;
}

// Desenha o centro de massa do objeto
int vc_draw_center_mass(IVC* src, IVC* dst, OVC* blobs, int labels)
{
	unsigned char* data = (unsigned char*)src->data;
	unsigned char* datadst = (unsigned char*)dst->data;
	int width = src->width;
	int height = src->height;
	int bytesperline = src->width * src->channels;
	int channels = src->channels;
	int channels_dst = dst->channels;
	int x, y, i, j;
	long int pos, pos_dst;

	//Verificacao de erros
	if ((width <= 0) || (height <= 0) || (data == NULL)) return 0;
	if (channels != 1 || channels_dst != 3) return 0;
	//Verifica se existe blobs
	if (blobs != NULL)
	{
		// Percorre todos os objetos existentes
		for (i = 0; i < labels; i++)
		{
			// Percorre todos os pixeis da imagem
			for (y = 0; y < height; y++)
			{
				// Caso o y corresponda ao valor do blobg calculado em y
				if (y==blobs[i].yc){
					for (x = 0; x < width; x++)
					{
						// Caso o x corresponda ao valor do blobg calculado em x
						if(x==blobs[i].xc) {
							// Calculo da posicao
							pos_dst = y * dst->bytesperline + x * dst->channels;

							// Alteracao dos pixeis para a cor branca de forma a salientar o centro de massa

							// Pixel central
							datadst[pos_dst] = 255;
							datadst[pos_dst+1] = 255;
							datadst[pos_dst+2] = 255;

							// Pixel seguinte
							datadst[pos_dst+3] = 255;
							datadst[pos_dst+4] = 255;
							datadst[pos_dst+5] = 255;

							// Pixel Anterior
							datadst[pos_dst-1] = 255;
							datadst[pos_dst-2] = 255;
							datadst[pos_dst-3] = 255;
							
							// Pixel superior em relacao ao pixel central
							datadst[pos_dst-width*3] = 255;
							datadst[pos_dst-width*3+1] = 255;
							datadst[pos_dst-width*3+2] = 255;

							// Pixel inferior em relacao ao pixel central
							datadst[pos_dst+width*3] = 255;
							datadst[pos_dst+width*3+1] = 255;
							datadst[pos_dst+width*3+2] = 255;

							break;
						}
					}
					break;
				}
			}
		}
	}

	return 1;
}