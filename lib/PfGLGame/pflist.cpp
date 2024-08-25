#include "pflist.h"

#include "errors.h"
#include "misc.h"
#include "viewable.h"

#define PFLIST_ARROW_SIZE 0.75 //!< La taille d'une flèche de défilement par rapport à celle d'un bouton d'une liste.
#define PFLIST_SPACE_SIZE 0.25 //!< La taille de l'interligne par rapport à celle d'un bouton d'une liste.

PfList::PfList(const string& name, float x, float y, float w, float buttonSize, unsigned int rows, unsigned int layer,
               float hMargin, float vMargin, PfWidget::PfWidgetStatusMap statusMap) :
	PfWidget(name, x, y, w, 2*PFLIST_ARROW_SIZE*buttonSize + buttonSize*rows + PFLIST_SPACE_SIZE*buttonSize*(rows+1), layer, statusMap),
	m_currentIndex(0), m_firstButtonIndex(1)
{
	m_repeatingSelection = true;

	try
	{
		if (rows > 255)
			throw ArgumentException(__LINE__, __FILE__, "Impossible de créer une liste de plus de 255 boutons.", "rows", "PfList::PfList");

		for (unsigned int i=0;i<rows;i++)
			mp_buttons_v.push_back(new PfWidget(name + "_button_" + itostr(i+1),
												PfRectangle(x, y + PFLIST_ARROW_SIZE*buttonSize + (rows-i)*PFLIST_SPACE_SIZE*buttonSize + (rows-i-1)*buttonSize, w, buttonSize),
                                                MAX_LAYER, 1, hMargin, vMargin, statusMap));
		mp_upArrow = new PfWidget(name + "_upArrow",
                                  x + w/2 - PFLIST_ARROW_SIZE/2*buttonSize, y + PFLIST_ARROW_SIZE*buttonSize + (rows+1)*PFLIST_SPACE_SIZE*buttonSize + rows*buttonSize,
                                  PFLIST_ARROW_SIZE*buttonSize, PFLIST_ARROW_SIZE*buttonSize,
                                  MAX_LAYER, statusMap);
		mp_downArrow = new PfWidget(name + "_downArrow",
                                    x + w/2 - PFLIST_ARROW_SIZE/2*buttonSize, y, PFLIST_ARROW_SIZE*buttonSize, PFLIST_ARROW_SIZE*buttonSize,
									MAX_LAYER, statusMap);
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de créer cette liste.", "PfList", e);
	}
}

PfList::~PfList()
{
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (mp_buttons_v[i] != 0)
			delete mp_buttons_v[i];
	}
	delete mp_upArrow;
	delete mp_downArrow;
}

void PfList::scroll(bool down)
{
	if (m_currentIndex != 0)
	{
		if (down)
		{
			if (m_currentIndex == mp_buttons_v.size())
				m_firstButtonIndex = MAX(1, MIN((int) m_texts_v.size() - (int) mp_buttons_v.size() + 1, (int) m_firstButtonIndex + 1));
			else
				m_currentIndex = MIN(m_texts_v.size(), m_currentIndex + 1);
		}
		else
		{
			if (m_currentIndex == 1)
				m_firstButtonIndex = MAX(1, m_firstButtonIndex - 1);
			else
				m_currentIndex--;
		}
	}
	setPoint(PfPoint(0.0, 0.0));
	select();

	m_modified = true;
}

void PfList::addText(const string& text)
{
    if (m_texts_v.size() == 255)
        throw PfException(__LINE__, __FILE__, "Impossible d'ajouter un texte à la liste qui en contient déjà 255.");
	m_texts_v.push_back(text);
}

void PfList::addTexts(const vector<string>& texts_v)
{
    if (m_texts_v.size() + texts_v.size() >= 256)
        throw PfException(__LINE__, __FILE__, "Impossible d'ajouter ces textes à la liste qui en contiendrait alors plus de 255.");
	for (unsigned int i=0, size=texts_v.size();i<size;i++)
		m_texts_v.push_back(texts_v[i]);
}

