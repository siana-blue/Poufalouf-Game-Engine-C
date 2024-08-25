#include "wad.h"

#include "glitem.h"
#include "misc.h"
#include "errors.h"
#include "glfunc.h"
#include "fmodfunc.h"
#include "pfgui.h"
#include "mapobject.h"
#include "mob.h"
#include "jungle.h"
#include "grass.h"
#include "menumvcsystem.h"
#include "mapbackground.h"
#include "portal.h"
#include "fence.h"
#include "sign.h"

bool PfWad::s_wadOpen = false;
unsigned int PfWad::s_maxTextureIndex = 0;
map<string, unsigned int> g_wadFirstIndexes_map;

PfWad::PfWad(const string& wadName) : m_resCount(0), m_totalTextCount(0), m_totalSoundCount(0), m_currentLoadStep(0), m_name(wadName), m_prevSoundsCount(0)
{
	if (s_wadOpen)
		throw PfException(__LINE__, __FILE__, "Impossible de créer l'objet PfWad : un PfWad est déjà ouvert.");

	string str = string(WAD_DIR) + wadName + "." + WAD_EXT;
	ifstream ifs(str.c_str(), ios::binary);

	int val;

	unsigned int offset = s_maxTextureIndex;
    if (g_wadFirstIndexes_map.find(wadName) != g_wadFirstIndexes_map.end())
        offset = g_wadFirstIndexes_map[wadName];
	try
	{
		// Tests de validité

		if (!ifs.is_open())
			throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier.", wadName + "." + WAD_EXT);

		ifs.read((char*) &val, sizeof(int));
		if (val != PFGAME_VERSION)
		{
			ifs.close();
			throw FileException(__LINE__, __FILE__, string("ce fichier est d'une version dépassée (version ") + itostr(val) + "). La version actuelle est " + itostr(PFGAME_VERSION) + ".", wadName + "." + WAD_EXT);
		}

		// Ajout des ressources

		addRes(ifs, offset);
		addSlots(ifs, offset);

		if (ifs.fail())
        {
            ifs.close();
            throw FileException(__LINE__, __FILE__, "fichier WAD non valide.", wadName + "." + WAD_EXT);
        }

		ifs.close();
	}
	catch (PfException& e)
	{
		if (ifs.is_open())
			ifs.close();
		throw ConstructorException(__LINE__, __FILE__, string("impossible de créer le WAD ") + wadName + ".", "PfWad", e);
	}

	g_wadFirstIndexes_map.insert(pair<string, unsigned int>(wadName, offset));

	s_wadOpen = true;
}

PfWad::PfWad(const string& wadName, const string& menuName, const string& menuWadName) : m_resCount(0),
m_totalTextCount(0), m_totalSoundCount(0), m_currentLoadStep(0), m_name(wadName), m_prevSoundsCount(0)
{
	if (s_wadOpen)
		throw PfException(__LINE__, __FILE__, "Impossible de créer l'objet PfWad : un PfWad est déjà ouvert.");

    unsigned int offset;
	try
	{
		PfWad* p_wad = new PfWad(menuWadName);
		MenuMVCSystem* p_loadingMVCSystem = new MenuMVCSystem(menuName, *p_wad);
		delete p_wad;

		string wadStr = string(WAD_DIR) + wadName + "." + WAD_EXT;
		ifstream ifs(wadStr.c_str(), ios::binary);
		if (!ifs.is_open())
			throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", wadStr);

		unsigned int uval = 0;
		int ival = 0, preVal = -1;

		offset = s_maxTextureIndex;
        if (g_wadFirstIndexes_map.find(wadName) != g_wadFirstIndexes_map.end())
            offset = g_wadFirstIndexes_map[wadName];

		ifs.read((char*) &ival, sizeof(int));
		if (ival != PFGAME_VERSION)
		{
			delete p_loadingMVCSystem;
			ifs.close();
			throw FileException(__LINE__, __FILE__, string("ce fichier est d'une version dépassée (version ") + itostr(ival) + "). La version actuelle est " + itostr(PFGAME_VERSION) + ".", wadStr);
		}
		do
		{
			if (m_totalTextCount > 0 || m_totalSoundCount > 0)
				ival = ((float) uval+FLOAT_MARGIN)/(m_totalTextCount+m_totalSoundCount)*100;
			if (ival > preVal) // afin d'éviter trop de rendus inutiles
			{
				p_loadingMVCSystem->clearValues();
				p_loadingMVCSystem->addValue(ival);
				p_loadingMVCSystem->run(true);
				preVal = ival;
			}
			uval = loadStepByStep(ifs, offset);
		} while (uval != 0);
		ifs.close();
		delete p_loadingMVCSystem;
	}
	catch (PfException& e)
	{
		throw ConstructorException(__LINE__, __FILE__, "Erreur de lecture des fichiers.", "PfWad", e);
	}

	g_wadFirstIndexes_map.insert(pair<string, unsigned int>(wadName, offset));

	s_wadOpen = true;
}

