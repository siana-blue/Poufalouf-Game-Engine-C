#include "launcher.h"

#include <dirent.h>
#include <fstream>
#include "datapackage.h"
#include "misc.h"
#include "errors.h"
#include "menumvcsystem.h"
#include "wad.h"
#include "enum.h"

vector<int> g_readCharsCount_v; //!< Utilisé par la fonction <em>readIntFrom4Chars</em>.
vector<int> g_readInt_v; //!< Utilisé par la fonction <em>readIntFrom4Chars</em>.
bool g_mustCopyByte = true; //!< Utilisé par la fonction <em>upgradeFile</em> pour savoir si l'octet copié doit être recopié dans la nouvelle version.
int g_intToCopy = INVALID_INT; //!< Utilisé par la fonction <em>upgradeFile</em> pour écrire une valeur entière si celle-ci est différente de INVALID_INT (fichier "gen.h").
int g_upgradeWadAfterAddedSlot_count = 0; //!< Utilisé par la fonction <em>upgradeWadAfterAddedSlot</em>.
int g_upgradeWadAfterAddedSlot_flag1 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeWadAfterAddedSlot</em>.
int g_upgradeWadAfterAddedSlot_flag2 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeWadAfterAddedSlot</em>.
int g_upgradeWadAfterAddedSlot_tmp1 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeWadAfterAddedSlot</em>.
int g_upgradeWadAfterAddedSlot_tmp2 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeWadAfterAddedSlot</em>.
int g_upgradeMenAfterAddedSlot_tmp1 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeMenAfterAddedSlot</em>.
int g_upgradeMenAfterAddedSlot_tmp2 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeMenAfterAddedSlot</em>.
int g_upgradeMapAfterAddedSlot_tmp1 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeMapAfterAddedSlot</em>.
int g_upgradeMapAfterAddedSlot_tmp2 = INVALID_INT; //!< Utilisé par la fonction <em>upgradeMapAfterAddedSlot</em>.
bool g_upgradeMapAfterAddedSlot_slot = false; //!< Utilisé par la fonction <em>upgradeMapAfterAddedSlot</em>.
vector<PfWadSlot> g_upgradeFileAfterAddedSlot_slots_v; //!< A initialiser avant d'utiliser la fonction <em>upgradeWadAfterAddedSlot</em>, <em>upgradeMenAfterAddedSlot</em> ou <em>upgradeMapAfterAddedSlot</em>.

#define UPGRADE_WAD_AFTER_ADDED_SLOT_INDEX 0 //!< Utilisé pour identifier la fonction <em>upgradeWadAfterAddedSlot</em> dans la fonction <em>readIntFrom4Chars</em>.
#define UPGRADE_MEN_AFTER_ADDED_SLOT_INDEX 1 //!< Utilisé pour identifier la fonction <em>upgradeMenAfterAddedSlot</em> dans la fonction <em>readIntFrom4Chars</em>.
#define UPGRADE_MAP_AFTER_ADDED_SLOT_INDEX 2 //!< Utilisé pour identifier la fonction <em>upgradeMapAfterAddedSlot</em> dans la fonction <em>readIntFrom4Chars</em>.

bool executeLauncher(const string& wadName, const string& menuName)
{
	vector<string> files_v = outdatedFiles();
	bool cnt = true;
	if (files_v.size() > 0)
	{
		PfWad* p_wad = new PfWad(wadName);
		MenuMVCSystem* p_launcherMVCSystem = new MenuMVCSystem(menuName, *p_wad);
		cnt = p_launcherMVCSystem->run();
		if (cnt)
		{
			string extStr;
			for (unsigned int i=0, size=files_v.size();i<size;i++)
			{
				p_launcherMVCSystem->clearTexts();
				p_launcherMVCSystem->clearValues();
				extStr = files_v[i].substr(files_v[i].find_last_of(".")+1);
				if (extStr == WAD_EXT)
					extStr = "WADS";
				else if (extStr == MENU_EXT)
					extStr = "MENUS";
				else if (extStr == MAPS_EXT)
					extStr = "MAPS";
                else if (extStr == TEXTURES_EXT)
					extStr = "TEXTURES";
				p_launcherMVCSystem->addText(extStr);
				p_launcherMVCSystem->addText(translate("Converting") + " " + files_v[i]);
				p_launcherMVCSystem->addValue(((float) i+FLOAT_MARGIN)/size*100);
				p_launcherMVCSystem->run(true);
				upgradeFile(files_v[i]);
			}
			// afficher le 100%
			p_launcherMVCSystem->clearTexts();
			p_launcherMVCSystem->clearValues();
			p_launcherMVCSystem->addValue(100.0 + FLOAT_MARGIN);
			p_launcherMVCSystem->run(true);
		}
		delete p_launcherMVCSystem;
		delete p_wad;
	}

	return cnt;
}

