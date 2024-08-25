#include "script.h"

#include <cstdlib>
#include <map>
#include <vector>
#include <dirent.h>
#include <iostream>
#include "enum.h"
#include "misc.h"
#include "errors.h"

map<string, int> g_textures_map;
map<string, int> g_sounds_map;

void addResToWad(ifstream& r_txtFile, ofstream& r_wadFile)
{
	int texturesCount = 0, soundsCount = 0;
	string str, str2;
	vector<string> textures_v; // pour écrire les textures dans l'ordre dans le WAD

	while (r_txtFile.good())
	{
		str = readString(r_txtFile, END_OF_LINE);
		if (r_txtFile.good())
		{
			if (str.compare(0, 4, textFrom(WADTXT_IMG) + "=") == 0)
			{
				str = str.substr(4);
				if (g_textures_map.find(str) == g_textures_map.end())
				{
					g_textures_map.insert(pair<string, int>(str, ++texturesCount));
					textures_v.push_back(str);
				}
			}
			else if (str.compare(0, 4, textFrom(WADTXT_VID) + "=") == 0)
			{
				str = str.substr(4);
				DIR* dir = opendir(str.c_str());
				if (dir == 0)
					throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le répertoire.", str);
				struct dirent* file;
				while ((file=readdir(dir)) != 0)
				{
					str2 = file->d_name;
					if (str2.size() < 4 || str2.substr(str2.size()-4, 4) != ".png") // inclut donc != "." et ".."
						continue;
					str2 = str + ((str.at(str.size()-1)==DIR_SEPARATOR)?"":(string("")+DIR_SEPARATOR)) + str2;
					if (g_textures_map.find(str2) == g_textures_map.end())
					{
						g_textures_map.insert(pair<string, int>(str2, ++texturesCount));
						textures_v.push_back(str2);
					}
				}
				closedir(dir);
			}
			else if (str.compare(0, 4, textFrom(WADTXT_SND) + "=") == 0)
			{
				str = str.substr(4);
				if (g_sounds_map.find(str) == g_sounds_map.end())
					g_sounds_map.insert(pair<string, int>(str, ++soundsCount));
			}
		}
	}

	r_txtFile.clear();
	r_txtFile.seekg(0, ios::beg);

	unsigned int size;
	char c;

	// Ajout des textures

	size = g_textures_map.size();
	r_wadFile.write((char*) &size, sizeof(unsigned int));
	map<string, int>::iterator it;
	for (unsigned int i=0, sz=textures_v.size();i<sz;i++)
	{
		it = g_textures_map.find(textures_v[i]);
		if (it == g_textures_map.end())
			throw PfException(__LINE__, __FILE__, "erreur de nom de fichier texture dans le vecteur textures_v.");
		ifstream binary(it->first.c_str(), ios::binary);
		if (!binary.is_open())
			throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier", it->first);

		writeBinaryFile(r_wadFile, binary);

		binary.close();
	}

	// Ajout des fichiers sons

	size = g_sounds_map.size();
	r_wadFile.write((char*) &size, sizeof(unsigned int));
	for (map<string, int>::iterator it=g_sounds_map.begin();it!=g_sounds_map.end();it++)
	{
		const char* p_cstr = it->first.c_str();
		for (unsigned int i=0, sz=it->first.size();i<sz;i++)
			r_wadFile.write(p_cstr + i, sizeof(char));
		c = '\0';
		r_wadFile.write(&c, sizeof(char));
	}
}

void addObjectsToWad(ifstream& r_txtFile, ofstream& r_wadFile, const string& fileName)
{
	string str;
	unsigned int i;

	while (r_txtFile.good())
	{
		str = readString(r_txtFile, END_OF_LINE);
		if (r_txtFile.fail())
		{
			r_txtFile.clear(); // fin de fichier
			break;
		}
		if (str.at(0) == '$')
		{
			for (i=0;i<ENUM_PF_WAD_SLOT_COUNT;i++)
			{
				if (textFrom((PfWadSlot) i) == str.substr(1))
				{
				    int tmp = NEW_SLOT_FLAG;
				    r_wadFile.write((char*) &tmp, sizeof(int));
					r_wadFile.write((char*) &i, sizeof(int));
					break;
				}
			}
			if (i == ENUM_PF_WAD_SLOT_COUNT)
				throw PfException(__LINE__, __FILE__, "Erreur dans le script : slot de wad attendu et non trouvé.");
            addPropertiesToObject(r_txtFile, r_wadFile, fileName);
			addAnimsToObject(r_txtFile, r_wadFile, fileName);
			i = END_OF_SECTION;
			r_wadFile.write((char*) &i, sizeof(int));
		}
	}
}

