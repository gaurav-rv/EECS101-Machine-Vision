#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS	480
#define COLS	640

#define sqr(x)	((x)*(x))

void clear( unsigned char image[][COLUMNS] );
void header( int row, int col, unsigned char head[32] );

int main( int argc, char** argv )
{
	int				i,j;
	int				dedx, dedy, sgm, rad;
	int				sgm_threshold, hough_threshold, voting[180][400];
	FILE*			fp;
	unsigned char	image[ROWS][COLS];
	char			filename[50];
	int 				pi = 3.14;
	
	strcpy ( filename, "image.raw" );
	memset ( voting, 0, sizeof(int) * 180 * 1600 );


	if (!( fp = fopen( filename, "rb" ) ))
	{
		fprintf( stderr, "error: couldn't open %s\n", argv[1] );
		exit(1);
	}

	for ( i = 0 ; i < ROWS ; i++ )
		if (!( COLS == fread( image[i], sizeof(char), COLS, fp ) ))
		{
			fprintf( stderr, "error: couldn't read %s\n", argv[1] );
			exit(1);
		}
	fclose(fp);

		sgm_threshold = 0;
		max = 0; 
/* 	for ( i = ROWS-2 ; i > 0 ; i-- )
		for ( j = COLS-2 ; j > 0 ; j-- )
		{
			dedx =
					  -image[i-1][j-1] +   image[i-1][j+1] +
					-2*image[ i ][j-1] + 2*image[ i ][j+1] +
					  -image[i+1][j-1] +   image[i+1][j+1] ;

			dedy =
				-image[i-1][j-1] - 2*image[i-1][ j ] - image[i-1][j+1] +
				 image[i+1][j-1] + 2*image[i+1][ j ] + image[i+1][j+1] ;

			sgm = sqr(dedx) + sqr(dedy);

		} */

// Gx Gy		
		for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLS; y++)
			{
				if(x == 0 || y == 0 || x == ROWS-1 || y == COLS-1)
				{
					Gx[x][y] = 0;
					Gy[x][y] = 0;
					continue;
				}
				Gx[x][y] = abs(-image[x-1][y-1] - 2*image[x][y-1] - image[x+1][y-1] + image[x-1][y+1] + 2*image[x][y+1] + image[x+1][y+1]);
				Gy[x][y] = abs(-image[x-1][y-1] - 2*image[x-1][y] - image[x-1][y+1] + image[x+1][y-1] + 2*image[x+1][y] + image[x+1][y+1]);

			}
		//SGM
		int SGMmax = 0;
		for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLS; y++)
			{
				Sgm[x][y] = (Gx[x][y])*(Gx[x][y]) + (Gy[x][y])*(Gy[x][y]);
				if( Sgm[x][y]  > SGMmax ) SGMmax = Sgm[x][y]; 
			}
			
		for( i = 0; i < ROWS; i++)
			for(j = 0; j < COLS; j++)
			{
				image[i][j] = (float) tempI[i][j] / SGMmax * 255;
			}
			
		for( i = 0; i < ROWS; i++)
			for( j = 0; j < COLS; j++)
			{
				if( sgm_threshold > image[i][j]) image[i][j] = 0;
				else{ image[i][j] = 255; }
			}
			
			strcpy(filename, "image");	
			/*Write the Binary to the new image*/
			if(!( fp = fopen( strcat( filename, "-binary.ras"), "wb") ))
			{
				fprintf( stderr, "error: could not open %s\n", filename );
				exit( 1 );
			}		
			fwrite(head, 4, 8, fp);
			for( i = 0; i< ROWS; i++) fwrite(image[i], 1, COLS, fp);
			fclose(fp);	
			
		for( i = 0; i < ROWS; i++)
		for( j = 0; j < COLS; j++ )
		{
			if(image[i][j] == 255)
			{
				for( theta = 0; theta < 180; theta ++)
				{
					radian = pi/180 * theta;
					rho = j * cos(radian) - i * sin(radian);
					if(rho < -799 || rho >= 799)
					{
						printf("rho is out of range:\n rho = %f, theta = %f\n i = %d, j = %d", rho, theta, i, j);
						return 1;
					}

					voting[(int)theta ][(int)rho +775] += 1; //increment 1 when finding rho and theta
				}
			}
		}
		int maxvote[3];
		int maxtheta[3];
		int maxrho[3];
				
		for(i = 0; i<180; i++){
		for(j = 0; j< 1600; j++)
			if(maxvote[0] < voting[i][j])
			{
				maxvote[0] = voting[i][j];
				maxtheta[0] = i;
				maxrho[0] = j;
			}
		}
		
		voting[maxtheta[0]][maxrho[0]] = 0;
		
		for(i = 0; i<180; i++){
		for(j = 0; j< 1600; j++)
			if(maxvote[1] < voting[i][j])
			{
				maxvote[1] = voting[i][j];
				maxtheta[1] = i;
				maxrho[1] = j;
			}
		}
		
		voting[maxtheta[1]][maxrho[1]] = 0;
		
		for(i = 0; i<180; i++){
		for(j = 0; j< 1600; j++)
			if(maxvote[2] < voting[i][j])
			{
				maxvote[2] = voting[i][j];
				maxtheta[2] = i;
				maxrho[2] = j;
			}
		}
		maxrho[0] = maxrho[0] -800;
		maxrho[1] = maxrho[1] -800;
		maxrho[2] = maxrho[2] -800;
		
		printf("max 1 = %d,       max2 = %d,        max3 = %d",maxvote[0], maxvote[1],maxvote[2] );	
		printf("max 1 = %d,       max2 = %d,        max3 = %d",maxtheta[0], maxtheta[1],maxtheta[2] );	
		printf("max 1 = %d,       max2 = %d,        max3 = %d",maxrho[0] , maxrho[1], maxrho[2]  );	

		int radian[3];
		radian[0] = pi/180 *maxtheta[0];
		radian[1] = pi/180 *maxtheta[1];
		radian[2] = pi/180 *maxtheta[2];
		
		int it;
		for(i = 0; i < ROWS; i++){
			for(it  = 0; it <= 2; it ++){
				int l = (float) i*tan(radian[it]) + rho1/cos(radian[it]);
				if(l < COLS && l >= 0)
				image[i][l] = 255;
			}
		}
		
	strcpy(filename, "image");
	if(!(fp = fopen( strcat( filename, "-voting.ras"), "wb") ))
	{
		fprintf( stderr, "error: could not open %s\n", filename );
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for( i = 0; i< ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);
		
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

