#include "mapmodel.h"

#include "wad.h"
#include "glfunc.h"
#include "misc.h"
#include "mob.h"
#include "pfgui.h"
#include "mapbackground.h"
#include "errors.h"
#include "mapzone.h"

#define MAP_GUI_WAD_NAME "PF_map_gui" //!< Le nom du wad à utiliser pour l'interface utilisateur sur une map.

MapModel::MapModel(const string& fileName) : m_effects(EFFECT_NONE), m_userActivation(false)
{
	try
	{
		string str(string(MAPS_DIR) + fileName);
		ifstream ifs(str.c_str(), ios::binary);
		if (!ifs.is_open())
			throw ArgumentException(__LINE__, __FILE__, string("Impossible d'ouvrir le fichier ") + fileName + ".", "fileName", "MapModel::MapModel");
		int tmp;
		ifs.read((char*) &tmp, sizeof(int));
		if (tmp != PFGAME_VERSION)
		{
			ifs.close();
			throw FileException(__LINE__, __FILE__, string("Le fichier ") + fileName + " n'est pas de la version la plus récente.\n\tVersion actuelle : "
                       + itostr(PFGAME_VERSION) + "\n\tVersion du fichier : " + itostr(tmp), str);
		}

		PfWad* p_wad;
		ifs.read((char*) &tmp, sizeof(int)); // passer STRING_FLAG
		str = readString(ifs);
		if (str != "")
			p_wad = new PfWad(str, "launcher_load_menu", "PF_launcher_menu");
		else
			throw PfException(__LINE__, __FILE__, string("Le fichier ") + fileName + " fait référence à un wad inexistant.");

		DataPackage dp(ifs);
		mp_map = new Map(dp);
		dp = DataPackage(ifs);
		ifs.close();

		addItem(mp_map);

		MapObject* p_object;
		unsigned int objectsCount = dp.nextUInt();
		for (unsigned int i=0;i<objectsCount;i++)
		{
		    dp.nextInt(); // passer NEW_SLOT_FLAG
			PfWadSlot slot = (PfWadSlot) dp.nextInt();
			p_object = dynamic_cast<MapObject*>(p_wad->generateGLItem(slot, PfRectangle(), 0, 0, 0, 0.0, 0.0, GAME_MAP));
			if (p_object == 0)
				throw PfException(__LINE__, __FILE__, string("Impossible de créer l'objet au slot ") + textFrom(slot) + ".");
			p_object->loadData(dp);

			addItem(p_object);

			try
			{
				mp_map->addObject(*p_object);

				#ifdef DBG_ADDOBJECTS
                LOG("Added: " << textFrom(slot) << " at layer " << itostr(p_object->getLayer()) << "\n");
                #endif
			}
			catch (PfException& e)
			{
				throw PfException(__LINE__, __FILE__, string("Impossible d'ajouter l'objet ") + p_object->getName() + ".", e);
			}
		}
        if (dp.nextChar() != 0)
        {
            MapBackground* p_background = dynamic_cast<MapBackground*>(p_wad->generateGLItem(WAD_BACKGROUND, PfRectangle(), 0,
                                                                                              mp_map->getRowsCount(), mp_map->getColumnsCount(), 0.0, 0.0,
                                                                                              GAME_MAP));
            addItem(p_background);
        }

		m_controlledMobName = "MOB_1";
		p_object = findItem<Mob>("MOB_1");
		if (p_object == 0)
			throw PfException(__LINE__,  __FILE__, "Impossible de trouver l'objet MOB_1.");
		camera()->follow(*p_object);
		camera()->changeLimit(PfRectangle(0.0, 0.0,
                                    mp_map->getColumnsCount()*MAP_CELL_SIZE, (mp_map->getRowsCount()+MAP_MAX_HEIGHT/MAP_CELL_SQUARE_HEIGHT)*MAP_CELL_SIZE));
		camera()->setMinStep(MAP_CELL_SIZE/MAP_STEPS_PER_CELL);

		createGUI(p_wad);

		delete p_wad;
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Impossible de créer la map pour ce modèle.", "MapModel", e);
	}
}

