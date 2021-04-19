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

#define VC_DEBUG
#define MAX(a,b) (a>b?a:b) 
#define MIN(a,b) (a<b?a:b) 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


typedef struct {
	unsigned char* data;
	int width, height;
	int channels;			// Binario/Cinzentos=1; RGB=3
	int levels;				// Binario=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;

typedef struct {
	int x, y, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// Area
	int xc, yc;					// Centro-de-massa
	int perimeter;				// Perimetro
	int label;					// Etiqueta
} OVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROTOTIPOS DE FUNCOES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// FUNCOES: ALOCAR E LIBERTAR UMA IMAGEM
IVC* vc_image_new(int width, int height, int channels, int levels);
IVC* vc_image_free(IVC* image);

// FUNCOES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC* vc_read_image(char* filename);
int vc_write_image(char* filename, IVC* image);
int vc_gray_negative(IVC* srcdst);
int vc_rgb_negative(IVC* srcdst);
int vc_rgb_get_red_gray(IVC* srcdst);
int vc_rgb_get_green_gray(IVC* srcdst);
int vc_rgb_get_blue_gray(IVC* srcdst);
int vc_rgb_to_gray(IVC* src, IVC* dst);
int vc_rgb_to_hsv(IVC* src, IVC* dst);
int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);
int vc_scale_gray_to_rgb(IVC* src, IVC* dst); //1 canal -> 3 canais
int vc_gray_to_binary(IVC* src, IVC* dst, int treshold);
int vc_gray_to_binary_global_mean(IVC* src, IVC* dst);
int vc_gray_to_binary_midpoint(IVC* src, IVC* dst, int kernel);
int vc_binary_dilate(IVC* src, IVC* dst, int kernel);
int vc_binary_erode(IVC* src, IVC* dst, int kernel);
int vc_binary_open(IVC* src, IVC* dst, int kernel, int kernel2);
int vc_binary_close(IVC* src, IVC* dst, int kernel, int kernel2);
int vc_gray_histogram_show(IVC* src, IVC* dst);
int vc_gray_histogram_equalization(IVC* src, IVC* dst);

int vc_binary_to_original(IVC* src, IVC* dst);
/*OVC*/
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels);
int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs);