#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <string.h>
using namespace std;

typedef struct RGB {
	BYTE r;
	BYTE g;
	BYTE b;
} RGB;

typedef struct YUV {
	BYTE y;
	BYTE u;
	BYTE v;
} YUV;

BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
RGB *img_rgb;
YUV *img_yuv;

void readBmp(char *filename);
void saveBmp(char *filename);
YUV trans_RGB2YUV(RGB rgb);
RGB trans_YUV2RGB(YUV yuv);
int argPos(char *str, int argc, char **argv);

int main(int argc, char **argv){
	int i;
	char in_file_name[100], out_file_name1[100], out_file_name2[100];

//	if ((i = argPos((char *)"-input", argc, argv)) > 0) strcpy(in_file_name, argv[i + 1]);
//	if ((i = argPos((char *)"-output1", argc, argv)) > 0) strcpy(out_file_name1, argv[i + 1]);
//	if ((i = argPos((char *)"-output2", argc, argv)) > 0) strcpy(out_file_name2, argv[i + 1]);
	strcpy(in_file_name, "input.bmp");
	strcpy(out_file_name1, "output1.bmp");
	strcpy(out_file_name2, "output2.bmp");
	readBmp(in_file_name);
	
	for (int i = 0; i < bmih.biHeight * bmih.biWidth; i++)
		img_yuv[i] = trans_RGB2YUV(img_rgb[i]);
	for (int i = 0; i < bmih.biHeight * bmih.biWidth; i++)
		img_rgb[i].r = img_rgb[i].g = img_rgb[i].b = img_yuv[i].y;

	saveBmp(out_file_name1);

	for (int i = 0; i < bmih.biHeight * bmih.biWidth; i++){
		img_yuv[i].y = 100;
		img_rgb[i] = trans_YUV2RGB(img_yuv[i]);
	}

	saveBmp(out_file_name2);

	return 0;
}

RGB trans_YUV2RGB(YUV yuv){
	RGB rgb;
	rgb.r = yuv.y + 1.14 * yuv.v;
	rgb.g = yuv.y - 0.39 * yuv.u - 0.58 * yuv.v;
	rgb.b = yuv.y + 2.03 * yuv.u;
	return rgb;
}
void saveBmp(char *filename){
	FILE *fout = fopen(filename, "wb");
	fwrite(&bmfh, sizeof(bmfh), 1, fout);
	fwrite(&bmih, sizeof(bmih), 1, fout);
	fwrite(img_rgb, sizeof(RGB), bmih.biWidth * bmih.biHeight, fout);
	fclose(fout);
}

YUV trans_RGB2YUV(RGB rgb){
	YUV yuv;
	yuv.y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
	yuv.u = -0.147 * rgb.r - 0.289 * rgb.g + 0.435 * rgb.b;
	yuv.v = 0.615 * rgb.r - 0.515 * rgb.g - 0.100 * rgb.b;
	return yuv;
}

void readBmp(char *filename){
	int bsize;
	FILE *fin = fopen(filename, "rb");
	fread(&bmfh.bfType, sizeof(WORD), 1, fin);
	fread(&bmfh.bfSize, sizeof(DWORD), 1, fin);
	fread(&bmfh.bfReserved1, sizeof(WORD), 1, fin);
	fread(&bmfh.bfReserved2, sizeof(WORD), 1, fin);
	fread(&bmfh.bfOffBits, sizeof(DWORD), 1, fin);
	
	fread(&bmih.biSize, sizeof(DWORD), 1, fin);
	fread(&bmih.biWidth, sizeof(LONG), 1, fin);
	fread(&bmih.biHeight, sizeof(LONG), 1, fin);
	fread(&bmih.biPlanes, sizeof(WORD), 1, fin);
	fread(&bmih.biBitCount, sizeof(WORD), 1, fin);
	fread(&bmih.biCompression, sizeof(DWORD), 1, fin);
	fread(&bmih.biSizeImage, sizeof(DWORD), 1, fin);
	fread(&bmih.biXPelsPerMeter, sizeof(LONG), 1, fin);
	fread(&bmih.biYPelsPerMeter, sizeof(LONG), 1, fin);
	fread(&bmih.biClrUsed, sizeof(DWORD), 1, fin);
	fread(&bmih.biClrImportant, sizeof(DWORD), 1, fin);
	
	bmih.biWidth = bmih.biWidth ;
	while (bmih.biWidth % 4 != 0)
		bmih.biWidth++;
	bsize = bmih.biWidth * bmih.biHeight;
	
	img_rgb = new RGB[bsize];
	img_yuv = new YUV[bsize];
	fread(img_rgb, sizeof(RGB), bsize, fin);
	fclose(fin);
}

int argPos(char *str, int argc, char **argv){
	int a;
	for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
		if (a == argc - 1) {
			printf("Argument missing for %s\n", str);
			exit(1);
		}
		return a;
	}
	return -1;
}

