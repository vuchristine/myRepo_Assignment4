#include "pixutils.h"

int main(int argc, char *argv[]){
  char *inputfile=0,*outputfile=0;
  float degrees=0;
  char grayFlag=0;
  char convertToBmp=0;
  char sortPixels=0;
  int i=1;
  while (i<argc){
			//check for flags -i -o -f -d 
			if(strcmp(argv[i],"-r") == 0){ 
				degrees=atof(argv[i+1]);
				i+=2;
			}
			else if(strcmp(argv[i],"-g") == 0){ 
				grayFlag=1;
				i++;
			}
			else if(strcmp(argv[i],"-i") == 0){
				inputfile=argv[i+1];
				i+=2;
			}	
			//save png file as 16 bit bmp file
			else if(strcmp(argv[i],"-b") == 0){
				convertToBmp=1;
				i++;
			}
			//sort png file by sum of rgb of pixels
			else if(strcmp(argv[i],"-s") == 0){
				sortPixels=1;
				i++;
			}
			else if(strcmp(argv[i],"-o")==0){
				outputfile=argv[i+1];
			 	i+=2;
			}
			else{
				fprintf(stderr,"pngPlay -g -r degrees -i inputfile -o outputfile -b -s  ");
				exit(0);
			}	
		}		
  pixMap *p=pixMap_init_filename(inputfile);
  if(degrees)pixMap_rotate(p,degrees);
  if(grayFlag)pixMap_gray(p);
  
  //sort pixels by color
  if(sortPixels)pixMap_sort(p);
  
  //convert and write to output file
  if(convertToBmp){
  	pixMap_write_bmp16(p, outputfile);
  } else {
  	pixMap_write(p,outputfile);
  }
  
  pixMap_destroy(p);
  return 0;
}
