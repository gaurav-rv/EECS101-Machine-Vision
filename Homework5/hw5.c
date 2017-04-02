#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define ROWS	(int)480
#define COLS	(int)640
#define pi		(double) 3.14
#define sqr(x)	((x)*(x))

void clear( unsigned char image[][COLS] );
void header( int row, int col, unsigned char head[32] );

int main( int argc, char **argv )
{
	int				i,j, x, y;
	int				dedx, dedy, sgm, max, max1, max2, max3, theta1, theta2, theta3, rho1, rho2, rho3;
	int				sgm_threshold, hough_threshold, voting[180][1550], tempI[ROWS][COLS], tempX[ROWS][COLS], tempY[ROWS][COLS];
	double 			rho, theta, radian, cosValue, sinValue, tanValue, radian1, radian2, radian3;
	FILE			*fp;
	unsigned char	image[ROWS][COLS], head[32];
	char			filename[50], ch;
	
	strcpy ( filename, "image" );
	memset ( voting, 0, sizeof(voting));
	header (ROWS, COLS, head);
	
	if (( fp = fopen( strcat(filename, ".raw"), "rb" )) == NULL )
	{
		fprintf( stderr, "error: couldn't open %s\n", filename );
		exit(1);
	}

	for ( i = 0 ; i < ROWS ; i++ )
		if (( fread( image[i], sizeof(char), COLS, fp ) != COLS))
		{
			fprintf( stderr, "error: couldn't read %s\n", argv[1] );
			exit(1);
		}
	fclose(fp);
	
	//put threshold value to get the binary value
	sgm_threshold = 100;
	max = 0;
	
	for(i = 0; i<ROWS; i++)
		for(j = 0; j < COLS; j++)
		{
			tempI[i][j] = image[i][j];
		}
	
	/* Compute Gx, Gy, SGM, find out the maximum and normalize*/
		for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLS; y++)
			{
				tempI[x][y] =image[x][y];
			}
		//Gy
		for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLS; y++)
			{
				if(x == 0 || y == 0 || x == ROWS-1 || y == COLS-1)
				{
					tempY[x][y] = 0;
					continue;
				}
				tempY[x][y] = abs(-tempI[x-1][y-1] - 2*tempI[x-1][y] - tempI[x-1][y+1] + tempI[x+1][y-1] + 2*tempI[x+1][y] + tempI[x+1][y+1]);
			}
			
		
			
		//Gx
		for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLS; y++)
			{
				if(x == 0 || y == 0 || x == ROWS-1 || y == COLS-1)
				{
					tempX[x][y] = 0;
					continue;
				}
				tempX[x][y] = abs(-tempI[x-1][y-1] - 2*tempI[x][y-1] - tempI[x+1][y-1] + tempI[x-1][y+1] + 2*tempI[x][y+1] + tempI[x+1][y+1]);
			}
	
		//SGM
		for(x = 0; x<ROWS; x++)
			for(y = 0; y<COLS; y++)
			{
				tempI[x][y] = (tempX[x][y])*(tempX[x][y]) + (tempY[x][y])*(tempY[x][y]);
			}
		//Maximum
		for(x = 0; x<ROWS; x++)
			for(y = 0; y< COLS; y++)
			{
				if(max < tempI[x][y])
					max = tempI[x][y];
			}
	
	
	//Normalize
	for( i = 0; i < ROWS; i++)
		for(j = 0; j < COLS; j++)
		{
			image[i][j] = (float) tempI[i][j] / max * 255;
		}
		
		
	strcpy(filename, "image");
	/*Write the SGM to the new image*/
	if(!( fp = fopen( strcat( filename, "-sgm.ras"), "wb") ))
	{
		fprintf( stderr, "error: could not open %s\n", filename );
		exit( 1 );
	}		
	fwrite(head, 4, 8, fp);
	for( i = 0; i< ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);
	/*END write SGM image*/
	
	//get the binary version of SGM image
	
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
	/*END write Binary image*/
	

	//initialize voting
	for(i = 0; i < 180; i++)
		for(j = 0; j<1550; j++)
			voting[i][j] = 0;
		
	//calculate rho and theta
	for( i = 0; i < ROWS; i++)
		for( j = 0; j < COLS; j++ )
		{
			if(image[i][j] == 255)
			{
				for( theta = 0; theta < 180; theta ++)
				{
					radian = pi/180 * theta;
					rho = j * cos(radian) - i * sin(radian);
					if(rho < -775 || rho >= 774)
					{
						printf("rho is out of range:\n rho = %f, theta = %f\n i = %d, j = %d", rho, theta, i, j);
						return 1;
					}

					voting[(int)theta ][(int)rho +775] += 1; //increment 1 when finding rho and theta
				}
			}
		}
		
	
	max1 = INT_MIN;
	max2 = INT_MIN;
	max3 = INT_MIN;
	theta1 = INT_MIN; 
	theta2 = INT_MIN;
	theta3 = INT_MIN;
	rho1 = INT_MIN;
	rho2 = INT_MIN;
	rho3 = INT_MIN;
	
	//MAX1
	for(i = 0; i<180; i++)
		for(j = 0; j< 1550; j++)
			if(max1<voting[i][j])
			{
				max1 = voting[i][j];
				theta1 = i;
				rho1 = j;
			}
		
	voting[theta1][rho1] = INT_MIN;

	//MAX2
	for(i = 0; i<180; i++)
		for(j = 0; j< 1550; j++)
			if(max2<voting[i][j])
			{
				max2 = voting[i][j];
				theta2 = i;
				rho2 = j;
			}
	voting[theta2][rho2] = INT_MIN;
	
	//MAX3
	for(i = 0; i<180; i++)
		for(j = 0; j< 1550; j++)
			if(max3<voting[i][j])
			{
				if(voting[i][j]>max2 -20  && voting[i][j] < max2 +20)
					continue;
				max3 = voting[i][j];
				theta3 = i;
				rho3 = j;
			}
			
		
	printf("max  :%d=====%d=====%d\n", max1, max2, max3);
	printf("theta:%d=====%d=====%d\n", theta1, theta2, theta3);
	printf("rho  :%d=====%d=====%d\n", rho1 -775, rho2 -775, rho3 -775);
	
	rho1 -= 775;
	rho2 -= 775;
	rho3 -= 775;
	
	/*for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS; j++)
			image[i][j] = 0;*/
	
	radian1 = pi/180 * theta1;
	radian2 = pi/180 * theta2;
	radian3 = pi/180 * theta3;
	for(i = 0; i< ROWS; i++)
	{		
		j = (float) i*tan(radian1) + rho1/cos(radian1);
		if(j < COLS && j >= 0)
			image[i][j] = 255;
		j = (float) i*tan(radian2) + rho2/cos(radian2);
		if(j < COLS && j >= 0)
			image[i][j] = 255;
		j = (float) i*tan(radian3) + rho3/cos(radian3);
		if(j < COLS && j >= 0)
			image[i][j] = 255;
	}
		
	
			//Write the Voting image
	strcpy(filename, "image");
	if(!(fp = fopen( strcat( filename, "-voting.ras"), "wb") ))
	{
		fprintf( stderr, "error: could not open %s\n", filename );
		exit(1);
	}
	fwrite(head, 4, 8, fp);
	for( i = 0; i< ROWS; i++) fwrite(image[i], 1, COLS, fp);
	fclose(fp);
	
	// END write Voting image
	
	return 0;

}

void clear( unsigned char image[][COLS] )
{
	int	i,j;
	for ( i = 0 ; i < ROWS ; i++ )
		for ( j = 0 ; j < COLS ; j++ ) image[i][j] = 0;
}

void header( int row, int col, unsigned char head[32] )
{
	int *p = (int *)head;
	char *ch;
	int num = row * col;

	/* Choose little-endian or big-endian header depending on the machine. Don't modify this */
	/* Little-endian for PC */
	/*
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
	*/
 
	/* Big-endian for unix */
	
	*p = 0x59a66a95;
	*(p + 1) = col;
	*(p + 2) = row;
	*(p + 3) = 0x8;
	*(p + 4) = num;
	*(p + 5) = 0x1;
	*(p + 6) = 0x0;
	*(p + 7) = 0xf8;

}