vector<string> outdatedFiles()
{
	vector<string> rtn_v;
	string str;
	string dirName, ext;
	int version;
	vector<string> dirNames_v, ext_v;
	dirNames_v.push_back(MAPS_DIR);
	dirNames_v.push_back(WAD_DIR);
	dirNames_v.push_back(MENU_DIR);
	dirNames_v.push_back(TEXTURES_DIR);
	ext_v.push_back(MAPS_EXT);
	ext_v.push_back(WAD_EXT);
	ext_v.push_back(MENU_EXT);
	ext_v.push_back(TEXTURES_EXT);

	for (int i=0;i<4;i++)
	{
		dirName = dirNames_v[i];
		ext = ext_v[i];

		DIR* dir = opendir(dirName.c_str());
		if (dir == 0)
			throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le répertoire.", dirName);
		struct dirent* file;
		while((file=readdir(dir)) != 0)
		{
			str = string(dirName) + file->d_name;
			if (str.size() > ext.size() && str.compare(str.size()-ext.size(), ext.size(), ext) == 0)
			{
				ifstream ifs(str.c_str(), ios::binary);
				if (!ifs.is_open())
				{
					closedir(dir);
					throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", str);
				}
				ifs.read((char*) &version, sizeof(int));
				if (version < PFGAME_VERSION)
					rtn_v.push_back(str);
				ifs.close();
			}
		}
		closedir(dir);
	}

	return rtn_v;
}

