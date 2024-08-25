#include "pfscrollbar.h"

#include "errors.h"
#include "misc.h"
#include "viewable.h"

PfScrollBar::PfScrollBar(const string& name, const PfRectangle& barCoordRect, float buttonW, float buttonH, unsigned int steps, bool vertical, unsigned int layer) :
	PfWidget(name, barCoordRect.getX()-(vertical?0:buttonW/2), barCoordRect.getY()-(vertical?buttonH/2:0), barCoordRect.getW()+(vertical?0:buttonW), barCoordRect.getH()+(vertical?buttonH:0), layer),
	m_stepsCount(steps), m_vertical(vertical), m_currentStep(0), m_previousStep(0), m_readyToSlide(false)
{
	m_repeatingSelection = true;

	try
	{
		if (steps > 256)
			throw ArgumentException(__LINE__, __FILE__, "Impossible de créer un ascenseur à plus de 256 pas.", "steps", "PfScrollBar::PfScrollBar");
		if (steps == 0)
			throw ArgumentException(__LINE__, __FILE__, "Impossible de créer un ascenseur à 0 pas.", "steps", "PfScrollBar::PfScrollBar");

		mp_firstButton = new PfWidget(name+"_first_button", barCoordRect.getX()-(m_vertical?0:buttonW/2), barCoordRect.getY()+(m_vertical?barCoordRect.getH()-buttonH/2:0), buttonW, buttonH);
		mp_secondButton = new PfWidget(name+"_second_button", barCoordRect.getX()+(m_vertical?0:barCoordRect.getW()-buttonW/2), barCoordRect.getY()-(m_vertical?buttonH/2:0), buttonW, buttonH);
		mp_rail = new AnimatedGLItem(name+"_rail", getLayer(), barCoordRect);
		mp_bar = new AnimatedGLItem(name+"_bar", getLayer(), PfRectangle(barCoordRect.getX()+(vertical?0:buttonW/4),
																		barCoordRect.getY()+(vertical?(steps-1)*(barCoordRect.getH()-buttonH/2)/steps+buttonH/4:0),
																		(vertical?barCoordRect.getW():(barCoordRect.getW()-buttonW/2)/steps),
																		(vertical?(barCoordRect.getH()-buttonH/2)/steps:barCoordRect.getH())));
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire cette barre.", "PfScrollBar", e);
	}
}

PfScrollBar::~PfScrollBar()
{
	delete mp_firstButton;
	delete mp_secondButton;
	delete mp_rail;
	delete mp_bar;
}

