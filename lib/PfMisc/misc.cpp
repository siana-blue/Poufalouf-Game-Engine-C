#include "misc.h"

#include <sstream>
#include <dirent.h>

string ConfigSettings::s_language = DEFAULT_LANGUAGE;

void readConfig()
{
    ifstream configFile(CONFIG_FILE);

    if (!configFile.is_open())
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", CONFIG_FILE);

    string str, str1, str2;
    while (configFile.good())
    {
        getline(configFile, str);
        size_t pos = str.find_first_of('=');
        if (pos == str.length()-1 || pos == string::npos)
            throw ScriptException(__LINE__, __FILE__, "Ligne non valide, manque '=' ou une valeur associée.", CONFIG_FILE, str);
        str1 = str.substr(0, pos);
        str2 = str.substr(pos+1);
        if (str1 == "LANGUAGE")
            ConfigSettings::s_language = str2;
        else
            throw ScriptException(__LINE__, __FILE__, string("Ligne non valide, le paramètre ") + str1 + " n'est pas pris en charge.", CONFIG_FILE, str);
    }

    configFile.close();
}

vector<string> readScript(const string& scriptName, const string& language, const string& readingMode)
{
    vector<string> x_lines_v;
    string fileName = scriptName + (language.empty()?"":"_" + language) + "." + SCRIPT_EXT;
    ifstream scriptFile(fileName.c_str());

    if (!scriptFile.is_open())
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", fileName);

    string str;
    size_t pos = readingMode.find_first_of(' ');

    if (readingMode.substr(0, pos) == "titles") // retourne les titres de toutes les sections
    {
        // si un argument suit "titles", seules les sections dont la première ligne contient le texte suivant "titles" retournent leurs titres.
        string firstLine = (pos == string::npos || pos == readingMode.length()-1)?"":readingMode.substr(pos+1); // première ligne de la section à tester
        string firstLineTest; // stockera le contenu de la première ligne des sections
        while (scriptFile.good())
        {
            getline(scriptFile, str);
            if (!str.empty() && str.at(0) == '[') // titre de section ?
            {
                getline(scriptFile, firstLineTest); // lecture de la première ligne de la section
                if (firstLine == "" || firstLine == firstLineTest) // aucun test requis ou condition remplie ?
                {
                    str = str.substr(1, str.find_first_of(']')-1); // recopie du titre sans crochets
                    x_lines_v.push_back(str);
                }
            }
        }
    }
    else if (readingMode.substr(0, pos) == "section") // retourne le contenu d'une section
    {
        bool found = false;
        if (pos == string::npos || pos == readingMode.length()-1) // aucune section spécifiée en argument après "section" ?
        {
            scriptFile.close();
            throw ArgumentException(__LINE__, __FILE__, "Mode de lecture \"section\" et aucune section spécifiée.", "readingMode", "readScript");
        }
        while (scriptFile.good())
        {
            getline(scriptFile, str);
            if (!str.empty() && str.at(0) == '[') // titre de section ?
            {
                if (str.substr(1, str.find_first_of(']')-1) == readingMode.substr(pos+1)) // est-ce la bonne section ?
                {
                    getline(scriptFile, str);
                    while (scriptFile.good() && (str.empty() || str.at(0) != '[')) // lecture jusqu'à la prochaine section
                    {
                        x_lines_v.push_back(str);
                        getline(scriptFile, str);
                    }
                    found = true;
                }
            }
        }
        if (!found)
        {
            scriptFile.close();
            throw FileException(__LINE__, __FILE__, "Aucune section nommée " + readingMode.substr(pos+1) + " dans ce script.", fileName);
        }
    }
    else // par défaut, lit tout le fichier
    {
        while (scriptFile.good())
        {
            getline(scriptFile, str);
            x_lines_v.push_back(str);
        }
    }

    scriptFile.close();

    return x_lines_v;
}

