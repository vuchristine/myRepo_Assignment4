#include "pixutils.h"
#include "lodepng.h"
#include "bmp.h"

//private methods
static pixMap *pixMap_init(); //allocate memory for pixMap and set variables to zero
static void pixMap_reset();  //free the allocated memoray and set to zero but do not free memory for pixMap
static void pixMap_copy(pixMap *dest,pixMap *source);
static int pixMap_read(pixMap *p,char *filename);

static pixMap* pixMap_init(){
	pixMap *p;
	if(!(p=malloc(sizeof(pixMap))))exit(EXIT_FAILURE);
	p->image=0;
	p->pixArray=0;
	p->width=0;
	p->height=0;
	return p;
}	
pixMap* pixMap_init_filename(char *filename){
	pixMap *p=pixMap_init();
 pixMap_read(p,filename);
 if(!(p->pixArray=malloc(p->height*sizeof(rgba*))))exit(EXIT_FAILURE);
 p->pixArray[0]=(rgba*)p->image;
 for (int i=1;i<p->height;i++)
  p->pixArray[i]=p->pixArray[i-1]+p->width;
 return p;
}
static int pixMap_read(pixMap *p,char *filename){
	int error;
 if((error=lodepng_decode32_file(&(p->image), &(p->width), &(p->height),filename))){
  fprintf(stderr,"error %u: %s\n", error, lodepng_error_text(error));
  return 1;
	}
	return 0;
}	
static void pixMap_copy(pixMap *dest,pixMap *source){
	//if source image is zero then reset dest and copy width and height
	//if source image is not zero 
	  //if width or height are different 
	    //if width*height is different
	      //if dest->image is zero use malloc for dest->image
	      //else use realloc for dest->image
	      //if dest->height is different malloc or realloc dest->pixArray depending on whether it is zero 
	      //set dest->pixArray[0] to dest->image and dest->pixArray[i]=dest->pixArray[i-1]+source->width
	    //
	  //
	 //do a memcpy from source to dest
	 //set dest->width and dest->height to source values
	//
 if(!source->image){
	 pixMap_reset(dest);
	 dest->width=source->width;
	 dest->height=source->height;
	}
	if(dest->width != source->width || dest->height != source->height){
		if(dest->width * dest->height != source->width * source->height){
			if(dest->image){
	   if(!(dest->image=realloc(dest->image,source->height*source->width*sizeof(rgba))))exit(EXIT_FAILURE);
			}
			else if(!(dest->image=malloc(source->height*source->width*sizeof(rgba))))exit(EXIT_FAILURE);
		}
	 if(dest->height != source->height){
	  if(dest->pixArray){
	   if(!(dest->pixArray=realloc(dest->pixArray,source->height*sizeof(rgba*))))exit(EXIT_FAILURE);
			}
			else if(!(dest->pixArray=malloc(source->height*sizeof(rgba*))))exit(EXIT_FAILURE);
		}
		dest->pixArray[0]=(rgba*)dest->image;
		for(int i=1;i<source->height;i++){
			dest->pixArray[i]=dest->pixArray[i-1]+source->width;
		}	
	}		
	memcpy(dest->image,source->image,source->height*source->width*sizeof(rgba));
 dest->height=source->height;
 dest->width=source->width;
}

static void pixMap_reset(pixMap *p){
	if (p && p->image){
		free(p->image);
	 p->image=0;
	} 
 if (p && p->pixArray){
		free(p->pixArray);
		p->pixArray=0;
	}
}	


void pixMap_destroy(pixMap *p){
 if(p) pixMap_reset(p);
 if(p) free(p);
}


