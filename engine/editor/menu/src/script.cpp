#include "script.h"

#include <cstdlib>
#include <iostream>
#include "enum.h"
#include "misc.h"
#include "errors.h"

void convertTxtToMen(const string& fileName)
{
	try
	{
		string str, str2, str3;

		// ouverture des fichiers
		ifstream ifs(string(string(SCRIPT_DIR)+DIR_SEPARATOR+fileName).c_str(), ios::binary);
		if (!ifs.is_open())
			throw FileException(__LINE__, __FILE__, "impossible d'ouvrir le fichier.", string(SCRIPT_DIR)+DIR_SEPARATOR+fileName);

		string outputName = string(OBJ_DIR) + DIR_SEPARATOR + fileName.substr(fileName.find_last_of(DIR_SEPARATOR)+1, fileName.find('.')-fileName.find_last_of(DIR_SEPARATOR)-1) + ".men";

		// création du fichier
		ofstream ofs(outputName.c_str(), ios::binary);
		if (!ofs.is_open())
		{
			ifs.close();
			throw FileException(__LINE__, __FILE__, "impossible de créer le fichier.", outputName);
		}

		// écriture du fichier

		bool ready = false;
		unsigned int pos, pos2, count;
		int value;
		float f;
		char c;
		int i, j;
		pfflag32 flags;

		i = PFGAME_VERSION;
		ofs.write((char*) &i, sizeof(int));

		while (ifs.good())
		{
			str = readString(ifs, END_OF_LINE);
			if (str == "")
			{
				ifs.clear();
				break; // ligne vide = fin de fichier
			}

			if (str.at(0) == '$') // section
			{
				str2 = str.substr(1);
				for (i=0;i<ENUM_PF_MENU_SCRIPT_SECTION_COUNT;i++)
				{
					if (str2 == textFrom((PfMenuScriptSection) i))
					{
						ofs.write((char*) &i, sizeof(int));
						ready = true;
						break;
					}
				}
				if (i == ENUM_PF_MENU_SCRIPT_SECTION_COUNT)
				{
					ifs.close();
					ofs.close();
					throw ScriptException(__LINE__, __FILE__, str2 + " ne définit pas une section.", fileName, str);
				}
				// comptage des lignes
				pos = ifs.tellg();
				count = 0;
				do // cet ordre de do while est mûrement réfléchi (attention à la fin de fichier en fin de ligne)
				{
					str = readString(ifs, END_OF_LINE);
					if (str != "")
						count++;
				} while (ifs.good() && str.at(0) != '$');
				if (ifs.good() && str.at(0) == '$')
					count--;
				ifs.clear();
				ifs.seekg(pos, ios::beg);
				ofs.write((char*) &count, sizeof(unsigned int));
			}
			else if (str.size() < 4)
                throw ScriptException(__LINE__, __FILE__, "Ligne incorrecte.", fileName, str);
			else if (str.at(3) == '=') // option
			{
				if (!ready)
				{
					ifs.close();
					ofs.close();
					throw ScriptException(__LINE__, __FILE__, "la première ligne ne définit pas de section.", fileName, str);
				}
				if (str.size() <= 4)
				{
					ifs.close();
					ofs.close();
					throw ScriptException(__LINE__, __FILE__, "l'option n'a pas de valeur.", fileName, str);
				}

				str2 = str.substr(0, 3);
				for (i=0;i<ENUM_PF_MENU_SCRIPT_OPTION_COUNT;i++)
				{
					if (str2 == textFrom((PfMenuScriptOption) i))
					{
						ofs.write((char*) &i, sizeof(int));
						str3 = str.substr(4);
						int tmp;
						switch ((PfMenuScriptOption) i)
						{
							case MENOPT_OBJ:
								for (j=0;j<ENUM_PF_WAD_SLOT_COUNT;j++)
								{
									if (str3 == textFrom((PfWadSlot) j))
									{
									    tmp = NEW_SLOT_FLAG;
									    ofs.write((char*) &tmp, sizeof(int));
										ofs.write((char*) &j, sizeof(int));
										break;
									}
								}
								if (j == ENUM_PF_WAD_SLOT_COUNT)
								{
									ifs.close();
									ofs.close();
									throw ScriptException(__LINE__, __FILE__, str3 + " n'est pas un slot de wad valide.", fileName, str);
								}
								break;
							case MENOPT_CRD:
								pos = 4;
								for (int j=0;j<4;j++)
								{
									str3 = str.substr(pos, str.find(';', pos) - ((j==3)?0:pos));
									f = atof(str3.c_str());
									pos += str3.size() + 1;
									ofs.write((char*) &f, sizeof(float));
								}
								break;
							case MENOPT_TXT:
							    tmp = STRING_FLAG;
							    ofs.write((char*) &tmp, sizeof(int));
								for (unsigned int j=4, size=str.size();j<size;j++)
								{
									c = str.at(j);
									ofs.write(&c, sizeof(char));
								}
								c = '\0';
								ofs.write(&c, sizeof(char));
								break;
							case MENOPT_LYT:
								pos = str.find('(');
								str3 = str.substr(4, pos);
								value = atoi(str3.c_str());
								if (value <= 0)
								{
									ifs.close();
									ofs.close();
									throw ScriptException(__LINE__, __FILE__, "id négatif ou nul interdit.", fileName, str);
								}
								ofs.write((char*) &value, sizeof(int));
								str3 = str.substr(pos + 1, str.find(';') - pos - 1);
								value = atoi(str3.c_str());
								if (value < 0)
								{
									ifs.close();
									ofs.close();
									throw ScriptException(__LINE__, __FILE__, "ligne de layout négative.", fileName, str);
								}
								ofs.write((char*) &value, sizeof(int));
								str3 = str.substr(str.find(';') + 1, str.find(')') - str.find(';') - 1);
								value = atoi(str3.c_str());
								if (value < 0)
								{
									ifs.close();
									ofs.close();
									throw ScriptException(__LINE__, __FILE__, "colonne de layout négative.", fileName, str);
								}
								ofs.write((char*) &value, sizeof(int));
								break;
							case MENOPT_EFT:
								pos = 3;
								flags = 0;
								while (pos < str.size() && pos != (unsigned int) string::npos)
								{
									str3 = str.substr(pos + 1, str.find_first_of(";", pos + 1) - pos - 1);
									pos = str.find_first_of(";", pos + 1);
									for (int k=0;k<ENUM_PF_EFFECT_COUNT;k++)
									{
										pos2 = str3.find_first_of(":");
										if (str3.substr(0, pos2) == textFrom((PfEffect) enumtopfflag32(k)))
										{
											flags |= enumtopfflag32(k);
											if (k >= 8 && pos2 != (unsigned int) string::npos)
												flags |= atoi(str3.substr(pos2+1).c_str());
										}
									}
								}
								ofs.write((char*) &flags, sizeof(int));
								break;
							case MENOPT_VAL:
								pos = 4;
								for (int j=0;j<4;j++)
								{
									str3 = str.substr(pos, str.find(';', pos) - ((j==3)?0:pos));
									if (j < 2)
									{
										value = atoi(str3.c_str());
										pos += str3.size() + 1;
										ofs.write((char*) &value, sizeof(int));
									}
									else
									{
										f = atof(str3.c_str());
										pos += str3.size() + 1;
										ofs.write((char*) &f, sizeof(float));
									}
								}
								break;
							case MENOPT_DYN:
								value = atoi(str.substr(4).c_str());
								ofs.write((char*) &value, sizeof(int));
								break;
						}
						break;
					}
				}
				if (i == ENUM_PF_MENU_SCRIPT_OPTION_COUNT)
				{
					ifs.close();
					ofs.close();
					throw ScriptException(__LINE__, __FILE__, str2 + " n'est pas une option valide.", fileName, str);
				}
			}
			else
			{
				ifs.close();
				ofs.close();
				throw ScriptException(__LINE__, __FILE__, "cette ligne n'est ni un début de section ni une option.", fileName, str);
			}
		}

		i = END_OF_LIST_INT;
		ofs.write((char*) &i, sizeof(int));

		// fin de fonction

		if (ifs.fail())
        {
            ifs.close();
            ofs.close();
            throw ScriptException(__LINE__, __FILE__, "erreur lors de la conversion du TXT en MEN.", fileName, "Fin de fichier rencontrée prématurément.");
        }
        if (ofs.fail())
        {
            ifs.close();
            ofs.close();
            throw FileException(__LINE__, __FILE__, "erreur lors de l'écriture du fichier MEN.", outputName);
        }

		ifs.close();
		ofs.close();

		cout << fileName << " ...OK" << endl;
	}
	catch (PfException& e)
	{
		throw PfException(__LINE__, __FILE__, "erreur lors de la conversion du script en fichier MEN.", e);
	}
}