PfWad::~PfWad()
{
	s_wadOpen = false;
}

AnimatedGLItem* PfWad::generateGLItem(PfWadSlot slot, const PfRectangle& rectangle, int layer, int value1, int value2, float value3, float value4, PfGameMode gameMode)
{
	if (m_wadObjects_map.find(slot) == m_wadObjects_map.end())
		throw ArgumentException(__LINE__, __FILE__, string("Le slot ") + textFrom(slot) + " n'est pas dans le wad " + m_name + ".", "slot", "PfWad::generateGLItem");
	if (m_wadObjects_map[slot].frames_v_map.size() == 0)
		throw PfException(__LINE__, __FILE__, string("L'objet au slot ") + textFrom(slot) + " n'a pas d'animation.");

	AnimatedGLItem* p_rtn = 0;

	map<string, int>::iterator it = m_indexes_map.find(textFrom(slot));
	if (it == m_indexes_map.end())
	{
		m_indexes_map[textFrom(slot)] = 1;
		it = m_indexes_map.find(textFrom(slot));
	}
	else
		(it->second)++;
	string name = it->first + "_" + itostr(it->second);

	// pour les widgets : choix de la map d'animations
	PfWidget::PfWidgetStatusMap stMap = PfWidget::WIDGET_IDLE_MAP;
	if (m_wadObjects_map[slot].frames_v_map.find(ANIM_HIGHLIGHTED) != m_wadObjects_map[slot].frames_v_map.end())
		stMap = PfWidget::WIDGET_HIGHLIGHT_MAP;
	if (m_wadObjects_map[slot].frames_v_map.find(ANIM_SELECTED) != m_wadObjects_map[slot].frames_v_map.end())
		stMap = PfWidget::WIDGET_SELECT_MAP;
	if (m_wadObjects_map[slot].frames_v_map.find(ANIM_ACTIVATED) != m_wadObjects_map[slot].frames_v_map.end())
		stMap = PfWidget::WIDGET_STANDARD_MAP;
	// fin du traitement widgets

	// Génération des objets

	map<PfBoxType, vector<MapZone> >::const_iterator z_it;
	switch (gameMode)
	{
		case GAME_MAP:
			switch (slot)
			{
				case WAD_NULL_OBJECT:
					throw PfException(__LINE__, __FILE__, "Impossible d'associer un objet au slot WAD_NULL_OBJECT.");
				case WAD_MOB:
					p_rtn = new Mob(name, rectangle);
					break;
                case WAD_JUNGLE1:
                case WAD_JUNGLE2:
                    p_rtn = new Jungle(name, rectangle);
                    break;
                case WAD_GRASS:
                    p_rtn = new Grass(name, rectangle);
                    break;
                case WAD_FENCE1:
                case WAD_FENCE2:
                    p_rtn = new Fence(name, rectangle);
                    break;
                case WAD_SIGN1:
                case WAD_SIGN2:
                    p_rtn = new Sign(name, rectangle);
                    break;
                case WAD_BACKGROUND:
                    p_rtn = new MapBackground(name, (unsigned int) value1, (unsigned int) value2);
                    break;
                case WAD_PORTAL1:
                    p_rtn = new Portal(name, rectangle);
                    break;
				default:
					p_rtn = new MapObject(name, rectangle);
					break;
			}

			z_it = m_wadObjects_map[slot].zones_v_map.find(BOX_TYPE_MAIN);
			if (z_it != m_wadObjects_map[slot].zones_v_map.end() && z_it->second.size() > 0)
				p_rtn->setRect(PfRectangle(p_rtn->rect_x(), p_rtn->rect_y(), z_it->second[0].getRect().getW(), z_it->second[0].getRect().getH()));
            for (int i=1;i<ENUM_PF_BOX_TYPE_COUNT;i++)
            {
                z_it = m_wadObjects_map[slot].zones_v_map.find((PfBoxType) i);
                if (z_it != m_wadObjects_map[slot].zones_v_map.end())
                {
                    for (unsigned int j=0, size=z_it->second.size();j<size;j++)
                        (dynamic_cast<MapObject*>(p_rtn))->addZone(z_it->first, z_it->second[j]);
                }
            }
            if (slot != WAD_BACKGROUND)
            {
                (dynamic_cast<MapObject*>(p_rtn))->setBoxAnimLinks(m_wadObjects_map[slot].boxAnimLinks_v_map);
                (dynamic_cast<MapObject*>(p_rtn))->setCenter(m_wadObjects_map[slot].center);
            }
			break;
		default:
			switch (slot)
			{
				case WAD_NULL_OBJECT:
					throw PfException(__LINE__, __FILE__, "Impossible d'associer un objet au slot WAD_NULL_OBJECT.");
				case WAD_MOB:
					p_rtn = new Mob(name, rectangle);
					break;
                case WAD_JUNGLE1:
                case WAD_JUNGLE2:
                    p_rtn = new Jungle(name, rectangle);
                    break;
                case WAD_GRASS:
                    p_rtn = new Grass(name, rectangle);
                    break;
				case WAD_GUI_BUTTON1:
				case WAD_GUI_BUTTON2:
					p_rtn = new PfWidget(name, rectangle, layer, 1, value3, value4, stMap);
					break;
				case WAD_GUI_SELECTIONGRID:
					p_rtn = new PfSelectionGrid(name, rectangle.getX(), rectangle.getY(), rectangle.getW(), rectangle.getH(), value1, value2, layer, stMap);
					break;
				case WAD_GUI_SCROLLBAR:
					p_rtn = new PfScrollBar(name, rectangle, value3, value4, value1, (value2 == 0), layer);
					break;
				case WAD_GUI_LIST1:
                case WAD_GUI_LIST2:
					p_rtn = new PfList(name, rectangle.getX(), rectangle.getY(), rectangle.getW(), value3, value1, layer, 0.02, 0.4, stMap);
					break;
				case WAD_GUI_LABEL1:
				case WAD_GUI_LABEL2:
					p_rtn = new PfWidget(name, rectangle, layer, MAX(1, value1), value3, value4);
					break;
				case WAD_GUI_TEXTBOX:
					p_rtn = new PfTextBox(name, rectangle, layer, value3, value4, (value1==0)?FONT_TEXTURE_INDEX:value1, stMap);
					break;
				case WAD_GUI_SPINBOX:
					p_rtn = new PfSpinBox(name, rectangle, layer, (unsigned int) value1, (unsigned int) value2, stMap);
					break;
				case WAD_GUI_SLIDINGCURSOR:
					p_rtn = new PfSlidingCursor(name, rectangle, (unsigned int) value1, (value2 != 0), layer);
					break;
				case WAD_GUI_PROGRESSBAR:
					p_rtn = new PfProgressBar(name, rectangle, (unsigned int) value1, layer);
					break;
				default:
					p_rtn = new AnimatedGLItem(name, layer, rectangle);
					break;
			}
			break;
	}

	// Ajout des animations

	for (map<PfAnimationStatus, vector<PfAnimationFrame> >::iterator it=m_wadObjects_map[slot].frames_v_map.begin();it!=m_wadObjects_map[slot].frames_v_map.end();++it)
	{
		PfAnimation* p_anim = new PfAnimation;
		for (unsigned int i=0, size=it->second.size();i<size;i++)
		{
			PfAnimationFrame* p_frame = new PfAnimationFrame(it->second[i]);
			if (m_wadObjects_map[slot].flags_map[it->first] & WADMSC_TURNABLE)
				p_frame->addProperties(PfAnimationFrame::FRAME_TURNABLE);
			p_anim->addFrame(p_frame);
		}
		p_anim->reset();
		if (m_wadObjects_map[slot].flags_map[it->first] & WADMSC_LOOP)
			p_anim->setLoop(true);
		p_rtn->addAnimation(p_anim, it->first);
	}

	// Traitements particuliers de certains objets

	switch (slot)
	{
    case WAD_JUNGLE1:
        (dynamic_cast<Jungle*>(p_rtn))->generateJungleElements(3, 5, 0.01);
        break;
    case WAD_JUNGLE2:
        (dynamic_cast<Jungle*>(p_rtn))->generateJungleElements(2, 3, 0.02);
        break;
    case WAD_GRASS:
        if (value1 == 0)
            (dynamic_cast<Grass*>(p_rtn))->generateElements(16);
        break;
    default:
        break;
	}

	return p_rtn;
}