void addPropertiesToObject(ifstream& r_txtFile, ofstream& r_wadFile, const string& fileName)
{
    int pos = r_txtFile.tellg(), linesCount = 0;
    string str, str2;

    do
    {
        str = readString(r_txtFile, END_OF_LINE);
        if (r_txtFile.fail())
            break;
        if (str.at(0) == ':')
            linesCount++;
    } while (str.at(0) == ':');

    r_txtFile.clear();
	r_txtFile.seekg(pos, ios::beg);
	r_wadFile.write((char*) &linesCount, sizeof(int));

	int i;
	float fTmp;
	PfBoxType boxType = BOX_TYPE_MAIN;
	size_t posE, posTmp;
	for (int j=0;j<linesCount;j++)
    {
        str = readString(r_txtFile, END_OF_LINE);
        posE = str.find_first_of("=");

        for (i=0;i<ENUM_PF_WAD_OBJ_PROPERTY_COUNT;i++)
        {
            if (str.substr(1, posE-1) == textFrom((PfWadObjProperty) i))
            {
                r_wadFile.write((char*) &i, sizeof(int));

                switch ((PfWadObjProperty) i)
                {
                case WADOBJ_COLHEIGHT:
                    fTmp = atof(str.substr(posE+1).c_str());
                    r_wadFile.write((char*) &fTmp, sizeof(float));
                    break;
                case WADOBJ_BOX:
                    str2 = str.substr(posE+1, str.find_first_of(";")-posE-1);
                    for (int k=0;k<ENUM_PF_BOX_TYPE_COUNT;k++)
                    {
                        if (textFrom((PfBoxType) k) == str2)
                            boxType = (PfBoxType) k;
                    }
                    r_wadFile.write((char*) &boxType, sizeof(PfBoxType));
                    posTmp = str.find_first_of(";") + 1;
                    for (int k=0;k<4;k++)
                    {
                        fTmp = atof(str.substr(posTmp, str.find_first_of(";", posTmp)-posTmp).c_str());
                        posTmp = str.find_first_of(";", posTmp) + 1;
                        r_wadFile.write((char*) &fTmp, sizeof(float));
                    }
                    break;
                case WADOBJ_CENTER:
                    fTmp = atof(str.substr(posE+1, str.find_first_of(";")-posE-1).c_str());
                    r_wadFile.write((char*) &fTmp, sizeof(float));
                    fTmp = atof(str.substr(str.find_first_of(";")+1).c_str());
                    r_wadFile.write((char*) &fTmp, sizeof(float));
                    break;
                default:
                    break;
                }

                break;
            }
        }
        if (i == ENUM_PF_WAD_OBJ_PROPERTY_COUNT)
            throw ScriptException(__LINE__, __FILE__, "erreur dans le script : propriété d'objet attendue et non trouvée.", fileName, str);
    }
}

