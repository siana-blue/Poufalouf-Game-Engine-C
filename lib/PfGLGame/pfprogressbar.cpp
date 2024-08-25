#include "pfprogressbar.h"

#include "errors.h"
#include "misc.h"
#include "viewable.h"

PfProgressBar::PfProgressBar(const string& name, const PfRectangle& rect, unsigned int steps, int layer) :
    PfWidget(name, rect.getX(), rect.getY(), rect.getW(), rect.getH(), layer), m_stepsCount(steps), m_currentStep(0)
{
	try
	{
		if (steps == 0)
			throw ArgumentException(__LINE__, __FILE__, "Impossible de créer une barre de progression de 0 division.", "steps", "PfProgressBar::PfProgressBar");
		mp_left = new AnimatedGLItem(name+"_left", getLayer(), PfRectangle(rect.getX(), rect.getY(), 1./(steps+2)*rect.getW(), rect.getH()));
		mp_right = new AnimatedGLItem(name+"_right", getLayer(), PfRectangle(rect.getX()+(float) (steps+1)/(steps+2)*rect.getW(), rect.getY(), 1./(steps+2)*rect.getW(), rect.getH()));

		mp_bar_t = new AnimatedGLItem*[steps];
		mp_tiles_t = new AnimatedGLItem*[steps];
		for (unsigned int i=0;i<steps;i++)
		{
			mp_bar_t[i] = new AnimatedGLItem(name+"_bar_"+itostr(i), getLayer(), PfRectangle(rect.getX()+(1.+i)/(steps+2)*rect.getW(), rect.getY(), rect.getW()/(steps+2), rect.getH()));
			mp_tiles_t[i] = new AnimatedGLItem(name+"_tile_"+itostr(i), getLayer(), PfRectangle(rect.getX()+(1.+i)/(steps+2)*rect.getW(), rect.getY()+1./3*rect.getH(), rect.getW()/(steps+2), rect.getH()/3));
		}
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire cette barre de progression.", "PfProgressBar", e);
	}
}

PfProgressBar::~PfProgressBar()
{
	delete mp_left;
	delete mp_right;
	for (unsigned int i=0;i<m_stepsCount;i++)
	{
		delete mp_bar_t[i];
		delete mp_tiles_t[i];
	}
	delete [] mp_bar_t;
	delete [] mp_tiles_t;
}

void PfProgressBar::changeStep(float ratio)
{
	float f = MAX(0.0, MIN(1.0, ratio));

	m_currentStep = (f+FLOAT_MARGIN)*m_stepsCount;

	m_modified = true;
}

void PfProgressBar::changeValue(int value)
{
	changeStep(value/100.0);
}

void PfProgressBar::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
	for (unsigned int i=0;i<m_stepsCount;i++)
		mp_tiles_t[i]->addAnimation(new PfAnimation(*p_anim), value);

	// Modification de l'animation ANIM_IDLE de la barre
	if (value == ANIM_IDLE)
	{
		PfRectangle rect = p_anim->currentFrame().textCoordRectangle();
		mp_left->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(1.0-3*rect.getW(), 0.0, rect.getW(), rect.getH()*3)), ANIM_IDLE);
		mp_right->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(1.0-rect.getW(), 0.0, rect.getW(), rect.getH()*3)), ANIM_IDLE);
		for (unsigned int i=0;i<m_stepsCount;i++)
			mp_bar_t[i]->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(1.0-2*rect.getW(), 0.0, rect.getW(), rect.getH()*3)), ANIM_IDLE);
	}

	delete p_anim;
}

Viewable* PfProgressBar::generateViewable() const
{
	mp_left->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_left->setStatic(isStatic());
	mp_right->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_right->setStatic(isStatic());
	for (unsigned int i=0;i<m_stepsCount;i++)
	{
		mp_bar_t[i]->setCoordRelativeToBorder(isCoordRelativeToBorder());
		mp_bar_t[i]->setStatic(isStatic());
		mp_tiles_t[i]->setCoordRelativeToBorder(isCoordRelativeToBorder());
		mp_tiles_t[i]->setStatic(isStatic());
	}

	Viewable* p_rtn = mp_left->generateViewable();
	Viewable* p_tmp;
	p_tmp = mp_right->generateViewable();
	p_rtn->addImages(*p_tmp);
	delete p_tmp;
	for (unsigned int i=0;i<m_stepsCount;i++)
	{
		p_tmp = mp_bar_t[i]->generateViewable();
		p_rtn->addImages(*p_tmp);
		delete p_tmp;
		if (i < m_currentStep)
		{
			p_tmp = mp_tiles_t[i]->generateViewable();
			p_rtn->addImages(*p_tmp);
			delete p_tmp;
		}
	}

	return p_rtn;
}
