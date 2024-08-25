#include "glcontroller.h"

#include <SDL.h>
#include "glfunc.h"
#include "glmodel.h"

GLController::GLController(GLModel& r_glmodel) : AbstractController(r_glmodel), m_fullscreen(SYSTEM_FULLSCREEN), m_flags(SYSTEM_SDL_FLAGS) {}

void GLController::pollInput()
{
	m_eventHandler.pollEvents();
}

void GLController::flushInput()
{
	m_eventHandler.reset();
}

void GLController::update()
{
	if (m_eventHandler.isQuitting())
		m_status = DEAD;
	if (m_eventHandler.isKeyPressed(SDLK_ESCAPE, true))
		m_status = DEAD;
	if (m_eventHandler.isKeyPressed(SDLK_F11, true))
		m_fullscreen = !m_fullscreen;

	if (((m_flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != SDL_WINDOW_FULLSCREEN_DESKTOP) == m_fullscreen)
	{
		m_flags ^= SDL_WINDOW_FULLSCREEN_DESKTOP;
		if (!(m_flags & SDL_WINDOW_FULLSCREEN_DESKTOP))
			changeSDLMode(m_flags, g_windowWidth/1.5, g_windowHeight/1.5); // garder des proportions similaires à cause des bordures
		else
			changeSDLMode(m_flags, -1, -1);
	}

	GLModel* p_model;
	p_model = dynamic_cast<GLModel*>(mp_model);

	p_model->camera()->update();
}

bool GLController::isAvailable() const
{
	return true;
}

