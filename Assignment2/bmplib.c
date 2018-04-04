#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "bmplib.h"

static int myread(int fd, char* buf, unsigned int size)
{
  int r = 0;
  while(r < size) {
    int x = read(fd, &buf[r], size-r);
    if(x < 0) return x;
    else r += x;
  }
  return size;
}

static int mywrite(int fd, char* buf, unsigned int size)
{
  int w = 0;
  while(w < size) {
    int x = write(fd, &buf[w], size-w);
    if(x < 0) return x;
    else w += x;
  }
  return size;
}

int readFile (char *filename, int *rows, int *cols, PIXEL** bitmap) 
{
  int fd, ret;
  unsigned int start;
  
  if(filename) {
    if((fd = open(filename, O_RDONLY)) < 0) {
      perror("Can't open bmp file to read");
      return -1;
    }
  } else fd = STDIN_FILENO;

  ret = readHeader (fd, rows, cols, &start);
  if(ret) return ret;

  *bitmap = (PIXEL*)malloc(sizeof(PIXEL)*(*rows)*(*cols));
  ret = readBits (fd, *bitmap, *rows, *cols, start);
  if(ret) return ret;

  if(filename) close(fd);

  return 0;
}

int writeFile (char* filename, int rows, int cols, PIXEL* bitmap) 
{
  int fd, ret;
  unsigned int start = DEFAULT_BITMAP_OFFSET;

  if(filename) {
    if((fd = open(filename, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0) {
      perror("Can't open bmp file to write");
      return -1;
    }
  } else fd = STDOUT_FILENO;

  ret = writeHeader (fd, rows, cols, start);
  if(ret) return ret;

  ret = writeBits(fd, rows, cols, bitmap, start);
  if(ret) return ret;

  if(filename) close(fd);

  return 0;
}

int readHeader(int fd, int *rows, int *cols, unsigned int *start) 
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;

  if(myread(fd, ((char*)&bmfh)+2, sizeof(bmfh)-2) <= 0) {
    perror("Can't read BITMAPFILEHEADER");
    return -2;
  }
  if(myread(fd, (char*)&bmih, sizeof(bmih)) <= 0) {
    perror("Can't read BITMAPINFOHEADER");
    return -3;
  }

  if(bmih.biCompression != 0) {
    fprintf(stderr, "Can't read compressed bmp");
    return -4;
  }
  if(bmih.biBitCount != 24) {
    fprintf(stderr, "Can't handle bmp other than 24-bit");
    return -5;
  }

  *rows = bmih.biHeight;
  *cols = bmih.biWidth;
  *start = bmfh.bfOffBits;

  return 0;
}

int writeHeader(int fd, int rows, int cols, unsigned int start)
{
  unsigned int fileSize;
  unsigned int headerSize;
  unsigned int paddedCols;
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmih;

  memset (&bmfh, 0, sizeof(bmfh));
  memset (&bmih, 0, sizeof(bmih));

  paddedCols = ((cols/4)*4 !=cols ? ((cols+4)/4)*4 : cols);
  headerSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  fileSize = rows*paddedCols*sizeof(PIXEL)+headerSize;

  bmfh.bfType = 0x4D42;
  bmfh.bfSize = fileSize;
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfOffBits = start;

  bmih.biSize = 40;
  bmih.biWidth  = cols;
  bmih.biHeight = rows;
  bmih.biPlanes = 1;
  bmih.biBitCount = 24;
  bmih.biCompression = 0;
  bmih.biSizeImage = 0;
  bmih.biXPelsPerMeter  = 0;
  bmih.biYPelsPerMeter = 0;
  bmih.biClrUsed = 0;
  bmih.biClrImportant = 0;

  if(mywrite(fd, ((char*)&bmfh)+2, sizeof(bmfh)-2) < 0) {
    perror("Can't write BITMAPFILEHEADER");
    return -2;
  }
  if(mywrite(fd, (char*)&bmih, sizeof(bmih)) < 0) {
    perror("Can't write BITMAPINFOHEADER");
    return -3;
  }

  return 0;
}

int readBits(int fd, PIXEL* bitmap, int rows, int cols, unsigned int start) 
{
  int row;
  char padding[3];
  int padAmount;
  char useless[DEFAULT_BITMAP_OFFSET];

  padAmount = ((cols * sizeof(PIXEL)) % 4) ? (4 - ((cols * sizeof(PIXEL)) % 4)) : 0;

  start -= sizeof(BITMAPFILEHEADER)-2+sizeof(BITMAPINFOHEADER);
  if(start > 0 && myread(fd, useless, start) < 0) { 
    perror("Can't lseek to bitmap");
    return -6;
  }
  /*
  if(lseek (fd, start, SEEK_SET) < 0) {
    perror("Can't lseek to bitmap");
    return -6;
  }
  */

  for (row=0; row < rows; row++) {
    if(myread(fd, (char*)(bitmap+(row*cols)), cols*sizeof(PIXEL)) < 0) {
      perror("Can't read bitmap");
      return -7;
    }
    if(padAmount > 0) { 
      if(myread(fd, padding, padAmount) < 0) {
	perror("Can't read bitmap");
	return -8;
      }
    }
  }

  return 0;
}
  
int writeBits(int fd, int rows, int cols, PIXEL* bitmap, unsigned int start) 
{
  int row;
  char padding[3];
  int padAmount;
  char useless[DEFAULT_BITMAP_OFFSET];

  padAmount = ((cols * sizeof(PIXEL)) % 4) ? (4 - ((cols * sizeof(PIXEL)) % 4)) : 0;
  memset(padding, 0, 3);

  start -= sizeof(BITMAPFILEHEADER)-2+sizeof(BITMAPINFOHEADER);
  if(start > 0) {
    memset(useless, 0, start);
    if(mywrite(fd, useless, start) < 0) { 
      perror("Can't lseek to bitmap");
      return -6;
    }
  }
  /*
  if(lseek (fd, start, SEEK_SET) < 0) {
    perror("Can't lseek to bitmap");
    return -6;
  }
  */

  for (row=0; row < rows; row++) {
    if(mywrite(fd, (char*)(bitmap+(row*cols)), cols*sizeof(PIXEL)) < 0) {
      perror("Can't write bitmap");
      return -7;
    }
    if(padAmount > 0) { 
      if(mywrite(fd, padding, padAmount) < 0) {
	perror("Can't write bitmap");
	return -8;
      }
    }
  }

  return 0;
}
