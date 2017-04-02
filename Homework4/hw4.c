#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS		(int)480
#define COLUMNS		(int)640

void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );

int main( int argc, char **argv )
{

	int				i, j, k, threshold[3], max[3], x, y, s;
	int 			Gx[ROWS][COLUMNS], Gy[ROWS][COLUMNS], Sgm[ROWS][COLUMNS];
	FILE			*fp;
	unsigned char	image[ROWS][COLUMNS], ximage[ROWS][COLUMNS], yimage[ROWS][COLUMNS], head[32], Sgmfinal[ROWS][COLUMNS],blm[ROWS][COLUMNS] ;
	char			filename[6][50], ifilename[50], ch;

	strcpy( filename[0], "image1" );
	strcpy( filename[1], "image2" );
	strcpy( filename[2], "image3" );
	header ( ROWS, COLUMNS, head );
	
	threshold[0] = 160;


	
		/* Read in the image */
		strcpy( ifilename, filename[k] );
		if (( fp = fopen( strcat(ifilename, ".raw"), "rb" )) == NULL )
		{
		  fprintf( stderr, "error: couldn't open %s\n", ifilename );
		  exit( 1 );
		}			
		for ( i = 0; i < ROWS ; i++ )
		  if ( fread( image[i], sizeof(char), COLUMNS, fp ) != COLUMNS )
		  {
			fprintf( stderr, "error: couldn't read enough stuff\n" );
			exit( 1 );
		  }
		fclose( fp );

		max[0] = 0; //maximum of Gx
		max[1] = 0; //maximum of Gy
		max[2] = 0; //maximum of SGM

		/* Compute Gx, Gy, SGM, find out the maximum and normalize*/
		//Gx, Gy
			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++)
			{
				if(x == 0 || y == 0 || x == ROWS-1 || y == COLUMNS-1)
				{
					Gx[x][y] = 0;
					Gy[x][y] = 0;
					continue;
				}
				Gx[x][y] = abs(-image[x-1][y-1] - 2*image[x][y-1] - image[x+1][y-1] + image[x-1][y+1] + 2*image[x][y+1] + image[x+1][y+1]);
				Gy[x][y] = abs(-image[x-1][y-1] - 2*image[x-1][y] - image[x-1][y+1] + image[x+1][y-1] + 2*image[x+1][y] + image[x+1][y+1]);

			}
		
			
			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++)
			{
				if(max[0] < Gx[x][y])
					max[0] = Gx[x][y];
				if(max[1]< Gy[x][y])
					max[1] = Gy[x][y];
			}
	
/* 			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++)
			{
				ximage[x][y] = (float)Gx[x][y] /max[0] *255;
				yimage[x][y] = (float)Gy[x][y] /max[1] *255;
			}
			
			 Write Gx to a new image
			strcpy( ifilename, filename[k] );
			if (!( fp = fopen( strcat( ifilename, "-x.ras" ), "wb" ) ))
			{
			  fprintf( stderr, "error: could not open %s\n", ifilename );
			  exit( 1 );
			}
			fwrite( head, 4, 8, fp );
			for(i = 0; i < ROWS; i++) fwrite(ximage[i], 1, COLUMNS, fp);
			fclose( fp ); */
			
		
		/* 	strcpy( ifilename, filename[k] );
			if (!( fp = fopen( strcat( ifilename, "-y.ras" ), "wb" ) ))
			{
			  fprintf( stderr, "error: could not open %s\n", ifilename );
			  exit( 1 );
			}
			fwrite( head, 4, 8, fp );
			for(i = 0; i < ROWS; i++) fwrite(yimage[i], 1, COLUMNS, fp);
			fclose( fp ); */
					
			//SGM
			int SGMmax = 0;
			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++)
			{
				Sgm[x][y] = (Gx[x][y])*(Gx[x][y]) + (Gy[x][y])*(Gy[x][y]);
				if( Sgm[x][y]  > SGMmax ) SGMmax = Sgm[x][y]; 
			}
			
			//math to not exceed 255
			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++)
			{
				//maximum
				if(max[2]<Sgm[x][y])
					max[2] = Sgm[x][y];
			}
			
			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++)
			{			
				Sgmfinal[x][y] = (float)Sgm[x][y] / max[2] * 255;
			}

/* 		/* Write SGM to a new image 
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-s.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for(i = 0; i < ROWS; i++) fwrite(Sgmfinal[i], 1, COLUMNS, fp);
	
	
		fclose( fp ); */
		
		
		/* Compute the binary image */
		
			for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLUMNS; y++){
				if (Sgmfinal[x][y] > threshold[k]){
					blm[x][y] = 255;
				}
				else {
					blm[x][y] = 0;
				}
			}
		
		
		
		/* Write the binary image to a new image */
		strcpy( ifilename, filename[k] );
		if (!( fp = fopen( strcat( ifilename, "-b.ras" ), "wb" ) ))
		{
		  fprintf( stderr, "error: could not open %s\n", ifilename );
		  exit( 1 );
		}
		fwrite( head, 4, 8, fp );
		for(i = 0; i < ROWS; i++) fwrite(blm[i], 1, COLUMNS, fp);
		fclose( fp );

		printf( "%d %d %d\n", max[0], max[1], max[2] );



	printf( "Press any key to exit: " );
	gets( &ch );
	return 0;
}

void clear( unsigned char image[][COLUMNS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLUMNS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */
	
	*p = 0x956aa659;
	*(p + 3) = 0x08000000;
	*(p + 5) = 0x01000000;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8000000;

	ch = (char*)&col;
	head[7] = *ch;
	ch ++; 
	head[6] = *ch;
	ch ++;
	head[5] = *ch;
	ch ++;
	head[4] = *ch;

	ch = (char*)&row;
	head[11] = *ch;
	ch ++; 
	head[10] = *ch;
	ch ++;
	head[9] = *ch;
	ch ++;
	head[8] = *ch;
	
	ch = (char*)&num;
	head[19] = *ch;
	ch ++; 
	head[18] = *ch;
	ch ++;
	head[17] = *ch;
	ch ++;
	head[16] = *ch;
	

	/* Big-endian for unix */
	/*
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;
*/
}
