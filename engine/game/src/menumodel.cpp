#include "menumodel.h"

#include <cstdlib>
#include "errors.h"
#include "pfgui.h"
#include "enum.h"
#include "misc.h"
#include "wad.h"
#include "fmodfunc.h"

MenuModel::MenuModel(const string& name, PfWad& r_wad) : m_widgetEffects(EFFECT_NONE), m_currentLayoutId(1)
{
	unsigned int uint;
	int val;
	char c;
	string strTmp;

	// Lecture du fichier MEN

	string str = string(MENU_DIR) + name + "." + MENU_EXT;
	ifstream menIfs(str.c_str(), ios::binary);
	if (!menIfs.is_open())
		throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier.", str);

	// Test de version

	menIfs.read((char*) &val, sizeof(int));
	if (val != PFGAME_VERSION)
	{
		menIfs.close();
		throw FileException(__LINE__, __FILE__, string("le fichier ") + name + " n'est pas de la version la plus récente.\n\tVersion actuelle : " + itostr(PFGAME_VERSION) + "\n\tVersion du fichier : " + itostr(val), str);
	}

	// Variables utilisées pour la construction d'un objet
	vector<string> txt_v;
	int value1, value2;
	float value3, value4;
	float crd[4];
	int layer;
	pfflag32 effectFlags;
	PfWadSlot obj = WAD_NULL_OBJECT;

	// Variables de lecture des options
	int section, option, line, col;
	int selId, selLine, selCol;
	int tmp;
	string musicFileName, mscTxt;
	size_t strPos;
	vector<string> texts_v;
	vector<PfWidget*> p_widgets_v;
	map<int, PfLayout*> p_layouts_map; // associe à chaque indice de layout le layout correspondant.
	map<int, pair<int, int> > layouts_map; // associe à chaque indice de layout la taille du layout à construire
	map<unsigned int, int> widgetToLayout_map; // associe à chaque numéro de widget le layout auquel l'associer (clé : position du widget dans le vecteur).
	map<unsigned int, pair<int, int> > widgetPos_map; // associe à chaque numéro de widget la position dans son layout.
	map<int, pair<int, int> > layoutsSel_map; // associe à chaque indice de layout sa ligne et sa colonne sélectionnée (0 sur un des champs : aucune sélection).

	try
	{
		while (menIfs.good())
		{
			menIfs.read((char*) &section, sizeof(int));
			if (section == END_OF_LIST_INT) // fin de fichier
				break;
			if (section < 0 || section >= ENUM_PF_MENU_SCRIPT_SECTION_COUNT)
				throw PfException(__LINE__, __FILE__, "Section invalide.");
			menIfs.read((char*) &uint, sizeof(unsigned int)); // nombre de lignes dans la section
			switch ((PfMenuScriptSection) section)
			{
				case MENSCT_PUT:
					effectFlags = EFFECT_NONE;
					obj = WAD_NULL_OBJECT;
					txt_v.clear();
					value1 = value2 = 0;
					value3 = value4 = 0.0;
					for (unsigned int i=0;i<uint;i++)
					{
						menIfs.read((char*) &option, sizeof(int));
						if (option < 0 || option >= ENUM_PF_MENU_SCRIPT_OPTION_COUNT)
							throw PfException(__LINE__, __FILE__, "Option invalide.");
						if (((PfMenuScriptOption) option) != MENOPT_OBJ && obj == WAD_NULL_OBJECT)
							throw PfException(__LINE__, __FILE__, "La première ligne de la section PUT n'est pas 'obj'.");

						map<int, pair<int, int> >::iterator it;
						switch ((PfMenuScriptOption) option)
						{
							case MENOPT_OBJ:
							    menIfs.read((char*) &val, sizeof(int)); // on lit une première fois pour se défaire de NEW_SLOT_FLAG
								menIfs.read((char*) &val, sizeof(int));
								if (val < 0 || val >= ENUM_PF_WAD_SLOT_COUNT)
									throw PfException(__LINE__, __FILE__, "Le slot de l'objet n'est pas valide, section PUT.");
								obj = (PfWadSlot) val;
								break;
							case MENOPT_CRD:
								for (int j=0;j<4;j++)
									menIfs.read((char*) &crd[j], sizeof(float));
								break;
							case MENOPT_TXT:
								strTmp.clear();
								menIfs.read((char*) &tmp, sizeof(int)); // passer le flag STRING_FLAG
								do
								{
									menIfs.read(&c, sizeof(char));
									if (c == '\0')
										break;
									strTmp.append(1, c);
								} while (menIfs.good());
								txt_v.push_back(strTmp);
								// remplacement des textes dynamiques
								strPos = txt_v[txt_v.size()-1].find_first_of("<");
								while (strPos != string::npos)
								{
									strTmp = txt_v[txt_v.size()-1].substr(strPos+1, txt_v[txt_v.size()-1].find_first_of(">", strPos)-strPos-1);
									if (strTmp == "PFGAME_VERSION")
										strTmp = itostr(PFGAME_VERSION);
									else if (strTmp != "" && strTmp.at(0) == '%')
									{
										val = atoi(strTmp.substr(1, 2).c_str());
										if (val >= 0 && widgetToLayout_map.find(p_widgets_v.size()) != widgetToLayout_map.end()) // lyt avant txt dans le script !
											m_dynamicTextsWidgets_v_map[(unsigned int) val].push_back(pair<int, pair<int, int> >(widgetToLayout_map[p_widgets_v.size()], widgetPos_map[p_widgets_v.size()]));
										strTmp = "";
									}
									txt_v[txt_v.size()-1].replace(strPos, txt_v[txt_v.size()-1].find_first_of(">")+1, strTmp);
									strPos = txt_v[txt_v.size()-1].find_first_of("<", strPos);
								}
								break;
							case MENOPT_LYT:
								if (obj < ENUM_PF_WAD_SLOT_WIDGETS_BEG || obj > ENUM_PF_WAD_SLOT_WIDGETS_END)
									throw PfException(__LINE__, __FILE__, string("Impossible d'ajouter l'objet suivant dans un layout : ") + textFrom(obj) + ".");
								menIfs.read((char*) &val, sizeof(int));
								menIfs.read((char*) &line, sizeof(int));
								menIfs.read((char*) &col, sizeof(int));
								it = layouts_map.find(val);
								if (it == layouts_map.end())
									layouts_map[val] = pair<int, int>(line, col);
								else if (it->second.first < line || it->second.second < col)
									layouts_map[val] = pair<int, int>(MAX(line, it->second.first), MAX(col, it->second.second)); // modifier la taille du layout ?
								widgetToLayout_map[p_widgets_v.size()] = val;
								widgetPos_map[p_widgets_v.size()] = pair<int, int>(line, col);
								break;
							case MENOPT_EFT:
								menIfs.read((char*) &effectFlags, sizeof(pfflag32));
								break;
							case MENOPT_VAL:
								menIfs.read((char*) &value1, sizeof(int));
								menIfs.read((char*) &value2, sizeof(int));
								menIfs.read((char*) &value3, sizeof(float));
								menIfs.read((char*) &value4, sizeof(float));
								break;
							case MENOPT_DYN:
								menIfs.read((char*) &val, sizeof(int));
								if (val >= 0 && widgetToLayout_map.find(p_widgets_v.size()) != widgetToLayout_map.end()) // lyt avant dyn dans le script !
									m_dynamicValuesWidgets_v_map[(unsigned int) val].push_back(pair<int, pair<int, int> >(widgetToLayout_map[p_widgets_v.size()], widgetPos_map[p_widgets_v.size()]));
								else
									throw PfException(__LINE__, __FILE__, "Option 'dyn' sans ou avant l'option 'lyt' (ou valeur négative).");
								break;
						}
					}
					// fin boucle for, toutes les lignes lues, on construit l'objet
					try
					{
						switch (obj)
						{
							case WAD_BACKGROUND:
							case WAD_GUI_BORDER:
								layer = 1;
								break;
							case WAD_STATIC1:
							case WAD_STATIC2:
							case WAD_ANIMATED1:
							case WAD_ANIMATED2:
								layer = 2;
								break;
							case WAD_GUI_BUTTON1:
							case WAD_GUI_BUTTON2:
							case WAD_GUI_LIST1:
                            case WAD_GUI_LIST2:
							case WAD_GUI_LABEL1:
							case WAD_GUI_LABEL2:
							case WAD_GUI_SPINBOX:
							case WAD_GUI_SLIDINGCURSOR:
							case WAD_GUI_PROGRESSBAR:
								layer = MAX_LAYER;
								break;
							default:
								throw PfException(__LINE__, __FILE__, string("Slot d'objet non reconnu ici : ") + textFrom(obj) + ".");
						}
						AnimatedGLItem* p_item = r_wad.generateGLItem(obj, PfRectangle(crd[0], crd[1], crd[2], crd[3]), layer, value1, value2, value3, value4);

						if (obj >= ENUM_PF_WAD_SLOT_WIDGETS_BEG && obj <= ENUM_PF_WAD_SLOT_WIDGETS_END)
						{
							(dynamic_cast<PfWidget*>(p_item))->addEffects(effectFlags);
							switch (obj)
							{
								case WAD_GUI_BUTTON1:
								case WAD_GUI_BUTTON2:
									if (txt_v.size() > 0)
										(dynamic_cast<PfWidget*>(p_item))->changeText(txt_v[0]);
									p_widgets_v.push_back(dynamic_cast<PfWidget*>(p_item));
									break;
								case WAD_GUI_LIST1:
                                case WAD_GUI_LIST2:
									try
									{
										if (txt_v.size() == 0)
											throw PfException(__LINE__, __FILE__, "Aucun texte associé à la liste créée.");
										texts_v = filesInDir(txt_v[0], ((txt_v.size()>1)?txt_v[1]:""), SYSTEM_FILE_PREFIX);
										for (unsigned int j=0, size2=texts_v.size();j<size2;j++)
											texts_v[j] = texts_v[j].substr(0, texts_v[j].find_first_of("."));
										(dynamic_cast<PfList*>(p_item))->addTexts(texts_v);
										p_widgets_v.push_back(dynamic_cast<PfList*>(p_item));
									}
									catch (PfException& e)
									{
										throw PfException(__LINE__, __FILE__, "Une liste dans un menu doit être associée à un dossier.", e);
									}
									break;
								case WAD_GUI_LABEL1:
								case WAD_GUI_LABEL2:
									(dynamic_cast<PfWidget*>(p_item))->addText(txt_v[0]);
									p_widgets_v.push_back(dynamic_cast<PfWidget*>(p_item));
									break;
								case WAD_GUI_SPINBOX:
									p_widgets_v.push_back(dynamic_cast<PfSpinBox*>(p_item));
									break;
								case WAD_GUI_SLIDINGCURSOR:
									p_widgets_v.push_back(dynamic_cast<PfSlidingCursor*>(p_item));
									break;
								case WAD_GUI_PROGRESSBAR:
									p_widgets_v.push_back(dynamic_cast<PfProgressBar*>(p_item));
									break;
								default:
									break;
							}
						}
						else
							addItem(p_item);
					}
					catch (PfException& e)
					{
						throw PfException(__LINE__, __FILE__, string("Erreur lors du chargement des objets du menu ") + name + ".", e);
					}
					break;
				case MENSCT_SEL:
					for (unsigned int i=0;i<uint;i++)
					{
						menIfs.read((char*) &option, sizeof(int));
						if (option < 0 || option >= ENUM_PF_MENU_SCRIPT_OPTION_COUNT)
							throw PfException(__LINE__, __FILE__, "Option invalide.");
						switch ((PfMenuScriptOption) option)
						{
							case MENOPT_LYT:
								menIfs.read((char*) &selId, sizeof(int));
								menIfs.read((char*) &selLine, sizeof(int));
								menIfs.read((char*) &selCol, sizeof(int));
								layoutsSel_map[selId] = pair<int, int>(selLine, selCol);
								break;
							default:
								throw PfException(__LINE__, __FILE__, string("Option non autorisée dans la section SEL : ") + textFrom((PfWadScriptOption) option) + ".");
						}
					}
					break;
				case MENSCT_MUS:
					musicFileName.clear();
					for (unsigned int i=0;i<uint;i++)
					{
						menIfs.read((char*) &option, sizeof(int));
						if (option < 0 || option >= ENUM_PF_MENU_SCRIPT_OPTION_COUNT)
							throw PfException(__LINE__, __FILE__, "Option invalide.");
						switch ((PfMenuScriptOption) option)
						{
							case MENOPT_TXT:
								menIfs.read((char*) &tmp, sizeof(int)); // passer le flag STRING_FLAG
								do
								{
									menIfs.read(&c, sizeof(char));
									if (c == '\0')
										break;
									musicFileName.append(1, c);
								} while (menIfs.good());
								break;
							default:
								throw PfException(__LINE__, __FILE__, string("Option non autorisée dans la section MUS : ") + textFrom((PfMenuScriptOption) option) + ".");
						}
					}
					break;
				case MENSCT_MSC:
					for (unsigned int i=0;i<uint;i++)
					{
						menIfs.read((char*) &option, sizeof(int));
						if (option < 0 || option >= ENUM_PF_MENU_SCRIPT_OPTION_COUNT)
							throw PfException(__LINE__, __FILE__, "Option invalide.");
						switch ((PfMenuScriptOption) option)
						{
							case MENOPT_TXT:
								do
								{
									mscTxt.clear();
									do
									{
										menIfs.read(&c, sizeof(char));
										if (c == '\0' || c == ';')
											break;
										mscTxt.append(1, c);
									} while (menIfs.good());
//									if (mscTxt == "highlight")  <---------- cette section MSC n'est en fait plus utilisé,
//										m_allowToJustHighlight = true;  <------ ces lignes sont conservées à titre d'exemple pour plus tard.
									if (c == '\0')
										break;
								} while (menIfs.good());
								break;
							default:
								throw PfException(__LINE__, __FILE__, string("Option non autorisée dans la section MSC : ") + textFrom((PfMenuScriptOption) option) + ".");
						}
					}
					break;
			}
		}
	}
	catch (PfException& e)
	{
		menIfs.close();
		throw ConstructorException(__LINE__, __FILE__, "Erreur lors de la création des objets du menu.", "MenuModel", e);
	}

	// ajout des objets "GUI" au menu

	try
	{
		for (map<int, pair<int, int> >::iterator it=layouts_map.begin();it!=layouts_map.end();++it)
		{
			p_layouts_map[it->first] = new PfLayout(string("layout ") + itostr(it->first), it->second.first, it->second.second);
			if (it->first != m_currentLayoutId) // toujours 1 pour l'instant
				p_layouts_map[it->first]->setStatus(ModelItem::INVISIBLE);
		}

		for (unsigned int i=0, size=p_widgets_v.size();i<size;i++)
		{
			if (widgetToLayout_map.find(i) == widgetToLayout_map.end())
				addItem(p_widgets_v[i]); // pas de layout
			else
				p_layouts_map[widgetToLayout_map[i]]->addWidget(p_widgets_v[i], widgetPos_map[i].first, widgetPos_map[i].second);
		}

		for (map<int, PfLayout*>::iterator it=p_layouts_map.begin();it!=p_layouts_map.end();++it)
		{
			if (layoutsSel_map[it->first].first != 0 && layoutsSel_map[it->first].second != 0)
				it->second->selectWidget(layoutsSel_map[it->first].first, layoutsSel_map[it->first].second);
			addItem(it->second);
		}
	}
	catch (PfException& e)
	{
		menIfs.close();
		throw ConstructorException(__LINE__, __FILE__, "Erreur lors de l'ajout des objets au menu.", "MenuModel", e);
	}

	menIfs.close();

	// musique

	if (musicFileName != "")
		playSound(string(MUSIC_DIR) + musicFileName, true);
}