void MapModel::moveObjects()
{
	vector<MapObject*> p_objects_v = findAllItems<MapObject>();
	for (unsigned int i=0, size=p_objects_v.size();i<size;i++)
	{
		MapObject* p_obj = p_objects_v[i];

		unsigned int vxy = moveXY(*p_obj);
		int vz = moveZ(*p_obj, vxy);
		updateLayer(*p_obj);

		if (p_obj->getName() == camera()->followedObjectName())
		{
			if (p_obj->getObjStat() & OBJSTAT_JUMPING)
				camera()->move(0.0, -vz*MAP_Z_STEP_SIZE); // annule le suivi du saut
			else if (p_obj->getObjStat() & OBJSTAT_LANDING)
				camera()->moveAt(0.0, 0.0, 10);
		}
	}
}

void MapModel::applyEffects()
{
    PfTextBox* p_textBox = findItem<PfTextBox>("GUI_TEXTBOX_1");
    if (p_textBox == 0)
        throw PfException(__LINE__, __FILE__, "Impossible de trouver l'objet GUI_TEXTBOX_1");
    if (m_effects & EFFECT_PRINT)
    {
        p_textBox->changeText(mp_map->scriptEntry(m_effects & 255));
        showItem("GUI_TEXTBOX_1");
    }
    else
    {
        p_textBox->changeText(""); // pour réinitialiser l'affichage dynamique
        hideItem("GUI_TEXTBOX_1");
    }
}

