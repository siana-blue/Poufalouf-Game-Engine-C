#include "menucontroller.h"

#include "menumodel.h"
#include "enum.h"
#include "glfunc.h"
#include "misc.h"

MenuController::MenuController(MenuModel& r_model) : GLController(r_model), m_effects(EFFECT_NONE)
{
	m_values.addInt(0);
}

void MenuController::update()
{
	MenuModel* p_model = dynamic_cast<MenuModel*>(mp_model);

	// Touche directionnelle

	int keys_t[4] = {SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT};
	PfInstruction ins_t[4] = {INSTRUCTION_UP, INSTRUCTION_RIGHT, INSTRUCTION_DOWN, INSTRUCTION_LEFT};
	for (unsigned int i=0;i<4;i++)
	{
		if (m_eventHandler.isKeyPressed(keys_t[i], true))
			p_model->readInstruction(ins_t[i]);
	}

	// Touche ENTREE

	if (m_eventHandler.isKeyPressed(SDLK_RETURN, true))
		p_model->readInstruction(INSTRUCTION_ACTIVATE);

	// Touche BACKSPACE

	if (m_eventHandler.isKeyPressed(SDLK_BACKSPACE, true))
		p_model->readInstruction(INSTRUCTION_DELETE);

	// Touche de caractère : écriture de texte

	vector<unsigned char> inputText_v = m_eventHandler.getInputText();
	DataPackage data;
    for (unsigned int i=0, size=inputText_v.size();i<size;i++)
    {
        data.clear();
        data.addUInt(1); // modification de texte
        data.addInt((int) inputText_v[i]);
        p_model->readInstruction(INSTRUCTION_MODIFY, data);
    }

	// Glissement de souris

	int relX = m_eventHandler.getMouseRelX();
	int relY = m_eventHandler.getMouseRelY();
	if (relX != 0 || relY != 0)
	{
		if (g_showCursor)
			p_model->changeSelection(glCoordFromSDLPoint(m_eventHandler.getMouseCoord(), true), true);
	}

	// Clic souris gauche

	if (m_eventHandler.isLeftMouseButtonClicked(true))
	{
		if (g_showCursor)
		{
			if (p_model->changeSelection(glCoordFromSDLPoint(m_eventHandler.getMouseCoord())) != RETURN_EMPTY)
				p_model->readInstruction(INSTRUCTION_ACTIVATE);
		}
	}

	GLController::update();

	if (m_effects != EFFECT_NONE)
	{
		if (g_transitionFrame == TRANSITION_FRAMES_COUNT)
		{
			if (m_effects & EFFECT_NEXT)
				p_model->changeCurrentLayout(m_effects & 255);
			else if (m_effects & EFFECT_QUIT)
				m_status = DEAD;
			else if (m_effects & (EFFECT_NEW | EFFECT_SELECT))
			{
				//stopAllSounds();
				m_status = ASLEEP;
			}

			p_model->resetLayout();
			m_effects = EFFECT_NONE;
		}
	}
	else
	{
		pfflag32 effects = p_model->getWidgetEffects();
		if (effects & EFFECT_SWITCH)
		{
		    m_values.clear();
		    for (unsigned int i=0, size=p_model->values().size();i<size;i++)
                m_values.addInt(p_model->values()[i]);
			p_model->resetEffects();
			m_status = ASLEEP;
			effects ^= EFFECT_SWITCH;
		}
		if (effects & EFFECT_SLEEP)
		{
			p_model->readInstruction(INSTRUCTION_STOP);
			effects ^= EFFECT_SLEEP;
		}
		if (effects != EFFECT_NONE)
		{
		    m_values.clear();
			for (unsigned int i=0, size=p_model->values().size();i<size;i++)
                m_values.addInt(p_model->values()[i]);
			p_model->resetEffects();
			m_effects = effects;
			g_transitionFrame = 1;
		}
	}
}

bool MenuController::isAvailable() const
{
	return (m_effects == EFFECT_NONE);
}