void PfWad::addWad(const string& wadName)
{
	string str = string(WAD_DIR) + wadName + "." + WAD_EXT;
	ifstream ifs(str.c_str(), ios::binary);

	if (!ifs.is_open())
		throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier.", wadName + "." + WAD_EXT);

    m_prevSoundsCount = m_sounds_v.size();

	int val;
	ifs.read((char*) &val, sizeof(int));
	if (val != PFGAME_VERSION)
	{
	   	ifs.close();
	   	throw FileException(__LINE__, __FILE__, string("ce fichier est d'une version dépassée (version ") + itostr(val) + "). La version actuelle est " + itostr(PFGAME_VERSION) + ".", wadName + "." + WAD_EXT);
	}

	unsigned int offset = s_maxTextureIndex;
    if (g_wadFirstIndexes_map.find(wadName) != g_wadFirstIndexes_map.end())
        offset = g_wadFirstIndexes_map[wadName];
	try
	{
		addRes(ifs, offset);
		addSlots(ifs, offset);
	}
	catch (PfException& e)
	{
		ifs.close();
		throw ArgumentException(__LINE__, __FILE__, string("Le WAD ") + wadName + " n'est pas valide.", "wadName", "PfWad::addWad", e);
	}

	g_wadFirstIndexes_map.insert(pair<string, unsigned int>(wadName, offset));

	ifs.close();
}

