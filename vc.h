//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLITECNICO DO CAVADO E DO AVE
//                          2020/2021
//             ENGENHARIA DE SISTEMAS INFORMATICOS
//                    VISAO POR COMPUTADOR
//					   TRABALHO PRATICO 1
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

// Extraçao do componente Blue da imagem RGB para Gray
int vc_rgb_get_blue_gray(IVC* srcdst);
// Converte uma imagem RGB para Gray (1 canal)
int vc_rgb_to_gray(IVC* src, IVC* dst);
//Imagem binaria a partir do treshold 
int vc_gray_to_binary(IVC* src, IVC* dst, int treshold);
//Dilatacao de imagem binaria
int vc_binary_dilate(IVC* src, IVC* dst, int kernel);
//Erosao de imagem binaria
int vc_binary_erode(IVC* src, IVC* dst, int kernel);
//Fechamento de imagem bin�ria
int vc_binary_close(IVC* src, IVC* dst, int kernel, int kernel2);
// Converte uma imagem binaria na imagem original igualmente binaria
int vc_binary_to_original(IVC* src, IVC* dst);

/*OVC*/
// Etiquetagem de blobs
OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels);
// Calcular a Area, Perimetro de cada objeto
int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs);
// Desenha uma caixa delimitadora em cada um dos objetos
int vc_draw_bouding_box(IVC* src, IVC* dst, OVC* blobs, int labels);
// Desenha o centro de massa do objeto
int vc_draw_center_mass(IVC* src, IVC* dst, OVC* blobs, int labels);