void addAnimsToObject(ifstream& r_txtFile, ofstream& r_wadFile, const string& fileName)
{
	int pos = r_txtFile.tellg(), animsCount = 0, count = 0, linesCount = -1;
	vector<int> linesInAnim_v;
	string str, str2;
	pfflag32 flags;
	vector<string> vidFrames_v;
	vector<string>::iterator it;
	PfAnimationStatus status = ANIM_NONE;

	// nombre d'animations

	do
	{
		str = readString(r_txtFile, END_OF_LINE);
		if (r_txtFile.fail())
			break;
		if (str.at(0) == '[' && str.at(str.size()-1) == ']')
		{
			animsCount++;
			if (linesCount != -1)
				linesInAnim_v.push_back(linesCount);
			linesCount = 0;
		}
		else if (linesCount != -1)
			linesCount++;
	} while (str.at(0) != '$');
	if (str != "" && str.at(0) == '$')
		linesCount--; // pour le '$'
	linesInAnim_v.push_back(linesCount);

	r_txtFile.clear();
	r_txtFile.seekg(pos, ios::beg);
	r_wadFile.write((char*) &animsCount, sizeof(int));

	// animations

	PfWadScriptOption option;

	linesCount = 0;
	int j, readAnims = 0;
	for (j=0;j<animsCount;j++)
		linesCount += linesInAnim_v[j] + 1; // ligne pour statut + nb lignes
	j = 0;
	while (j < linesCount && r_txtFile.good())
	{
		str = readString(r_txtFile, END_OF_LINE);

		if (str.at(0) == '[' && str.at(str.size()-1) == ']')
		{
			for (int i=0;i<ENUM_PF_ANIMATION_STATUS_COUNT;i++)
			{
				if (str.substr(1, str.size()-2) == textFrom((PfAnimationStatus) i))
				{
					r_wadFile.write((char*) &i, sizeof(int));
					r_wadFile.write((char*) &linesInAnim_v[readAnims], sizeof(int));
					status = (PfAnimationStatus) i;
					break;
				}
			}
			readAnims++;
		}
		else if (status == ANIM_NONE)
		{
			throw ScriptException(__LINE__, __FILE__, "La première ligne lue dans la recherche d'animations n'est pas une ligne définissant un statut.", fileName, str);
		}
		else if (str.at(3) == '=')
		{
			str2 = str.substr(0, 3);

			if (str2 == textFrom(WADTXT_IMG))
			{
				option = WADTXT_IMG;
				r_wadFile.write((char*) &option, sizeof(int));
				map<string, int>::iterator it = g_textures_map.find(str.substr(4));
				if (it == g_textures_map.end())
					throw PfException(__LINE__, __FILE__, "erreur dans la map de textures.");
				r_wadFile.write((char*) &(it->second), sizeof(int));
			}
			else if (str2 == textFrom(WADTXT_SND))
			{
				option = WADTXT_SND;
				r_wadFile.write((char*) &option, sizeof(int));
				map<string, int>::iterator it = g_sounds_map.find(str.substr(4));
				if (it == g_sounds_map.end())
					throw PfException(__LINE__, __FILE__, "erreur dans la map de sons.");
				r_wadFile.write((char*) &(it->second), sizeof(int));
			}
			else if (str2 == textFrom(WADTXT_VID))
			{
				// comptage du nombre de frames ajoutées, et classement des fichiers par ordre alphabétique
				str2 = str.substr(4);
				DIR* dir = opendir(str2.c_str());
				if (dir == 0)
					throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le dossier.", str2);
				struct dirent* file;
				count = 0;
				vidFrames_v.clear();
				while ((file=readdir(dir)) != 0)
				{
					str2 = string(file->d_name);
					if (str2.size() > 4 && str2.substr(str2.size()-4, 4) == ".png") // inclut donc != "." et ".."
					{
						count++;
						for (it=vidFrames_v.begin();it!=vidFrames_v.end();it++)
						{
							if (it->compare(str2) > 0)
							{
								vidFrames_v.insert(it, str2);
								break;
							}
						}
						if (it == vidFrames_v.end())
							vidFrames_v.push_back(str2);
					}
				}
				closedir(dir);
				option = WADTXT_VID;
				r_wadFile.write((char*) &option, sizeof(int));
				r_wadFile.write((char*) &count, sizeof(int));

				for (it=vidFrames_v.begin();it!=vidFrames_v.end();it++)
				{
					str2 = *it;

					// on insère une instruction IMG...

					option = WADTXT_IMG;
					r_wadFile.write((char*) &option, sizeof(int));
					str2 = str.substr(4) + ((str.at(str.size()-1)==DIR_SEPARATOR)?"":(string("")+DIR_SEPARATOR)) + str2;
					map<string, int>::iterator it2 = g_textures_map.find(str2);
					if (it2 == g_textures_map.end())
						throw PfException(__LINE__, __FILE__, "erreur dans la map de textures.");
					r_wadFile.write((char*) &(it2->second), sizeof(int));

					// ... puis la frame correspondante.

					addFrameToAnim("1;1;1;1", r_wadFile, fileName);
				}
			}
			else if (str2 == textFrom(WADTXT_MSC))
			{
				option = WADTXT_MSC;
				r_wadFile.write((char*) &option, sizeof(int));
				pos = 3;
				flags = 0;
				while (pos < (int) str.size() && pos != (int) string::npos)
				{
					str2 = str.substr(pos + 1, str.find_first_of(";", pos + 1) - pos - 1);
					pos = str.find_first_of(";", pos + 1);
					for (int k=0;k<ENUM_PF_MSC_VALUE_COUNT;k++)
					{
						if (str2 == textFrom((PfMscValue) k))
							flags |= enumtopfflag32(k);
					}
				}
				r_wadFile.write((char*) &flags, sizeof(int));

			}
			else if (str2 == textFrom(WADTXT_SLW))
			{
				option = WADTXT_SLW;
				r_wadFile.write((char*) &option, sizeof(int));
				str2 = str.substr(4);
				unsigned int val = MAX(1, atoi(str2.c_str()));
				r_wadFile.write((char*) &val, sizeof(unsigned int));
			}
			else if (str2 == textFrom(WADTXT_BOX))
            {
                option = WADTXT_BOX;
				r_wadFile.write((char*) &option, sizeof(int));
				str2 = str.substr(4, str.find_first_of(";")-4);
				PfBoxType boxType = BOX_TYPE_COLLISION;
				for (int i=0;i<ENUM_PF_BOX_TYPE_COUNT;i++)
                {
                    if (str2 == textFrom((PfBoxType) i))
                    {
                        boxType = (PfBoxType) i;
                        break;
                    }
                }
                r_wadFile.write((char*) &boxType, sizeof(PfBoxType));
                str2 = str.substr(str.find_first_of(";")+1);
                unsigned int val = atoi(str2.c_str());
                r_wadFile.write((char*) &val, sizeof(unsigned int));
            }
			else if (str2 != textFrom(WADTXT_FRM))
				throw ScriptException(__LINE__, __FILE__, "option de script non valide.", fileName, str);
			else // WADTXT_FRM
				addFrameToAnim(str.substr(4), r_wadFile, fileName);
		}
		else // frame sans indication WADTXT_FRM
			addFrameToAnim(str, r_wadFile, fileName);

		if (readAnims > 0)
			j++;
	}
}