void MapModel::processInteractions()
{
    vector<MapObject*> p_objects_v = findAllItems<MapObject>();
    MapObject *p_obj, *p_otherObject;
    const MapZone *q_actionZone, *q_otherZone;
    vector<pair<unsigned int, unsigned int> > cellsInRect_v;
    vector<string> objects_v, tmp_v;
    bool objectInTriggerZone;
    for (unsigned int n=0, count=p_objects_v.size();n<count;n++)
    {
        p_obj = p_objects_v[n];
        q_actionZone = p_obj->constZone(BOX_TYPE_ACTION);

        if (q_actionZone != 0)
        {
            PfRectangle actionRect(q_actionZone->getRect());
            actionRect.shift(PfOrientation::SOUTH, ((float) p_obj->getZ()-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE);

            cellsInRect_v = mp_map->cellsCoord(q_actionZone->getRect(), p_obj->getZ());
            objects_v.clear();
            tmp_v.clear();
            for (unsigned int i=0, size=cellsInRect_v.size();i<size;i++)
            {
                tmp_v = mp_map->objectsOnCell(cellsInRect_v[i]);
                addOnlyNewValues(tmp_v, objects_v);
            }
            for (unsigned int i=0, size=objects_v.size();i<size;i++)
            {
                if (objects_v[i] == p_obj->getName())
                    continue;
                p_otherObject = findItem<MapObject>(objects_v[i]);
                if (p_otherObject == 0)
                    continue;

                // Gestion des zones DOOR

                q_otherZone = p_otherObject->constZone(BOX_TYPE_DOOR);
                if (q_otherZone != 0)
                {
                    PfRectangle otherRect(q_otherZone->getRect());
                    otherRect.shift(PfOrientation::SOUTH, ((float) p_otherObject->getZ()-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE);
                    if (otherRect.contains(actionRect))
                    {
                        if (p_obj->getName() == m_controlledMobName)
                            p_obj->setObjCode(p_otherObject->getObjCode());
                        else
                            mp_map->removeObject(p_obj->getName());
                    }
                }

                // Gestion de l'activation

                q_otherZone = p_otherObject->constZone(BOX_TYPE_TRIGGER);
                if (q_otherZone != 0)
                {
                    PfRectangle otherRect(q_otherZone->getRect());
                    otherRect.shift(PfOrientation::SOUTH, ((float) p_otherObject->getZ()-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE);
                    if (otherRect.contains(actionRect))
                    {
                        pfflag32 activCode = m_userActivation?ACTIVCODE_USER:ACTIVCODE_ANY;
                        switch (p_obj->getOrientation())
                        {
                        case PfOrientation::SOUTH:
                            activCode |= ACTIVCODE_SOUTH;
                            break;
                        case PfOrientation::WEST:
                            activCode |= ACTIVCODE_WEST;
                            break;
                        case PfOrientation::NORTH:
                            activCode |= ACTIVCODE_NORTH;
                            break;
                        case PfOrientation::EAST:
                            activCode |= ACTIVCODE_EAST;
                            break;
                        default:
                            break;
                        }
                        m_effects |= p_otherObject->activate(activCode);
                        m_effects &= 0xFFFFFF00;
                        m_effects |= (MAX(0, (p_otherObject->getObjCode()-1)) & 0xFF);
                    }
                }
            }
        }
    }

    for (unsigned int n=0, count=p_objects_v.size();n<count;n++)
    {
        // Gestion des désactivations

        p_obj = p_objects_v[n];
        if (!(p_obj->getObjStat() & OBJSTAT_ACTIVATED))
            continue;

        q_otherZone = p_obj->constZone(BOX_TYPE_TRIGGER);

        if (q_otherZone != 0)
        {
            objectInTriggerZone = false;
            PfRectangle triggerRect(q_otherZone->getRect());
            triggerRect.shift(PfOrientation::SOUTH, ((float) p_obj->getZ()-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE);

            cellsInRect_v = mp_map->cellsCoord(q_otherZone->getRect(), p_obj->getZ());
            tmp_v.clear();
            objects_v.clear();
            for (unsigned int i=0, size=cellsInRect_v.size();i<size;i++)
            {
                tmp_v = mp_map->objectsOnCell(cellsInRect_v[i]);
                addOnlyNewValues(tmp_v, objects_v);
            }
            for (unsigned int i=0, size=objects_v.size();i<size;i++)
            {
                p_otherObject = findItem<MapObject>(objects_v[i]);
                if (p_otherObject == 0)
                    continue;
                if (p_otherObject->getName() == p_obj->getName())
                    continue;
                if (p_otherObject->zone(BOX_TYPE_ACTION) == 0)
                    continue;
                PfRectangle rect(p_otherObject->zone(BOX_TYPE_ACTION)->getRect());
                rect.shift(PfOrientation::SOUTH, ((float) p_otherObject->getZ()-MAP_CELL_SQUARE_HEIGHT)/MAP_CELL_SQUARE_HEIGHT*MAP_CELL_SIZE);
                if (triggerRect.contains(rect))
                {
                    objectInTriggerZone = true;
                    break;
                }
            }

            if (!objectInTriggerZone)
                m_effects &= ~(p_obj->deactivate());
        }
    }
}

pfflag32 MapModel::endStatus() const
{
    PfReturnCode code = RETURN_NOTHING;
    char value = 0;

	const Mob* p_mob = findConstItem<Mob>(m_controlledMobName);
	if (p_mob == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + m_controlledMobName + ".");
	if (p_mob->getObjStat() & OBJSTAT_DEAD)
           code = RETURN_FAIL;
    else if (p_mob->getObjCode() != 0)
    {
        code = RETURN_OK;
        value = p_mob->getObjCode();
    }

    return code | value;
}

string MapModel::mapLink(unsigned int index) const
{
    string str;

    if (index > 0 && index <= mp_map->mapLinksCount())
        str = mp_map->mapLink(index);

    return str; // pour le compilateur
}

PfReturnCode MapModel::processInstruction()
{
	PfInstruction instruction = getInstruction();
	int value = instructionValues().nextInt();
	PfReturnCode rtnCode = RETURN_NOTHING;

	try
	{
		Mob* p_mob = findItem<Mob>(m_controlledMobName);
		if (p_mob == 0)
			throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + m_controlledMobName + ".");
        if (instruction == INSTRUCTION_CHECK)
            rtnCode = p_mob->readInstruction(instruction, mp_map->getGroundType());
        else if (instruction == INSTRUCTION_ACTIVATE)
            m_userActivation = (value==1)?1:0;
        else
            rtnCode = p_mob->readInstruction(instruction);
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Impossible d'appliquer l'instruction ") + textFrom(instruction) + ".", e);
	}

	return rtnCode;
}

unsigned int MapModel::moveXY(MapObject& r_obj)
{
	unsigned int steps = 0;

	if (r_obj.getSpeed() != 0)
	{
		// collisions avec la map et ses bords

		steps = stepsBeforeCollision(r_obj);

		// déplacement

		if (steps > 0)
        {
			r_obj.move(steps);
			mp_map->updateObjectPosition(r_obj);
        }
		if (steps != (unsigned int) ABS(r_obj.getSpeed()))
			r_obj.readInstruction(INSTRUCTION_STOP, 1); // 1 pour prendre l'état OBJSTAT_STOPPED
	}

	return steps;
}

int MapModel::moveZ(MapObject& r_obj, unsigned int xySteps)
{
	int vz = r_obj.speedZ();
	int initZ = r_obj.getZ();
	int zSteps = vz;
	int maxZ;

	if (vz != 0)
	{
		if (vz > 0)
			zSteps = MIN(vz, (int) zStepsBeforeObjectUpCollision(r_obj));
		else
		{
			zSteps = zStepsBeforeCellCollision(r_obj);
			zSteps = MIN(zSteps, (int) zStepsBeforeObjectDownCollision(r_obj));
			zSteps = MIN(zSteps, -vz);
			zSteps *= -1;
		}

		if (zSteps != 0)
			r_obj.changeZ(r_obj.getZ() + zSteps);
	}

	if (!r_obj.isFloating())
	{
        maxZ = maxZUnderObject(r_obj);

        if (maxZ != r_obj.getZ())
        {
            if (maxZ < r_obj.getZ())
            {
                if (r_obj.getObjStat() & OBJSTAT_ONTHEFLOOR)
                {
                    int tmpZ = r_obj.getZ();
                    r_obj.changeZ(maxZ); // nécessaire pour faire le test suivant, on l'annulera si nécessaire
                    PfOrientation::PfOrientationValue o = r_obj.getOrientation();
                    r_obj.changeOrientation(PfOrientation(o).oppositeValue());
                    if (stepsBeforeCollision(r_obj) != xySteps) // si l'objet est tombé, c'est-à-dire si dans le sens contraire il ne pourrait faire la même distance car bloqué.
                    {
                        r_obj.changeZ(tmpZ);
                        r_obj.addSpeedZ(-MAP_GRAVITY);
                    } // sinon, on aura donc changé l'altitude de l'objet car il s'agit d'une pente (l'objet était au sol mais maxZ a diminué).
                    r_obj.changeOrientation(o);
                }
                else // là, on est dans le cas où l'objet était déjà en l'air.
                    r_obj.addSpeedZ(-MAP_GRAVITY);
            }
            else
            {
                r_obj.changeZ(maxZ);
                r_obj.setSpeedZ(0);
            }
        }
        else if (vz <= 0) // atterrissage
            r_obj.setSpeedZ(0);
	}

	#ifdef DBG_MOVEZ
	if (g_debug && r_obj.getName() == m_controlledMobName)
	{
		g_dbInt_v_map["zSteps"].push_back(zSteps);
		g_dbInt_v_map["maxZ"].push_back(maxZ);
		g_dbInt_v_map["objZ"].push_back(r_obj.getZ());
		g_dbInt_v_map["vz"].push_back(vz);
	}
	#endif

	return r_obj.getZ() - initZ; // pour la caméra au retour de cette méthode
}

unsigned int MapModel::stepsBeforeCollision(const MapObject& rc_obj) const
{
	int steps = MIN(rc_obj.getSpeed(), mp_map->stepsBeforeCollision(rc_obj.pathRect(), rc_obj.getOrientation(), rc_obj.getZ()));

	if (steps > 0)
	{
		const MapObject* q_obj;
		vector<pair<unsigned int, unsigned int> > cellsOnPath_v = mp_map->cellsCoord(rc_obj.pathRect(), rc_obj.getZ());
		vector<string> objectsOnPath_v;
		for (unsigned int i=0, size=cellsOnPath_v.size();i<size;i++)
		{
			objectsOnPath_v = mp_map->objectsOnCell(cellsOnPath_v[i]);
			for (unsigned int j=0, size2=objectsOnPath_v.size();j<size2;j++)
			{
				q_obj = findConstItem<MapObject>(objectsOnPath_v[j]);
				if (q_obj == 0)
					throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + objectsOnPath_v[j] + ".");
				if (q_obj->getName() == rc_obj.getName())
					continue;
				steps = MIN(steps, (int) (rc_obj.distanceBeforeCollision(*q_obj)/MAP_CELL_SIZE*MAP_STEPS_PER_CELL+FLOAT_MARGIN));
			}
		}
	}

	return steps;
}

unsigned int MapModel::zStepsBeforeCellCollision(const MapObject& rc_obj) const
{
	vector<pair<unsigned int, unsigned int> > cellsInRect_v;
	PfRectangle collRect(rc_obj.rect_x(), rc_obj.rect_y(), 0.0, 0.0);
	const MapZone* q_zone = rc_obj.constZone(BOX_TYPE_COLLISION);
	if (q_zone != 0)
        collRect = q_zone->getRect();
	cellsInRect_v = mp_map->cellsCoord(collRect, rc_obj.getZ());

	for (int step=rc_obj.getZ();step>0;step--) // == 0 ---> rien ne se trouve à 0, et de plus le dernier return suffit.
	{
		for (unsigned int j=0, size=cellsInRect_v.size();j<size;j++)
		{
			if (mp_map->cell(cellsInRect_v[j])->maxZIn(collRect, rc_obj.getZ(), true) == step)
				return (unsigned int) (rc_obj.getZ() - step);
		}
	}

	return MAX(0, rc_obj.getZ());
}

unsigned int MapModel::zStepsBeforeObjectUpCollision(const MapObject& rc_obj) const
{
	vector<string> objects_v, tmp_v;
	const MapObject* q_collObj;
	vector<pair<unsigned int, unsigned int> > cellsInRect_v;
	PfRectangle collRect(rc_obj.rect_x(), rc_obj.rect_y(), 0.0, 0.0);
	const MapZone* q_zone = rc_obj.constZone(BOX_TYPE_COLLISION);
	if (q_zone != 0)
        collRect = q_zone->getRect();
	cellsInRect_v = mp_map->cellsCoord(collRect, rc_obj.getZ());

	for (unsigned int j=0, size=cellsInRect_v.size();j<size;j++)
	{
		tmp_v = mp_map->objectsOnCell(cellsInRect_v[j]);
		for (unsigned int k=0, size2=tmp_v.size();k<size2;k++)
			objects_v.push_back(tmp_v[k]);
	}

	for (int step=rc_obj.getZ();step<MAP_MAX_HEIGHT;step++)
	{
		for (unsigned int j=0, size=objects_v.size();j<size;j++)
		{
			q_collObj = findConstItem<MapObject>(objects_v[j]);
			if (q_collObj == 0)
				throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + objects_v[j] + ".");
			if (q_collObj->getName() == rc_obj.getName())
				continue;

			if (rc_obj.getZ() >= q_collObj->getZ())
				continue;
			if (rc_obj.isColliding(*q_collObj, step - rc_obj.getZ(), false))
				return (unsigned int) (step - rc_obj.getZ());
		}
	}

	return (unsigned int) (MAP_MAX_HEIGHT - rc_obj.getZ());
}

unsigned int MapModel::zStepsBeforeObjectDownCollision(const MapObject& rc_obj) const
{
	vector<string> objects_v, tmp_v;
	const MapObject* q_collObj;
	vector<pair<unsigned int, unsigned int> > cellsInRect_v;
	PfRectangle collRect;
	const MapZone* q_zone = rc_obj.constZone(BOX_TYPE_COLLISION);
	if (q_zone != 0)
        collRect = q_zone->getRect();
	cellsInRect_v = mp_map->cellsCoord(collRect, rc_obj.getZ());

	for (unsigned int j=0, size=cellsInRect_v.size();j<size;j++)
	{
		tmp_v = mp_map->objectsOnCell(cellsInRect_v[j]);
		addOnlyNewValues(tmp_v, objects_v);
	}

	for (int step=rc_obj.getZ();step>0;step--)
	{
		for (unsigned int j=0, size=objects_v.size();j<size;j++)
		{
			q_collObj = findConstItem<MapObject>(objects_v[j]);
			if (q_collObj == 0)
				throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + objects_v[j] + ".");
			if (q_collObj->getName() == rc_obj.getName())
				continue;

			if (rc_obj.isColliding(*q_collObj, step - rc_obj.getZ(), false))
				return (unsigned int) (rc_obj.getZ() - step);
		}
	}

	return MAX(0, rc_obj.getZ());
}