void PfScrollBar::changeStep(unsigned int step)
{
	if (step >= m_stepsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Le pas n'est pas valide, valeur maximale : ") + itostr(m_stepsCount-1) + ".", "step", "PfScrollBar::changeStep");

	mp_bar->shift((m_vertical?PfOrientation::SOUTH:PfOrientation::EAST), (m_vertical?mp_bar->rect_h():mp_bar->rect_w())*((int) step - (int) m_currentStep));
	m_currentStep = step;
	m_modified = true;
}

void PfScrollBar::highlight()
{
	PfWidget::highlight();

	mp_firstButton->leave();
	mp_secondButton->leave();

	if (getPoint() != PfPoint(0.0, 0.0) && mp_firstButton->getRect().contains(getPoint()))
		mp_firstButton->highlight();
	if (getPoint() != PfPoint(0.0, 0.0) && mp_secondButton->getRect().contains(getPoint()))
		mp_secondButton->highlight();
}

void PfScrollBar::leave()
{
	PfWidget::leave();

	mp_firstButton->leave();
	mp_secondButton->leave();
}

void PfScrollBar::select()
{
	PfWidget::select();

	if (mp_firstButton->getRect().contains(getPoint()))
		mp_firstButton->select();
	else if (mp_secondButton->getRect().contains(getPoint()))
		mp_secondButton->select();
	else
	{
		mp_firstButton->deselect();
		mp_secondButton->deselect();
	}
}

void PfScrollBar::deselect()
{
	PfWidget::deselect();

	mp_firstButton->deselect();
	mp_secondButton->deselect();
	m_readyToSlide = false;
}

void PfScrollBar::activate()
{
	PfWidget::activate();

	m_readyToSlide = false;

	if (mp_firstButton->getRect().contains(getPoint()) && m_currentStep > 0)
	{
		changeStep(m_currentStep-1);
		resetEffects();
		addEffects(EFFECT_PREV | 1);
	}
	else if (mp_secondButton->getRect().contains(getPoint()) && m_currentStep < m_stepsCount - 1)
	{
		changeStep(m_currentStep+1);
		resetEffects();
		addEffects(EFFECT_NEXT | 1);
	}
	else if (mp_bar->getRect().contains(getPoint()))
	{
		resetEffects();
		m_readyToSlide = true;
	}
	else
		resetEffects();
}

bool PfScrollBar::slide()
{
	resetEffects();

	if (m_readyToSlide)
	{
		PfRectangle rect(mp_rail->rect_x()+(m_vertical?0:mp_firstButton->rect_w()/4),
						mp_rail->rect_y()+(m_vertical?mp_firstButton->rect_h()/4:0),
						mp_rail->rect_w()-(m_vertical?0:mp_firstButton->rect_w()/2),
						mp_rail->rect_h()-(m_vertical?mp_firstButton->rect_h()/2:0));
		if (rect.contains(getPoint()))
		{
			float f;
			if (m_vertical)
				f = (1.0-(getPoint().getY()-rect.getY())/rect.getH())*m_stepsCount;
			else
				f = (getPoint().getX()-rect.getX())/rect.getW()*m_stepsCount;
			changeStep((unsigned int) f);

			int diff = (int) m_previousStep - (int) m_currentStep;
			if (diff < 0)
				addEffects(EFFECT_NEXT | -diff);
			else
				addEffects(EFFECT_PREV | diff);
			m_modified = true;
			return true;
		}
	}

	return false;
}

void PfScrollBar::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
	PfAnimation* p_1 = new PfAnimation(*p_anim);
	PfAnimation* p_2 = new PfAnimation(*p_anim);
	mp_firstButton->addAnimation(p_1, value);
	mp_secondButton->addAnimation(p_2, value);

	// Modification de l'animation ANIM_IDLE de cette barre
	if (value == ANIM_IDLE)
	{
		const PfRectangle rect = p_1->currentFrame().textCoordRectangle();
		mp_rail->addAnimation(new PfAnimation(p_1->currentFrame().getTextureIndex(), PfRectangle(0.0, 1.0-rect.getH(), rect.getW(), rect.getH())), ANIM_IDLE);
		mp_bar->addAnimation(new PfAnimation(p_1->currentFrame().getTextureIndex(), PfRectangle(rect.getW(), 1.0-rect.getH(), rect.getW(), rect.getH())), ANIM_IDLE);
	}

	delete p_anim;
}

void PfScrollBar::update()
{
	PfWidget::update();
	m_previousStep = m_currentStep;
}

Viewable* PfScrollBar::generateViewable() const
{
	mp_rail->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_rail->setStatic(isStatic());
	Viewable* p_rtn = mp_rail->generateViewable();
	mp_bar->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_bar->setStatic(isStatic());
	Viewable* p_tmp = mp_bar->generateViewable();
	p_rtn->addImages(*p_tmp);
	delete p_tmp;
	mp_firstButton->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_firstButton->setStatic(isStatic());
	p_tmp = mp_firstButton->generateViewable();
	if (m_vertical)
		p_tmp->changeAngle(180);
	else
		p_tmp->changeAngle(-90);
	p_rtn->addImages(*p_tmp);
	delete p_tmp;

	mp_secondButton->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_secondButton->setStatic(isStatic());
	p_tmp = mp_secondButton->generateViewable();
	if (!m_vertical)
		p_tmp->changeAngle(90);
	p_rtn->addImages(*p_tmp);
	delete p_tmp;

	return p_rtn;
}
