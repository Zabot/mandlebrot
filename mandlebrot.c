#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

int min(int a, int b)
{
	return a < b ? a : b;
}

int mandlebrot(long double real, long double imag, int maxSteps)
{
	int counter = 0;
	long double carryReal = 0;
	long double carryImag = 0;

	while(counter < maxSteps && sqrt(carryReal * carryReal + carryImag * carryImag) < 2)
	{
		long double t = ((carryReal * carryReal) - (carryImag * carryImag)) + real;
		carryImag = (2 * carryReal * carryImag) + imag;
		carryReal = t;
		counter ++;
	}

	return counter;
}


typedef struct
{
	long double iStep;
	long double rStep;

	unsigned short xRes;
	unsigned short yRes;

	unsigned char *data;
	unsigned char *colors;
	unsigned short colorCount;

  unsigned int maxSteps;
} MandlebrotConstants;
	

typedef struct 
{
	long double iStart;
	long double rStart;

	unsigned int xStart;
	unsigned int xEnd;

	unsigned int yStart;
	unsigned int yEnd;

	MandlebrotConstants *cn;
} MandlebrotInfo;

void *mandlebrotThread(void *in)
{
	MandlebrotInfo *info = (MandlebrotInfo *)in;
	MandlebrotConstants *cn = (info->cn);

	long double i;
	long double r;

	unsigned int x;
	unsigned int y;

	for(i = info->iStart, y = info->yStart; y < info->yEnd; i+=cn->iStep, y++)
	{
		for(r = info->rStart, x = info->xStart; x < info->xEnd; r+=cn->rStep, x++)
		{
			unsigned int o = mandlebrot(r, i, cn->maxSteps);
			if(o == cn->maxSteps)
				memcpy(cn->data + (cn->xRes * y * 3) + x * 3, cn->colors, 3);
			else
				memcpy(cn->data + (cn->xRes * y * 3) + x * 3, cn->colors + (o%cn->colorCount + 1) * 3, 3);
		}
	}

  return NULL;
}

int main(int argc, char **argv)
{
    const unsigned int maxSteps = 500;
		const unsigned short colorCount = 60;//125 * 5;
		const long long maxZoom = 1UL << 10;
		const unsigned char numThreads = 8;

		long long zoom = 1UL;
		long double rCenter = -1.108;
		long double iCenter = 0.230;

		const unsigned int xRes = 1024;
		const unsigned int yRes = 1024;

		// Budge the point around to maximize
		printf("mandlebrot(%LF, %LF) = %i\n", rCenter, iCenter, mandlebrot(rCenter, iCenter, maxSteps));

		// Calculate the colors
		unsigned char colors[colorCount + 1][3];
		{
			unsigned int cStep = 250 / (colorCount / 5);

			short r = 0;
			short g = 0;
			short b = 0;
			unsigned int j = 0;

			// Phase in blue
			for(b = 0; b < 250; b += cStep, j++)
			{
				colors[j][0] = r;
				colors[j][1] = g;
				colors[j][2] = b;
			}

			// Phase in green
			for(g = 0; g < 250; g += cStep, j++)
			{
				colors[j][0] = r;
				colors[j][1] = g;
				colors[j][2] = b;
			}

			// Phase out blue
			for(; b > 0; b -= cStep, j++)
			{
				colors[j][0] = r;
				colors[j][1] = g;
				colors[j][2] = b;
			}

			// Phase in red
			for(r = 0; r < 250; r += cStep, j++)
			{
				colors[j][0] = r;
				colors[j][1] = g;
				colors[j][2] = b;
			}

			// Phase out green
			for(; g > 0; g -= cStep, j++)
			{
				colors[j][0] = r;
				colors[j][1] = g;
				colors[j][2] = b;
			}
		}

		// Calculate constants
		long double aspectRatio = yRes / (long double)xRes;

		unsigned char *image = malloc(yRes * xRes * 3);

		MandlebrotConstants consts;
		consts.xRes   = xRes;
		consts.yRes   = yRes;
		consts.data   = image;
		consts.colors = (void*)&colors;
		consts.colorCount = colorCount;
    consts.maxSteps = maxSteps;
		
		for(; zoom < maxZoom; zoom += zoom)
		{
			printf("Starting %lli\n", zoom);
			long double rRange = 4.0 / zoom;
			long double iRange = rRange * aspectRatio;

			consts.rStep = rRange / xRes;
			consts.iStep = iRange / yRes;

			long double rStart = rCenter - rRange / 2;
			long double iStart = iCenter - iRange / 2;

			pthread_t ids[numThreads];
			MandlebrotInfo infos[numThreads];
			// Render
			for(unsigned char t = 0; t < numThreads; t++)
			{
				infos[t].cn = &consts;
				infos[t].yStart = (yRes / numThreads) * t;
				infos[t].yEnd = (yRes / numThreads) * (t + 1);
				infos[t].xStart = 0;
				infos[t].xEnd = xRes;
				infos[t].iStart = iStart + (infos[t].yStart * consts.iStep);
				infos[t].rStart = rStart;

				if(t < numThreads - 1)
					pthread_create(&ids[t], NULL, &mandlebrotThread, &infos[t]);
				else
					mandlebrotThread(&infos[t]);
			}

			for(unsigned char t = 0; t < numThreads - 1; t++)
				pthread_join(ids[t], NULL);
			printf("Done\n");

			// Write the bitmap
			printf("Writing x%lli...\n", zoom);
			{
				char file[100];
				sprintf(file, "seq/mandlebrot%lli.ppm", zoom);
				FILE *fp = fopen(file, "wb");
        fprintf(fp, "P6\n%d %d\n255\n", xRes, yRes);
        fwrite(image, 1, 3*yRes*xRes, fp);
        fclose(fp);
			}
			printf("Done\n");
		}
}

