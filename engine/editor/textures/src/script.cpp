#include "script.h"

#include <cstdlib>
#include <map>
#include <vector>
#include <dirent.h>
#include <iostream>
#include "enum.h"
#include "misc.h"
#include "errors.h"

void convertTxtToTex(const string& fileName)
{
	try
	{
		// ouverture des fichiers

		ifstream ifs(string(string(SCRIPT_DIR)+DIR_SEPARATOR+fileName).c_str(), ios::binary);
		if (!ifs.is_open())
			throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier.", string(SCRIPT_DIR)+DIR_SEPARATOR+fileName);

		string outputName = string(OBJ_DIR) + DIR_SEPARATOR + fileName.substr(fileName.find_last_of(DIR_SEPARATOR)+1, fileName.find('.')-fileName.find_last_of(DIR_SEPARATOR)-1) + ".tex";

		// création du fichier

		ofstream ofs(outputName.c_str(), ios::binary);
		if (!ofs.is_open())
		{
			ifs.close();
			throw FileException(__LINE__, __FILE__, "impossible de créer le fichier.", outputName);
		}

		string str, str2;
		int val = PFGAME_VERSION;
		ofs.write((char*) &val, sizeof(int));

		// fichiers images des terrains

		bool sectionFound = false;
		int counter = 0;
		vector<string> files_v;
		while (!sectionFound && ifs.good())
		{
			str = readString(ifs, END_OF_LINE);
			if (str.empty())
				continue;
			if (str.at(0) == '$')
				sectionFound = true;
			else
			{
				files_v.push_back(str);
				counter++;
			}
		}

		if (ifs.fail())
		{
			ifs.close();
			ofs.close();
			throw ScriptException(__LINE__, __FILE__, "erreur lors de la conversion du TXT en TEX.", fileName, "Fin de fichier rencontrée prématurément.");
		}

		ofs.write((char*) &counter, sizeof(int));
		for (unsigned int i=0, size=files_v.size();i<size;i++)
		{
			ifstream terrainFile(files_v[i].c_str(), ios::binary);
			if (!terrainFile.is_open())
			{
				ifs.close();
				ofs.close();
				throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier terrain.", files_v[i]);
			}
			writeBinaryFile(ofs, terrainFile);
			terrainFile.close();
		}

		// liens entre fichiers

		vector<int> values_v;
		counter = 0;
		int section = -1;
		while (ifs.good())
		{
			if (str.empty())
			{
				str = readString(ifs, END_OF_LINE);
				continue;
			}

			if (str.at(0) == '$')
			{
				if (counter > 0)
				{
					ofs.write((char*) &counter, sizeof(int));
					for (unsigned int i=0, size=values_v.size();i<size;i++)
						ofs.write((char*) &values_v[i], sizeof(int));
					values_v.clear();
					section = -1;
				}

				counter = 0;
				str2 = str.substr(1);
				for (int i=0;i<ENUM_PF_TEXTURE_SCRIPT_SECTION_COUNT;i++)
				{
					if (str2 == textFrom((PfTextureScriptSection) i))
					{
						ofs.write((char*) &i, sizeof(int));
						section = i;
						break;
					}
				}
			}
			else
			{
				values_v.push_back(atoi(str.substr(0, str.find_first_of(":")).c_str()));
				values_v.push_back(atoi(str.substr(str.find_first_of(":")+1, str.find_first_of(";")).c_str()));
				values_v.push_back(atoi(str.substr(str.find_first_of(";")+1, str.find_first_of("#")).c_str()));
				if (section == TEXSCT_SPR)
					values_v.push_back(atoi(str.substr(str.find_first_of("#")+1).c_str()));
				counter++;
			}

			str = readString(ifs, END_OF_LINE);
		}

		if (counter > 0) // pour le dernier hors boucle
		{
			ofs.write((char*) &counter, sizeof(int));
			for (unsigned int i=0, size=values_v.size();i<size;i++)
				ofs.write((char*) &values_v[i], sizeof(int));
		}

		val = END_OF_LIST_INT;
		ofs.write((char*) &val, sizeof(int));

		if (ofs.fail())
		{
			ifs.close();
			ofs.close();
			throw FileException(__LINE__, __FILE__, "erreur lors de l'écriture dans le fichier.", str);
		}

		ofs.close();
		ifs.close();

		cout << fileName << " ...OK" << endl;
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, string("Erreur lors de la conversion du fichier ") + fileName + ".", e);
	}
}