string readLineAtSection(const string& scriptName, const string& sectionName, const string& language)
{
    string fileName = scriptName + (language.empty()?"":"_" + language) + "." + SCRIPT_EXT;
    ifstream scriptFile(fileName.c_str());

    if (!scriptFile.is_open())
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", fileName);

    string x_str;
    bool found = false;
    while (scriptFile.good())
    {
        getline(scriptFile, x_str);
        if (!x_str.empty() && x_str.at(0) == '[') // titre de section ?
        {
            if (x_str.substr(1, x_str.find_first_of(']')-1) == sectionName) // est-ce la bonne section ?
            {
                getline(scriptFile, x_str);
                found = true;
                break;
            }
        }
    }

    scriptFile.close();

    if (!found)
        throw FileException(__LINE__, __FILE__, "Aucune section nommée " + sectionName + " dans ce script.", fileName);

    return x_str;
}

string translate(const string& s, const string& language)
{
    if (language == DEFAULT_LANGUAGE)
        return s;
    if (s.size() == 0)
        return s;

    string x_str, str2;
    string fileName = string(TRANSLATION_SCRIPT_NAME) + "_" + language + "." + SCRIPT_EXT;
    ifstream scriptFile(fileName.c_str());

    if (!scriptFile.is_open())
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", fileName);

    bool found = false;
    while (scriptFile.good())
    {
        getline(scriptFile, x_str);
        size_t pos = x_str.find_first_of('=');
        if (pos == string::npos || pos == x_str.length()-1)
            continue;
        // ce mic-mac n'est plus trop nécessaire me semble-t-il, mais je me souviens du bug qui avait été résolu comme cela, alors je laisse :
        str2 = (s.at(s.size()-1) == '\0')?s.substr(0, s.size()-1):s; // retire '\0' pour pouvoir comparer avec la chaîne extraite ci-dessous, donc sans '\0'
        if (x_str.substr(0, pos) == str2)
        {
            x_str = x_str.substr(pos+1);
            found = true;
            break;
        }
    }
    if (!found)
        x_str = s;

    scriptFile.close();

    return x_str;
}

vector<string> translate(const vector<string>& stringList, const string& language)
{
    if (language == DEFAULT_LANGUAGE)
        return stringList;

    vector<string> x_list;
    string fileName = string(TRANSLATION_SCRIPT_NAME) + "_" + language + "." + SCRIPT_EXT;
    ifstream scriptFile(fileName.c_str());

    if (!scriptFile.is_open())
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le fichier.", fileName);

    string str, str2;
    for (unsigned int i=0; i<stringList.size(); i++)
    {
        bool found = false;
        if (stringList[i].size() > 0)
        {
            while (scriptFile.good())
            {
                getline(scriptFile, str);
                size_t pos = str.find_first_of('=');
                if (pos == string::npos || pos == str.length()-1)
                    continue;
                // ce mic-mac n'est plus trop nécessaire me semble-t-il, mais je me souviens du bug qui avait été résolu comme cela, alors je laisse :
                // retire '\0' pour pouvoir comparer avec la chaîne extraite ci-dessous, donc sans '\0'
                str2 = (stringList[i].at(stringList[i].size()-1) == '\0')?stringList[i].substr(0, stringList[i].size()-1):stringList[i];
                if (str.substr(0, pos) == str2)
                {
                    x_list.push_back(str.substr(pos+1));
                    found = true;
                    break;
                }
            }
        }
        if (!found)
            x_list.push_back(stringList[i]);
        scriptFile.clear();
        scriptFile.seekg(0, ios_base::beg);
    }

    scriptFile.close();

    return x_list;
}

vector<string> filesInDir(const string& dirName, const string& ext, const string& prefix, bool recursive)
{
    vector<string> x_v;
    string str;

    DIR* dir = opendir(dirName.c_str());
    if (dir == 0)
        throw FileException(__LINE__, __FILE__, "Impossible d'ouvrir le répertoire.", dirName);
    struct dirent* file;
    vector<string> recFiles_v;
    while ((file=readdir(dir)) != 0)
    {
        str = file->d_name;
        if (str == "." || str == "..")
            continue;
        if (prefix != "" && str.substr(0, prefix.size()) == prefix)
            continue;
        if (str.find(".") == string::npos)
        {
            if (recursive)
            {
                recFiles_v = filesInDir(dirName+"\\"+str, ext, prefix, true);
                for (unsigned int i=0, size=recFiles_v.size(); i<size; i++)
                    x_v.push_back(str + "\\" + recFiles_v[i]);
            }
        }
        else if (ext.empty() || (str.size() > ext.size() && str.compare(str.size()-ext.size(), ext.size(), ext) == 0))
            x_v.push_back(str);
    }
    closedir(dir);

    return x_v;
}

