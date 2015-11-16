#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long 	DWORD;

typedef struct BITMAPINFOHEADER {
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD bmCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD beClrImportant;
} BITMAPINFOHEADER;

typedef struct BITMAPFILEHEADER {
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct RGBQUAD {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;

class BMP {
	private:
		BITMAPFILEHEADER fh;
		BITMAPINFOHEADER ih;
		int widthPadding;
		char * RGB;
		char * YUV;
		char * tempMaxtrix;
		char * tempMaxtrix2;
		int grayLevel[256];
		int minGray, maxGray;
		int *sum_threshold;
		int *num_threshold;
		
		void circulateGrayLevel(int x, int y, int wide, int high) {
			int temp, Y;
			minGray = 255;
			maxGray = 0;
			for (int i = 0; i < 256; i++)
				grayLevel[i] = 0;
			for (int i = x; i <= x + wide; i++) {
				temp = i * widthPadding;
				for (int j = y; j <= y + high; j++) {
					Y = (int)YUV[temp + 3 * j];
					if (Y < 0)
						Y += 256;
					if (Y < minGray) minGray = Y; 
					if (Y > maxGray) maxGray = Y;
					grayLevel[Y] ++;
				}
			}
		}

	public:
		~BMP() {
			free(RGB);
			free(YUV);
			free(tempMaxtrix);
			free(tempMaxtrix2);
			delete []sum_threshold;
			delete []num_threshold;
		}

		void getfh(ifstream& in){
			in.read((char*)&fh.bfType,sizeof(fh.bfType));
			in.read((char*)&fh.bfSize,sizeof(fh.bfSize));
			in.read((char*)&fh.bfReserved1,sizeof(fh.bfReserved1));
			in.read((char*)&fh.bfReserved2,sizeof(fh.bfReserved2));
			in.read((char*)&fh.bfOffBits,sizeof(fh.bfOffBits));
		}
		void getih(ifstream& in){
			in.read((char*)&ih.biSize,sizeof(ih.biSize));
			in.read((char*)&ih.biWidth,sizeof(ih.biWidth));
			in.read((char*)&ih.biHeight,sizeof(ih.biHeight));
			in.read((char*)&ih.biPlanes,sizeof(ih.biPlanes));
			in.read((char*)&ih.biBitCount,sizeof(ih.biBitCount));
			in.read((char*)&ih.bmCompression,sizeof(ih.bmCompression));
			in.read((char*)&ih.biSizeImage,sizeof(ih.biSizeImage));
			in.read((char*)&ih.biXPelsPerMeter,sizeof(ih.biXPelsPerMeter));
			in.read((char*)&ih.biYPelsPerMeter,sizeof(ih.biYPelsPerMeter));
			in.read((char*)&ih.biClrUsed,sizeof(ih.biClrUsed));
			in.read((char*)&ih.beClrImportant,sizeof(ih.beClrImportant));
		}
		void writefh(ofstream& out){
			out.write((char*)&fh.bfType,sizeof(fh.bfType));
			out.write((char*)&fh.bfSize,sizeof(fh.bfSize));
			out.write((char*)&fh.bfReserved1,sizeof(fh.bfReserved1));
			out.write((char*)&fh.bfReserved2,sizeof(fh.bfReserved2));
			out.write((char*)&fh.bfOffBits,sizeof(fh.bfOffBits));
		}
		void writeih(ofstream& out){
			out.write((char*)&ih.biSize,sizeof(ih.biSize));
			out.write((char*)&ih.biWidth,sizeof(ih.biWidth));
			out.write((char*)&ih.biHeight,sizeof(ih.biHeight));
			out.write((char*)&ih.biPlanes,sizeof(ih.biPlanes));
			out.write((char*)&ih.biBitCount,sizeof(ih.biBitCount));
			out.write((char*)&ih.bmCompression,sizeof(ih.bmCompression));
			out.write((char*)&ih.biSizeImage,sizeof(ih.biSizeImage));
			out.write((char*)&ih.biXPelsPerMeter,sizeof(ih.biXPelsPerMeter));
			out.write((char*)&ih.biYPelsPerMeter,sizeof(ih.biYPelsPerMeter));
			out.write((char*)&ih.biClrUsed,sizeof(ih.biClrUsed));
			out.write((char*)&ih.beClrImportant,sizeof(ih.beClrImportant));
		}
		void BMP2Gray(ifstream& in) {
			for (int i = 0; i < 256; i++)
				grayLevel[i] = 0;
			getfh(in);
			getih(in);
			widthPadding = ceil(3.0 * ih.biWidth / 4) * 4;
			RGB = (char *)malloc(sizeof(char) * widthPadding * ih.biHeight);
			YUV = (char *)malloc(sizeof(char) * widthPadding * ih.biHeight);
			tempMaxtrix = (char *)malloc(sizeof(char) * widthPadding * ih.biHeight);
			tempMaxtrix2 = (char *)malloc(sizeof(char) * widthPadding * ih.biHeight);
			minGray = 256; maxGray = 0;

			for (int i = 0; i < ih.biHeight; i++) {
				in.read(&RGB[i * widthPadding], sizeof(char) * widthPadding);
				int uu = i * widthPadding;

				for (int j = 0; j < ih.biWidth; j++) {
					unsigned char B = (unsigned char) RGB[uu + 3 * j];
					unsigned char G = (unsigned char) RGB[uu + 3 * j + 1];
					unsigned char R = (unsigned char) RGB[uu + 3 * j + 2];

					double y = (0.299 * R + 0.587 * G + 0.114 * B);
					double u = (-0.147 * R - 0.289 * G + 0.435 * B);
					double v = (0.615 * R - 0.515 * G - 0.1 * B);
					unsigned char Y = y;
					RGB[uu + 3 * j + 1] = RGB[uu + 3 * j + 2] = RGB[uu + 3 * j] = y;
					YUV[uu + 3 * j + 1] = YUV[uu + 3 * j + 2] = YUV[uu + 3 * j] = y;
				}
			}
			sum_threshold = new int[ih.biHeight * ih.biWidth];
			num_threshold = new int[ih.biHeight * ih.biWidth];
			in.close();
		}

		void circulateByWindow() {
			int wide = ih.biWidth / 3, high = ih.biHeight / 3;
			int stepWidth = wide / 3, stepHigh = high / 3;
			int startX = 0, startY = 0, i, j;
			int current_threshold = 0;

			for (i = 0; i < ih.biHeight; i++) {
				int uu = i * ih.biWidth;
				for (j = 0; j < ih.biWidth; j++) {
					sum_threshold[uu + j] = 0;
					num_threshold[uu + j] = 0;
				}
			}
			while (startY + high < ih.biHeight) {
				while (startX + wide < ih.biWidth) {
					circulateGrayLevel(startX, startY, wide, high);
					current_threshold = circulateThreshold(startX, startY, wide, high);
					for (i = startY; i < startY + high; i++)
					for (j = startX; j < startX + wide; j++) {
						sum_threshold[i * ih.biWidth + j] += current_threshold;
						num_threshold[i * ih.biWidth + j] ++;
					}
					startX += stepWidth;
				}
				startX = 0;
				startY += stepHigh;
			}
		}

		int circulateThreshold(int x, int y, int wide, int high) {
			circulateGrayLevel(x, y, wide, high);
			int numMin, numMax, u, ans, n;
			double aveMin, aveMax, res = 0, maxRes = 0;
			for (u = minGray; u <= maxGray; u++) {
				numMin = 0; numMax = 0; aveMin = 0; aveMax = 0;
				for (int i = minGray; i < u; i++) {
					numMin += grayLevel[i];
					aveMin += grayLevel[i] * i;
				}
				for (int i = u; i <= maxGray; i++) {
					numMax += grayLevel[i];
					aveMax += grayLevel[i] * i;
				}
				n = numMin + numMax;
				res = (double)numMin * numMax / n / n * pow(aveMin / numMin - aveMax / numMax, 2);
				if (res > maxRes) { 
					maxRes = res;
					ans = u;
				}
			}
			return ans;
		}

		void BMP2binary() {
			ofstream out("binarization.bmp");
			writefh(out);
			writeih(out);
			int Y;
			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					Y = YUV[uu + 3 * j];
					if (Y < 0)
						Y += 256;
					if (Y * num_threshold[i * ih.biWidth + j] < (double)sum_threshold[i * ih.biWidth + j]) {
						YUV[uu + 3 * j] = 0;
						YUV[uu + 3 * j + 1] = 0;
						YUV[uu + 3 * j + 2] = 0;
					} else {
						YUV[uu + 3 * j] = 255;
						YUV[uu + 3 * j + 1] = 255;
						YUV[uu + 3 * j + 2] = 255;
					}
				}		
			}
			out.write(YUV, ih.biHeight * widthPadding);
			out.close();
		}

		void erosion() {
			ofstream out("erosion.bmp");
			writefh(out);
			writeih(out);
			int matrix[3][3], temp;
			matrix[0][0] = 0; matrix[0][1] = 0; matrix[0][2] = 0;
			matrix[0][1] = 0; matrix[1][1] = 1; matrix[1][2] = 1;
			matrix[0][2] = 0; matrix[2][1] = 1; matrix[2][2] = 0;

			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j - 1; jj <= j + 1; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0 && matrix[ii - i + 1][jj - j + 1] == 1 && YUV[ii * widthPadding + 3 * jj] == -1)
						temp += 1;
			//		if (temp == 3)
			//			cout<<i<<" "<<j<<endl;
					if (temp == 3) {
						tempMaxtrix[uu + 3 * j] = 255;
						tempMaxtrix[uu + 3 * j + 1] = 255;
						tempMaxtrix[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix[uu + 3 * j] = 0;
						tempMaxtrix[uu + 3 * j + 1] = 0;
						tempMaxtrix[uu + 3 * j + 2] = 0;
					}
				}
			}
			out.write(tempMaxtrix, ih.biHeight * widthPadding);
			out.close();
		}

		void delation() {
			ofstream out("delation.bmp");
			writefh(out);
			writeih(out);
			int matrix[3][3], temp;
			matrix[0][0] = 0; matrix[0][1] = 0; matrix[0][2] = 0;
			matrix[0][1] = 0; matrix[1][1] = 1; matrix[1][2] = 1;
			matrix[0][2] = 0; matrix[2][1] = 1; matrix[2][2] = 0;

			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j - 1; jj <= j + 1; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0 && matrix[ii - i + 1][jj - j + 1] == 1 && YUV[ii * widthPadding + 3 * jj] != 0)
						temp += 1;
