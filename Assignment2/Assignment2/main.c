/*
 * Programmer: Anwar Numa
 *
 * PID: 4887513 
 *
 * Assignment 2 Bitmap
 *
 * Certification: I certify that this is my own work and 
 * 		that none of it is the work of any other person
 *
 * Description: The program allows the user to flip(horizontally), 
 * rotate(by any multiple of 90), and scale(by any integer) a bitmap image. 
 * The program allows the user to modify the image using one or all three of the options.
 * The order of operations for the options are as follows: scale, rotate, then flip. 
 * The user can also pipe the bmptool. If no output file is specified with the -o option 
 * then the program will output to the stdout and if no input file is specified the 
 * program will attempt to read from the stdin. The output and input files should always be specified unless piping.
 * */




#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "bmplib.h"

/*
 * This method enlarges a 24-bit, uncompressed .bmp file
 * that has been read in using readFile()
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the original number of rows
 * cols     - the original number of columns
 *
 * scale    - the multiplier applied to EACH OF the rows and columns, e.g.
 *           if scale=2, then 2* rows and 2*cols
 *
 * new      - the new array containing the PIXELs, allocated within
 * newrows  - the new number of rows (scale*rows)
 * newcols  - the new number of cols (scale*cols)
 */
int enlarge(PIXEL* original, int rows, int cols, int scale, 
	    PIXEL** new, int* newrows, int* newcols) 
{
	int row, col, j, i;
 	if((rows <= 0) || (cols <= 0)){
		return -1;
	}
		
	*newrows = rows * scale;
	*newcols = cols * scale;

	*new = (PIXEL*)malloc((*newrows)*(*newcols)*sizeof(PIXEL));
	/*as the loop goes the original picture
	* the new array points a 'scale' number of 'Pixels' from the new
	* array to the orignal thus enlarging the image
	*/ 
	for(row = 0; row < rows; row++){
		for(col = 0; col < cols; col++){
			PIXEL* o = original + row*cols + col;
			for(j = 0;j < scale; j++){
				for(i = 0; i < scale; i++){
					PIXEL* n = (*new) + (row*scale+i)*(*newcols)+(col*scale+j);
					*n = *o;
				}		
			}	
		}	
	}
		
  	return 0;
}

/*
 * This method rotates a 24-bit, uncompressed .bmp file that has been read 
 * in using readFile(). The rotation is expressed in degrees and can be
 * positive, negative, or 0 -- but it must be a multiple of 90 degrees
 * 
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 * rotation - a positive or negative rotation, 
 *
 * new      - the new array containing the PIXELs, allocated within
 * 
 * 
 */
int rotate(PIXEL* original, int rows, int cols, int rotation,
	   PIXEL** new)
{
 	
	if((rows <= 0) || cols <= 0){
		return -1;
	}

	int row, col;
	int j = 0;
	
	*new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));
	//for possible cases
	//when degree/90 = (3 or -1) or (2 or -2) or (1 or -3) or (-4 or 4) 
	if((rotation == -3) || (rotation == 1)){
		//Picture will end up in the same place
		for(row = 0; row < rows; row++){
			for(col = 0; col < cols; col++){
				PIXEL* o = original + row*cols + col;
				PIXEL* n = (*new) + row + (cols* (cols-(col+1)));
				*n = *o;
			}		
		}
	} 
	if((rotation == 3) || (rotation == -1)){
		//Picture will end up in the same place
		for(row = 0; row < rows; row++){
			for(col = 0; col < cols; col++){
				PIXEL* o = original + row*cols + col;
				PIXEL* n = (*new) + (rows-(row+1)) + (col*cols);
				*n = *o;
			}
		}	
	}
	if((rotation == -2) || (rotation == 2)){
		//Picture will end up in the same place
		for(row = 0; row < rows; row++){
			for(col = 0; col < cols; col++){
				PIXEL* o = original + row*cols + col;
				PIXEL* n = (*new) + ((rows*cols)-1) - j;
				*n = *o;
				j++;
			}
		}
	}
	if((rotation == -4) || (rotation == 4)){
		//Picture doesnt change
		for(row = 0; row < rows; row++ ){
			for(col = 0; col < cols; col++){
				PIXEL* o = original + row*cols + col;
				PIXEL* n = (*new) + row*cols + col;
				*n = *o;
			}
		}
	}
		

  		
	return 0;
}