void pixMap_rotate (pixMap *p, float theta){
	//correct rotation that uses inverse rotation to determine which pixels to copy from the original
	//the previous method would leave holes in the image
	
	pixMap *temp=pixMap_init();//make a new temp blank pixMap structure
	pixMap_copy(temp,p); //copy the p to temp
	//set the values in the image to zero using memset - that the 3rd argument of memset is the size in BYTES
 memset(temp->image,0,temp->width*temp->height*sizeof(rgba));
 
 const float ox=p->width/2.0f;
 const float oy=p->height/2.0f;
 const float s=sin(degreesToRadians(-theta));
	const float c=cos(degreesToRadians(-theta));
	
	//for()   //double for loop to loop through each pixel in the original
	  //for()
	    //calculate the old coordinates rotx roty to copy from using the formula from 
	    //http://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d
	    //use the answer from stackoverflowery
	    
 	   //round the coordinates to the nearest integer in your calculations (add 0.5 and cast to integer)	
	    //if rotated coordinates are still inside the image
	      //copy the pixel at the old coords to the pixel to the temporary copy using memcpy
	      //i.e. 	memcpy(temp->pixArray[y]+x,p->pixArray[roty]+rotx,sizeof(rgba))
	    //
	  //
	//  
	//copy the temp pixMap to the original
	//destroy the temp

	for(int y=0;y<p->height;y++){
		for(int x=0;x<p->width;x++){ 
   float rotx = c*(x-ox) - s * (oy-y) + ox;
   float roty = -(s*(x-ox) + c * (oy-y) - oy);
		 int rotj=rotx+.5;
		 int roti=roty+.5; 
		 if(roti >=0 && roti < temp->height && rotj >=0 && rotj < temp->width){
				memcpy(temp->pixArray[y]+x,p->pixArray[roti]+rotj,sizeof(rgba));
			}	
		}	
	}
	pixMap_copy(p,temp);
	pixMap_destroy(temp);
	return;
}	
void pixMap_gray (pixMap *p){

	//loop through pixels using double for loop
	   //calculate average value of r and g and b values (assign to a float variable)
	   //round float variable to integer (add 0.5 before casting to integer)
	   //assign the rounded value to r,g and b values
	for(int y=0;y<p->height;y++){
	 for(int x=0;x<p->width;x++){
			const rgba pixel=p->pixArray[y][x]; 
			const float average= (pixel.r+pixel.g+pixel.b)/3.0f;
			const unsigned char iaverage=average+.5;
			p->pixArray[y][x].r=iaverage;
		 p->pixArray[y][x].g=iaverage;			
		 p->pixArray[y][x].b=iaverage;
		}	
	}
	return;
}
int pixMap_write(pixMap *p,char *filename){
	int error=0;
 if(lodepng_encode32_file(filename, p->image, p->width, p->height)){
  fprintf(stderr,"error %u: %s\n", error, lodepng_error_text(error));
  return 1;
	}
	return 0;
}	 

void pixMap_convert(pixMap *p, char *filename){
	//variables
	int i = 0;
	int j = 0;
	int i1 = 0; //for b's pixel array
	rgba tempR, tempG, tempB, bit16;

	//initialize the bmp file
	BMP16_map *b = BMP16_map_init(p->height, p->width, 0, 5, 6, 5);

	//convert the pixels
	for(i = 0; i < p->height; i++){
		//for every i make b's i1 this
		int i1 = p->height - i - 1;

		for(j = 0; j < p->width; j++){
			//take the first 5 bits for red
			rgba tempR = p->pixArray[i][j].r >> 5;

			//take the first 6 bits for green
			rgba tempG = p->pixArray[i][j].g >> 6;

			//take the first 5 bits for blue
			rgba tempB = p->pixArray[i][j].b >> 5;

			//put these bits together as 16 bit
			bit16 = ((tempR << 11) | (tempG << 5) | (tempB));

			//place the 16 bit into the bmp array
			b->pixArray[i1][j] = bit16;
		}
	}

	//write to the new file to contain newly made bmp file
	BMP16_write(b, filename);

	//destroy the bmp file
	BMP16_map_destroy(b);
}

//compares sums of rgb
static int pixMap_cmp(const void *a, const void *b){
	//compares the sum of r g and b
	//variables
	const rgba *ra =(rgba*) a;
	const rgba *rb =(rgba*) b;
	int i = 0;
	int sumA = 0;
	int sumB = 0;

	//for as long as i is not size of rgba
	for(i = 0; i < sizeof(rgba); i++){
		//add sum of first
		sumA = ra->r + ra->b + ra->g;

		//add sum of second
		sumB = rb->r + rb->b + rb->g;
	}

	//compare sums
	if(sumA < sumB) return -1;

	if(sumA == sumB) return 0;

	return 1;
}

void pixMap_sort(pixMap *p){
	//compares the sum of r g and b
	qsort(p->image, p->width, p->height, sizeof(rgba), pixMap_cmp);
}
