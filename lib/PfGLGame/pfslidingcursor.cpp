#include "pfslidingcursor.h"

#include "errors.h"
#include "misc.h"
#include "viewable.h"

#include "pftext.h"

PfSlidingCursor::PfSlidingCursor(const string& name, const PfRectangle& rect, unsigned int steps, bool vertical, int layer) :
	PfWidget(name, rect.getX(), rect.getY(), rect.getW(), rect.getH(), layer), m_stepsCount(steps), m_vertical(vertical), m_currentStep(0), m_previousStep(0), m_readyToSlide(false)
{
	try
	{
		if (steps > 256)
			throw ArgumentException(__LINE__, __FILE__, "Impossible de créer un curseur à plus de 256 pas.", "steps", "PfSlidingCursor::PfSlidingCursor");
		if (m_stepsCount == 0)
			throw ArgumentException(__LINE__, __FILE__, "Impossible de créer un curseur avec 0 pas.", "steps", "PfSlidingCursor::PfSlidingCursor");

		mp_rail = new AnimatedGLItem(name+"_rail", getLayer(), PfRectangle());
		mp_cursor = new AnimatedGLItem(name+"_cursor", getLayer(), PfRectangle());
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire ce curseur.", "PfSlidingCursor", e);
	}
}

PfSlidingCursor::~PfSlidingCursor()
{
	delete mp_rail;
	delete mp_cursor;
}

void PfSlidingCursor::addText(const string& text)
{
	m_texts_v.push_back(text);
}

void PfSlidingCursor::changeStep(unsigned int step)
{
	if (step >= m_stepsCount)
		throw ArgumentException(__LINE__, __FILE__, string("Le pas n'est pas valide, valeur maximale : ") + itostr(m_stepsCount-1) + ".", "step", "PfSlidingCursor::changeStep");

	m_currentStep = step;
	m_modified = true;
}

void PfSlidingCursor::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
	mp_cursor->addAnimation(p_anim, value);

	// Modification de l'animation ANIM_IDLE du rail.
	if (value == ANIM_IDLE)
	{
		const PfRectangle rect = p_anim->currentFrame().textCoordRectangle();
		mp_rail->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(0.0, 1.0-rect.getH(), 2*rect.getW(), rect.getH())), ANIM_IDLE);
	}
}

void PfSlidingCursor::select()
{
    PfWidget::select();

	resetEffects();
}

void PfSlidingCursor::deselect()
{
    PfWidget::deselect();

	m_readyToSlide = false;
}

void PfSlidingCursor::activate()
{
	PfWidget::activate();

	m_readyToSlide = false;

	if (mp_cursor->getRect().contains(getPoint()))
	{
		resetEffects();
		m_readyToSlide = true;
	}
	else
		resetEffects();
}

bool PfSlidingCursor::slide()
{
	if (m_readyToSlide)
	{
		PfRectangle rect = this->getRect();
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

void PfSlidingCursor::update()
{
	PfWidget::update();
	m_previousStep = m_currentStep;
}

Viewable* PfSlidingCursor::generateViewable() const
{
	float vRatio = 0.25;

	float x, y, w, h;
	x = rect_x();
	y = rect_y();
	w = m_vertical?((m_texts_v.size()>0)?rect_w()*vRatio:rect_w()):rect_w();
	h = m_vertical?rect_h():((m_texts_v.size()>0)?(1-vRatio)*rect_h():rect_h());
	PfRectangle r(x, y, w, h); // rectangle contenant rail + curseur

	x = m_vertical?r.getX()+r.getW()/3:r.getX();
	y = m_vertical?r.getY():r.getY()+r.getH()/3;
	w = m_vertical?2*r.getW()/3:r.getW();
	h = m_vertical?r.getH():2*r.getH()/3;
	if (m_vertical)
		mp_rail->setRect(PfRectangle(x+w/2-h*Y_X_RATIO/2, y+h/2-w/Y_X_RATIO/2, h*Y_X_RATIO, w/Y_X_RATIO)); // car il y aura rotation
	else
		mp_rail->setRect(PfRectangle(x, y, w, h));

    h = w = 2*(m_vertical?r.getW():r.getH())/3;
	x = r.getX()+(m_vertical?0:((0.5+(float) m_currentStep)/m_stepsCount*r.getW()-w/2));
	y = m_vertical?(r.getY()+r.getH()*(1-((float) (m_currentStep+0.5))/m_stepsCount)-h/2):r.getY();
	if (m_vertical)
		mp_cursor->setRect(PfRectangle(x+w/2-h*Y_X_RATIO/2, y+h/2-w/Y_X_RATIO/2, h*Y_X_RATIO, w/Y_X_RATIO));
	else
		mp_cursor->setRect(PfRectangle(x, y, w, h));

	mp_rail->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_rail->setStatic(isStatic());
	Viewable* p_rtn = mp_rail->generateViewable();
	if (m_vertical)
		p_rtn->changeAngle(-90);
	mp_cursor->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_cursor->setStatic(isStatic());
	Viewable* p_tmp = mp_cursor->generateViewable();
	if (m_vertical)
		p_tmp->changeAngle(-90);
	p_rtn->addImages(*p_tmp);
	delete p_tmp;

	vector<GLImage*>* pn_images_v_p_n;
	for (unsigned int i=0, size=MIN(m_texts_v.size(), m_stepsCount);i<size;i++)
	{
		x = m_vertical?rect_x()+rect_w()*vRatio:rect_x()+((float)i/m_stepsCount)*rect_w();
		y = m_vertical?rect_y()+rect_h()-((i+1-0.25)/m_stepsCount)*rect_h():rect_y()+(1-vRatio)*rect_h(); // 0.25 pour centrer le texte par pas
		w = m_vertical?(1-vRatio)*rect_w():rect_w()/m_stepsCount;
		h = m_vertical?rect_h()/(m_stepsCount*2):rect_h()*vRatio;
		PfText text(m_texts_v[i], FONT_TEXTURE_INDEX, PfRectangle(x, y, w, h), 1, m_vertical?PfText::TEXT_ALIGN_LEFT:PfText::TEXT_ALIGN_CENTER);
		pn_images_v_p_n = text.toGLImages(isCoordRelativeToBorder(), isStatic());
		for (unsigned int j=0, size2=pn_images_v_p_n->size();j<size2;j++)
			p_rtn->addImage(pn_images_v_p_n->at(j));
		delete pn_images_v_p_n;
	}

	return p_rtn;
}
