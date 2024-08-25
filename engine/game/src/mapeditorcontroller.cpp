#include "mapeditorcontroller.h"

#include <string>
#include "mapeditormodel.h"
#include "eventhandler.h"
#include "datapackage.h"
#include "glfunc.h"
#include "geometry.h"
#include "misc.h"

MapEditorController::MapEditorController(MapEditorModel& r_model) : GLController(r_model) {}

void MapEditorController::update()
{
	MapEditorModel* p_model = dynamic_cast<MapEditorModel*>(mp_model);
	int insV; // valeur associée aux instructions
	DataPackage data;
	PfPoint point;

	// Touches directionnelles

	int keys_t[4] = {SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT};
	PfInstruction ins_t[4] = {INSTRUCTION_UP, INSTRUCTION_RIGHT, INSTRUCTION_DOWN, INSTRUCTION_LEFT};
	for (unsigned int i=0;i<4;i++)
	{
		if (m_eventHandler.isKeyPressed(keys_t[i], true))
			p_model->readInstruction(ins_t[i]);
	}

	bool altDown = (m_eventHandler.isKeyPressed(SDLK_LALT) || m_eventHandler.isKeyPressed(SDLK_RALT));
	bool ctrlDown = (m_eventHandler.isKeyPressed(SDLK_LCTRL) || m_eventHandler.isKeyPressed(SDLK_RCTRL));
	bool shiftDown = (m_eventHandler.isKeyPressed(SDLK_LSHIFT) || m_eventHandler.isKeyPressed(SDLK_RSHIFT));

	// Modification de la hauteur du terrain

	insV = 1;
	if (ctrlDown)
		insV = 2;
	else if (altDown)
		insV = 3;
	if (m_eventHandler.isKeyPressed(SDLK_KP_PLUS, true) || m_eventHandler.isKeyPressed(SDLK_PAGEUP, true))
		p_model->readInstruction(INSTRUCTION_MODIFY, insV);
	if (m_eventHandler.isKeyPressed(SDLK_KP_MINUS, true) || m_eventHandler.isKeyPressed(SDLK_PAGEDOWN, true))
		p_model->readInstruction(INSTRUCTION_MODIFY, -insV);

	// Menu de sauvegarde

	if (m_eventHandler.isKeyPressed(SDLK_F4, true))
		p_model->readInstruction(INSTRUCTION_SWITCH, 0);

    // Menu d'édition des propriétés de la map

    if (m_eventHandler.isKeyPressed(SDLK_F9, true))
		p_model->readInstruction(INSTRUCTION_SWITCH, 1);

    // Menu d'édition des propriétés d'un objet

    if (m_eventHandler.isKeyPressed(SDLK_o, true))
        p_model->readInstruction(INSTRUCTION_SWITCH, 2);

	// Validation par ENTREE

	if (m_eventHandler.isKeyPressed(SDLK_RETURN, true))
		p_model->readInstruction(INSTRUCTION_VALIDATE);

	// Backspace

	if (m_eventHandler.isKeyPressed(SDLK_BACKSPACE, true))
		p_model->readInstruction(INSTRUCTION_DELETE, 0);

	// Suppr

	if (m_eventHandler.isKeyPressed(SDLK_DELETE, true))
		p_model->readInstruction(INSTRUCTION_DELETE, 1);

	// Touche de caractère : écriture de texte

	vector<unsigned char> inputText_v = m_eventHandler.getInputText();
    for (unsigned int i=0, size=inputText_v.size();i<size;i++)
    {
        data.clear();
        data.addUInt(1); // modification de texte
        data.addInt((int) inputText_v[i]);
        p_model->readInstruction(INSTRUCTION_MODIFY, data);
    }

	// Clic gauche

	if (m_eventHandler.isLeftMouseButtonClicked(true))
	{
		point = glCoordFromSDLPoint(m_eventHandler.getMouseCoord());
		data.clear();
		data.addChar(altDown?1:0);
		data.addChar(ctrlDown?1:0);
		data.addChar(shiftDown?1:0);
		data.addChar(0); // clic
		data.addChar(0); // glissement souris gauche
		data.addChar(0); // glissement souris droite

		if (point.getX() < 0 || point.getX() > 1)
		{
			point = glCoordFromSDLPoint(m_eventHandler.getMouseCoord(), false);
			data.addFloat(point.getX());
			data.addFloat(point.getY());
			data.addChar(1); // bordures
		}
		else
		{
			data.addFloat(point.getX());
			data.addFloat(point.getY());
			data.addChar(0); // bordures
		}

		p_model->readInstruction(INSTRUCTION_SELECT, data);
	}

	// Relâchement souris gauche

	if (m_eventHandler.isLeftMouseButtonReleased(true))
		p_model->readInstruction(INSTRUCTION_VALIDATE, 1);

	// Glissement de souris

	int relX = m_eventHandler.getMouseRelX();
	int relY = m_eventHandler.getMouseRelY();
	if (relX != 0 || relY != 0)
	{
		point = glCoordFromSDLPoint(m_eventHandler.getMouseCoord());
		data.clear();
		data.addChar(altDown?1:0);
		data.addChar(ctrlDown?1:0);
		data.addChar(shiftDown?1:0);
		data.addChar(1); // glissement de souris
		data.addChar(m_eventHandler.isLeftMouseButtonClicked());
		data.addChar(m_eventHandler.isRightMouseButtonClicked());

		if (point.getX() < 0 || point.getX() > 1)
		{
			point = glCoordFromSDLPoint(m_eventHandler.getMouseCoord(), false);
			data.addFloat(point.getX());
			data.addFloat(point.getY());
			data.addChar(1); // bordures
		}
		else
		{
			data.addFloat(point.getX());
			data.addFloat(point.getY());
			data.addFloat(-(float) m_eventHandler.getMouseRelX()/g_windowWidth);
			data.addFloat((float) m_eventHandler.getMouseRelY()/g_windowHeight); // y SDL vers le bas
			data.addChar(0); // bordures
		}

		p_model->readInstruction(INSTRUCTION_SELECT, data);
	}

	GLController::update();
	p_model->displayMouseObjectIcon();
	p_model->playCurrentPhase();
	p_model->applyEffects();
}