void addFrameToAnim(const string& str, ofstream& r_wadFile, const string& fileName)
{
	PfWadScriptOption option = WADTXT_FRM;
	r_wadFile.write((char*) &option, sizeof(int));

	string str2;
	unsigned int pos = 0;
	char crd;

	for (int i=0;i<4;i++)
	{
		str2 = str.substr(pos, str.find_first_of(";#", pos) - pos);
		if (str2.size() > 2)
			throw ScriptException(__LINE__, __FILE__, string("frame non valide, la ") + itostr(i+1) + "° coordonnée est un nombre supérieur à 99 ou n'est pas renseignée.", fileName, str);
		pos += str2.size() + ((i == 3)?0:1); // dernière coordonnée : pas de ;
		crd = (char) atoi(str2.c_str());
		if (crd == 0)
			throw ScriptException(__LINE__, __FILE__, string("frame non valide, la ") + itostr(i+1) + "° coordonnée est manquante.", fileName, str);
		r_wadFile.write(&crd, sizeof(char));
	}

	while (pos < str.size())
	{
		if (str.at(pos) == '#')
		{
			str2 = str.substr(pos + 1, str.find('#', pos + 1) - (pos + 1));
			pos += str2.size() + 2;
			if (str2 == textFrom(WADTXT_SND))
			{
				option = WADTXT_SND;
				r_wadFile.write((char*) &option, sizeof(int));
			}
			else
				throw ScriptException(__LINE__, __FILE__, string("L'option ") + str2 + " n'est pas utilisable dans une frame.", fileName, str);
		}
		else
			pos++;
	}

	int end = END_OF_LIST_INT;
	r_wadFile.write((char*) &end, sizeof(int));
}

void convertTxtToWad(const string& fileName)
{
	try
	{
		// ouverture des fichiers
		ifstream ifs(string(string(SCRIPT_DIR)+DIR_SEPARATOR+fileName).c_str(), ios::binary);
		if (!ifs.is_open())
			throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier.", string(SCRIPT_DIR)+DIR_SEPARATOR+fileName);

		string outputName = string(OBJ_DIR) + DIR_SEPARATOR + fileName.substr(fileName.find_last_of(DIR_SEPARATOR)+1, fileName.find('.')-fileName.find_last_of(DIR_SEPARATOR)-1) + ".wad";

		// création du fichier
		ofstream ofs(outputName.c_str(), ios::binary);
		if (!ofs.is_open())
		{
			ifs.close();
			throw FileException(__LINE__, __FILE__, "impossible de créer le fichier.", outputName);
		}

		try
		{
			int i = PFGAME_VERSION;
			ofs.write((char*) &i, sizeof(int));
			addResToWad(ifs, ofs);
		}
		catch (PfException& e)
		{
			ifs.close();
			ofs.close();
			throw PfException(__LINE__, __FILE__, "erreur lors de l'écriture des ressources.", e);
		}

		try
		{
			addObjectsToWad(ifs, ofs, fileName);
			int end = END_OF_LIST_INT;
			ofs.write((char*) &end, sizeof(int));
		}
		catch (PfException& e)
		{
			ifs.close();
			ofs.close();
			throw PfException(__LINE__, __FILE__, "erreur lors de l'écriture des objets.", e);
		}

		if (ifs.fail())
		{
			ifs.close();
			ofs.close();
			throw ScriptException(__LINE__, __FILE__, "erreur lors de la conversion du TXT en WAD.", fileName, "Fin de fichier rencontrée prématurément.");
		}
		if (ofs.fail())
		{
			ifs.close();
			ofs.close();
			throw FileException(__LINE__, __FILE__, "erreur lors de l'écriture dans le fichier.", outputName);
		}

		cout << fileName << " ...OK" << endl;
		g_textures_map.clear();
		g_sounds_map.clear();

		ofs.close();
		ifs.close();
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Erreur lors de la conversion du fichier ") + fileName + ".", e);
	}
}