//					cout<<YUV[i * ih.biWidth + 3 * j]<<" ";
//					cout<<temp;
					if (temp != 0){
						tempMaxtrix[uu + 3 * j] = 255;
						tempMaxtrix[uu + 3 * j + 1] = 255;
						tempMaxtrix[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix[uu + 3 * j] = 0;
						tempMaxtrix[uu + 3 * j + 1] = 0;
						tempMaxtrix[uu + 3 * j + 2] = 0;
					}
				}
//				cout<<endl;
			}
			out.write(tempMaxtrix, ih.biHeight * widthPadding);
			out.close();
		}

		void opening() {
			ofstream out("opening.bmp");
			writefh(out);
			writeih(out);
			int matrix[3][3], temp;
			matrix[0][0] = 0; matrix[0][1] = 0; matrix[0][2] = 0;
			matrix[0][1] = 0; matrix[1][1] = 1; matrix[1][2] = 1;
			matrix[0][2] = 0; matrix[2][1] = 1; matrix[2][2] = 0;

			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j - 1; jj <= j + 1; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0 && matrix[ii - i + 1][jj - j + 1] == 1 && YUV[ii * widthPadding + 3 * jj] != 0)
						temp += 1;
					if (temp == 3) {
						tempMaxtrix[uu + 3 * j] = 255;
						tempMaxtrix[uu + 3 * j + 1] = 255;
						tempMaxtrix[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix[uu + 3 * j] = 0;
						tempMaxtrix[uu + 3 * j + 1] = 0;
						tempMaxtrix[uu + 3 * j + 2] = 0;
					}
				}
			}
			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j - 1; jj <= j + 1; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0 && matrix[ii - i + 1][jj - j + 1] == 1 && tempMaxtrix[ii * widthPadding + 3 * jj] != 0)
						temp += 1;
					if (temp != 0){
						tempMaxtrix2[uu + 3 * j] = 255;
						tempMaxtrix2[uu + 3 * j + 1] = 255;
						tempMaxtrix2[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix2[uu + 3 * j] = 0;
						tempMaxtrix2[uu + 3 * j + 1] = 0;
						tempMaxtrix2[uu + 3 * j + 2] = 0;
					}
				}
			}

			out.write(tempMaxtrix2, ih.biHeight * widthPadding);
			out.close();
		}
	

		void closing() {
			ofstream out("closing.bmp");
			writefh(out);
			writeih(out);
			int matrix[3][3], temp;
			matrix[0][0] = 0; matrix[0][1] = 0; matrix[0][2] = 0;
			matrix[0][1] = 0; matrix[1][1] = 1; matrix[1][2] = 1;
			matrix[0][2] = 0; matrix[2][1] = 1; matrix[2][2] = 0;

			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j - 1; jj <= j + 1; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0 && matrix[ii - i + 1][jj - j + 1] == 1 && YUV[ii * widthPadding + 3 * jj] != 0)
						temp += 1;
					if (temp != 0){
						tempMaxtrix[uu + 3 * j] = 255;
						tempMaxtrix[uu + 3 * j + 1] = 255;
						tempMaxtrix[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix[uu + 3 * j] = 0;
						tempMaxtrix[uu + 3 * j + 1] = 0;
						tempMaxtrix[uu + 3 * j + 2] = 0;
					}
				}
			}
			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j - 1; jj <= j + 1; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0 && matrix[ii - i + 1][jj - j + 1] == 1 && tempMaxtrix[ii * widthPadding + 3 * jj] != 0)
						temp += 1;
					if (temp == 3) {
						tempMaxtrix2[uu + 3 * j] = 255;
						tempMaxtrix2[uu + 3 * j + 1] = 255;
						tempMaxtrix2[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix2[uu + 3 * j] = 0;
						tempMaxtrix2[uu + 3 * j + 1] = 0;
						tempMaxtrix2[uu + 3 * j + 2] = 0;
					}
				}
			}

			out.write(tempMaxtrix2, ih.biHeight * widthPadding);
			out.close();
		}
	
		void HMT() {
			ofstream out("HMT.bmp");
			writefh(out);
			writeih(out);
			int matrix[3][3], temp;
			matrix[0][0] = 0; matrix[0][1] = 0; matrix[0][2] = 0;
			matrix[0][1] = 0; matrix[1][1] = 1; matrix[1][2] = 1;
			matrix[0][2] = 0; matrix[2][1] = 1; matrix[2][2] = 0;

			for (int i = 0; i < ih.biHeight; i++) {
				int uu = i * widthPadding;
				for (int j = 0; j < ih.biWidth; j++) {
					temp = 0;
					for (int ii = i - 1; ii <= i + 1; ii++)
					for (int jj = j; jj <= j ; jj++)
					if (ii < ih.biHeight && ii >= 0 && jj < ih.biWidth && jj >= 0){
						if (matrix[ii - i + 1][jj - j + 1] == 1 && YUV[ii * widthPadding + 3 * jj] != 0)
							temp += 1;
						else if (matrix[ii - i + 1][jj - j + 1] == 0 && YUV[ii * widthPadding + 3 * jj] == 0)
							temp += 1;
					} else if (matrix[ii - 1 + 1][jj - j + 1] == 0)
						temp += 1;
					if (temp == 3){
						tempMaxtrix[uu + 3 * j] = 255;
						tempMaxtrix[uu + 3 * j + 1] = 255;
						tempMaxtrix[uu + 3 * j + 2] = 255;
					}
					else {
						tempMaxtrix[uu + 3 * j] = 0;
						tempMaxtrix[uu + 3 * j + 1] = 0;
						tempMaxtrix[uu + 3 * j + 2] = 0;
					}
				}
//				cout<<endl;
			}
			out.write(tempMaxtrix, ih.biHeight * widthPadding);
			out.close();
		}

};

int main(){
	BMP bmp;
	ifstream in("input.bmp",ios::binary);
	
	bmp.BMP2Gray(in);
	bmp.circulateByWindow();
	bmp.BMP2binary();
	bmp.erosion();
	bmp.delation();
	bmp.opening();
	bmp.closing();
	bmp.HMT();
	return 0;
}