int MapModel::maxZUnderObject(const MapObject& rc_obj) const
{
	vector<pair<unsigned int, unsigned int> > cellsInRect_v;
	vector<string> objects_v, tmp_v;
	PfRectangle collRect(rc_obj.rect_x(), rc_obj.rect_y(), 0.0, 0.0);
	const MapZone* q_zone = rc_obj.constZone(BOX_TYPE_COLLISION);
	if (q_zone != 0)
        collRect = q_zone->getRect();
	cellsInRect_v = mp_map->cellsCoord(collRect, rc_obj.getZ());
	int maxZ = 0, tmpZ;
	const MapObject* q_obj;

	for (unsigned int i=0, size=cellsInRect_v.size();i<size;i++)
	{
		// boucle de contrôle d'altitude

		tmpZ = mp_map->cell(cellsInRect_v[i])->maxZIn(collRect, rc_obj.getZ(), true);
		maxZ = MAX(maxZ, tmpZ);

		// on en profite pour remplir la liste d'objets

		tmp_v = mp_map->objectsOnCell(cellsInRect_v[i]);
		for (unsigned int k=0, size2=tmp_v.size();k<size2;k++)
			objects_v.push_back(tmp_v[k]);
	}

	for (unsigned int i=0, size=objects_v.size();i<size;i++)
	{
		q_obj = findConstItem<MapObject>(objects_v[i]);
		if (q_obj == 0)
			throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + objects_v[i] + ".");
		if (q_obj->getName() == rc_obj.getName())
			continue;

        unsigned int collHeight = 0;
        q_zone = q_obj->constZone(BOX_TYPE_COLLISION);
        if (q_zone != 0)
            collHeight = q_zone->getHeight();
		tmpZ = q_obj->getZ() + collHeight;
		if (tmpZ <= rc_obj.getZ() && rc_obj.isColliding(*q_obj, tmpZ - rc_obj.getZ(), false))
			maxZ = MAX(maxZ, tmpZ);
	}

	return maxZ;
}