pair<unsigned int, PfRectangle> PfWad::icon(PfWadSlot slot) const
{
    pair<unsigned int, PfRectangle> rtn;

    map<PfWadSlot, PfWadObject>::const_iterator it = m_wadObjects_map.find(slot);
    if (it == m_wadObjects_map.end())
        throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet au slot ") + textFrom(slot) + ".");
    if (it->second.frames_v_map.find(ANIM_IDLE) == it->second.frames_v_map.end())
        throw PfException(__LINE__, __FILE__, string("Animation ANIM_IDLE non trouvée, impossible de générer une icône au slot ") + textFrom(it->first) + ".");
    if (it->second.frames_v_map.find(ANIM_IDLE)->second.size() == 0)
        throw PfException(__LINE__, __FILE__, string("Animation ANIM_IDLE sans frame, impossible de générer une icône au slot ") + textFrom(it->first) + ".");
    rtn = pair<unsigned int, PfRectangle>(it->second.frames_v_map.find(ANIM_IDLE)->second[0].getTextureIndex(),
                                          it->second.frames_v_map.find(ANIM_IDLE)->second[0].textCoordRectangle());

    return rtn;
}

vector<pair<unsigned int, PfRectangle> > PfWad::icons(const set<PfWadSlot>& excludedSlots_set) const
{
	vector<pair<unsigned int, PfRectangle> > rtn_v;

	for (map<PfWadSlot, PfWadObject>::const_iterator it=m_wadObjects_map.begin();it!=m_wadObjects_map.end();++it)
	{
		if (excludedSlots_set.find(it->first) != excludedSlots_set.end())
			continue;
		rtn_v.push_back(icon(it->first));
	}

	return rtn_v;
}