void upgradeFile(const string& fileName)
{
	ifstream ifs(fileName.c_str(), ios::binary);
	if (!ifs.is_open())
		throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", fileName);

	int version, tmp = PFGAME_VERSION;
	ifs.read((char*) &version, sizeof(int));
	ofstream ofs(TMP_FILE, ios::trunc | ios::binary);
	if (!ofs.is_open())
	{
		ifs.close();
		throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier en écriture.", TMP_FILE);
	}

	long tmpLength, fileLength = sizeof(int); // on ajoute déjà l'octet de version
	char c; //, tmpC;
	ifs.read(&c, sizeof(char));
	while (ifs.good())
	{
		ofs.write(&c, sizeof(char));
		ifs.read(&c, sizeof(char));
		fileLength++;
	}
	ofs.close();
	ifs.close();

	ifs.open(TMP_FILE, ios::binary);
	if (!ifs.is_open())
		throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier en lecture.", TMP_FILE);
	ofs.open(fileName.c_str(), ios::trunc | ios::binary);
	if (!ofs.is_open())
    {
        ifs.close();
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier en écriture.", fileName);
    }
	ofs.write((char*) &tmp, sizeof(int));

	string fileType = fileName.substr(fileName.find_last_of(".")+1);
	while (version < PFGAME_VERSION) // conversion version par version
	{
	    // Initialisations pour la version en cours de traitement
	    switch (version)
	    {
	        /*
	        Gardé pour exemple
	        -------------------
        case 0:
            g_upgradeFileAfterAddedSlot_slots_v.clear();
            g_upgradeFileAfterAddedSlot_slots_v.push_back((PfWadSlot) 2); // important de mettre la place du slot à l'époque de la version considérée
            g_upgradeFileAfterAddedSlot_slots_v.push_back((PfWadSlot) 2); // ici par exemple, en double car deux slots JUNGLE insérés au même endroit
            g_upgradeFileAfterAddedSlot_slots_v.push_back((PfWadSlot) 2); // ici pour WAD_GRASS
            break;
            */
        default:
            break;
	    }

	    int byteCount = sizeof(int); // entier de version en début de fichier déjà compté.

        tmpLength = sizeof(int);
		ifs.read(&c, sizeof(char)); // la version n'a pas été écrite dans ifs, on commence directement au début du fichier.
		while (ifs.good()) // conversion de l'ensemble du fichier dans la version supérieure
		{
			if (fileType == WAD_EXT)
			{
				switch (version)
				{
				    /*
                case 0: // de 0 vers 1
                    upgradeWadAfterAddedSlot(c, byteCount);
                    break;
                    */
                default:
                    break;
				}
			}
			else if (fileType == MENU_EXT)
            {
                switch (version)
				{
				    /*
                case 0: // de 0 vers 1
                    upgradeMenAfterAddedSlot(c, byteCount);
                    break;
                    */
                default:
                    break;
				}
            }
            else if (fileType == MAPS_EXT)
            {
                switch (version)
				{
                    /*
                case 0: // de 0 vers 1
                    upgradeMapAfterAddedSlot(c, byteCount);
                    break;
                case 1:
                    if (byteCount == fileLength - (int) sizeof(DataPackage::DataType))
                    {
                        tmpC = 0;
                        WRITE_CHAR(ofs, tmpC); // par défaut pas de décor de fond
                    }
                    break;
                    */
                default:
                    break;
				}
            }
			if (g_mustCopyByte)
            {
                ofs.write(&c, sizeof(char));
                tmpLength++;
            }
            g_mustCopyByte = true;
            if (g_intToCopy != INVALID_INT)
                ofs.write((char*) &g_intToCopy, sizeof(int));
            g_intToCopy = INVALID_INT;
			ifs.read(&c, sizeof(char));
			byteCount++;
		} // fin de la conversion vers la version supérieure
		fileLength = tmpLength;

		ifs.clear();
		ofs.clear();
		ifs.seekg(0, ios::beg);
		ofs.seekp(0, ios::beg);

		version++;
	} // la version est enfin égale à PFGAME_VERSION

	ofs.close();
	ifs.close();
}