void MapModel::updateLayer(MapObject& r_obj)
{
    bool hasColl = (r_obj.constZone(BOX_TYPE_COLLISION, true) != 0);
    int layer = Map::layerAt(hasColl?r_obj.constZone(BOX_TYPE_COLLISION, true)->getRect():r_obj.getRect(), r_obj.getZ());

    if (hasColl)
    {
        vector<string> objectsOnPath_v, tmp_v;
        vector<pair<unsigned int, unsigned int> > cellsInRect_v = mp_map->cellsCoord(r_obj.constZone(BOX_TYPE_COLLISION, true)->getRect(), r_obj.getZ());
        for (unsigned int i=0, size=cellsInRect_v.size();i<size;i++)
        {
            tmp_v = mp_map->objectsOnCell(cellsInRect_v[i]);
            for (unsigned int j=0, size2=tmp_v.size();j<size2;j++)
                objectsOnPath_v.push_back(tmp_v[j]);
        }
        for (unsigned int i=0, size=objectsOnPath_v.size();i<size;i++)
        {
            MapObject* p_collObj = findItem<MapObject>(objectsOnPath_v[i]);
            if (p_collObj == 0)
                throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet ") + objectsOnPath_v[i] + ".");
            if (p_collObj->getName() == r_obj.getName())
                continue;
            if (r_obj.getZ() > p_collObj->getZ() && r_obj.isColliding(*p_collObj))
                layer = MAX(layer, p_collObj->getLayer()+1);
        }
    }

	r_obj.setLayer(layer);
}

