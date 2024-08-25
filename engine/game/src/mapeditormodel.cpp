#include "mapeditormodel.h"

#include <set>
#include "misc.h"
#include "datapackage.h"
#include "errors.h"
#include "pfgui.h"
#include "mapbackground.h"
#include "grass.h"
#include "fence.h"

#define EDITOR_GUI_WAD_NAME "PF_editor_gui" //!< Le nom du wad GUI de l'éditeur.
#define LAYOUT_ROWS 9
#define LAYOUT_COLUMNS 4
#define TEXTURE_GRID_ROW 9
#define TEXTURE_GRID_COL 2 // attention, laisser de la place à gauche pour l'ascenseur
#define RELIEF_TEXTURE_BUTTON_ROW 8
#define RELIEF_TEXTURE_BUTTON_COL 1
#define WAD_GRID_ROW 2
#define WAD_GRID_COL 3 // laisser de la place à droite pour l'ascenseur
#define CURSOR_ROW 7 // <------------------------| +1
#define CURSOR_COL 1 //                          |
#define RADIUS_ROW 5 // <----------------| +1 (et idem)
#define RADIUS_COL 1 //                  |
#define MIN_HEIGHT_ROW 3 // <--| +1 (et idem)
#define MIN_HEIGHT_COL 1 //    |
#define SLOPE_ROW 1 // laisser de la place au-dessus pour le label
#define SLOPE_COL 1

MapEditorModel::MapEditorModel(unsigned int rows, unsigned int columns, PfWad* p_wad, const string& texName) :
    m_pinRow(0), m_pinCol(0), m_widgetEffects(EFFECT_NONE), m_textureMode(false), m_currentTerrainIndex(0), mp_wad(p_wad), m_currentWadSlot(WAD_NULL_OBJECT),
	m_topoMode(GRAPHICS_FLAT), m_selOr(PfOrientation::NO_ORIENTATION), m_ANIM_SELECTEDObjIndex(0), m_currentScriptEntry(1)
{
	try
	{
		addPhase("Edit");
		addPhase("Save");
		addPhase("Text");
		addPhase("List");
		addPhase("Properties");
		addPhase("Map links list");
		addPhase("Object properties");
		addPhase("Script");
		switchPhase("Edit");

		mp_map = new Map(rows, columns, texName);
		addItem(mp_map);
		if (p_wad->hasSlot(WAD_BACKGROUND))
        {
            MapBackground* p_object = dynamic_cast<MapBackground*>(mp_wad->generateGLItem(WAD_BACKGROUND, PfRectangle(), 0, rows, columns, 0.0, 0.0, GAME_MAP));
            addItem(p_object);
        }

		createGUI();
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de créer l'environnement de l'éditeur.", "MapEditorModel", e);
	}
}

