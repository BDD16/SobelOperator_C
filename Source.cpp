
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory>
#include <string.h>
/*------structure*/
typedef struct {int rows;
				int cols;
				unsigned char* data;}
				sImage;
/*------prototypes-----*/
long getImageInfo(FILE*, long, int);
void copyImageInfo(FILE* inputFile, FILE* outputFile);
void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors);
char* UserInput(int path);//function I'm adding in to have an arbitrary file name
int main(int argc, char* argv[]);

//---some globals---
char* filenamep=UserInput(1);//it'll wait for user to input file name (user needs to include .bmp)
char* outputfile=UserInput(2);//it'll wait for user to input name of output file name (user needs to include .bmp)
char* Prostype = UserInput(3); //user will pick the type choose a particular processing type
char* *variable= &filenamep;
char text[20];
char output[20];
char proccessing[20];

//---call for user input---
int y = main(2, variable);


int main(int argc, char* argv[]){
   FILE        *bmpInput, *bmpOutput;
   sImage		originalImage;
   sImage		edgeImage;
   unsigned int		X, Y;
   int			I, J;
   long			sumX, sumY;
   int			nColors, SUM;
   unsigned long	vectorSize;
   unsigned long	fileSize;
   int			GX[3][3];
   int			GY[3][3];
   unsigned char 	*pChar, someChar;
   unsigned int		row, col;

  


      someChar = '0'; pChar = &someChar;

	  //3x3 Gx Sobel mask
   GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
   GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
   GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;
     //3x3 Gy Sobel mask
   GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
   GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
   GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;


    if(proccessing == "sobel"){
	   //do nothing continue to the code
		//maybe need a string or need to add in my own comparator to sobel
		return 42;
   }
   else if(proccessing == "Gaussian blur"){
	//gaussian blur filter
	}
	else if(proccessing == "Object detection"){
	//object detection
	}

   if(argc < 2) {
     printf("Usage: %s bmpInput.bmp\n", argv[0]);
     exit(0);
   };

   printf("Reading filename %s\n", argv[0]);

   /*-------DECLARE INPUT & OUTPUT FILES-------*/
   bmpInput = fopen(argv[0], "rb");
   bmpOutput = fopen(outputfile, "wb");

   /*---SET POINTER TO BEGINNING OF FILE----*/
   fseek(bmpInput, 0L, SEEK_END);

   /*-------GET INPUT BMP DATA--------*/
   fileSize = getImageInfo(bmpInput, 2, 4);
   originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
   originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
   edgeImage.rows = originalImage.rows;
   edgeImage.cols = originalImage.cols;

   /*--------PRINT DATA TO SCREEN----------*/
   printf("Width: %d\n", originalImage.cols);
   printf("Height: %d\n", originalImage.rows);
   printf("File size: %lu\n", fileSize);
   nColors = (int)getImageInfo(bmpInput, 46, 4);
   printf("nColors: %d\n", nColors);

   /*------ALLOCATE MEMORY FOR FILES--------*/
   vectorSize = fileSize - (14+40+4*nColors);
   printf("vectorSize: %lu\n", vectorSize);
   edgeImage.data = (unsigned char*) malloc(vectorSize*sizeof(unsigned char));
   if(edgeImage.data == NULL) {
	printf("Failed to malloc edgeImage.data\n");
	exit(0);
   }
   printf("%lu bytes malloc'ed for edgeImage.data\n", vectorSize);

   originalImage.data = (unsigned char*) malloc(vectorSize*sizeof(unsigned char));
   if(originalImage.data == NULL) {
	printf("Failed to malloc originalImage.data\n");
	exit(0);
   }
   printf("%lu bytes malloc'ed for originalImage.datt\n", vectorSize);

   /*------COPY HEADER AND COLOR TABLE---------*/
   copyImageInfo(bmpInput, bmpOutput);
   copyColorTable(bmpInput, bmpOutput, nColors);
   fseek(bmpInput, (14+40+4*nColors), SEEK_SET);
   fseek(bmpOutput, (14+40+4*nColors), SEEK_SET);

   /* Read input.bmp and store it's raster data into originalImage.data */
   for(row=0; row<=originalImage.rows-1; row++) {
	for(col=0; col<=originalImage.cols-1; col++) {
	     fread(pChar, sizeof(char), 1, bmpInput);
	     *(originalImage.data + row*originalImage.cols + col) = *pChar;
	}
   }

   //----Sobel Algorithm starts here----
   for(Y=0; Y<=(originalImage.rows-1); Y++)  {
	for(X=0; X<=(originalImage.cols-1); X++)  {
	     sumX = 0;
	     sumY = 0;

             /* image boundaries */
	     if(Y==0 || Y==originalImage.rows-1)
		  SUM = 0;
	     else if(X==0 || X==originalImage.cols-1)
		  SUM = 0;

	     /* Convolution starts here */
	     else   {

	       /*-------X GRADIENT APPROXIMATION------*/
	       for(I=-1; I<=1; I++)  {
		   for(J=-1; J<=1; J++)  {
		      sumX = sumX + (int)( (*(originalImage.data + X + I + 
                             (Y + J)*originalImage.cols)) * GX[I+1][J+1]);
		   }
	       }

	       /*-------Y GRADIENT APPROXIMATION-------*/
	       for(I=-1; I<=1; I++)  {
		   for(J=-1; J<=1; J++)  {
		       sumY = sumY + (int)( (*(originalImage.data + X + I + 
                              (Y + J)*originalImage.cols)) * GY[I+1][J+1]);
		   }
	       }

	       /*---GRADIENT MAGNITUDE APPROXIMATION (Myler p.218)----*/
               SUM = abs(sumX) + abs(sumY);
             }

             if(SUM>255) SUM=255;
             if(SUM<0) SUM=0;
			 //threshold---arbritary up for changing  
			 if(SUM>20) SUM=255; //add in a histogram flattening to the filter
			 else SUM=0;


	     *(edgeImage.data + X + Y*originalImage.cols) = 255 - (unsigned char)(SUM);
	    if( fwrite((edgeImage.data + X + Y*originalImage.cols),sizeof(char),1,bmpOutput)!=1){
			printf("it failed writing on column: %d row: %d",Y,X);}

	}
   }

   printf("See %s for results\n",output);
   fclose(bmpInput);
   fclose(bmpOutput);
   free(edgeImage.data);      /* Finished with edgeImage.data */
   free(originalImage.data);  /* Finished with originalImage.data */
   return 0;
}