string itostr(int i)
{
    ostringstream oss;
    oss << i;
    return oss.str();
}

string ftostr(float f)
{
    ostringstream oss;
    oss << f;
    return oss.str();
}

pfflag enumtopfflag(int i)
{
    char x = 2;

    if (i <= 0 || i > 7)
        return 0;
    if (i == 1)
        return 1;

    for (int j=2; j<i; j++)
        x *= 2;

    return x;
}

pfflag32 enumtopfflag32(int i)
{
    int x = 2;

    if (i <= 0 || i > 31)
        return 0;
    if (i == 1)
        return 1;

    for (int j=2; j<i; j++)
        x *= 2;

    return x;
}

unsigned char unicodeToAscii(unsigned int code)
{
    if (code <= 127)
        return (unsigned char) code;
    else
    {
        switch (code)
        {
        case 162:
            return 155;
        case 176:
            return 167;
        case 178:
            return 166;
        case 191:
            return 168;
        case 196:
            return 142;
        case 197:
            return 143;
        case 198:
            return 146;
        case 199:
            return 128;
        case 201:
            return 144;
        case 209:
            return 165;
        case 214:
            return 153;
        case 216:
            return 237;
        case 220:
            return 154;
        case 224:
            return 133;
        case 225:
            return 160;
        case 226:
            return 131;
        case 228:
            return 132;
        case 229:
            return 134;
        case 230:
            return 145;
        case 231:
            return 135;
        case 232:
            return 138;
        case 233:
            return 130;
        case 234:
            return 136;
        case 235:
            return 137;
        case 236:
            return 141;
        case 237:
            return 161;
        case 238:
            return 140;
        case 239:
            return 139;
        case 241:
            return 164;
        case 242:
            return 149;
        case 243:
            return 162;
        case 244:
            return 147;
        case 246:
            return 148;
        case 249:
            return 151;
        case 250:
            return 163;
        case 251:
            return 150;
        case 252:
            return 129;
        case 255:
            return 152;
        default:
            return 0;
        }
    }

    return 0;
}

vector<unsigned char> unicodeToAscii(const string& rc_text)
{
    vector<unsigned char> x_v;

    for (unsigned int i=0, size=rc_text.size();i<size;i++)
        x_v.push_back(unicodeToAscii(rc_text.at(i)));

    return x_v;
}

void writeString(ofstream& r_ofs, const string& str)
{
    char c;
    for (unsigned int i=0, size=str.size(); i<size; i++)
    {
        c = str.at(i);
        r_ofs.write(&c, sizeof(char));
    }
    c = '\0';
    r_ofs.write(&c, sizeof(char));
}

string readString(ifstream& r_ifs, char delim)
{
    char c;
    string x_str;
    do
    {
        r_ifs.read(&c, sizeof(char));
        if (c != '\0' && c != delim)
            x_str.append(1, c);
    }
    while (r_ifs.good() && c != '\0' && c != delim);

    return x_str;
}

string readString(ifstream& r_ifs, const string& delim)
{
    char c;
    string x_str, dStr;
    int index = 0;
    do
    {
        r_ifs.read(&c, sizeof(char));
        if (r_ifs.fail())
            break;
        dStr.append(1, c);
        if (c != '\0' && dStr != delim)
        {
            x_str.append(1, c);
            if (dStr.size() > delim.size() || dStr[index] != delim[index]) // première condition juste pour éviter une segfault en cas de delim = "".
            {
                dStr.clear();
                index = 0;
            }
            else
                index++;
        }
        else if (dStr == delim)
            x_str = x_str.substr(0, x_str.size()-dStr.size()+1);
    }
    while (r_ifs.good() && c != '\0' && dStr != delim);

    return x_str;
}

void writeBinaryFile(ofstream& r_ofs, ifstream& r_ifs)
{
    unsigned int size;
    char c;

    r_ifs.seekg(0, ios::end);
    size = r_ifs.tellg();
    r_ofs.write((char*) &size, sizeof(unsigned int));
    r_ifs.seekg(0, ios::beg);
    r_ifs.read(&c, sizeof(char));

    while (r_ifs.good())
    {
        r_ofs.write(&c, sizeof(char));
        r_ifs.read(&c, sizeof(char));
    }
}