vector<PfWadSlot> PfWad::slots(const set<PfWadSlot>& excludedSlots_set) const
{
    vector<PfWadSlot> rtn_v;

    for (map<PfWadSlot, PfWadObject>::const_iterator it=m_wadObjects_map.begin();it!=m_wadObjects_map.end();++it)
    {
        if (excludedSlots_set.find(it->first) != excludedSlots_set.end())
			continue;
	    rtn_v.push_back(it->first);
    }

    return rtn_v;
}

bool PfWad::hasSlot(PfWadSlot slot) const
{
    return (m_wadObjects_map.find(slot) != m_wadObjects_map.end());
}

unsigned int PfWad::loadStepByStep(ifstream& r_ifs, unsigned int textureIndexOffset)
{
	if (m_currentLoadStep == 0)
		r_ifs.read((char*) &m_totalTextCount, sizeof(unsigned int));
	if (m_currentLoadStep == m_totalTextCount)
		r_ifs.read((char*) &m_totalSoundCount, sizeof(unsigned int));

	if (m_currentLoadStep < m_totalTextCount)
	{
		unsigned int ind = m_currentLoadStep + 1 + textureIndexOffset;
		addTexture(r_ifs, ind);
		if (ind >= s_maxTextureIndex)
			s_maxTextureIndex++;
		m_resCount++;
		m_currentLoadStep++;
	}
	else if (m_currentLoadStep < m_totalTextCount + m_totalSoundCount)
	{
	   	string str;
		char c;
	   	do
	   	{
		   	r_ifs.read(&c, sizeof(char));
		   	str.append(1, c);
	   	} while (c != '\0' && r_ifs.good());
		try
		{
	   		m_sounds_v.push_back(addSound(string(SOUNDS_DIR) + str, false));
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, string("Impossible d'ajouter le son ") + str + ".", e);
		}
		m_currentLoadStep++;
	}
	else
	{
		addSlots(r_ifs, textureIndexOffset);
		m_currentLoadStep = 0;
	}

	return m_currentLoadStep;
}

void PfWad::addRes(ifstream& r_ifs, unsigned int textureIndexOffset)
{
	unsigned int uint;
	string str;
	char c;

	r_ifs.read((char*) &uint, sizeof(unsigned int));
	for (unsigned int i=1;i<=uint;i++)
	{
		unsigned int ind = i + textureIndexOffset;
	   	addTexture(r_ifs, ind);
		if (ind >= s_maxTextureIndex) // ne pas incrémenter quand il n'y a pas chargement
			s_maxTextureIndex++;
		m_resCount++;
	}

	r_ifs.read((char*) &uint, sizeof(unsigned int));
	for (unsigned int i=1;i<=uint;i++)
	{
	   	str.clear();
	   	do
	   	{
		   	r_ifs.read(&c, sizeof(char));
		   	str.append(1, c);
	   	} while (c != '\0' && r_ifs.good());
		try
		{
	   		m_sounds_v.push_back(addSound(string(SOUNDS_DIR) + str, false));
		}
		catch (PfException& e)
		{
			throw PfException(__LINE__, __FILE__, string("Impossible d'ajouter le son ") + str + ".", e);
		}
	}
}

