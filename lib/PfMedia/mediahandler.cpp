#include "mediahandler.h"

#include "errors.h"

#include "glfunc.h"
#include "fmodfunc.h"

bool g_SDLOpen = false;
bool g_FMODOpen = false;

void initEverything(const string& appName)
{
	uint32_t flags = SYSTEM_SDL_FLAGS; // Variable utilisée pour la gestion des flags SDL.

	// Initialisation de la SDL

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		throw PfException(__LINE__, __FILE__, string("L'initialisation de la SDL a échoué : ") + SDL_GetError());
	g_SDLOpen = true;

	#ifdef SYSTEM_LINUX
	// A réécrire pour prendre en compte SDL 2.0
	// on recrée l'écran après avoir mesuré la taille du bureau (au moyen d'une taille d'initialisation de (0;0)).
	gp_mainScreen = SDL_SetVideoMode(g_windowWidth, g_windowHeight, 32, flags);
	if (gp_mainScreen == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de créer l'écran principal SDL : ") + SDL_GetError());
	g_windowWidth = (int) (gp_mainScreen->w/1.05);
	g_windowHeight = (int) (gp_mainScreen->h/1.05);
	g_windowWidth = g_windowHeight = MIN(g_windowWidth, g_windowHeight);
	g_windowWidth /= (1 - 2*SYSTEM_BORDER_WIDTH);
	while (g_windowWidth >= gp_mainScreen->w)
	{
		g_windowWidth /= 1.1;
		g_windowHeight /= 1.1;
	}
	#endif

	SDL_CreateWindowAndRenderer(0, 0, flags, &gp_mainScreen, &gp_renderer);
	if (gp_mainScreen == 0 || gp_renderer == 0)
	{
		SDL_Quit();
		throw PfException(__LINE__, __FILE__, string("La création du contexte SDL a échoué : ") + SDL_GetError());
	}
	SDL_GL_CreateContext(gp_mainScreen);

	SDL_SetWindowTitle(gp_mainScreen, appName.c_str());
	SDL_ShowCursor(g_showCursor);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Initialisation d'OpenGL

	initGL();
	if (SYSTEM_FULLSCREEN)
		SDL_GetWindowSize(gp_mainScreen, &g_windowWidth, &g_windowHeight);
	else
		SDL_SetWindowSize(gp_mainScreen, g_windowWidth, g_windowHeight);
	SDL_RenderSetLogicalSize(gp_renderer, g_windowWidth, g_windowHeight);
	resizeGL(g_windowWidth, g_windowHeight);

	// Initialisation de FMOD

	initFMOD();
	g_FMODOpen = true;
}

void closeEverything()
{
	try
	{
		if (g_SDLOpen)
			SDL_Quit();

		freeTextures();

		if (g_FMODOpen)
			closeFMOD();
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Une erreur est survenue à la fermeture du programme.", e);
	}

	#ifndef NDEBUG
	if (g_log.is_open())
		g_log.close();
	#endif
}