/*----------GET IMAGE INFO SUBPROGRAM--------------*/
long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
  unsigned char		*ptrC;
  long				value = 0L;
  unsigned char		dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, offset, SEEK_SET);

  for(i=1; i<=numberOfChars; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    /* calculate value based on adding bytes */
    value = (long)(value + (*ptrC)*(pow(256, (i-1))));
  }
  return(value);

} /* end of getImageInfo */


/*-------------COPIES HEADER AND INFO HEADER----------------*/
void copyImageInfo(FILE* inputFile, FILE* outputFile)
{
  unsigned char		*ptrC;
  unsigned char		dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, 0L, SEEK_SET);
  fseek(outputFile, 0L, SEEK_SET);

  for(i=0; i<=50; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    fwrite(ptrC, sizeof(char), 1, outputFile);
  }

}

/*----------------COPIES COLOR TABLE-----------------------------*/
void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors)
{
  unsigned char		*ptrC;
  unsigned char		dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, 54L, SEEK_SET);
  fseek(outputFile, 54L, SEEK_SET);

  for(i=0; i<=(4*nColors); i++)  /* there are (4*nColors) bytesin color table */
  {
    fread(ptrC, sizeof(char), 1, inputFile); 
    fwrite(ptrC, sizeof(char), 1, outputFile);
  }

}

//-----TAKES IN FILE TO MANIPULATE (SYNTAX IS "FILENAME.bmp"-----
char* UserInput(int path){
	if(path==1){
		fputs("enter the input file name: ", stdout);
		fflush(stdout);
		if ( fgets(text, sizeof text, stdin) != NULL ){
		char *newline = strchr(text, '\n'); /* search for newline character */
		if ( newline != NULL ){
		*newline = '\0'; /* overwrite trailing newline */
		}
		printf("file name = \"%s\"\n", output);
		}
		return text;
	}
	if(path==2){
		fputs("enter the output file name: ", stdout);
		fflush(stdout);
		if ( fgets(output, sizeof output, stdin) != NULL ){
		char *newline = strchr(output, '\n'); /* search for newline character */
		if ( newline != NULL ){
		*newline = '\0'; /* overwrite trailing newline */
		}
		printf("file name = \"%s\"\n", output);
		}
		return output;

	}
		//Add in a feature to selct which Type of proccessing to use (sobel, Gaussian blur, Object Detection
	if(path==3){
		fputs("enter the Processing type (sobel, Gaussian blur, Object Detection): ",stdout);
			fflush(stdout);
			if ( fgets(proccessing, sizeof proccessing, stdin) != NULL ){
			char *newline = strchr(proccessing, '\n'); /* search for newline character */
			if ( newline != NULL ){
			*newline = '\0'; /* overwrite trailing newline */
			}
			printf("file name = \"%s\"\n", proccessing);
			}
			return proccessing;
		}
	
	

}

int Object_Detection(FILE *bmpinput, FILE *bmpoutput)
{
	int NumberObject=0;

		return NumberObject;
	}	