void PfWad::addSlots(ifstream& r_ifs, unsigned int textureIndexOffset)
{
	int val, val2;
	float fVal, fVal2;
	float colHeight;
	int count, count2;
	unsigned int slowFactor, zoneIndex;
	PfBoxType boxType;
	PfAnimationStatus status;
	unsigned int textureIndex = 0, soundIndex = 0;
	char crd[4];
	float coord[4];
	pfflag32 flags;
	PfRectangle rect, mainRect;

	while (r_ifs.good())
	{
		flags = WADMSC_NONE;

		r_ifs.read((char*) &val, sizeof(int));
		if (val == END_OF_LIST_INT)
			break;
        r_ifs.read((char*) &val, sizeof(int)); // on relit une valeur car la première est donc NEW_SLOT_FLAG.
		if (val < 0 || val >= ENUM_PF_WAD_SLOT_COUNT)
			throw PfException(__LINE__, __FILE__, "Le slot lu n'est pas valide.");

		PfWadSlot s = (PfWadSlot) val;

		if (m_wadObjects_map.find(s) != m_wadObjects_map.end())
			throw PfException(__LINE__, __FILE__, string("Plusieurs définitions du slot ") + textFrom(s) + ".");

        r_ifs.read((char*) &count, sizeof(int));
		if (r_ifs.fail())
			break;
        colHeight = 1.0; // hauteur de collision par défaut
        for (int i=0;i<count;i++) // nombre de propriétés
        {
            r_ifs.read((char*) &val, sizeof(int));
			if (val < 0 || val >= ENUM_PF_WAD_OBJ_PROPERTY_COUNT)
				throw PfException(__LINE__, __FILE__, "Propriété d'objet non valide.");
            switch ((PfWadObjProperty) val)
            {
            case WADOBJ_COLHEIGHT:
                r_ifs.read((char*) &colHeight, sizeof(float));
                break;
            case WADOBJ_BOX:
                r_ifs.read((char*) &boxType, sizeof(PfBoxType));
                for (int k=0;k<4;k++)
                    r_ifs.read((char*) &coord[k], sizeof(float));
                if (boxType == BOX_TYPE_MAIN)
                    rect = mainRect = PfRectangle(0.0, 0.0, coord[2]*MAP_CELL_SIZE, coord[3]*MAP_CELL_SIZE);
                else
                {
                    if (m_wadObjects_map[s].zones_v_map.find(BOX_TYPE_MAIN) == m_wadObjects_map[s].zones_v_map.end())
                        throw PfException(__LINE__, __FILE__, "BOX_TYPE_MAIN non définie, impossible de générer une autre box.");
                    rect = PfRectangle(coord[0]*mainRect.getW(),
                                       coord[1]*mainRect.getH(),
                                       coord[2]*mainRect.getW(),
                                       coord[3]*mainRect.getH());
                }
                m_wadObjects_map[s].zones_v_map[boxType].push_back(MapZone(rect, MAP_CELL_SQUARE_HEIGHT, (colHeight+FLOAT_MARGIN)*MAP_CELL_SQUARE_HEIGHT));
                break;
            case WADOBJ_CENTER:
                r_ifs.read((char*) &fVal, sizeof(float));
                r_ifs.read((char*) &fVal2, sizeof(float));
                m_wadObjects_map[s].center = PfPoint(fVal, fVal2);
                break;
            }
        }

		r_ifs.read((char*) &count, sizeof(int));
		if (r_ifs.fail())
			break;
		for (int i=0;i<count;i++) // nombre d'animations
		{
			r_ifs.read((char*) &val, sizeof(int));
			if (val < 0 || val >= ENUM_PF_ANIMATION_STATUS_COUNT)
				throw PfException(__LINE__, __FILE__, "Statut d'animation non valide.");
			status = (PfAnimationStatus) val;
			slowFactor = 1;

			r_ifs.read((char*) &count2, sizeof(int));
			if (r_ifs.fail())
				break;
			for (int j=0;j<count2;j++) // nombre de lignes
			{
				r_ifs.read((char*) &val, sizeof(int));
				if (val < 0 || val >= ENUM_PF_WAD_SCRIPT_OPTION_COUNT)
					throw PfException(__LINE__, __FILE__, "Option de wad non valide.");
				bool snd;
				switch ((PfWadScriptOption) val)
				{
					case WADTXT_IMG:
						r_ifs.read((char*) &textureIndex, sizeof(unsigned int));
						break;
					case WADTXT_SND:
						r_ifs.read((char*) &soundIndex, sizeof(unsigned int));
						break;
					case WADTXT_VID:
						r_ifs.read((char*) &val2, sizeof(int));
						count2 += val2 * 2; // WADTXT_IMG + WADTXT_FRM, VID indique que s'enchaîne une liste "cachée" de WADTXT_IMG et FRM
						break;
					case WADTXT_MSC:
						r_ifs.read((char*) &flags, sizeof(int));
						break;
					case WADTXT_FRM:
						snd = false;
						for (int k=0;k<4;k++)
							r_ifs.read(&crd[k], sizeof(char));
						r_ifs.read((char*) &val, sizeof(int));
						while (val != END_OF_LIST_INT)
						{
							if (val < 0 || val >= ENUM_PF_WAD_SCRIPT_OPTION_COUNT)
								throw PfException(__LINE__, __FILE__, "Option de frame non valide.");
							switch ((PfWadScriptOption) val)
							{
								case WADTXT_SND:
									snd = true;
									break;
								default:
									throw PfException(__LINE__, __FILE__, string("Option de frame non valide : ") + textFrom((PfWadScriptOption) val));
							}
							r_ifs.read((char*) &val, sizeof(int));
						}
						m_wadObjects_map[s].frames_v_map[status].push_back(
                                                         PfAnimationFrame(textureIndex + textureIndexOffset,
                                                                          PfRectangle((crd[1]-1)*1./crd[3], (crd[2]-crd[0])*1./crd[2], 1./crd[3], 1./crd[2]),
                                                                          snd?m_sounds_v[m_prevSoundsCount+soundIndex-1]:0));
						break;
					case WADTXT_SLW:
						r_ifs.read((char*) &slowFactor, sizeof(unsigned int));
						break;
                    case WADTXT_BOX:
                        r_ifs.read((char*) &boxType, sizeof(PfBoxType));
                        r_ifs.read((char*) &zoneIndex, sizeof(unsigned int));
                        m_wadObjects_map[s].boxAnimLinks_v_map[status].push_back(pair<PfBoxType, unsigned int>(boxType, zoneIndex));
                        break;
					default:
						break;
				}
			}
			m_wadObjects_map[s].flags_map[status] = flags;
			if (slowFactor > 1)
			{
				vector<PfAnimationFrame>* frames_v_p = &(m_wadObjects_map[s].frames_v_map[status]);
				vector<PfAnimationFrame>::iterator it = frames_v_p->begin();
				while (it != frames_v_p->end())
				{
					for (unsigned int k=0;k<slowFactor-1;k++)
						it = frames_v_p->insert(it, *it) + 1;
					++it;
				}
			}
		}
		r_ifs.read((char*) &val, sizeof(int)); // END_OF_SECTION
		/*
		* END_OF_SECTION était utile du temps lointain où l'on lisait les slots
		* directement dans le fichier WAD.
		* Maintenant qu'ils sont tous lus ensemble et stockés dans les PfWad, ces
		* valeurs n'ont plus vraiment d'intérêt.
		*/
	}
}