const string& PfList::currentText() const
{
	if (m_currentIndex == 0)
		throw PfException(__LINE__, __FILE__, "L'indice actuel est 0.");
	return m_texts_v[m_firstButtonIndex+m_currentIndex-2]; // -2 car les deux variables sont basées sur 1 en premier indice
}

unsigned int PfList::currentTextIndex() const
{
	if (m_currentIndex == 0)
		return 0;
	return m_firstButtonIndex + m_currentIndex - 1; // -1 car les deux variables sont basées sur 1 en premier indice
}

void PfList::removeText(unsigned int index)
{
    if (index == 0 || index > m_texts_v.size())
        throw ArgumentException(__LINE__, __FILE__, string("Indice non valide : nombre de textes = ")+itostr(m_texts_v.size())+" ; indice fourni = "+itostr(index) + "en base 1.",
                                "index", "PfList::removeText");

    m_texts_v.erase(m_texts_v.begin() + index - 1);
}

void PfList::clear()
{
    m_texts_v.clear();
}

void PfList::highlight()
{
	PfWidget::highlight();

	mp_upArrow->leave();
	mp_downArrow->leave();
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (m_currentIndex != i+1)
			mp_buttons_v[i]->leave();
	}

	if (getPoint() != PfPoint(0.0, 0.0) && mp_upArrow->getRect().contains(getPoint()))
		mp_upArrow->highlight();
	else if (getPoint() != PfPoint(0.0, 0.0) && mp_downArrow->getRect().contains(getPoint()))
		mp_downArrow->highlight();
	else
	{
		bool done = false;
		for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
		{
			if (getPoint() != PfPoint(0.0, 0.0) && mp_buttons_v[i]->getRect().contains(getPoint()))
			{
				mp_buttons_v[i]->highlight();
				done = true;
				break;
			}
		}
		if (!done)
		{
			if (m_currentIndex > 0)
				mp_buttons_v[m_currentIndex-1]->highlight();
			else if (mp_buttons_v.size() > 0)
				mp_buttons_v[0]->highlight();
		}
	}
}

void PfList::leave()
{
	PfWidget::leave();

	mp_upArrow->leave();
	mp_downArrow->leave();
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
		mp_buttons_v[i]->leave();
}

void PfList::select()
{
    PfWidget::select();

    mp_upArrow->leave();
	mp_downArrow->leave();
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (m_currentIndex != i+1)
			mp_buttons_v[i]->leave();
	}

	if (getPoint() != PfPoint(0.0, 0.0) && mp_upArrow->getRect().contains(getPoint()))
		mp_upArrow->select();
	else if (getPoint() != PfPoint(0.0, 0.0) && mp_downArrow->getRect().contains(getPoint()))
		mp_downArrow->select();
	else
	{
		bool done = false;
		for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
		{
			if (getPoint() != PfPoint(0.0, 0.0) && mp_buttons_v[i]->getRect().contains(getPoint()))
			{
				mp_buttons_v[i]->select();
				m_currentIndex = i + 1;
				done = true;
			}
		}
		if (!done)
		{
			if (m_currentIndex > 0)
				mp_buttons_v[m_currentIndex-1]->select();
			else if (mp_buttons_v.size() > 0)
			{
				mp_buttons_v[0]->select();
				m_currentIndex = 1;
			}
		}
	}

	m_modified = true;
}

void PfList::deselect()
{
    PfWidget::deselect();

	mp_upArrow->deselect();
	mp_downArrow->deselect();
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (mp_buttons_v[i] != 0)
			mp_buttons_v[i]->deselect();
	}

	m_modified = true;
}