void MenuModel::changeCurrentLayout(int id)
{
	PfLayout* p_newLayout = findItem<PfLayout>(string("layout ") + itostr(id));
	PfLayout* p_oldLayout = findItem<PfLayout>(string("layout ") + itostr(m_currentLayoutId));

	if (p_newLayout == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet layout ") + itostr(id) + ".");
	if (p_oldLayout == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet layout ") + itostr(m_currentLayoutId) + ".");

	m_currentLayoutId = id;

	p_oldLayout->setStatus(ModelItem::INVISIBLE);
	p_newLayout->setStatus(ModelItem::VISIBLE);
}

void MenuModel::resetLayout()
{
	PfLayout* p_layout = findItem<PfLayout>(string("layout ") + itostr(m_currentLayoutId));
	if (p_layout == 0)
		throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet layout ") + itostr(m_currentLayoutId) + ".");
	p_layout->leaveAllButCurrent();
}

PfReturnCode MenuModel::changeSelection(const PfPoint& rc_point, bool justHighlight)
{
	PfReturnCode code = RETURN_EMPTY;

	try
	{
		PfLayout* p_layout = findItem<PfLayout>(string("layout ") + itostr(m_currentLayoutId));
		if (p_layout == 0)
			throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet layout ") + itostr(m_currentLayoutId) + ".");
		code = p_layout->selectWidgetAt(rc_point, justHighlight);
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Sélection impossible.", e);
	}

	return code;
}

void MenuModel::resetEffects()
{
	m_widgetEffects = EFFECT_NONE;
}

vector<int> MenuModel::values() const
{
	vector<int> rtn_v;
	const PfLayout* q_layout;
	const PfWidget* q_widget;

	rtn_v.push_back(m_widgetEffects & 0xFF);
	for (unsigned int i=0;i<MENU_MAX_LAYOUTS_COUNT;i++)
	{
		q_layout = findConstItem<PfLayout>(string("layout ") + itostr(i+1));
		if (q_layout != 0)
		{
			for (unsigned int j=0, size=q_layout->getRowsCount();j<size;j++)
			{
				for (unsigned int k=0, size2=q_layout->getColumnsCount();k<size2;k++)
				{
					q_widget = q_layout->constWidget(j+1, k+1);
					if (q_widget != 0)
					{
						if (q_widget->getName().compare(0, 11, "GUI_SPINBOX") == 0)
							rtn_v.push_back((dynamic_cast<const PfSpinBox*>(q_widget))->getValue());
						else if (q_widget->getName().compare(0, 8, "GUI_LIST") == 0)
							rtn_v.push_back((dynamic_cast<const PfList*>(q_widget))->currentTextIndex());
					}
				}
			}
		}
	}

	return rtn_v;
}

void MenuModel::addText(const string& text)
{
	m_texts_v.push_back(text);

	unsigned int textId = m_texts_v.size() - 1;
	if (m_dynamicTextsWidgets_v_map.find(textId) != m_dynamicTextsWidgets_v_map.end())
	{
		PfLayout* p_layout;
		PfWidget* p_widget;
		vector<pair<int, pair<int, int> > >* p_v = &(m_dynamicTextsWidgets_v_map[textId]);
		for (unsigned int i=0, size=p_v->size();i<size;i++)
		{
			p_layout = findItem<PfLayout>(string("layout ") + itostr((*p_v)[i].first));
			if (p_layout != 0)
			{
				p_widget = p_layout->widget((*p_v)[i].second.first, (*p_v)[i].second.second);
				if (p_widget != 0)
					p_widget->changeText(text);
			}
		}
	}
}

void MenuModel::clearTexts(bool update)
{
	if (update)
	{
		for (unsigned int textId=0, size=m_texts_v.size();textId<size;textId++)
		{
			if (m_dynamicTextsWidgets_v_map.find(textId) != m_dynamicTextsWidgets_v_map.end())
			{
				PfLayout* p_layout;
				PfWidget* p_widget;
				vector<pair<int, pair<int, int> > >* p_v = &(m_dynamicTextsWidgets_v_map[textId]);
				for (unsigned int i=0, size=p_v->size();i<size;i++)
				{
					p_layout = findItem<PfLayout>(string("layout ") + itostr((*p_v)[i].first));
					if (p_layout != 0)
					{
						p_widget = p_layout->widget((*p_v)[i].second.first, (*p_v)[i].second.second);
						if (p_widget != 0)
							p_widget->changeText("");
					}
				}
			}
		}
	}

	m_texts_v.clear();
}

void MenuModel::addValue(int value)
{
	m_val_v.push_back(value);

	unsigned int valId = m_val_v.size() - 1;
	if (m_dynamicValuesWidgets_v_map.find(valId) != m_dynamicValuesWidgets_v_map.end())
	{
		PfLayout* p_layout;
		PfWidget* p_widget;
		vector<pair<int, pair<int, int> > >* p_v = &(m_dynamicValuesWidgets_v_map[valId]);
		for (unsigned int i=0, size=p_v->size();i<size;i++)
		{
			p_layout = findItem<PfLayout>(string("layout ") + itostr((*p_v)[i].first));
			if (p_layout != 0)
			{
				p_widget = p_layout->widget((*p_v)[i].second.first, (*p_v)[i].second.second);
				if (p_widget != 0)
					p_widget->changeValue(value);
			}
		}
	}
}

void MenuModel::clearValues(bool update)
{
	if (update)
	{
		for (unsigned int valId=0, size=m_val_v.size();valId<size;valId++)
		{
			if (m_dynamicValuesWidgets_v_map.find(valId) != m_dynamicValuesWidgets_v_map.end())
			{
				PfLayout* p_layout;
				PfWidget* p_widget;
				vector<pair<int, pair<int, int> > >* p_v = &(m_dynamicValuesWidgets_v_map[valId]);
				for (unsigned int i=0, size=p_v->size();i<size;i++)
				{
					p_layout = findItem<PfLayout>(string("layout ") + itostr((*p_v)[i].first));
					if (p_layout != 0)
					{
						p_widget = p_layout->widget((*p_v)[i].second.first, (*p_v)[i].second.second);
						if (p_widget != 0)
							p_widget->changeValue(0);
					}
				}
			}
		}
	}

	m_val_v.clear();
}

PfReturnCode MenuModel::processInstruction()
{
	PfInstruction instruction = getInstruction();
	int value = instructionValues().nextInt();
	PfReturnCode rtnCode = RETURN_NOTHING;

	try
	{
		PfLayout* p_layout = findItem<PfLayout>(string("layout ") + itostr(m_currentLayoutId));
		if (p_layout == 0)
			throw PfException(__LINE__, __FILE__, string("Impossible de trouver l'objet layout ") + itostr(m_currentLayoutId) + ".");
		rtnCode = p_layout->readInstruction(instruction, value);
		if (instruction == INSTRUCTION_ACTIVATE)
			m_widgetEffects = p_layout->getEffectFlags();
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "Impossible de traiter l'instruction.", e);
	}

	return rtnCode;
}


