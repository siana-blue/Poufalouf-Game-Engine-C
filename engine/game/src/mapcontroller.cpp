#include "mapcontroller.h"

#include "mapmodel.h"
#include "eventhandler.h"

MapController::MapController(MapModel& r_model) : GLController(r_model) {}

void MapController::update()
{
	MapModel* p_model = dynamic_cast<MapModel*>(mp_model);

    p_model->readInstruction(INSTRUCTION_CHECK);
    pfflag32 stat = p_model->endStatus();
    if (stat != RETURN_NOTHING && g_transitionFrame == 0)
        g_transitionFrame = 1;
    if (g_transitionFrame == TRANSITION_FRAMES_COUNT)
    {
        if (stat & RETURN_OK)
            m_values.addString(p_model->mapLink(stat & 255) + "." + MAPS_EXT);
        m_status = DEAD;
    }

    if (g_transitionFrame == 0)
    {
        p_model->applyEffects();
        p_model->moveObjects(); // avant mise à jour de la caméra dans GLController::update.
        // et avant gestion des événements pour qu'une vitesse modifiée soit d'abord prise par l'update des objets
        p_model->processInteractions();

        // Gestion des événements

        int keys_t[4] = {SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT};
        PfInstruction ins_t[4] = {INSTRUCTION_UP, INSTRUCTION_RIGHT, INSTRUCTION_DOWN, INSTRUCTION_LEFT};
        for (unsigned int i=0;i<4;i++)
        {
            if (m_eventHandler.isKeyPressed(keys_t[i], true))
                p_model->readInstruction(arrowKeyInstruction(ins_t[i], true));
            if (m_eventHandler.isKeyReleased(keys_t[i], true))
                p_model->readInstruction(arrowKeyInstruction(ins_t[i], false));
        }

        if (m_eventHandler.isKeyPressed(SDLK_LALT, true))
            p_model->readInstruction(INSTRUCTION_JUMP);
        if (m_eventHandler.isKeyPressed(SDLK_LCTRL, true))
            p_model->readInstruction(INSTRUCTION_ACTIVATE, 1);
        if (m_eventHandler.isKeyReleased(SDLK_LCTRL, true))
            p_model->readInstruction(INSTRUCTION_ACTIVATE, 0);

        #ifndef NDEBUG
        g_debug = m_eventHandler.isKeyPressed(SDLK_F1);
        #endif
    }

	// Actions

	GLController::update();

	p_model->showDebugInfo();
}

PfInstruction MapController::arrowKeyInstruction(PfInstruction arrowKey, bool down)
{
	PfInstruction rtn = INSTRUCTION_STOP;

	if (down)
	{
		bool alreadyIn = false; // on peut penser que cela ne sert à rien, mais au cas où un jour il y ait répétition de touches...
		for (unsigned int i=0, size=m_arrowKeys_v.size();i<size;i++)
		{
			if (m_arrowKeys_v[i] == arrowKey)
            {
                alreadyIn = true;
                break;
            }
		}
		if (!alreadyIn)
			m_arrowKeys_v.push_back(arrowKey);
		rtn = arrowKey;
	}
	else
	{
		for (vector<PfInstruction>::iterator it=m_arrowKeys_v.begin();it!=m_arrowKeys_v.end();++it)
		{
			if (*it == arrowKey)
			{
				m_arrowKeys_v.erase(it);
				break;
			}
		}
		if (m_arrowKeys_v.size() > 0)
			rtn = m_arrowKeys_v[0];
	}

	return rtn;
}

