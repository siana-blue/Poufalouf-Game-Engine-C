#include "glview.h"

#include <SDL.h>
#include <string>
#include "glfunc.h"
#include "fmodfunc.h"
#include "viewable.h"
#include "errors.h"
#include "misc.h"

GLView::GLView() : AbstractView(), m_viewport(0.0, 0.0, 1.0, Y_X_RATIO) {}

void GLView::updateViewport(float x, float y)
{
	m_viewport = PfRectangle(x, y*Y_X_RATIO, 1.0, Y_X_RATIO);
}

bool GLView::viewportContains(const Viewable& rc_viewable) const
{
	vector<PfPoint> pts_v;
	for (unsigned int i=0, size=rc_viewable.imagesCount();i<size;i++)
	{
		if (rc_viewable.imageAt(i).isStatic())
			return true;
		pts_v = rc_viewable.imageAt(i).points();
		for (unsigned int j=0, size2=pts_v.size();j<size2;j++)
		{
			if (m_viewport.contains(pts_v[j]))
				return true;
		}
	}

	return false;
}

void GLView::initializeDisplay() const
{
	clearGL();
}

void GLView::displayViewable(const Viewable& viewable) const
{
	unsigned int i = 0, size = viewable.imagesCount();
	try
	{
		for (;i<size;i++)
			drawGL(viewable.imageAt(i));
		if (viewable.getSoundIndex() != 0)
			playSound(viewable.getSoundIndex());
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible d'afficher le Viewable : ") + viewable.getName() + ", image " + itostr(i) +  ".", e);
	}
}

void GLView::finalizeDisplay() const
{
	drawTransition();
	flushGL();
	swapSDLBuffers();
}