MapEditorModel::MapEditorModel(const string& fileName):
    m_pinRow(0), m_pinCol(0), m_widgetEffects(EFFECT_NONE), m_textureMode(false), m_currentTerrainIndex(0),
	m_currentMapName(fileName.substr(0, fileName.find_first_of("."))), mp_wad(0), m_currentWadSlot(WAD_NULL_OBJECT), m_topoMode(GRAPHICS_FLAT),
	m_selOr(PfOrientation::NO_ORIENTATION), m_ANIM_SELECTEDObjIndex(0), m_currentScriptEntry(1)
{
	try
	{
		addPhase("Edit");
		addPhase("Save");
		addPhase("Text");
		addPhase("List");
		addPhase("Properties");
		addPhase("Map links list");
		addPhase("Object properties");
		addPhase("Script");
		switchPhase("Edit");

		string str(string(MAPS_DIR) + fileName);
		ifstream ifs(str.c_str(), ios::binary);
		if (!ifs.is_open())
			throw ArgumentException(__LINE__, __FILE__, string("Impossible d'ouvrir le fichier ") + fileName + ".", "fileName", "MapEditorModel::MapEditorModel");
		int tmp;
		ifs.read((char*) &tmp, sizeof(int));
		if (tmp != PFGAME_VERSION)
		{
			ifs.close();
			throw PfException(__LINE__, __FILE__, string("Le fichier ") + fileName + " n'est pas de la version la plus récente.\n\tVersion actuelle : " +
                     itostr(PFGAME_VERSION) + "\n\tVersion du fichier : " + itostr(tmp));
		}

        ifs.read((char*) &tmp, sizeof(int)); // passer STRING_FLAG
		str = readString(ifs);
		if (str != "")
			mp_wad = new PfWad(str, "launcher_load_menu", "PF_launcher_menu");
		else
			throw PfException(__LINE__, __FILE__, string("Le fichier ") + fileName + " fait référence à un WAD inexistant.");

		DataPackage dp(ifs); // rencontre un DT_END après la map.
		mp_map = new Map(dp);
		dp = DataPackage(ifs); // finit le fichier.
		unsigned int objectsCount = dp.nextUInt();
		for (unsigned int i=0;i<objectsCount;i++)
		{
		    dp.nextInt(); // passer NEW_SLOT_FLAG
			PfWadSlot slot = (PfWadSlot) dp.nextInt();
			MapObject* p_object = dynamic_cast<MapObject*>(mp_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
			if (p_object == 0)
				throw PfException(__LINE__, __FILE__, string("Impossible de créer l'objet au slot ") + textFrom(slot) + ".");
			p_object->loadData(dp);
			addItem(p_object);
			mp_map->addObject(*p_object);

			#ifdef DBG_ADDOBJECTS
			LOG("Added: " << textFrom(slot) << " at layer " << itostr(p_object->getLayer()) << "\n");
			#endif
		}
		if (dp.nextChar() != 0)
        {
            MapBackground* p_background = dynamic_cast<MapBackground*>(mp_wad->generateGLItem(WAD_BACKGROUND, PfRectangle(), 0,
                                                                                              mp_map->getRowsCount(), mp_map->getColumnsCount(), 0.0, 0.0,
                                                                                              GAME_MAP));
            addItem(p_background);
        }
		ifs.close();

		addItem(mp_map);

		createGUI();
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de créer l'environnement de l'éditeur.", "MapEditorModel", e);
	}
}

MapEditorModel::~MapEditorModel()
{
	if (mp_wad != 0)
		delete mp_wad;
}

void MapEditorModel::applyEffects()
{
    PfLayout* p_layout;
	if (getCurrentPhaseName() == "Edit")
	{
	    p_layout = findItem<PfLayout>("layout");
        if (p_layout == 0)
            throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");
	    /*
        * Dans le cas d'un défilement de widget, on détermine quelle action mener.
        * 0 : ascenseur de la grille de textures
        * 1 : ascenseur de la grille de wads
        * 2 : curseur de topologie
        */
        int slidingWidget = 2;
        if (p_layout->getCurrentRow() == WAD_GRID_ROW  && p_layout->getCurrentCol() == WAD_GRID_COL+1)
            slidingWidget = 1;
        else if (p_layout->getCurrentRow() == TEXTURE_GRID_ROW && p_layout->getCurrentCol() == TEXTURE_GRID_COL-1)
            slidingWidget = 0;

        // De même pour la grille et la sélection de textures ou d'objets
        bool wadGridClick = false;
        if (p_layout->getCurrentRow() == WAD_GRID_ROW && p_layout->getCurrentCol() == WAD_GRID_COL)
            wadGridClick = true;

		if (m_widgetEffects & EFFECT_NEXT)
		{
			if (slidingWidget < 2)
				scrollGrid(true, (slidingWidget==1), m_widgetEffects & 255);
			else
				changeTopoMode();
		}
		if (m_widgetEffects & EFFECT_PREV)
		{
			if (slidingWidget < 2)
				scrollGrid(false, (slidingWidget==1), m_widgetEffects & 255);
			else
				changeTopoMode();
		}
		if (m_widgetEffects & EFFECT_SWITCH)
			changeMode();
		if (m_widgetEffects & EFFECT_SELECT)
		{
			if (wadGridClick)
			{
				m_currentWadSlot = WAD_NULL_OBJECT;
				if ((m_widgetEffects & 255) < m_wadSlotsInGrid_v.size())
					m_currentWadSlot = m_wadSlotsInGrid_v[m_widgetEffects & 255];
			}
			else
				m_currentTerrainIndex = m_widgetEffects & 255;
		}
	}
	else if (getCurrentPhaseName() == "Save")
    {
        if (m_widgetEffects & EFFECT_SWITCH)
		{
            if ((m_widgetEffects & 1) == 0)
            {
                if (m_currentMapName != "")
                {
                    saveMap();
                    switchPhase("Edit");
                }
                else
                    switchPhase("List");
            }
            else
                switchPhase("List");
		}
    }
    else if (getCurrentPhaseName() == "Properties")
    {
        if (m_widgetEffects & (EFFECT_PREV | EFFECT_NEXT))
        {
            p_layout = findItem<PfLayout>("properties layout");
            if (p_layout == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet properties layout.");
            PfSlidingCursor* p_cursor = dynamic_cast<PfSlidingCursor*>(p_layout->widget("GUI_SLIDINGCURSOR_2"));
            if (p_cursor == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet GUI_SLIDINGCURSOR_2");

            mp_map->setGroundType((Map::MapGroundType) p_cursor->getCurrentStep());
        }
        else if (m_widgetEffects & EFFECT_SWITCH)
        {
            switch (m_widgetEffects & 255)
            {
            case 0: // liens de map
                switchPhase("Map links list");
                break;
            case 1: // éditeur de script
                switchPhase("Script");
                break;
            default:
                break;
            }
        }
        else if (m_widgetEffects & EFFECT_DELETE)
        {
            PfLayout* p_layout = findItem<PfLayout>("properties layout");
            if (p_layout == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet properties layout.");
            PfList* p_list = dynamic_cast<PfList*>(p_layout->widget("GUI_LIST2_1"));
            if (p_list == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet GUI_LIST2_1 dans properties layout.");
            if (p_list->currentTextIndex() > 0)
            {
                p_list->clear();
                mp_map->removeMapLink(p_list->currentTextIndex());
                for (unsigned int i=0, size=mp_map->mapLinksCount();i<size;i++)
                    p_list->addText(itostr(i+1) + " - " + mp_map->mapLink(i+1));
                p_list->setModified(true);
            }
        }
        else if (m_widgetEffects & EFFECT_QUIT)
            switchPhase("Edit");
    }
    else if (getCurrentPhaseName() == "Object properties")
    {
        if (m_widgetEffects & EFFECT_QUIT)
        {
            p_layout = findItem<PfLayout>("object properties layout");
            if (p_layout == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet object properties layout.");
            PfSpinBox* p_spinBox = dynamic_cast<PfSpinBox*>(p_layout->widget("GUI_SPINBOX_4"));
            if (p_spinBox == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet GUI_SPINBOX_4");

            MapObject* p_obj = findItem<MapObject>(m_ANIM_SELECTEDObj);
            if (p_obj == 0)
                throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + m_ANIM_SELECTEDObj + ".");
            p_obj->setObjCode((int) p_spinBox->getValue());

            switchPhase("Edit");
        }
    }
    else if (getCurrentPhaseName() == "Script")
    {
        if (m_widgetEffects & (EFFECT_QUIT | EFFECT_PREV | EFFECT_NEXT))
        {
            p_layout = findItem<PfLayout>("script layout");
            if (p_layout == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet script layout.");
            PfTextBox* p_textBox = dynamic_cast<PfTextBox*>(p_layout->widget(2, 2));
            if (p_textBox == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de récupérer la boîte de texte dans script layout.");

            if (m_widgetEffects & (EFFECT_PREV | EFFECT_NEXT))
            {
                PfWidget* p_label = findItem<PfWidget>("GUI_LABEL2_7");
                if (p_label == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet GUI_LABEL2_7.");

                if ((m_widgetEffects & EFFECT_PREV) && m_currentScriptEntry > 1)
                {
                    m_currentScriptEntry--;
                    p_textBox->changeText(mp_map->scriptEntry(m_currentScriptEntry-1));
                }
                else if ((m_widgetEffects & EFFECT_NEXT) && m_currentScriptEntry <= mp_map->scriptEntriesCount())
                {
                    m_currentScriptEntry++;
                    if (m_currentScriptEntry <= mp_map->scriptEntriesCount())
                        p_textBox->changeText(mp_map->scriptEntry(m_currentScriptEntry-1));
                    else
                        p_textBox->changeText("");
                }
                p_label->changeText(translate("Entry") + translate(":") + " " + itostr(m_currentScriptEntry));
            }
            else if (m_widgetEffects & EFFECT_QUIT)
            {
                if (p_textBox->getText() != "")
                {
                    if (m_currentScriptEntry > mp_map->scriptEntriesCount())
                        mp_map->addScriptEntry(p_textBox->getText());
                    else
                        mp_map->editScriptEntry(p_textBox->getText(), m_currentScriptEntry-1);
                }
                switchPhase("Edit");
            }
        }
    }

	resetEffects();
}

void MapEditorModel::displayMouseObjectIcon()
{
    PolygonGLItem* p_gl = findItem<PolygonGLItem>("Cursor_wad_icon");
    if (p_gl == 0)
    {
        p_gl = new PolygonGLItem("Cursor_wad_icon", MAX_LAYER);
        p_gl->setStatic(true);
        addItem(p_gl);
    }
    if (m_currentWadSlot != WAD_NULL_OBJECT)
    {
        showItem("Cursor_wad_icon");
        pair<unsigned int, PfRectangle> icon = mp_wad->icon(m_currentWadSlot);
        p_gl->setPolygon(m_mouseCursorRect);
        p_gl->setTextureIndex(icon.first);
        p_gl->setTextCoordPolygon(icon.second);
        p_gl->setModified(true);
    }
    else
        hideItem("Cursor_wad_icon");
}

PfReturnCode MapEditorModel::processInstruction()
{
	PfLayout* p_layout;
	string phaseName = getCurrentPhaseName();
	PfInstruction instruction = getInstruction();
	PfPoint point(0.0, 0.0);
	bool bAlt, bCtrl, bShift, bSlide, bLeftSlide, bRightSlide, bBorders;
	bool ok;
	int value;
	PfReturnCode rtnCode = RETURN_OK;

	point.setX(instructionValues().nextFloat());
	point.setY(instructionValues().nextFloat());
	bAlt = (instructionValues().nextChar() == 1); // ALT
	bCtrl = (instructionValues().nextChar() == 1); // CTRL
	bShift = (instructionValues().nextChar() == 1); // SHIFT
	bSlide = (instructionValues().nextChar() == 1); // glissement de souris
	bLeftSlide = (instructionValues().nextChar() == 1); // glissement souris gauche
	bRightSlide = (instructionValues().nextChar() == 1); // glissement souris droite
	bBorders = (instructionValues().nextChar() == 1); // dans les bordures

	if (phaseName == "Edit")
	{
		try
		{
			switch (instruction)
			{
				case INSTRUCTION_LEFT:
					camera()->move(-MAP_CELL_SIZE, 0.0);
					break;
				case INSTRUCTION_UP:
					camera()->move(0.0, MAP_CELL_SIZE);
					break;
				case INSTRUCTION_RIGHT:
					camera()->move(MAP_CELL_SIZE, 0.0);
					break;
				case INSTRUCTION_DOWN:
					camera()->move(0.0, -MAP_CELL_SIZE);
					break;
				case INSTRUCTION_SWITCH:
				    switch (instructionValues().nextInt())
				    {
                    case 0:
                        switchPhase("Save");
                        break;
                    case 1:
                        switchPhase("Properties");
                        break;
                    case 2:
                        if (m_ANIM_SELECTEDObj != "")
                            switchPhase("Object properties");
                        break;
                    default:
                        break;
				    }
					break;
				case INSTRUCTION_SELECT:
					if (bBorders) // widgets de bordure
					{
						if (changeSelection(point, "layout", bSlide) != RETURN_EMPTY)
						{
							if (bLeftSlide) // glissement souris gauche
								slideSelection("layout");
							else if (!bSlide) // clic
								readInstruction(INSTRUCTION_ACTIVATE);
						}
					}
					else // map
					{
                        m_mouseCursorRect.setX(point.getX()+0.05);
                        m_mouseCursorRect.setY(point.getY()-0.03);
						if (bSlide) // glissement de souris
						{
							changeSelection(PfPoint(0.0, 0.0), "layout");
							if (bRightSlide) // souris droite
							{
								float f1 = instructionValues().nextFloat();
								float f2 = instructionValues().nextFloat();
								f1 /= 1-2*SYSTEM_BORDER_WIDTH;
								camera()->move(f1, f2);
							}
							if (bLeftSlide) // souris gauche
							{
                                if (m_currentWadSlot != WAD_NULL_OBJECT)
                                {
                                    ok = true;
                                    for (unsigned int i=0, size=m_objOnCells_v.size();i<size;i++)
                                    {
                                        pair<unsigned int, unsigned int> tmp_pair = mapCoordAt(point);
                                        if (tmp_pair.first == m_objOnCells_v[i].first && tmp_pair.second == m_objOnCells_v[i].second)
                                        {
                                            ok = false;
                                            break;
                                        }
                                    }
                                    if (ok)
                                    {
                                        addObject(m_currentWadSlot, point);
                                        m_objOnCells_v.push_back(mapCoordAt(point));
                                    }
                                }
								else if (m_textureMode)
								{
									unselectAll();
									selectCell(mapCoordAt(point));
									changeSelCellTerrain(m_currentTerrainIndex);
								}
								else
								{
									squareSelect(mapCoordAt(point));
									updateCursors();
								}
							}
						}
						else // clic
						{
						    if (bShift)
                                selectObject(point);
							else if (bAlt) // ALT enfoncée
							{
								if (m_selOr == PfOrientation::NO_ORIENTATION)
									m_selOr = PfOrientation::NORTH;
								else if (m_selOr == PfOrientation::WEST)
									m_selOr |= PfOrientation::NORTH;
								else if (m_selOr == (PfOrientation::SOUTH | PfOrientation::WEST))
									m_selOr = PfOrientation::NO_ORIENTATION;
								else
									m_selOr = PfOrientation((PfOrientation::PfCardinalPoint) ((int) PfOrientation(m_selOr).toCardinal()+2)).toFlag();
								updateCursors();
								unselectAllObjects();
							}
							else if (m_currentWadSlot != WAD_NULL_OBJECT)
                            {
								addObject(m_currentWadSlot, point);
								m_objOnCells_v.push_back(mapCoordAt(point));
								unselectAllObjects();
                            }
							else
							{
							    unselectAllObjects();
								if (!bCtrl) // CTRL non enfoncée
									unselectAll();
								if (m_textureMode)
								{
									updateCursors(); // pour supprimer les curseurs
									selectCell(mapCoordAt(point));
									changeSelCellTerrain(m_currentTerrainIndex);
								}
								else
								{
									selectCell(mapCoordAt(point));
									updateCursors();
								}
							}
						}
					}
					break;
				case INSTRUCTION_DELETE:
					if (instructionValues().nextInt() == 1)
                    {
						removeObjects();
						unselectAllObjects();
                    }
					else
						executeLayoutInstruction(instruction);
					break;
				case INSTRUCTION_MODIFY:
					if (instructionValues().nextUInt() == 0) // non modification de texte
					{
						if (m_textureMode)
							break;
						value = instructionValues().nextInt();
						if (ABS(value) == 1)
							changeSelCellsHeight(SGN(value)*MAP_CELL_SQUARE_HEIGHT/4, true);
						else if (ABS(value) == 2)
							changeTopology(SGN(value)*MAP_CELL_SQUARE_HEIGHT/4, topoRadius(), m_topoMode, true, SGN(value), minFloorValue(), topoSlope());
						else if (ABS(value) == 3)
							changeSelCellSlope(SGN(value)*MAP_CELL_SQUARE_HEIGHT/4);
						updateCursors();
						unselectAllObjects();
					}
					else
						executeLayoutInstruction(instruction, instructionValues().nextInt());
					break;
				case INSTRUCTION_VALIDATE:
				    if (m_currentWadSlot != WAD_NULL_OBJECT)
                    {
                        if (m_objOnCells_v.size() > 0)
                        {
                            m_currentWadSlot = WAD_NULL_OBJECT;
                            m_objOnCells_v.clear();
                        }
                    }
					else if (!m_textureMode)
					{
						validateBufferSelection();
						updateCursors();
					}
					break;
				default:
					rtnCode = executeLayoutInstruction(instruction);
					break;
			}
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase Edit.", e);
		}
	}
	else if (phaseName == "Save")
	{
		try
		{
			switch (instruction)
			{
				case INSTRUCTION_SELECT:
					if (!bBorders) // hors des bordures
					{
						if (changeSelection(point, "save layout") != RETURN_EMPTY)
						{
							if (!bSlide) // clic de souris
								readInstruction(INSTRUCTION_ACTIVATE);
						}
					}
					break;
				default:
					p_layout = findItem<PfLayout>("save layout");
					if (p_layout == 0)
						throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet save layout.");
					rtnCode = p_layout->readInstruction(instruction);
					if (instruction == INSTRUCTION_ACTIVATE)
						m_widgetEffects = p_layout->getEffectFlags();
					break;
			}

		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase Save.", e);
		}
	}
	else if (phaseName == "Text")
	{
		try
		{
			PfTextBox* p_textBox;

			switch (instruction)
			{
				case INSTRUCTION_VALIDATE:
					if (instructionValues().nextInt() != 0) // validation par relâchement de souris
						break;
					p_textBox = findItem<PfTextBox>("GUI_TEXTBOX_1");
					if (p_textBox == 0)
						throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_TEXTBOX_1.");
					m_currentMapName = p_textBox->getText();
					saveMap();
					switchPhase("Edit");
					break;
				case INSTRUCTION_SELECT:
					if (!bSlide) // clic de souris
						clickOnTextBox(point);
					break;
				default:
					p_textBox = findItem<PfTextBox>("GUI_TEXTBOX_1");
					if (p_textBox == 0)
						throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_TEXTBOX_1.");
					rtnCode = p_textBox->readInstruction(instruction, (unsigned char) instructionValues().nextInt());
					break;
			}
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase Text.", e);
		}
	}
	else if (phaseName == "List")
	{
		try
		{
			PfList* p_list;
			PfTextBox* p_textBox;

			switch (instruction)
			{
				case INSTRUCTION_SWITCH:
					p_list = findItem<PfList>("GUI_LIST1_1");
					if (p_list == 0)
						throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_LIST1_1.");
					if (p_list->currentTextIndex() == 1)
					{
						switchPhase("Text");
						p_textBox = findItem<PfTextBox>("GUI_TEXTBOX_1");
						if (p_textBox == 0)
							throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_TEXTBOX_1.");
						p_textBox->activate();
					}
					else
					{
						m_currentMapName = p_list->currentText();
						saveMap();
						switchPhase("Edit");
					}
					break;
				case INSTRUCTION_SELECT:
					if (selectInList("GUI_LIST1_1", point, !bSlide) && !bSlide) // bSlide : glissement de souris
						rtnCode = readInstruction(INSTRUCTION_SWITCH);
					break;
				default:
					rtnCode = RETURN_NOTHING;
					break;
			}
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase List.", e);
		}
	}
	else if (phaseName == "Properties")
    {
        switch (instruction)
        {
        case INSTRUCTION_SELECT:
            if (changeSelection(point, "properties layout", bSlide) != RETURN_EMPTY)
            {
                if (bLeftSlide) // glissement souris gauche
                    slideSelection("properties layout");
                else if (!bSlide) // clic
                    readInstruction(INSTRUCTION_ACTIVATE);
            }
            break;
        case INSTRUCTION_VALIDATE:
            if (instructionValues().nextInt() != 0) // validation par relâchement de souris
                break;
            readInstruction(INSTRUCTION_ACTIVATE);
        default:
            p_layout = findItem<PfLayout>("properties layout");
            if (p_layout == 0)
                throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet properties layout.");
            rtnCode = p_layout->readInstruction(instruction);
            if (instruction == INSTRUCTION_ACTIVATE)
                m_widgetEffects = p_layout->getEffectFlags();
            break;
        }
    }
    else if (phaseName == "Map links list")
    {
        try
		{
            PfList* p_list;
            PfLayout* p_layout;

            switch (instruction)
            {
            case INSTRUCTION_SWITCH:
                p_list = findItem<PfList>("GUI_LIST1_2");
                if (p_list == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_LIST1_2.");
                if (p_list->currentTextIndex() > 1)
                {
                    unsigned int mapCount = mp_map->mapLinksCount();
                    string mapName = p_list->currentText();
                    if (mp_map->addMapLink(mapName) == mapCount + 1)
                    {
                        p_layout = findItem<PfLayout>("properties layout");
                        if (p_layout == 0)
                            throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet properties layout.");
                        p_list = dynamic_cast<PfList*>(p_layout->widget("GUI_LIST2_1"));
                        if (p_list == 0)
                            throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_LIST2_1 dans properties layout.");
                        p_list->addText(itostr(mp_map->mapLinksCount()) + " - " + mapName);
                        p_list->setModified(true);
                    }
                }
                switchPhase("Properties");
                break;
            case INSTRUCTION_SELECT:
                if (selectInList("GUI_LIST1_2", point, !bSlide) && !bSlide) // bSlide : glissement de souris
                    rtnCode = readInstruction(INSTRUCTION_SWITCH);
                break;
            default:
                rtnCode = RETURN_NOTHING;
                break;
            }
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase Map links list.", e);
		}
    }
    else if (phaseName == "Object properties")
    {
        try
        {
            PfLayout* p_layout;
            PfSpinBox* p_spinBox;
            MapObject* p_obj;

            switch (instruction)
            {
            case INSTRUCTION_MODIFY:
                if (instructionValues().nextUInt() == 1) // modification de texte
                {
                    p_layout = findItem<PfLayout>("object properties layout");
                    if (p_layout == 0)
                        throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet object properties layout.");
                    rtnCode = p_layout->readInstruction(instruction, instructionValues().nextInt());
                }
                break;
            case INSTRUCTION_SELECT:
                if (changeSelection(point, "object properties layout", bSlide) != RETURN_EMPTY)
                {
                    if (bLeftSlide) // glissement souris gauche
                        slideSelection("object properties layout");
                    else if (!bSlide) // clic
                        readInstruction(INSTRUCTION_ACTIVATE);
                }
                break;
            case INSTRUCTION_VALIDATE:
                if (instructionValues().nextInt() != 0) // validation par relâchement de souris
                    break;
                p_layout = findItem<PfLayout>("object properties layout");
                if (p_layout == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet object properties layout.");
                p_spinBox = dynamic_cast<PfSpinBox*>(p_layout->widget("GUI_SPINBOX_4"));
                if (p_spinBox == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_SPINBOX_4.");
                p_obj = findItem<MapObject>(m_ANIM_SELECTEDObj);
                if (p_obj == 0)
                    throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + m_ANIM_SELECTEDObj + ".");
                p_obj->setObjCode((int) p_spinBox->getValue());
                p_obj->setModified(true);
                readInstruction(INSTRUCTION_ACTIVATE);
                break;
            default:
                p_layout = findItem<PfLayout>("object properties layout");
                if (p_layout == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet object properties layout.");
                rtnCode = p_layout->readInstruction(instruction);
                if (instruction == INSTRUCTION_ACTIVATE)
                    m_widgetEffects = p_layout->getEffectFlags();
                break;
            }
        }
        catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase Object properties.", e);
		}
    }
    else if (phaseName == "Script")
    {
        try
        {
            PfLayout* p_layout;

            switch (instruction)
            {
            case INSTRUCTION_SELECT:
                if (changeSelection(point, "script layout", bSlide) != RETURN_EMPTY)
                {
                    if (!bSlide)
                        readInstruction(INSTRUCTION_ACTIVATE);
                }
                break;
            case INSTRUCTION_VALIDATE:
                if (instructionValues().nextInt() != 0) // validation par relâchement de souris
                    break;
                p_layout = findItem<PfLayout>("script layout");
                if (p_layout == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet script layout.");
                rtnCode = p_layout->readInstruction(INSTRUCTION_MODIFY, '\n'); // touche ENTREE
                if (rtnCode != RETURN_OK)
                    readInstruction(INSTRUCTION_ACTIVATE);
            default:
                p_layout = findItem<PfLayout>("script layout");
                if (p_layout == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet script layout.");
                rtnCode = p_layout->readInstruction(instruction, instructionValues());
                if (instruction == INSTRUCTION_ACTIVATE)
                    m_widgetEffects = p_layout->getEffectFlags();
                break;
            }
        }
        catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Impossible de jouer la phase Script.", e);
		}
    }

	return rtnCode;
}

void MapEditorModel::processCurrentPhase()
{
	string phaseName = getCurrentPhaseName();
	PfLayout* p_layout;

	try
	{
		if (phaseName == "Save")
		{
			if (isNewPhase())
			{
				showItem("GUI_LABEL1_1");
				showItem("save layout");
			}
		}
		else if (phaseName == "Text")
		{
			if (isNewPhase())
				showItem("GUI_TEXTBOX_1");
		}
		else if (phaseName == "List")
		{
			if (isNewPhase())
				showItem("GUI_LIST1_1");
		}
		else if (phaseName == "Properties")
        {
            if (isNewPhase())
			{
				showItem("GUI_LABEL1_2");
				showItem("GUI_LABEL2_4");
				showItem("GUI_LABEL2_5");
				showItem("properties layout");
			}
        }
        else if (phaseName == "Map links list")
        {
            if (isNewPhase())
                showItem("GUI_LIST1_2");
        }
        else if (phaseName == "Object properties")
        {
            if (isNewPhase())
            {
                p_layout = findItem<PfLayout>("object properties layout");
                if (p_layout == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet object properties layout.");
                PfSpinBox* p_spinBox = dynamic_cast<PfSpinBox*>(p_layout->widget("GUI_SPINBOX_4"));
                if (p_spinBox == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_SPINBOX_4");
                MapObject* p_obj = findItem<MapObject>(m_ANIM_SELECTEDObj);
                if (p_obj == 0)
                    throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + m_ANIM_SELECTEDObj + ".");
                p_spinBox->changeValue((unsigned int) ABS(p_obj->getObjCode()));

                showItem("GUI_LABEL1_3");
                showItem("GUI_LABEL2_6");
                showItem("object properties layout");
            }
        }
        else if (phaseName == "Script")
        {
            if (isNewPhase())
            {
                showItem("GUI_LABEL1_4");
                showItem("GUI_LABEL2_7");
                p_layout = findItem<PfLayout>("script layout");
                if (p_layout == 0)
                    throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet script layout.");
                if (mp_map->scriptEntriesCount() > 0)
                {
                    PfTextBox* p_textBox = dynamic_cast<PfTextBox*>(p_layout->widget(2, 2));
                    if (p_textBox == 0)
                        throw PfException(__LINE__, __FILE__, "Impossible de récupérer la boîte de texte dans script layout.");
                    p_textBox->changeText(mp_map->scriptEntry(0));
                }
                showItem("script layout");
            }
        }
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible de préparer la phase ") + phaseName + ".", e);
	}
}

void MapEditorModel::exitCurrentPhase()
{
    if (getCurrentPhaseName() == "Save")
    {
        hideItem("GUI_LABEL1_1");
        hideItem("save layout");
    }
    else if (getCurrentPhaseName() == "Text")
    {
        hideItem("GUI_TEXTBOX_1");
        PfTextBox* p_textBox = findItem<PfTextBox>("GUI_TEXTBOX_1");
        if (p_textBox == 0)
            throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_TEXTBOX_1.");
        p_textBox->reset();
    }
    else if (getCurrentPhaseName() == "List")
    {
        hideItem("GUI_LIST1_1");
    }
    else if (getCurrentPhaseName() == "Properties")
    {
        hideItem("GUI_LABEL1_2");
        hideItem("GUI_LABEL2_4");
        hideItem("GUI_LABEL2_5");
        hideItem("properties layout");
    }
    else if (getCurrentPhaseName() == "Map links list")
    {
        hideItem("GUI_LIST1_2");
    }
    else if (getCurrentPhaseName() == "Object properties")
    {
        hideItem("GUI_LABEL1_3");
        hideItem("GUI_LABEL2_6");
        hideItem("object properties layout");
    }
    else if (getCurrentPhaseName() == "Script")
    {
        hideItem("GUI_LABEL1_4");
        hideItem("GUI_LABEL2_7");
        hideItem("script layout");
    }
}

PfReturnCode MapEditorModel::changeSelection(const PfPoint& rc_point, const string& name, bool justHighlight)
{
	PfReturnCode code = RETURN_EMPTY;

	try
	{
		PfLayout* p_layout = findItem<PfLayout>(name);
		if (p_layout == 0)
			throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + name + ".");
		code = p_layout->selectWidgetAt(rc_point, justHighlight);
		if (code == RETURN_EMPTY) // aucun widget sous la souris
			p_layout->deselect();
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Sélection impossible.", e);
	}

	return code;
}

pair<unsigned int, unsigned int> MapEditorModel::mapCoordAt(const PfPoint& rc_point) const
{
	pair<unsigned int, unsigned int> rtn_pair(0, 0);
	if (rc_point.getX() < 0.0 || rc_point.getX() > 1.0)
		return rtn_pair;

	float x, y;
	x = rc_point.getX() + m_camera.getX();
	y = rc_point.getY() + m_camera.getY();
	if (x > 0 && y > -MAP_CELL_SQUARE_HEIGHT)
	{
		unsigned int col = x/MAP_CELL_SIZE + 1;
		if (col <= mp_map->getColumnsCount())
		{
		    unsigned int row = 0;
			// on parcourt toutes les cases de la colonne pour tenir compte des effets de hauteur
			for (unsigned int i=0, rowsCount=mp_map->getRowsCount();i<rowsCount;i++)
			{
				const Cell* p_cell = mp_map->cell(i+1, col);
				if (y >= p_cell->poly_minY() && y < p_cell->poly_minY() + p_cell->poly_maxH())
				{
					if (row == 0 || mp_map->cell(row, col)->getZ() < p_cell->getZ())
						row = i + 1;
				}
			}
			if (row > 0)
			{
				rtn_pair.first = row;
				rtn_pair.second = col;
			}
		}
	}

	return rtn_pair;
}

void MapEditorModel::selectCell(unsigned int row, unsigned int col)
{
	if (row == 0 || row > mp_map->getRowsCount())
		return;
    if (col == 0 || col > mp_map->getColumnsCount())
		return;

	bool b = true;
	for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
	{
		if (m_selCoord_v[i].first == row && m_selCoord_v[i].second == col)
		{
			b = false;
			break;
		}
	}
	if (b)
		m_selCoord_v.push_back(pair<unsigned int, unsigned int>(row, col));

	m_pinRow = row;
	m_pinCol = col;
}

void MapEditorModel::selectCell(pair<unsigned int, unsigned int> coord_pair)
{
	selectCell(coord_pair.first, coord_pair.second);
}

void MapEditorModel::squareSelect(pair<unsigned int, unsigned int> coord_pair)
{
	unsigned int x1, y1, x2, y2;
	x1 = MIN(coord_pair.second, m_pinCol);
	y1 = MIN(coord_pair.first, m_pinRow);
	x2 = MAX(coord_pair.second, m_pinCol);
	y2 = MAX(coord_pair.first, m_pinRow);

	if (x1 == 0 || y1 == 0)
		return;

	for (unsigned int i=0, size=m_bufSelCoord_v.size();i<size;i++)
	{
		unsigned int row = m_bufSelCoord_v[i].first;
		unsigned int col = m_bufSelCoord_v[i].second;
		if (row < y1 || row > y2 || col < x1 || col > x2)
			m_dltSelCoord_v.push_back(m_bufSelCoord_v[i]);
	}
	m_bufSelCoord_v.clear();

	for (unsigned int row=y1;row<=y2;row++)
	{
		for (unsigned int col=x1;col<=x2;col++)
		{
			if (row != m_pinRow || col != m_pinCol)
				m_bufSelCoord_v.push_back(pair<unsigned int, unsigned int>(row, col));
		}
	}
}

void MapEditorModel::unselectAll()
{
	for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
		m_dltSelCoord_v.push_back(m_selCoord_v[i]);
	m_selCoord_v.clear();

	m_pinRow = m_pinCol = 0;
}

bool MapEditorModel::selectInList(const string& name, const PfPoint& rc_point, bool activate)
{
	bool b = false;

	PfList* p_list = findItem<PfList>(name);
	if (p_list == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + name + ".");

	if (p_list->getRect().contains(rc_point))
	{
		p_list->setPoint(rc_point);
		if (activate)
		{
			p_list->activate();
			if (p_list->getEffectFlags() != EFFECT_NONE)
				b = true;
		}
		else
			p_list->select();
	}
	else
		p_list->deselect();

	return b;
}

void MapEditorModel::clickOnTextBox(const PfPoint& rc_point)
{
	PfTextBox* p_box = findItem<PfTextBox>("GUI_TEXTBOX_1");
	if (p_box == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_TEXTBOX_1.");

	if (p_box->getRect().contains(rc_point))
		p_box->activate();
	else
		p_box->deselect();
}

void MapEditorModel::validateBufferSelection()
{
	for (unsigned int i=0, size=m_bufSelCoord_v.size();i<size;i++)
	{
		bool b = true;
		for (unsigned int j=0, size2=m_selCoord_v.size();j<size2;j++)
		{
			if (m_selCoord_v[j] == m_bufSelCoord_v[i])
			{
				b = false;
				break;
			}
		}
		if (b)
			m_selCoord_v.push_back(m_bufSelCoord_v[i]);
	}
	m_bufSelCoord_v.clear();
}

void MapEditorModel::updateCursors()
{
	PfPolygon arrowPolygon(3);
	string cursorName;
	vector<pair<unsigned int, unsigned int> > cursors_v;
	for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
		cursors_v.push_back(m_selCoord_v[i]);
	for (unsigned int i=0, size=m_bufSelCoord_v.size();i<size;i++)
		cursors_v.push_back(m_bufSelCoord_v[i]);
	for (unsigned int i=0, size=cursors_v.size();i<size;i++)
	{
		unsigned int row = cursors_v[i].first;
		unsigned int col = cursors_v[i].second;
		cursorName = string("MapCursor(") + itostr(row) + ";" + itostr(col) + ")";
		PolygonGLItem* p_cursor = findItem<PolygonGLItem>(cursorName);
		PfPolygon cursorRect = mp_map->cell(row, col)->reconstructedPolygon();
		cursorRect.shift(PfOrientation(PfOrientation::NORTH), 1./g_windowHeight); // pour éviter le recouvrement par la case du dessous
		if (p_cursor != 0)
			p_cursor->setPolygon(cursorRect);
		else
		{
			p_cursor = new PolygonGLItem(cursorName, mp_map->cell(row, col)->getLayer()+1, cursorRect, PfColor(PfColor::RED), true);
			addItem(p_cursor);
		}
		p_cursor->setModified(true);

		PolygonGLItem* p_arrow = findItem<PolygonGLItem>(cursorName + "_arrow");
		if (p_arrow != 0)
			removeItem(cursorName + "_arrow");
		if (m_selOr != PfOrientation::NO_ORIENTATION)
		{
			PfOrientation::PfCardinalPoint cardOri = PfOrientation(m_selOr).toCardinal();

			// orientation ouest
			arrowPolygon.replacePointAt(0, p_cursor->poly_minX()+p_cursor->poly_maxW()/3, p_cursor->poly_minY()+p_cursor->poly_maxH()/2);
			arrowPolygon.replacePointAt(1, p_cursor->poly_minX()+2*p_cursor->poly_maxW()/3, p_cursor->poly_minY()+p_cursor->poly_maxH()/4);
			arrowPolygon.replacePointAt(2, p_cursor->poly_minX()+2*p_cursor->poly_maxW()/3, p_cursor->poly_minY()+3*p_cursor->poly_maxH()/4);

			double angle = -45.0 * ((int) cardOri + 1);
			p_arrow = new PolygonGLItem(cursorName + "_arrow", mp_map->cell(row, col)->getLayer()+1, arrowPolygon, PfColor(PfColor::RED), true, angle);
			addItem(p_arrow);
		}
	}
	for (unsigned int i=0, size=m_dltSelCoord_v.size();i<size;i++)
	{
		cursorName = string("MapCursor(") + itostr(m_dltSelCoord_v[i].first) + ";" + itostr(m_dltSelCoord_v[i].second) + ")";
		if (findItem<GLItem>(cursorName) != 0)
			removeItem(cursorName);
		if (findItem<GLItem>(cursorName + "_arrow") != 0)
			removeItem(cursorName + "_arrow");
	}
	m_dltSelCoord_v.clear();
}

void MapEditorModel::changeTopology(int z, unsigned int radius, TextureGenerationMode mode, bool rel, int behavior, unsigned int minValue, unsigned int slope)
{
	int newZ;
	// < ligne < colonne , +/-modif > >
	map<unsigned int, map<unsigned int, int> > cellsModif_map;
	map<unsigned int, map<unsigned int, int> >::iterator rowIt;
	map<unsigned int, int>::iterator colIt;
	float** f_t2;
	unsigned int l = 1 + radius * 2;
	int row, col;
	for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
	{
		newZ = (rel?mp_map->cell(m_selCoord_v[i].first, m_selCoord_v[i].second)->getZ()+z:z);

		if (mode == GRAPHICS_NOISE)
			f_t2 = generate2DTexture(l, l, mode, (float) newZ, MAP_MAX_LINES_COUNT/(2*(slope+6)), mp_map->getSeed());
		else
			f_t2 = generate2DTexture(l, l, mode, (float) newZ, slope);

		for (unsigned int j=0;j<l;j++)
		{
			row = m_selCoord_v[i].first-radius+j;
			if (row < 1 || row > (int) mp_map->getRowsCount())
				continue;
			rowIt = cellsModif_map.find(row);
			for (unsigned int k=0;k<l;k++)
			{
				col = m_selCoord_v[i].second-radius+k;
				if (col < 1 || col > (int) mp_map->getColumnsCount())
					continue;
				if (rowIt != cellsModif_map.end())
					colIt = rowIt->second.find(col);
				else
					colIt = cellsModif_map[row].end();

				// calcul du poids (distance à la sélection, calculée par couronnes)
				if (colIt == cellsModif_map[row].end())
				{
					cellsModif_map[row][col] = MAX(minValue, f_t2[j][k]);
					rowIt = cellsModif_map.find(row);
					colIt = rowIt->second.find(col);
				}
				else
					colIt->second = MAX(colIt->second, f_t2[j][k]);
			}
		}

		for (unsigned int j=0;j<l;j++)
			delete [] f_t2[j];
		delete [] f_t2;
	}

	vector<pair<unsigned int, unsigned int> > tmp_v;
	bool ANIM_SELECTED;
	for (rowIt=cellsModif_map.begin();rowIt!=cellsModif_map.end();++rowIt)
	{
		for (colIt=rowIt->second.begin();colIt!=rowIt->second.end();++colIt)
		{
			ANIM_SELECTED = false;
			tmp_v.clear();
			tmp_v.push_back(pair<unsigned int, unsigned int>(rowIt->first, colIt->first));
			// Quoi qu'il arrive, les cases sélectionnées vont à newZ.
			for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
			{
				if (m_selCoord_v[i].first == rowIt->first && m_selCoord_v[i].second == colIt->first)
					ANIM_SELECTED = true;
			}
			changeCellsHeight(tmp_v, (ANIM_SELECTED?newZ:colIt->second), false, behavior);
		}
	}
}

void MapEditorModel::changeCellsHeight(const vector<pair<unsigned int, unsigned int> >& rc_cells_v, int z, bool rel, int behavior)
{
	const Cell* pc_cell;
	vector<string> objects_v;
	MapObject* p_object;
	for (unsigned int i=0, size=rc_cells_v.size();i<size;i++)
	{
		unsigned int row = rc_cells_v[i].first;
		unsigned int col = rc_cells_v[i].second;
		try
		{
			pc_cell = mp_map->cell(row, col);
			objects_v = mp_map->objectsOnCell(pair<unsigned int, unsigned int>(row, col));
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, "Coordonnées invalides pour l'une des cases sélectionnées.", e);
		}
		for (unsigned int j=0, size2=objects_v.size();j<size2;j++)
		{
			p_object = dynamic_cast<MapObject*>(findItem<MapObject>(objects_v[j]));
			if (p_object == 0)
				throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + p_object->getName() + ".");
			mp_map->removeObject(p_object->getName());
		}
		int newZ = rel?pc_cell->getZ()+z:z;
		if (behavior != 0)
			newZ = (behavior<0)?MIN(pc_cell->getZ(), newZ):MAX(pc_cell->getZ(), newZ);
		newZ = MIN(MAP_MAX_HEIGHT, MAX(0, newZ));
		mp_map->changeCell(row, col, pc_cell->getTerrainIndex(), newZ);
		for (unsigned int j=0, size2=objects_v.size();j<size2;j++)
		{
			p_object = dynamic_cast<MapObject*>(findItem<MapObject>(objects_v[j]));
			if (p_object == 0)
				throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + p_object->getName() + ".");
			mp_map->addObject(*p_object, row, col);
		}
	}
}

void MapEditorModel::changeSelCellsHeight(int z, bool rel)
{
	changeCellsHeight(m_selCoord_v, z, rel);
}

void MapEditorModel::changeSelCellTerrain(int index)
{
	for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
	{
		unsigned int row = m_selCoord_v[i].first;
		unsigned int col = m_selCoord_v[i].second;
		mp_map->changeCell(row, col, index);
	}
}

void MapEditorModel::changeSelCellSlope(int deltaSlope)
{
	if (m_selOr == PfOrientation::NO_ORIENTATION)
		changeSelCellsHeight(deltaSlope, true);
	else
	{
		for (unsigned int i=0, size=m_selCoord_v.size();i<size;i++)
		{
			unsigned int row = m_selCoord_v[i].first;
			unsigned int col = m_selCoord_v[i].second;
			mp_map->changeCellSlope(row, col, PfOrientation(m_selOr).toCardinal(), deltaSlope, true);
		}
	}
}

PfReturnCode MapEditorModel::executeLayoutInstruction(PfInstruction instruction, int value)
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");
	PfReturnCode rtnCode = p_layout->readInstruction(instruction, value);
	if (instruction == INSTRUCTION_ACTIVATE)
		m_widgetEffects = p_layout->getEffectFlags();

	return rtnCode;
}

void MapEditorModel::scrollGrid(bool forward, bool wadGrid, int steps)
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");

	PfSelectionGrid* p_grid = dynamic_cast<PfSelectionGrid*>(p_layout->widget((wadGrid?"GUI_SELECTIONGRID_2":"GUI_SELECTIONGRID_1")));
	int pos = MAX(0, (int) p_grid->getStep()+(forward?steps:-steps));
	p_grid->changeStep((unsigned int) pos);
}

void MapEditorModel::slideSelection(const string& layout)
{
	PfLayout* p_layout = findItem<PfLayout>(layout);
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + layout + ".");

	if (p_layout->slide())
		m_widgetEffects = p_layout->getEffectFlags();
}

void MapEditorModel::changeMode()
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");

	PfWidget* p_button = dynamic_cast<PfWidget*>(p_layout->widget("GUI_BUTTON1_1"));
	if (p_button == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_BUTTON1_1");

	m_textureMode = !m_textureMode;
	p_button->changeText(!m_textureMode?"Relief":"Texture");
}

void MapEditorModel::changeTopoMode()
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");
	PfSlidingCursor* p_cursor = dynamic_cast<PfSlidingCursor*>(p_layout->widget("GUI_SLIDINGCURSOR_1"));
	if (p_cursor == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_SLIDINGCURSOR_1");

	m_topoMode = (TextureGenerationMode) p_cursor->getCurrentStep();
}

unsigned int MapEditorModel::topoRadius()
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");

	PfSpinBox* p_box = dynamic_cast<PfSpinBox*>(p_layout->widget("GUI_SPINBOX_1"));
	if (p_box == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_SPINBOX_1");

	return p_box->getValue();
}

unsigned int MapEditorModel::minFloorValue()
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");

	PfSpinBox* p_box = dynamic_cast<PfSpinBox*>(p_layout->widget("GUI_SPINBOX_2"));
	if (p_box == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_SPINBOX_2");

	return p_box->getValue();
}

unsigned int MapEditorModel::topoSlope()
{
	PfLayout* p_layout = findItem<PfLayout>("layout");
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet layout.");

	PfSpinBox* p_box = dynamic_cast<PfSpinBox*>(p_layout->widget("GUI_SPINBOX_3"));
	if (p_box == 0)
		throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_SPINBOX_3");

	return p_box->getValue();
}

void MapEditorModel::selectObject(const PfPoint& rc_point)
{
    pair<unsigned int, unsigned int> coord_pair = mapCoordAt(rc_point);
    vector<string> objects_v = mp_map->objectsOnCell(coord_pair);

    if (objects_v.size() > 0)
    {
        if (m_ANIM_SELECTEDObjIndex >= objects_v.size())
            m_ANIM_SELECTEDObjIndex = 0;
        if (m_ANIM_SELECTEDObj == objects_v[m_ANIM_SELECTEDObjIndex])
            m_ANIM_SELECTEDObjIndex = (m_ANIM_SELECTEDObjIndex==objects_v.size()-1)?0:(m_ANIM_SELECTEDObjIndex+1);
        unselectAllObjects();
        m_ANIM_SELECTEDObj = objects_v[m_ANIM_SELECTEDObjIndex];
        MapObject* p_obj = findItem<MapObject>(objects_v[m_ANIM_SELECTEDObjIndex]);
        if (p_obj == 0)
            throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + objects_v[m_ANIM_SELECTEDObjIndex] + ".");
        p_obj->blink(10, PfColor::RED);
    }
}

void MapEditorModel::unselectAllObjects()
{
    if (m_ANIM_SELECTEDObj != "")
    {
        MapObject* p_obj = findItem<MapObject>(m_ANIM_SELECTEDObj);
        if (p_obj == 0)
            throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + m_ANIM_SELECTEDObj + ".");
        p_obj->blink(0);
        m_ANIM_SELECTEDObj = "";
    }
}

void MapEditorModel::addObject(PfWadSlot slot, const PfPoint& rc_point)
{
	try
	{
		pair<unsigned int, unsigned int> coord_pair = mapCoordAt(rc_point);
		if (coord_pair.first != 0 && coord_pair.second != 0)
		{
		    MapObject* p_object = dynamic_cast<MapObject*>(mp_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
			mp_map->addObject(*p_object, coord_pair.first, coord_pair.second);
			addItem(p_object);

			#ifdef DBG_ADDOBJECTS
			LOG("Added: " << textFrom(slot) << " at layer " << itostr(p_object->getLayer()) << "\n");
			#endif

			// traitements spécifiques à certains objets

			vector<string> objects_v;
			string str;
			switch (slot)
			{
            case WAD_GRASS:
                // suppression d'un autre objet WAD_GRASS sur la même case
                objects_v = mp_map->objectsOnCell(coord_pair);
                for (unsigned int i=0, size=objects_v.size();i<size;i++)
                {
                    if (objects_v[i] != p_object->getName() && objects_v[i].substr(0, objects_v[i].find_last_of("_")) == textFrom(WAD_GRASS))
                        mp_map->removeObject(objects_v[i]);
                }
                // Génération des bordures
                try
                {
                    placeGrass(coord_pair);
                }
                catch (PfException& e)
                {
                    throw PfException(__LINE__, __FILE__, "Erreur lors de la génération des bordures d'herbe.", e);
                }
                break;
            case WAD_FENCE1:
            case WAD_FENCE2:
                // suppression d'un autre objet FENCEx sur la même case
                objects_v = mp_map->objectsOnCell(coord_pair);
                for (unsigned int i=0, size=objects_v.size();i<size;i++)
                {
                    str = objects_v[i].substr(0, objects_v[i].find_last_of("_"));
                    if (objects_v[i] != p_object->getName() && (str == textFrom(WAD_FENCE1) || str == textFrom(WAD_FENCE2)))
                        mp_map->removeObject(objects_v[i]);
                }
                // Connexion des barrières
                try
                {
                    connectFences(coord_pair, slot);
                }
                catch (PfException& e)
                {
                    throw PfException(__LINE__, __FILE__, "Erreur lors de la connexion des barrières.", e);
                }
                break;
            default:
                break;
			}
		}
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible d'ajouter l'objet du slot ") + textFrom(slot) + ".", e);
	}
}

void MapEditorModel::removeObjects()
{
    if (m_ANIM_SELECTEDObj != "")
    {
        mp_map->removeObject(m_ANIM_SELECTEDObj);
        removeItem(m_ANIM_SELECTEDObj);
    }
}

void MapEditorModel::resetEffects()
{
	m_widgetEffects = EFFECT_NONE;
}

void MapEditorModel::saveMap()
{
	if (m_currentMapName != "")
	{
		vector<string> maps_v = filesInDir(MAPS_DIR, MAPS_EXT);

		string str(string(MAPS_DIR) + m_currentMapName + "." + MAPS_EXT);
		ofstream ofs(str.c_str(), ios::binary);
		if (!ofs.is_open())
			throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", str);
		int tmp = PFGAME_VERSION;
		ofs.write((char*) &tmp, sizeof(int));
		tmp = STRING_FLAG;
		ofs.write((char*) &tmp, sizeof(int));
		writeString(ofs, mp_wad->getName());
		mp_map->saveData(ofs);
		WRITE_END(ofs);
		// sauvegarde des objets
		vector<string> objectsOnCell_v;
		set<string> objects_set;
		for (unsigned int i=0, size=mp_map->getRowsCount();i<size;i++)
		{
			for (unsigned int j=0, size2=mp_map->getColumnsCount();j<size2;j++)
			{
				objectsOnCell_v = mp_map->objectsOnCell(pair<unsigned int, unsigned int>(i+1, j+1));
				for (unsigned int k=0, size3=objectsOnCell_v.size();k<size3;k++)
					objects_set.insert(objectsOnCell_v[k]);
			}
		}
		WRITE_UINT(ofs, objects_set.size());
		for (set<string>::iterator it=objects_set.begin();it!=objects_set.end();++it)
		{
			MapObject* p_object = findItem<MapObject>(*it);
			if (p_object == 0)
            {
                ofs.close();
				throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + *it + ".");
            }

			// on enregistre le slot de wad à partir du nom
			for (unsigned int i=0;i<ENUM_PF_WAD_SLOT_COUNT;i++)
			{
				if (textFrom((PfWadSlot) i) == it->substr(0, it->find_last_of("_")))
                {
                    WRITE_INT(ofs, NEW_SLOT_FLAG);
					WRITE_INT(ofs, i);
					break;
                }
			}
			// puis on enregistre les données de l'objet
			p_object->saveData(ofs);
		}
		// sauvegarde du Background
		MapBackground* p_background = findItem<MapBackground>(textFrom(WAD_BACKGROUND)+"_1");
		char c = (p_background==0)?0:1;
        WRITE_CHAR(ofs, c);
		WRITE_END(ofs);
		ofs.close();

		bool b = true;
		for (unsigned int i=0, size=maps_v.size();i<size;i++)
		{
			if (maps_v[i].substr(0, maps_v[i].find_first_of(".")) == m_currentMapName)
			{
				b = false;
				break;
			}
		}

		if (b)
		{
			PfList* p_list = findItem<PfList>("GUI_LIST1_1");
			if (p_list == 0)
				throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet WAD_GUI_LIST1_1.");
			p_list->addText(m_currentMapName);
		}
	}

}

void MapEditorModel::placeGrass(pair<unsigned int, unsigned int> cellCoord)
{
    vector<string> objects_v;

    const Grass* pc_thisGrass = 0;
    objects_v = mp_map->objectsOnCell(cellCoord);
    for (unsigned int i=0, size=objects_v.size();i<size;i++)
    {
        if (objects_v[i].substr(0, objects_v[i].find_last_of("_")) == textFrom(WAD_GRASS))
        {
            pc_thisGrass = findConstItem<Grass>(objects_v[i]);
            break;
        }
    }
    if (pc_thisGrass == 0)
        throw PfException(__LINE__, __FILE__, "Impossible de trouver un objet WAD_GRASS sur la case spécifiée en paramètres.");

    PfOrientation o;
    Grass* p_grass;
    unsigned int row, col;
    int code;

    vector<pfflag> ori_v = PfOrientation::cardinalPoints();
    for (unsigned int i=0, size=ori_v.size();i<size;i++)
    {
        o = PfOrientation(ori_v[i]);
        p_grass = 0;
        row = cellCoord.first+o.getY();
        col = cellCoord.second+o.getX();

        if (row == 0 || row > mp_map->getRowsCount() || col == 0 || col > mp_map->getColumnsCount())
            continue;

        if (o.isNSEW())
            code = (int) o.opposite();
        else
            code = PfOrientation(o.opposite()).toExactValue();

        // vérifier la présence d'un objet WAD_GRASS sur cette case
        objects_v = mp_map->objectsOnCell(pair<unsigned int, unsigned int>(row, col));
        for (unsigned int j=0, size2=objects_v.size();j<size2;j++)
        {
            if (objects_v[j].substr(0, objects_v[j].find_last_of("_")) == textFrom(WAD_GRASS))
            {
                p_grass = findItem<Grass>(objects_v[j]);
                break;
            }
        }

        if (p_grass == 0)
        {
            p_grass = dynamic_cast<Grass*>(mp_wad->generateGLItem(WAD_GRASS, PfRectangle(), 0, o.isEW()?1:0, 0, 0.0, 0.0, GAME_MAP));
            p_grass->setObjCode(code);
            if (o.isEW())
                p_grass->generateElements(*pc_thisGrass);
            addItem(p_grass);
            mp_map->addObject(*p_grass, row, col);
        }
        else if (p_grass->getObjCode() != 0) // on ne touche pas à une herbe pleine qui est très bien comme ça
        {
            p_grass->setObjCode(p_grass->getObjCode() | code);
            p_grass->setModified(true);
            if (o.isEW())
            {
                p_grass->generateElements(*pc_thisGrass);
                if (p_grass->getObjCode() & o.toFlag())
                {
                    // maintenant qu'on a harmonisé l'herbe voisine avec l'herbe de cette case, il faut encore aller voir la case suivante
                    // ceci jusqu'à atteindre une herbe pleine ou la fin de la rangée d'herbe
                    bool ok = false, flag = false;
                    if (ori_v[i] == PfOrientation::CARDINAL_E && col == mp_map->getColumnsCount())
                        ok = true;
                    if (ori_v[i] == PfOrientation::CARDINAL_W && col == 1)
                        ok = true;
                    while (!ok)
                    {
                        col += o.getX();
                        objects_v = mp_map->objectsOnCell(pair<unsigned int, unsigned int>(row, col));
                        for (unsigned int j=0, size2=objects_v.size();j<size2;j++)
                        {
                            if (objects_v[j].substr(0, objects_v[j].find_last_of("_")) == textFrom(WAD_GRASS))
                            {
                                p_grass = findItem<Grass>(objects_v[j]);
                                break;
                            }
                        }
                        if (p_grass == 0) // ne devrait jamais arriver (je crois) du moins tant qu'on ne modifie pas manuelle le code objet
                            ok = true;
                        else
                        {
                            if (p_grass->getObjCode() != 0 || !flag) // bordure ou première herbe pleine
                                p_grass->generateElements(*pc_thisGrass);
                            if (p_grass->getObjCode() != 0)
                            {
                                if (p_grass->getObjCode() & o.toFlag())
                                    flag = false; // on revient dans la situation de départ
                                else
                                    ok = true;
                            }
                            else if (!flag)
                                flag = true; // si une deuxième herbe pleine suit, on arrêtera
                            else
                                ok = true;
                        }
                    }
                }
            }
        }
    }
}

void MapEditorModel::connectFences(pair<unsigned int, unsigned int> cellCoord, PfWadSlot slot)
{
    vector<string> objects_v = mp_map->objectsOnCell(cellCoord);

    Fence *p_thisFence = 0, *p_fence, *p_f;
    for (unsigned int i=0, size=objects_v.size();i<size;i++)
    {
        if (objects_v[i].substr(0, objects_v[i].find_last_of("_")) == textFrom(slot))
        {
            p_thisFence = findItem<Fence>(objects_v[i]);
            break;
        }
    }
    if (p_thisFence == 0)
        throw PfException(__LINE__, __FILE__, "Impossible de trouver un objet Fence sur la case spécifiée.");

    unsigned int row, col;
    bool picket = true;
    PfOrientation ori[4] = {PfOrientation::SOUTH, PfOrientation::WEST, PfOrientation::NORTH, PfOrientation::EAST};
    for (int i=0;i<4;i++)
    {
        p_fence = 0;
        row = cellCoord.first+ori[i].getY();
        col = cellCoord.second+ori[i].getX();
        if (row == 0 || col == 0 || row > mp_map->getRowsCount() || col > mp_map->getColumnsCount())
            continue;
        objects_v = mp_map->objectsOnCell(pair<unsigned int, unsigned int>(row, col));
        for (unsigned int j=0, size=objects_v.size();j<size;j++)
        {
            if (objects_v[j].substr(0, objects_v[j].find_last_of("_")) == textFrom(slot))
            {
                p_fence = findItem<Fence>(objects_v[j]);
                break;
            }
        }

        if (p_fence != 0)
        {
            switch (ori[i].toValue())
            {
            case PfOrientation::SOUTH:
                if (p_fence->getObjCode() == 1)
                {
                    p_f = dynamic_cast<Fence*>(mp_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
                    mp_map->addObject(*p_f, row, col);
                    addItem(p_f);
                }
                else
                    p_f = p_fence;
                p_f->setObjCode(2);
                break;
            case PfOrientation::WEST:
                if (p_fence->getObjCode() == 2)
                {
                    p_f = dynamic_cast<Fence*>(mp_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
                    mp_map->addObject(*p_f, row, col);
                    addItem(p_f);
                }
                else
                    p_f = p_fence;
                p_f->setObjCode(1);
                break;
            case PfOrientation::NORTH:
                if (p_thisFence->getObjCode() == 1)
                {
                    p_f = dynamic_cast<Fence*>(mp_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
                    mp_map->addObject(*p_f, cellCoord.first, cellCoord.second);
                    addItem(p_f);
                }
                else
                    p_f = p_thisFence;
                p_f->setObjCode(2);
                picket = false;
                break;
            case PfOrientation::EAST:
                if (p_thisFence->getObjCode() == 2)
                {
                    p_f = dynamic_cast<Fence*>(mp_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
                    mp_map->addObject(*p_f, cellCoord.first, cellCoord.second);
                    addItem(p_f);
                }
                else
                    p_f = p_thisFence;
                p_f->setObjCode(1);
                picket = false;
                break;
            default:
                break;
            }
        }
    }

    if (picket)
    {
        // on en profite pour faire un nettoyage de toute barrière éventuellement superposée
        objects_v = mp_map->objectsOnCell(cellCoord);
        for (unsigned int i=0, size=objects_v.size();i<size;i++)
        {
            if (objects_v[i].substr(0, objects_v[i].find_last_of("_")) == textFrom(slot))
            {
                if (objects_v[i] != p_thisFence->getName())
                    mp_map->removeObject(objects_v[i]);
            }
        }
        // puis on transforme en piquet
        p_thisFence->setObjCode(0);
    }
}

void MapEditorModel::createGUI()
{
	try
	{
	    m_mouseCursorRect.setW(0.05);
	    m_mouseCursorRect.setH(0.05);

		// génération des objets GUI pour l'éditeur

		mp_wad->addWad(EDITOR_GUI_WAD_NAME);

		AnimatedGLItem* p_glItem;

		// bordures
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BORDER, PfRectangle(0, 0, SYSTEM_BORDER_WIDTH, 1), MAX_LAYER-1);
		p_glItem->setCoordRelativeToBorder(false);
		addItem(p_glItem);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BORDER, PfRectangle(1-SYSTEM_BORDER_WIDTH, 0, SYSTEM_BORDER_WIDTH, 1), MAX_LAYER-1);
		p_glItem->setCoordRelativeToBorder(false);
		addItem(p_glItem);

		PfLayout* p_layout = new PfLayout("layout", LAYOUT_ROWS, LAYOUT_COLUMNS);

		int rows, columns;
		columns = 4;
		float cellSize = SYSTEM_BORDER_WIDTH/(columns+1);
		rows = 0.5 / cellSize * Y_X_RATIO;

		// grille de sélection : terrain
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SELECTIONGRID,
                                    PfRectangle(cellSize/2, 1.0 - (rows+0.3)*cellSize/Y_X_RATIO, cellSize*columns, cellSize/Y_X_RATIO*rows),
                                    MAX_LAYER, rows, columns);
		p_glItem->setCoordRelativeToBorder(false);
		for (int i=0;i<TERRAIN_CELLS_COUNT;i++)
		{
			for (int j=0;j<TERRAIN_CELLS_COUNT;j++)
			{
				(dynamic_cast<PfSelectionGrid*>(p_glItem))->addCellImage(TERRAIN_TEXTURE_INDEX,
                                                PfRectangle((float) j/TERRAIN_CELLS_COUNT, 1.0 - (float) (i+1)/TERRAIN_CELLS_COUNT,
                                                            1./TERRAIN_CELLS_COUNT, 1./TERRAIN_CELLS_COUNT));
			}
		}
		p_layout->addWidget(dynamic_cast<PfSelectionGrid*>(p_glItem), TEXTURE_GRID_ROW, TEXTURE_GRID_COL);

		// ascenseur : terrain
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SCROLLBAR, PfRectangle(0, 1.0 - (rows+0.3)*cellSize/Y_X_RATIO, cellSize/2, rows*cellSize/Y_X_RATIO),
                                    MAX_LAYER, TERRAIN_CELLS_COUNT*TERRAIN_CELLS_COUNT/columns-rows+1, 0, cellSize/2, cellSize/2/Y_X_RATIO);
		p_glItem->setCoordRelativeToBorder(false);
		p_layout->addWidget(dynamic_cast<PfScrollBar*>(p_glItem), TEXTURE_GRID_ROW, TEXTURE_GRID_COL-1);

		// bouton relief/texture
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON1, PfRectangle(0.0, 0.38, SYSTEM_BORDER_WIDTH, 0.11), MAX_LAYER, 0, 0, 0.12, 0.4);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText("Relief");
		p_glItem->setCoordRelativeToBorder(false);
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_SWITCH);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), RELIEF_TEXTURE_BUTTON_ROW, RELIEF_TEXTURE_BUTTON_COL);

		// grille de sélection : objets
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SELECTIONGRID,
                                    PfRectangle(1.0-SYSTEM_BORDER_WIDTH+cellSize/2, 1.0 - (rows+0.3)*cellSize/Y_X_RATIO,
                                                cellSize*columns, cellSize/Y_X_RATIO*rows), MAX_LAYER, rows, columns);
		p_glItem->setCoordRelativeToBorder(false);
		set<PfWadSlot> excludedSlots_set;
		excludedSlots_set.insert(WAD_NULL_OBJECT);
		for (int slot=(int) ENUM_PF_WAD_SLOT_MAP_END+1;slot<ENUM_PF_WAD_SLOT_COUNT;slot++)
			excludedSlots_set.insert((PfWadSlot) slot);
		m_wadSlotsInGrid_v = mp_wad->slots(excludedSlots_set);
		vector<pair<unsigned int, PfRectangle> > icons_v = mp_wad->icons(excludedSlots_set);
		for (unsigned int i=0, size=icons_v.size();i<size;i++)
			(dynamic_cast<PfSelectionGrid*>(p_glItem))->addCellImage(icons_v[i].first, icons_v[i].second);
		p_layout->addWidget(dynamic_cast<PfSelectionGrid*>(p_glItem), WAD_GRID_ROW, WAD_GRID_COL);

		// ascenseur : objets
		int linesCount = MIN(256, MAX(1, (int) (icons_v.size()/columns)+((icons_v.size()%columns==0)?0:1)-rows+1));
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SCROLLBAR,
                                    PfRectangle(1.0-cellSize/2, 1.0 - (rows+0.3)*cellSize/Y_X_RATIO, cellSize/2, rows*cellSize/Y_X_RATIO),
                                    MAX_LAYER, linesCount, 0, cellSize/2, cellSize/2/Y_X_RATIO);
		p_glItem->setCoordRelativeToBorder(false);
		p_layout->addWidget(dynamic_cast<PfScrollBar*>(p_glItem), WAD_GRID_ROW, WAD_GRID_COL+1);

		// curseur de topologie
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SLIDINGCURSOR, PfRectangle(0.1*SYSTEM_BORDER_WIDTH, 0.22, 0.8*SYSTEM_BORDER_WIDTH, 0.15), MAX_LAYER, 5, 1);
		p_glItem->setCoordRelativeToBorder(false);
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("FLAT");
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("PYRAMID");
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("DOME");
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("RANDOM");
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("NOISE");
		p_layout->addWidget(dynamic_cast<PfSlidingCursor*>(p_glItem), CURSOR_ROW, CURSOR_COL);

		// label de rayon
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.0, 0.19, SYSTEM_BORDER_WIDTH, 0.02), MAX_LAYER);
		p_glItem->setCoordRelativeToBorder(false);
		(dynamic_cast<PfWidget*>(p_glItem))->addText("Radius");
		(dynamic_cast<PfWidget*>(p_glItem))->changeTextAlignProp(PfText::TEXT_ALIGN_LEFT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), RADIUS_ROW+1, RADIUS_COL);

		// spinbox de rayon
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SPINBOX,
                                    PfRectangle(0.2*SYSTEM_BORDER_WIDTH, 0.15, 0.6*SYSTEM_BORDER_WIDTH, 0.03), MAX_LAYER, 1, MAP_MAX_LINES_COUNT/2);
		p_glItem->setCoordRelativeToBorder(false);
		p_layout->addWidget(dynamic_cast<PfSpinBox*>(p_glItem), RADIUS_ROW, RADIUS_COL);

		// label de hauteur minimale pour topologie
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.0, 0.12, SYSTEM_BORDER_WIDTH, 0.02), MAX_LAYER);
		p_glItem->setCoordRelativeToBorder(false);
		(dynamic_cast<PfWidget*>(p_glItem))->addText("Floor min");
		(dynamic_cast<PfWidget*>(p_glItem))->changeTextAlignProp(PfText::TEXT_ALIGN_LEFT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), MIN_HEIGHT_ROW+1, MIN_HEIGHT_COL);

		// spinbox de hauteur minimale
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SPINBOX, PfRectangle(0.2*SYSTEM_BORDER_WIDTH, 0.08, 0.6*SYSTEM_BORDER_WIDTH, 0.03), MAX_LAYER, 1, 9);
		p_glItem->setCoordRelativeToBorder(false);
		(dynamic_cast<PfSpinBox*>(p_glItem))->changeValue(MAP_CELL_SQUARE_HEIGHT);
		p_layout->addWidget(dynamic_cast<PfSpinBox*>(p_glItem), MIN_HEIGHT_ROW, MIN_HEIGHT_COL);

		// label de pente
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.0, 0.05, SYSTEM_BORDER_WIDTH, 0.02), MAX_LAYER);
		p_glItem->setCoordRelativeToBorder(false);
		(dynamic_cast<PfWidget*>(p_glItem))->addText("Slope");
		(dynamic_cast<PfWidget*>(p_glItem))->changeTextAlignProp(PfText::TEXT_ALIGN_LEFT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), SLOPE_ROW+1, SLOPE_COL);

		// spinbox de pente
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SPINBOX, PfRectangle(0.2*SYSTEM_BORDER_WIDTH, 0.01, 0.6*SYSTEM_BORDER_WIDTH, 0.03), MAX_LAYER, 1, 9);
		p_glItem->setCoordRelativeToBorder(false);
		p_layout->addWidget(dynamic_cast<PfSpinBox*>(p_glItem), SLOPE_ROW, SLOPE_COL);

		addItem(p_layout);

		p_layout = new PfLayout("save layout", 1, 2);

		// label de sauvegarde et ses boutons
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL1, PfRectangle(0.1, 0.3, 0.8, 0.4), MAX_LAYER-1, 7, 0, 0.1, 0.25);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Save"));
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON2, PfRectangle(0.15, 0.4, 0.34, 0.1), MAX_LAYER, 0, 0, 0.02, 0.4);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText(translate("Overwrite"));
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_SWITCH | 0);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 1, 1);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON2, PfRectangle(0.51, 0.4, 0.34, 0.1), MAX_LAYER, 0, 0, 0.02, 0.4);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText(translate("Save as"));
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_SWITCH | 1);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 1, 2);

		// liste de sauvegarde
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LIST1, PfRectangle(0.1, 0.1, 0.8, 0), MAX_LAYER, 5, 0, 0.1);
		p_glItem->setStatic(true);
		(dynamic_cast<PfList*>(p_glItem))->addText(translate("New map"));
		vector<string> maps_v = filesInDir(MAPS_DIR, MAPS_EXT);
		for (unsigned int i=0, size=maps_v.size();i<size;i++)
			(dynamic_cast<PfList*>(p_glItem))->addText(maps_v[i].substr(0, maps_v[i].find_first_of(".")));
		addItem(p_glItem);
		hideItem(p_glItem->getName());

		addItem(p_layout);
		hideItem(p_layout->getName());

		// zone de texte de sauvegarde
		p_glItem = mp_wad->generateGLItem(WAD_GUI_TEXTBOX, PfRectangle(0.1, 0.48, 0.8, 0.04), MAX_LAYER, 0, 0, 0.05, 0.1);
		p_glItem->setStatic(true);
		(dynamic_cast<PfTextBox*>(p_glItem))->setFileNameChar(true);

		addItem(p_glItem);
		hideItem(p_glItem->getName());

		// label des propriétés de la map
        p_layout = new PfLayout("properties layout", 3, 2);

        p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL1, PfRectangle(0.2, 0.2, 0.6, 0.6), MAX_LAYER-1, 11, 0, 0.1, 0.1);
        p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Map Properties"));
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.21, 0.6, 0.3, 0.02), MAX_LAYER);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Ground type"));
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SLIDINGCURSOR, PfRectangle(0.28, 0.51, 0.25, 0.07), MAX_LAYER, 2, 0);
		p_glItem->setStatic(true);
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("FLOOR");
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->addText("WATER");
		(dynamic_cast<PfSlidingCursor*>(p_glItem))->changeStep((unsigned int) mp_map->getGroundType());
		p_layout->addWidget(dynamic_cast<PfSlidingCursor*>(p_glItem), 3, 1);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.21, 0.48, 0.3, 0.02), MAX_LAYER);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Map links"));
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LIST2, PfRectangle(0.25, 0.22, 0.3, 0.15), MAX_LAYER, 3, 0, 0.05);
		p_glItem->setStatic(true);
		for (unsigned int i=0, size=mp_map->mapLinksCount();i<size;i++)
            dynamic_cast<PfList*>(p_glItem)->addText(itostr(i+1) + " - " + mp_map->mapLink(i+1));
		p_layout->addWidget(dynamic_cast<PfList*>(p_glItem), 2, 1);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON1, PfRectangle(0.55, 0.4, 0.15, 0.08), MAX_LAYER, 0, 0, 0.08, 0.4);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText(translate("Add"));
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 3, 2);
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_SWITCH | 0);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON1, PfRectangle(0.55, 0.3, 0.15, 0.08), MAX_LAYER, 0, 0, 0.08, 0.4);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText(translate("Remove"));
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_DELETE);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 2, 2);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON1, PfRectangle(0.68, 0.23, 0.08, 0.04), MAX_LAYER, 0, 0, 0.08, 0.3);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText("OK");
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_QUIT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 1, 2);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON2, PfRectangle(0.5, 0.23, 0.1, 0.04), MAX_LAYER, 0, 0, 0.08, 0.35);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText(translate("Script"));
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_SWITCH | 1);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 1, 1);
		p_layout->selectWidget(1, 2);

		addItem(p_layout);
		hideItem(p_layout->getName());

		// liste de maps pour liens
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LIST1, PfRectangle(0.1, 0.1, 0.8, 0), MAX_LAYER, 5, 0, 0.1);
		p_glItem->setStatic(true);
		(dynamic_cast<PfList*>(p_glItem))->addText(translate("Back to map properties"));
		for (unsigned int i=0, size=maps_v.size();i<size;i++) // maps_v n'a pas changé
			(dynamic_cast<PfList*>(p_glItem))->addText(maps_v[i].substr(0, maps_v[i].find_first_of(".")));
		addItem(p_glItem);
		hideItem(p_glItem->getName());

		// layout des propriétés d'objets
		p_layout = new PfLayout("object properties layout", 2, 1);

		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL1, PfRectangle(0.2, 0.2, 0.6, 0.6), MAX_LAYER-1, 11, 0, 0.1, 0.1);
        p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Object Properties"));
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.22, 0.55, 0.25, 0.03), MAX_LAYER);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText("Code : ");
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_SPINBOX, PfRectangle(0.45, 0.55, 0.15, 0.03), MAX_LAYER, 0, 999);
		p_glItem->setStatic(true);
		p_layout->addWidget(dynamic_cast<PfSpinBox*>(p_glItem), 1, 1);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON1, PfRectangle(0.68, 0.23, 0.08, 0.04), MAX_LAYER, 0, 0, 0.08, 0.3);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText("OK");
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_QUIT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 2, 1);
		p_layout->selectWidget(2, 1);

		addItem(p_layout);
		hideItem(p_layout->getName());

		// layout de l'édition de script
		p_layout = new PfLayout("script layout", 2, 3);

		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL1, PfRectangle(0.2, 0.2, 0.6, 0.6), MAX_LAYER-1, 13, 0, 0.1, 0.05);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Script Editor"));
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_LABEL2, PfRectangle(0.25, 0.65, 0.5, 0.02), MAX_LAYER);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->addText(translate("Entry") + translate(":") + " 1");
		addItem(p_glItem);
		hideItem(p_glItem->getName());
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON1, PfRectangle(0.68, 0.23, 0.08, 0.04), MAX_LAYER, 0, 0, 0.08, 0.3);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText("OK");
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_QUIT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 1, 1);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON2, PfRectangle(0.72, 0.48, 0.05, 0.04), MAX_LAYER);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText(">");
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_NEXT);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 2, 3);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_BUTTON2, PfRectangle(0.23, 0.48, 0.05, 0.04), MAX_LAYER);
		p_glItem->setStatic(true);
		(dynamic_cast<PfWidget*>(p_glItem))->changeText("<");
		(dynamic_cast<PfWidget*>(p_glItem))->addEffects(EFFECT_PREV);
		p_layout->addWidget(dynamic_cast<PfWidget*>(p_glItem), 2, 1);
		p_glItem = mp_wad->generateGLItem(WAD_GUI_TEXTBOX, PfRectangle(0.3, 0.35, 0.4, 0.3), MAX_LAYER, 0, 0, 0.05, 0.05);
		p_glItem->setStatic(true);
		(dynamic_cast<PfTextBox*>(p_glItem))->setMultiLine(true);
		(dynamic_cast<PfTextBox*>(p_glItem))->setLinesCount(10);
		p_layout->addWidget(dynamic_cast<PfTextBox*>(p_glItem), 2, 2);
		p_layout->selectWidget(1, 1);

		addItem(p_layout);
		hideItem(p_layout->getName());
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Impossible de créer l'ensemble des composants de l'éditeur.", e);
	}
}