/*
 * This method horizontally flips a 24-bit, uncompressed bmp file
 * that has been read in using readFile(). 
 * 
 * THIS IS GIVEN TO YOU SOLELY TO LOOK AT AS AN EXAMPLE
 * TRY TO UNDERSTAND HOW IT WORKS
 *
 * original - an array containing the original PIXELs, 3 bytes per each
 * rows     - the number of rows
 * cols     - the number of columns
 *
 * new      - the new array containing the PIXELs, allocated within
 */
int flip (PIXEL *original, PIXEL **new, int rows, int cols) 
{
  	int row, col;
  	if ((rows <= 0) || (cols <= 0)) return -1;

  	*new = (PIXEL*)malloc(rows*cols*sizeof(PIXEL));

  	for (row=0; row < rows; row++)
    		for (col=0; col < cols; col++) {
      			PIXEL* o = original + row*cols + col;
      			PIXEL* n = (*new) + row*cols + (cols-1-col);
      			*n = *o;
    		}

  return 0;
}

int main(int argc, char *argv[])
{
  	int r, c;
  	PIXEL *b, *nb;

	//the new rows for the enlarge function	
	int nr, nc;
		
	int g;
	extern int optind;
	extern char *optarg;

	//The possible options for the program
	int sFlag = 0;
	int rFlag = 0;
	int fFlag = 0;
	int oFlag = 0;
	
	//Where the filenames will be stored
	char inputFileName[25];
	char outputFileName[25];

	//These are the arguments for enlarge and rotate respectively
	int scaleFactor;
	int degreeFactor;


	int rotation;
	
	//usage
	static char usage[] = "Usage: bmptool [-s scale | -r degree | -f] [-o output_File] [input_file]";
	
	
	while((g = getopt(argc, argv, " s:r:o:f")) != -1){
		switch(g){
		//All possible options 
		//arguments are stored respectively
		case 's':
			sFlag = 1;
			scaleFactor = atoi(optarg);
			break;
		case 'r': 
			rFlag = 1;
			degreeFactor = atoi(optarg);
			break;
		case 'f':
			fFlag = 1;
			break;
		case 'o':
			oFlag = 1;
			strncpy(outputFileName, optarg, 25);
			break;
		
		}
	}
	//ATLEAST ONE ARGUMENT IS NEEDED
	if((sFlag == 0) && (rFlag == 0) && (fFlag ==0)){
		printf("Atleast one argument is necessary!\n%s \n", usage);
		exit(1);
	}


	//READING THE BMP
	if(optind == argc){
		//WHEN NO INPUT FILE IS SPECIFIED
		readFile(NULL, &r, &c, &b);
	}else{
		strncpy(inputFileName, argv[optind], 25);
		readFile(inputFileName, &r, &c, &b);
	}
	
	/*
 	*Order of Operation	 	
 	*Scale First
 	*then rotate
 	*then flip	 		
 	*/
	if(sFlag == 1){
		//CALL THE ENLARGE FUCNTION
		if(scaleFactor > 0){
			enlarge(b, r, c, scaleFactor ,&nb, &nr, &nc);
			r = nr;
			c = nc;		
		}else{
			printf("The scale Factor must be a positive integer!\n%s\n", usage);
			exit(1);
		}	
	}
	
	if(rFlag == 1){
		//CALL THE ROTATE FUNCTION
		//CHECK IF the factor is a multiple of 90	
		if((degreeFactor % 90) == 0 ){
			while(degreeFactor > 360){
				degreeFactor = degreeFactor - 360;	
			}
			while(degreeFactor < -360){
				degreeFactor = degreeFactor + 360; 
			}
			//rotation simplifies the degree to be rotated
			rotation = degreeFactor / 90;	 
			if((rotation >= -4) && (rotation <= 4)){
				if(sFlag == 1){
					rotate(nb, r, c, rotation, &nb);
				}else{
					rotate(b, r, c, rotation, &nb);	
				}	
			}
			
		}
		else{
			printf("The degree rotation needs to be a multiple of 90\n%s\n", usage);
			exit(1);
		}	
	}
	//The flip function
	if(fFlag == 1){
		if(sFlag == 1 || rFlag == 1){
			flip(nb, &nb, r, c);
		}else{
			flip(b, &nb, r, c);
		}	
			
	}
	
	
	//WRITING THE BMP
	if(oFlag == 0){
		//NO OUTPUT FILE SPECIFIED
		writeFile(NULL, r, c, nb);
	}else if(oFlag == 1){
		writeFile(outputFileName, r, c, nb);
	} 
	
	printf("%s \n", usage);	
	
	//freeing the allocated memory
  	free(b);
  	free(nb);
  return 0;
}





