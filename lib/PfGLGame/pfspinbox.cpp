#include "pfspinbox.h"

#include <cstdlib>

#include "errors.h"
#include "misc.h"
#include "viewable.h"

PfSpinBox::PfSpinBox(const string& name, const PfRectangle& rect, int layer, unsigned int minValue, unsigned int maxValue, PfWidget::PfWidgetStatusMap statusMap) :
	PfWidget(name, rect.getX(), rect.getY(), rect.getW(), rect.getH(), layer, statusMap), m_value(1), m_minValue(minValue), m_maxValue(maxValue)
{
	m_repeatingSelection = true;

	try
	{
		mp_textBox = new PfTextBox(name+"_text_box", PfRectangle(rect.getX(), rect.getY(), rect.getW()-rect.getH(), rect.getH()), layer, 0.1, 0.1, FONT_TEXTURE_INDEX, statusMap);
		mp_textBox->changeText(itostr(m_value));
		mp_firstButton = new PfWidget(name+"_first_button", rect.getX()+rect.getW()-rect.getH(), rect.getY()+rect.getH()/2, rect.getH(), rect.getH()/2,
                                      getLayer(), statusMap);
		mp_secondButton = new PfWidget(name+"_second_button", rect.getX()+rect.getW()-rect.getH(), rect.getY(), rect.getH(), rect.getH()/2,
                                       getLayer(), statusMap);
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de construire cette boîte numérique.", "PfSpinBox", e);
	}
}

PfSpinBox::~PfSpinBox()
{
	delete mp_firstButton;
	delete mp_secondButton;
	delete mp_textBox;
}

void PfSpinBox::changeValue(unsigned int value)
{
	if (value >= m_minValue && value <= m_maxValue)
	{
		mp_textBox->changeText(itostr(value));
		m_value = value;
		m_modified = true;
	}
}

void PfSpinBox::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
	PfAnimation* p_1 = new PfAnimation(*p_anim);
	PfAnimation* p_2 = new PfAnimation(*p_anim);
	mp_firstButton->addAnimation(p_1, value);
	mp_secondButton->addAnimation(p_2, value);

	// Modification de l'animation ANIM_IDLE de la boîte de texte.
	if (value == ANIM_IDLE)
	{
		const PfRectangle rect = p_1->currentFrame().textCoordRectangle();
		mp_textBox->addAnimation(new PfAnimation(p_1->currentFrame().getTextureIndex(), PfRectangle(0.0, 1.0-rect.getH(), rect.getW(), rect.getH())), ANIM_IDLE);
	}

	delete p_anim;
}

void PfSpinBox::highlight()
{
	PfWidget::highlight();

	if (getPoint() != PfPoint(0.0, 0.0) && mp_firstButton->getRect().contains(getPoint()))
	{
		mp_firstButton->highlight();
		mp_secondButton->leave();
	}
	else if (getPoint() != PfPoint(0.0, 0.0) && mp_secondButton->getRect().contains(getPoint()))
	{
		mp_firstButton->leave();
		mp_secondButton->highlight();
	}
	else
	{
		mp_firstButton->leave();
		mp_secondButton->leave();
	}
}

void PfSpinBox::leave()
{
	PfWidget::leave();

	mp_firstButton->leave();
	mp_secondButton->leave();
	mp_textBox->leave();

	if (atoi(mp_textBox->getText().c_str()) < (int) m_minValue)
	{
		mp_textBox->changeText(itostr(m_minValue));
		m_value = m_minValue;
	}
}

void PfSpinBox::select()
{
    PfWidget::select();

    resetEffects();

	mp_firstButton->leave();
	mp_secondButton->leave();
	mp_textBox->select();
}

void PfSpinBox::deselect()
{
    PfWidget::deselect();

	mp_firstButton->leave();
	mp_secondButton->leave();
	mp_textBox->deselect();

	if (atoi(mp_textBox->getText().c_str()) < (int) m_minValue)
	{
		mp_textBox->changeText(itostr(m_minValue));
		m_value = m_minValue;
	}
}

void PfSpinBox::activate()
{
	PfWidget::activate();

	if (getPoint() != PfPoint(0.0, 0.0) && mp_firstButton->getRect().contains(getPoint()))
	{
		changeValue(MIN(m_value+1, m_maxValue));
		mp_textBox->deselect();
	}
	else if (getPoint() != PfPoint(0.0, 0.0) && mp_secondButton->getRect().contains(getPoint()))
	{
		changeValue(MAX(m_minValue, (m_value>0)?m_value-1:0));
		mp_textBox->deselect();
	}
	else if (getPoint() == PfPoint(0.0, 0.0) || mp_textBox->getRect().contains(getPoint()))
	{
		mp_textBox->activate();
	}
}

void PfSpinBox::update()
{
	PfWidget::update();

	mp_textBox->update();
	mp_firstButton->update();
	mp_secondButton->update();
}

Viewable* PfSpinBox::generateViewable() const
{
	Viewable* p_;
	try
	{
		mp_firstButton->setCoordRelativeToBorder(isCoordRelativeToBorder());
		mp_firstButton->setStatic(isStatic());
		mp_secondButton->setCoordRelativeToBorder(isCoordRelativeToBorder());
		mp_secondButton->setStatic(isStatic());
		mp_textBox->setCoordRelativeToBorder(isCoordRelativeToBorder());
		mp_textBox->setStatic(isStatic());
		p_ = mp_textBox->generateViewable();
		Viewable* p_tmp = mp_firstButton->generateViewable();
		p_->addImages(*p_tmp);
		delete p_tmp;
		p_tmp = mp_secondButton->generateViewable();
		p_tmp->changeAngle(180);
		p_->addImages(*p_tmp);
		delete p_tmp;
	}
	catch (PfException& e)
	{
		throw ViewableGenerationException(__LINE__, __FILE__, "Impossible de générer ce Viewable.", getName(), e);
	}

	return p_;
}

PfReturnCode PfSpinBox::processInstruction()
{
	PfWidget::processInstruction();

	PfInstruction instruction = getInstruction();
	int value = instructionValues().nextInt();
	PfReturnCode rtnCode = RETURN_NOTHING;

	string str;
	if (instruction == INSTRUCTION_MODIFY)
	{
		if (value >= '0' && value <= '9')
		{
			str = mp_textBox->getText() + (char) value;
			if ((unsigned int) atoi(str.c_str()) <= m_maxValue)
			{
				changeValue(atoi(str.c_str()));
				mp_textBox->changeText(itostr(getValue())); // et non str car on veut supprimer d'éventuels 0 devant le nombre
				m_modified = true;
				rtnCode = RETURN_OK;
			}
		}
	}
	else if (instruction == INSTRUCTION_DELETE)
	{
		mp_textBox->readInstruction(instruction, value);
		changeValue(atoi(mp_textBox->getText().c_str()));
		m_modified = true;
		rtnCode = RETURN_OK;
	}

	return rtnCode;
}
