#ifndef BMPLIB_H
#define BMPLIB_H

typedef struct {
  unsigned short padding;
  unsigned short bfType;
  unsigned int bfSize;
  unsigned short bfReserved1;
  unsigned short bfReserved2;
  unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
  unsigned int biSize;
  int biWidth;
  int biHeight;
  unsigned short biPlanes;
  unsigned short biBitCount;
  unsigned int biCompression;
  unsigned int biSizeImage;
  unsigned int biXPelsPerMeter;
  unsigned int biYPelsPerMeter;
  unsigned int biClrUsed;
  unsigned int biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} PIXEL;

#define DEFAULT_BITMAP_OFFSET 1078

/* Read an uncompressed 24-bit bmp from a file named 'filename' (if
   null, it's standard input); return the number of 'rows', the number
   of 'cols', and the 'bitmap' as an array of PIXELs. The function
   return 0 if successful. */
int readFile (char* filename, int* rows, int* cols, PIXEL** bitmap);


/* Write an uncompressed 24-bit bmp to a file named 'filename' (if
   null, it's standard output); the dimension of the bmp is the number
   of 'rows' by the number of 'cols', and the 'bitmap' contains an
   array of PIXELs.  The function return 0 if successful. */
int writeFile (char* filename, int rows, int cols, PIXEL* bitmap);

/* Read bmp file header from file 'fd', return the number of 'rows',
   the number of 'cols', and the 'start' position of the bitmap. The
   function returns 0 if successful. */
int readHeader(int fd, int *rows, int *cols, unsigned int *start);

/* Write bmp file header to file 'fd'; the dimention of the bitmap is
   the number of 'rows' by the number of 'cols', and it starts at the
   'start' position. The function returns 0 if successful. */
int writeHeader(int fd, int rows, int cols, unsigned int start);

/* Read the 'bitmap' from file 'fd'; the dimention of the bitmap is
   the number of 'rows' by the number of 'cols', and it starts at the
   'start' position. The function returns 0 if successful. */
int readBits(int fd, PIXEL* bitmap, int rows, int cols, unsigned int start);

/* Write the 'bitmap' to file 'fd'; the dimention of the bitmap is the
   number of 'rows' by the number of 'cols', and it starts at the
   'start' position. The function returns 0 if successful. */
int writeBits(int fd, int rows, int cols, PIXEL* bitmap, unsigned int start);

#endif /*BMPLIB_H*/