void upgradeWadAfterAddedSlot(char c, int byteCount)
{
    // version du fichier + nombre de textures
    if (byteCount < (int) (2*sizeof(int)))
    {
        g_upgradeWadAfterAddedSlot_tmp1 = readIntFrom4Chars(c, UPGRADE_WAD_AFTER_ADDED_SLOT_INDEX); // nombre de fichiers textures
        if (g_upgradeWadAfterAddedSlot_tmp1 != INVALID_INT)
        {
            g_upgradeWadAfterAddedSlot_count = 1;
            g_upgradeWadAfterAddedSlot_flag1 = 3*sizeof(int); // + taille du premier fichier texture
            g_upgradeWadAfterAddedSlot_flag2 = INVALID_INT;
            g_upgradeWadAfterAddedSlot_tmp2 = INVALID_INT;
        }
        return;
    }
    // parcours des fichiers textures
    if (g_upgradeWadAfterAddedSlot_tmp1 > 0 && g_upgradeWadAfterAddedSlot_flag2 == INVALID_INT && byteCount < g_upgradeWadAfterAddedSlot_flag1)
    {
        if (g_upgradeWadAfterAddedSlot_tmp2 != INVALID_INT) // passages suivants
            return;
        g_upgradeWadAfterAddedSlot_tmp2 = readIntFrom4Chars(c, UPGRADE_WAD_AFTER_ADDED_SLOT_INDEX);
        if (g_upgradeWadAfterAddedSlot_tmp2 != INVALID_INT) // premier passage après lecture de la taille
            g_upgradeWadAfterAddedSlot_flag1 += g_upgradeWadAfterAddedSlot_tmp2;
        return;
    }
    if (g_upgradeWadAfterAddedSlot_flag2 == INVALID_INT && g_upgradeWadAfterAddedSlot_count < g_upgradeWadAfterAddedSlot_tmp1)
    {
        g_upgradeWadAfterAddedSlot_flag1 += sizeof(int); // pour repasser dans la boucle précédente et lire le fichier suivant
        g_upgradeWadAfterAddedSlot_tmp2 = readIntFrom4Chars(c, UPGRADE_WAD_AFTER_ADDED_SLOT_INDEX); // car sinon on commencerait au deuxième octet dans la boucle précédente
        g_upgradeWadAfterAddedSlot_count++;
        return;
    }
    // parcours des fichiers sons
    if (g_upgradeWadAfterAddedSlot_flag2 == INVALID_INT)
    {
        g_upgradeWadAfterAddedSlot_flag2 = g_upgradeWadAfterAddedSlot_flag1 + ((g_upgradeWadAfterAddedSlot_tmp1 > 0)?sizeof(int):0);
        g_upgradeWadAfterAddedSlot_tmp1 = INVALID_INT;
        g_upgradeWadAfterAddedSlot_tmp2 = INVALID_INT; // utilisé pour la suite aux slots mais pratique de l'initialiser ici
        g_upgradeWadAfterAddedSlot_count = 1;
    }
    if (byteCount < g_upgradeWadAfterAddedSlot_flag2)
    {
        if (g_upgradeWadAfterAddedSlot_tmp1 == INVALID_INT)
        {
            g_upgradeWadAfterAddedSlot_tmp1 = readIntFrom4Chars(c, UPGRADE_WAD_AFTER_ADDED_SLOT_INDEX);
            if (g_upgradeWadAfterAddedSlot_tmp1 != INVALID_INT)
                g_upgradeWadAfterAddedSlot_flag2++;
            return;
        }
        if (g_upgradeWadAfterAddedSlot_count <= g_upgradeWadAfterAddedSlot_tmp1)
        {
            if (c == '\0')
                g_upgradeWadAfterAddedSlot_count++;
            g_upgradeWadAfterAddedSlot_flag2++;
            return;
        }
    }
    // on en arrive aux slots
    g_upgradeWadAfterAddedSlot_tmp1 = readIntFrom4Chars(c, UPGRADE_WAD_AFTER_ADDED_SLOT_INDEX);
    if (g_upgradeWadAfterAddedSlot_tmp1 == NEW_SLOT_FLAG || g_upgradeWadAfterAddedSlot_tmp2 == NEW_SLOT_FLAG)
    {
        if (g_upgradeWadAfterAddedSlot_tmp2 == NEW_SLOT_FLAG)
        {
            g_mustCopyByte = false;
            if (g_upgradeWadAfterAddedSlot_tmp1 != INVALID_INT) // ici tmp1 est l'ancienne valeur de slot
            {
                int tmp = 0;
                for (unsigned int i=0, size=g_upgradeFileAfterAddedSlot_slots_v.size();i<size;i++)
                {
                    if (g_upgradeWadAfterAddedSlot_tmp1 >= g_upgradeFileAfterAddedSlot_slots_v[i])
                        tmp++;
                }
                g_intToCopy = g_upgradeWadAfterAddedSlot_tmp1 + tmp;
                g_upgradeWadAfterAddedSlot_tmp2 = INVALID_INT;
            }
        }
        else
            g_upgradeWadAfterAddedSlot_tmp2 = NEW_SLOT_FLAG;
    }
}