void PfList::activate()
{
	PfWidget::activate();

	resetEffects();
	if (getPoint() != PfPoint(0.0, 0.0) && mp_upArrow->getRect().contains(getPoint()))
		m_firstButtonIndex = MAX(1, m_firstButtonIndex-1);
	else if (getPoint() != PfPoint(0.0, 0.0) && mp_downArrow->getRect().contains(getPoint()))
		m_firstButtonIndex = MAX(1, MIN((int) m_texts_v.size() - (int) mp_buttons_v.size() + 1, (int) m_firstButtonIndex+1));
	else
	{
		if (m_currentIndex != 0)
			addEffects(EFFECT_SELECT | m_currentIndex);
	}

	m_modified = true;
}

void PfList::addAnimation(PfAnimation* p_anim, PfAnimationStatus value)
{
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (mp_buttons_v[i] != 0)
		{
			PfAnimation* p_ = new PfAnimation(*p_anim);
			mp_buttons_v[i]->addAnimation(p_, value);
		}
	}

	PfRectangle rect;
	if (value == ANIM_IDLE)
	{
		rect = p_anim->currentFrame().textCoordRectangle();
		mp_upArrow->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(0.0, 1.0-rect.getH(), rect.getW()/2, rect.getH())), ANIM_IDLE);
		mp_downArrow->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(0.0, 1.0-rect.getH(), rect.getW()/2, rect.getH())), ANIM_IDLE);
	}
	if (value == ANIM_HIGHLIGHTED)
	{
		rect = p_anim->currentFrame().textCoordRectangle();
		mp_upArrow->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(rect.getW()/2, 1.0-rect.getH(), rect.getW()/2, rect.getH())), ANIM_HIGHLIGHTED);
		mp_downArrow->addAnimation(new PfAnimation(p_anim->currentFrame().getTextureIndex(), PfRectangle(rect.getW()/2, 1.0-rect.getH(), rect.getW()/2, rect.getH())), ANIM_HIGHLIGHTED);
	}

	delete p_anim;
}

void PfList::update()
{
	PfWidget::update();

	mp_upArrow->update();
	mp_downArrow->update();

	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (mp_buttons_v[i] != 0)
			mp_buttons_v[i]->update();
	}
}

Viewable* PfList::generateViewable() const
{
	mp_upArrow->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_upArrow->setStatic(isStatic());
	mp_downArrow->setCoordRelativeToBorder(isCoordRelativeToBorder());
	mp_downArrow->setStatic(isStatic());

	Viewable* p_rtn = new Viewable(getName(), PfRectangle(0.0, 0.0, 0.0, 0.0), getLayer(), 0, PfColor::WHITE);
	Viewable* p_tmp;
	if (m_firstButtonIndex > 1)
	{
		p_tmp = mp_upArrow->generateViewable();
		p_tmp->changeAngle(180);
		p_rtn->addImages(*p_tmp);
		delete p_tmp;
	}
	if ((int) m_firstButtonIndex < (int) m_texts_v.size() - (int) mp_buttons_v.size() + 1)
	{
		p_tmp = mp_downArrow->generateViewable();
		p_rtn->addImages(*p_tmp);
		delete p_tmp;
	}
	for (unsigned int i=0, size=mp_buttons_v.size();i<size;i++)
	{
		if (mp_buttons_v[i] != 0)
		{
			if (m_texts_v.size() >= m_firstButtonIndex + i)
			{
				mp_buttons_v[i]->changeText(m_texts_v[m_firstButtonIndex + i - 1]);
				mp_buttons_v[i]->setCoordRelativeToBorder(isCoordRelativeToBorder());
				mp_buttons_v[i]->setStatic(isStatic());
				p_tmp = mp_buttons_v[i]->generateViewable();
				p_rtn->addImages(*p_tmp);
				delete p_tmp;
			}
		}
	}

	return p_rtn;
}

PfReturnCode PfList::processInstruction()
{
	switch (getInstruction())
	{
		case INSTRUCTION_UP:
			scroll(false);
			return RETURN_OK;
		case INSTRUCTION_DOWN:
			scroll(true);
			return RETURN_OK;
		case INSTRUCTION_ACTIVATE:
			activate();
			return RETURN_OK;
		default:
			break;
	}

	return RETURN_NOTHING;
}