void MapModel::createGUI(PfWad* p_wad)
{
    try
	{
	    p_wad->addWad(MAP_GUI_WAD_NAME);

		AnimatedGLItem* p_glItem;

		// bordures
		p_glItem = p_wad->generateGLItem(WAD_GUI_BORDER, PfRectangle(0, 0, SYSTEM_BORDER_WIDTH, 1), MAX_LAYER-1);
		p_glItem->setCoordRelativeToBorder(false);
		addItem(p_glItem);
		p_glItem = p_wad->generateGLItem(WAD_GUI_BORDER, PfRectangle(1-SYSTEM_BORDER_WIDTH, 0, SYSTEM_BORDER_WIDTH, 1), MAX_LAYER-1);
		p_glItem->setCoordRelativeToBorder(false);
		addItem(p_glItem);

		// boîte de dialogue
		p_glItem = p_wad->generateGLItem(WAD_GUI_TEXTBOX, PfRectangle(0, 0, 1, 0.25), MAX_LAYER, FONT_TEXTURE_INDEX_2, 0, 0.02, 0.08);
		p_glItem->setStatic(true);
		(dynamic_cast<PfTextBox*>(p_glItem))->setMultiLine(true);
		(dynamic_cast<PfTextBox*>(p_glItem))->setLinesCount(5);
		addItem(p_glItem);
		hideItem(p_glItem->getName());
	}
	catch (PfException& e)
	{
	    throw PfException(__LINE__, __FILE__, "Impossible de créer l'ensemble des composants GUI.", e);
	}
}
