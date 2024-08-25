#include "glfunc.h"

#include <map>
#include "errors.h"
#include "pngtoglloader.h"
#include "glimage.h"

SDL_Window* gp_mainScreen = 0;
SDL_Renderer* gp_renderer = 0;
map<unsigned int, unsigned int> g_texturesNames_map;

void initGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
}

void resizeGL(GLsizei width, GLsizei height)
{
	glViewport(0.0, 0.0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, (float) height/width, 0.0, 1.0);
}

void changeSDLMode(uint32_t flags, GLsizei width, GLsizei height)
{
	try
	{
		SDL_SetWindowFullscreen(gp_mainScreen, flags);
		if (width < 0 || height < 0)
		{
			SDL_GetWindowSize(gp_mainScreen, &g_windowWidth, &g_windowHeight);
			SDL_RenderSetLogicalSize(gp_renderer, g_windowWidth, g_windowHeight);
			resizeGL(g_windowWidth, g_windowHeight);
		}
		else if (width != 0 && height != 0)
		{
			g_windowWidth = width;
			g_windowHeight = height;
			SDL_SetWindowSize(gp_mainScreen, g_windowWidth, g_windowHeight);
			SDL_RenderSetLogicalSize(gp_renderer, g_windowWidth, g_windowHeight);
			resizeGL(g_windowWidth, g_windowHeight);
		}
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Erreur lors du changement de mode d'affichage SDL.", e);
	}
}

void clearGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void flushGL()
{
	glFlush();
}

void swapSDLBuffers()
{
	SDL_GL_SwapWindow(gp_mainScreen);
}

void translateCamera(float x, float y)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-x, -y*Y_X_RATIO, 0.0);
}

void addTexture(const string& fileName, unsigned int textureIndex)
{
	try
	{
		if (g_texturesNames_map.find(textureIndex) == g_texturesNames_map.end())
		{
			PNGToGLLoader image(fileName);
			image.addTextureToGL();
			g_texturesNames_map.insert(pair<unsigned int, unsigned int>(textureIndex, image.getName()));
		}
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible de charger le fichier PNG : ") + fileName + ".", e);
	}
}

void addTexture(ifstream& r_ifs, unsigned int textureIndex)
{
	try
	{
		unsigned int length;
		r_ifs.read((char*) &length, sizeof(unsigned int));

		if (g_texturesNames_map.find(textureIndex) == g_texturesNames_map.end())
		{
			char* dt_t = new char[length]; // détruit par le destructeur de PNGDataBuffer.
			for (unsigned int i=0;i<length;i++)
				r_ifs.read(&dt_t[i], sizeof(char));
			if (r_ifs.fail())
			{
				r_ifs.close();
				delete [] dt_t;
				throw PfException(__LINE__, __FILE__, "Impossible de lire le fichier PNG, peut-être est-il corrompu.");
			}
			PNGDataBuffer dataBuffer(dt_t, length);
			PNGToGLLoader image(dataBuffer);
			image.addTextureToGL();
			g_texturesNames_map.insert(pair<unsigned int, unsigned int>(textureIndex, image.getName()));
		}
		else
			r_ifs.seekg(length, ios::cur);
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Impossible de charger le fichier PNG depuis le flux passé en paramètre.", e);
	}
}

void bindTexture(unsigned int textureIndex)
{
	map<unsigned int, unsigned int>::iterator it = g_texturesNames_map.find(textureIndex);
	if (it != g_texturesNames_map.end())
		glBindTexture(GL_TEXTURE_2D, it->second);
	else
		throw ArgumentException(__LINE__, __FILE__, "L'indice de texture ne correspond à aucune texture chargée.", "textureIndex", "bindTexture");
}

void drawGL(const GLImage& rc_glImage)
{
	if (!rc_glImage.isValid())
		return;

	if (rc_glImage.isStatic())
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	GLubyte indices_t[MAX_VERTICES_PER_POLYGON];
	for (GLubyte i=0;i<MAX_VERTICES_PER_POLYGON;i++)
		indices_t[i] = i;

	if (rc_glImage.getAngle() > 1.0 || rc_glImage.getAngle() < -1.0)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(rc_glImage.getCenterX(), rc_glImage.getCenterY(), 0.0);
		glRotatef(rc_glImage.getAngle(), 0, 0, 1);
		glTranslatef(-rc_glImage.getCenterX(), -rc_glImage.getCenterY(), 0.0);
	}

	if (rc_glImage.isTextured())
	{
		bindTexture(rc_glImage.getTextureIndex());
		glInterleavedArrays(GL_T2F_C3F_V3F, 0, rc_glImage.getData());
		glDrawElements(rc_glImage.getMode(), rc_glImage.getVerticesCount(), GL_UNSIGNED_BYTE, indices_t);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		glInterleavedArrays(GL_C3F_V3F, 0, rc_glImage.getData());
		glDrawElements(rc_glImage.getMode(), rc_glImage.getVerticesCount(), GL_UNSIGNED_BYTE, indices_t);
		glEnable(GL_TEXTURE_2D);
	}

	if (rc_glImage.isStatic())
		glPopMatrix();
	if (rc_glImage.getAngle() > 1.0 || rc_glImage.getAngle() < -1.0)
		glPopMatrix();
}

void drawTransition()
{
	if (g_transitionFrame > 0)
	{
	    glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor4d(0.0, 0.0, 0.0, 1./TRANSITION_FRAMES_COUNT*g_transitionFrame);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glVertex2d(0, 0);
			glVertex2d(0, 1);
			glVertex2d(1, 1);
			glVertex2d(1, 0);
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glColor4d(0.0, 0.0, 0.0, 1.0);

		glPopMatrix();

		g_transitionFrame = (g_transitionFrame == TRANSITION_FRAMES_COUNT)?0:g_transitionFrame+1;
	}
}

void freeTextures()
{
	for(map<unsigned int, unsigned int>::iterator it=g_texturesNames_map.begin();it!=g_texturesNames_map.end();++it)
		glDeleteTextures(1, (GLuint*) &(it->second));
	g_texturesNames_map.clear();
}

PfPoint glCoordFromSDLPoint(int x, int y, bool coordRelativeToBorders)
{
	float x_x, x_y;
	if (coordRelativeToBorders)
		x_x = (x-SYSTEM_BORDER_WIDTH*g_windowWidth)/(float) g_windowWidth/(1.-2*SYSTEM_BORDER_WIDTH);
	else
		x_x = (float) x/g_windowWidth;
	x_y = (1.0 - (float) y/g_windowHeight);
	return PfPoint(x_x, x_y);
}

PfPoint glCoordFromSDLPoint(const PfPoint& rc_point, bool coordRelativeToBorders)
{
	return glCoordFromSDLPoint(rc_point.getX(), rc_point.getY(), coordRelativeToBorders);
}