void upgradeMenAfterAddedSlot(char c, int byteCount)
{
    if (g_upgradeMenAfterAddedSlot_tmp2 != STRING_FLAG)
        g_upgradeMenAfterAddedSlot_tmp1 = readIntFrom4Chars(c, UPGRADE_MEN_AFTER_ADDED_SLOT_INDEX);
    if (g_upgradeMenAfterAddedSlot_tmp1 == NEW_SLOT_FLAG || g_upgradeMenAfterAddedSlot_tmp2 == NEW_SLOT_FLAG)
    {
        if (g_upgradeMenAfterAddedSlot_tmp2 == NEW_SLOT_FLAG)
        {
            g_mustCopyByte = false;
            if (g_upgradeMenAfterAddedSlot_tmp1 != INVALID_INT) // ici tmp1 est l'ancienne valeur de slot
            {
                int tmp = 0;
                for (unsigned int i=0, size=g_upgradeFileAfterAddedSlot_slots_v.size();i<size;i++)
                {
                    if (g_upgradeMenAfterAddedSlot_tmp1 >= g_upgradeFileAfterAddedSlot_slots_v[i])
                        tmp++;
                }
                g_intToCopy = g_upgradeMenAfterAddedSlot_tmp1 + tmp;
                g_upgradeMenAfterAddedSlot_tmp2 = INVALID_INT;
            }
        }
        else
            g_upgradeMenAfterAddedSlot_tmp2 = NEW_SLOT_FLAG;
        return;
    }
    if (g_upgradeMenAfterAddedSlot_tmp1 == STRING_FLAG || g_upgradeMenAfterAddedSlot_tmp2 == STRING_FLAG)
    {
        if (g_upgradeMenAfterAddedSlot_tmp2 != STRING_FLAG)
            g_upgradeMenAfterAddedSlot_tmp2 = STRING_FLAG;
        else if (c == '\0')
            g_upgradeMenAfterAddedSlot_tmp2 = INVALID_INT;
    }
}

void upgradeMapAfterAddedSlot(char c, int byteCount)
{
    if (g_upgradeMapAfterAddedSlot_tmp2 != STRING_FLAG)
        g_upgradeMapAfterAddedSlot_tmp1 = readIntFrom4Chars(c, UPGRADE_MAP_AFTER_ADDED_SLOT_INDEX);
    if (g_upgradeMapAfterAddedSlot_tmp1 == NEW_SLOT_FLAG || g_upgradeMapAfterAddedSlot_tmp2 == NEW_SLOT_FLAG)
    {
        if (g_upgradeMapAfterAddedSlot_tmp2 == NEW_SLOT_FLAG)
        {
            if (g_upgradeMapAfterAddedSlot_slot)
                g_mustCopyByte = false;
            if (g_upgradeMapAfterAddedSlot_tmp1 != INVALID_INT)
            {
                if (g_upgradeMapAfterAddedSlot_slot) // en fait comme c'est un DataPackage il y a un octet à sauter
                {
                    int tmp = 0;
                    for (unsigned int i=0, size=g_upgradeFileAfterAddedSlot_slots_v.size();i<size;i++)
                    {
                        if (g_upgradeMapAfterAddedSlot_tmp1 >= g_upgradeFileAfterAddedSlot_slots_v[i])
                            tmp++;
                    }
                    g_intToCopy = g_upgradeMapAfterAddedSlot_tmp1 + tmp;
                    g_upgradeMapAfterAddedSlot_slot = false;
                    g_upgradeMapAfterAddedSlot_tmp2 = INVALID_INT;
                }
                else
                    g_upgradeMapAfterAddedSlot_slot = true;
            }
        }
        else
            g_upgradeMapAfterAddedSlot_tmp2 = NEW_SLOT_FLAG;
        return;
    }
    if (g_upgradeMapAfterAddedSlot_tmp1 == STRING_FLAG || g_upgradeMapAfterAddedSlot_tmp2 == STRING_FLAG)
    {
        if (g_upgradeMapAfterAddedSlot_tmp2 != STRING_FLAG)
            g_upgradeMapAfterAddedSlot_tmp2 = STRING_FLAG;
        else if (c == '\0')
            g_upgradeMapAfterAddedSlot_tmp2 = INVALID_INT;
    }
}

int readIntFrom4Chars(char c, unsigned int index)
{
    for (unsigned int i=g_readCharsCount_v.size();i<=index;i++)
    {
        g_readCharsCount_v.push_back(0);
        g_readInt_v.push_back(0);
    }

    int shiftValue = g_readCharsCount_v[index] * 8;
    g_readInt_v[index] += (unsigned char) c << shiftValue;
    g_readCharsCount_v[index]++;

    if (g_readCharsCount_v[index] == 4)
    {
        g_readCharsCount_v[index] = 0;
        int rtn = g_readInt_v[index];
        g_readInt_v[index] = 0;
        return rtn;
    }
    return INVALID_INT;
}
