#include "graphics.h"

#include <vector>
#include <math.h>
#include <cstdlib>
#include "errors.h"

vector<vector<int> > g_noise2D_v2;

float** generate2DTexture(unsigned int r, unsigned int c, TextureGenerationMode mode, float v1, float v2, float v3)
{
	if (r == 0 || c == 0)
		throw ArgumentException(__LINE__, __FILE__, "Coordonnée nulle.", "r/c", "generate2DTexture");

	if (mode == GRAPHICS_NOISE)
		init2DNoise(r/v2+2, c/v2+2, (int) (ABS(v3)+FLOAT_MARGIN), MAX(1, (int) (ABS(v1)+FLOAT_MARGIN)));

	float** rtn_t2 = new float*[r];
	for (unsigned int i=0;i<r;i++)
	{
		rtn_t2[i] = new float[c];
		for (unsigned int j=0;j<c;j++)
		{
			switch (mode)
			{
				case GRAPHICS_FLAT:
					rtn_t2[i][j] = v1;
					break;
				case GRAPHICS_LINEAR:
					rtn_t2[i][j] = v1 - v2*ABS((int) i - (int) r/2);
					rtn_t2[i][j] += v1 - v2*ABS((int) j - (int) c/2);
					rtn_t2[i][j] /= 2;
					break;
				case GRAPHICS_DOME:
					rtn_t2[i][j] = v1 - v2 * ((int)i-(int)r/2)*((int)i-(int)r/2);
					rtn_t2[i][j] += v1 - v2 * ((int)j-(int)c/2)*((int)j-(int)c/2);
					rtn_t2[i][j] /= 2;
					break;
				case GRAPHICS_RANDOM:
					rtn_t2[i][j] = (float) rand()/RAND_MAX * v1 + 1;
					break;
				case GRAPHICS_NOISE:
					rtn_t2[i][j] = pfNoiseFct(i, j, (unsigned int) (ABS(v2)+FLOAT_MARGIN));
					break;
			}
		}
	}

	return rtn_t2;
}

float pfNoiseFct(float x, float y, unsigned int step)
{
	if (step == 0)
		throw ArgumentException(__LINE__, __FILE__, "Pas nul pour le bruit.", "step", "pfNoise");

	float ax = ABS(x) + FLOAT_MARGIN;
	float ay = ABS(y) + FLOAT_MARGIN;

	int i = (int) (ax / step);
	int j = (int) (ay / step);

	float** nVal = new float*[4];
	for (int k=0;k<4;k++)
		nVal[k] = new float[4];

	int size = g_noise2D_v2.size();
	for (int k=0;k<4;k++)
	{
		for (int l=0;l<4;l++)
			nVal[k][l] = g_noise2D_v2[MIN(size-1, MAX(0, i+k-1))][MIN(size-1, MAX(0, j+l-1))];
	}

	float rtn = cubicInterpolation2D(nVal[0][0], nVal[0][1], nVal[0][2], nVal[0][3],
									 nVal[1][0], nVal[1][1], nVal[1][2], nVal[1][3],
									 nVal[2][0], nVal[2][1], nVal[2][2], nVal[2][3],
									 nVal[3][0], nVal[3][1], nVal[3][2], nVal[3][3],
									 fmod((ax/step),1), fmod((ay/step),1));
	//float rtn = linearInterpolation2D(nVal[1][1], nVal[1][2], nVal[2][1], nVal[2][2], fmod((ax/step),1), fmod((ay/step),1));

	for (int k=0;k<4;k++)
		delete [] nVal[k];
	delete [] nVal;

	return rtn;
}

int pfRand(int seed, int max)
{
	if (seed < 0)
		throw ArgumentException(__LINE__, __FILE__, "Graine négative.", "seed", "pfRand");
	if (max <= 0)
		throw ArgumentException(__LINE__, __FILE__, "Maximum négatif ou nul.", "max", "pfRand");

	//int rtn = seed * 1103515245 + 12345;
	//rtn = (unsigned int) (rtn / 65536);
	//return ABS(round((float) rtn/65536 * max));

	srand(seed);
	rand();
	return rand() % (max+1);
}

float linearInterpolation(float a, float b, float x)
{
	return a*(1-x) + b*x;
}

float linearInterpolation2D(float a1, float b1, float a2, float b2, float x, float y)
{
	float v1 = linearInterpolation(a1, b1, x);
	float v2 = linearInterpolation(a2, b2, x);

	return linearInterpolation(v1, v2, y);
}

float cubicInterpolation(float y0, float y1, float y2, float y3, float x)
{
	float a = y3 - y2 - y0 + y1;
	float b = y0 - y1 - a;
	float c = y2 - y0;
	float d = y1;

	return a*x*x*x + b*x*x + c*x + d;
}

float cubicInterpolation2D(float y00, float y01, float y02, float y03, float y10, float y11, float y12, float y13, float y20, float y21, float y22, float y23,
						float y30,float y31, float y32, float y33, float x, float y)
{
	float v0 = cubicInterpolation(y00, y01, y02, y03, x);
	float v1 = cubicInterpolation(y10, y11, y12, y13, x);
	float v2 = cubicInterpolation(y20, y21, y22, y23, x);
	float v3 = cubicInterpolation(y30, y31, y32, y33, x);

	return cubicInterpolation(v0, v1, v2, v3, y);
}

void init2DNoise(unsigned int w, unsigned int h, int seed, int max)
{
	if (w == 0 || h == 0)
		throw ArgumentException(__LINE__, __FILE__, "Dimension nulle.", "w/h", "init2DNoise");
	if (seed < 0)
		throw ArgumentException(__LINE__, __FILE__, "Graine négative.", "seed", "init2DNoise");
	if (max <= 0)
		throw ArgumentException(__LINE__, __FILE__, "Maximum négatif ou nul.", "max", "init2DNoise");

	g_noise2D_v2.clear();

	try
	{
		srand(seed);
		for (unsigned int i=0;i<w;i++)
		{
			g_noise2D_v2.push_back(vector<int>());
			for (unsigned int j=0;j<h;j++)
				g_noise2D_v2[i].push_back(rand()%(max+1));
		}
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Erreur de génération aléatoire.", e);
	}
}

