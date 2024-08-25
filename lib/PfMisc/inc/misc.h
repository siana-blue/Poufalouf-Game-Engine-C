/**
* @file
* @author Anaïs Vernet
* @brief Fonctions d'utilité générale.
* @date xx/xx/xxxx
* @version 0.0.0
*
* La première version de ce fichier fut rédigée il y a fort longtemps, du temps peut-être où Rézipafs et Poufaloufs ne formaient qu'un seul et même peuple.
* Personne ne le sait vraiment.
*
* Les fonctions de lecture de scripts présentes ici, notamment <em>readScript</em>, ne sont plus utilisées (pour l'instant !). Elles servaient
* à la lecture de l'histoire interactive du jeu console de Poufalouf contre les Poulpes Cosmiques.
* Cependant, elles sont conservées car il est possible que leur utilisation redevienne nécessaire un jour.
*/

#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include "misc_gen.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "errors.h"

/**
* @brief Cette structure rassemble tous les paramètres de configuration générale du programme.
*
* Le langage utilisé pour la lecture des scripts est un groupe de deux lettres, pour l'instant "en" ou "fr".
*
* Les membres de cette structure sont statiques, et initialisés dans le fichier "misc.cpp" aux valeurs suivantes :
* <ul><li>s_language = DEFAULT_LANGUAGE (fichier "misc_gen.h")</li></ul>
*/
struct ConfigSettings
{
    static string s_language; //!< Le langage utilisé pour la lecture des scripts.
};

// Fonctions de gestion des scripts et fichiers

/**
* @brief Lit le fichier CONFIG_FILE (fichier "misc_gen.h") pour initialiser les membres de la structure ConfigSettings.
* @throw FileException si le fichier CONFIG_FILE ne peut pas être ouvert.
* @throw ScriptException si une ligne du fichier de configuration n'est pas valide.
*
* Le fichier CONFIG_FILE est composé de lignes de la forme [PARAMETRE]=[VALEUR]. Les paramètres valides sont :
* <ul><li>LANGUAGE=(en, fr...)</li></ul>
*/
void readConfig();

/**
* @brief Lit le contenu d'un script.
* @param scriptName Le nom du fichier script, sans "_[<em>language</em>]" et sans l'extension de fichier.
* @param language Le langage du script à lire.
* @param readingMode Le mode de lecture du script.
* @return Un vecteur contenant les lignes lues.
* @throw FileException si le fichier ne peut pas être ouvert.
* @throw FileException si la section spécifiée n'est pas trouvée, dans le cadre d'un mode de lecture par section.
* @throw ArgumentException si le mode de lecture déclaré est "section" sans titre de section spécifié.
*
* Cette fonction ouvre le fichier nommé [<em>scriptName</em>].[SCRIPT_EXT] ou [<em>scriptName</em>]_[<em>language</em>].[SCRIPT_EXT] si un langage est spécifié,
* SCRIPT_EXT étant défini dans le fichier "misc_gen.h".
*
* Les modes de lecture disponibles sont :
* <ul><li>"" : tout le fichier est lu.</li>
* <li>"titles" : retourne les titres de toutes les sections, sans les crochets.</li>
* <li>"titles [ligne à tester]" : retourne les titres de toutes les sections, sans crochets, dont la première ligne est égale à [ligne à tester].</li>
* <li>"section [titre de section]" : retourne le contenu de la section spécifiée sans son titre.</li></ul>
*
* Si le mode de lecture n'est pas valide, tout le fichier est lu.
*
* @warning
* De manière générale, éviter deux lignes de sections successives dans les scripts (section vide enchaînant directement sur une autre section)
* sous peine d'omission de section dans certains cas.
*/
vector<string> readScript(const string& scriptName, const string& language = ConfigSettings::s_language, const string& readingMode = "");

/**
* @brief Lit la première ligne d'une section d'un script.
* @param scriptName Le nom du fichier script, sans "_[<em>language</em>]" et sans l'extension de fichier.
* @param sectionName Le titre de la section à lire.
* @param language Le langage du script à lire.
* @return La ligne lue.
* @throw FileException si le fichier ne peut pas être ouvert.
* @throw FileException si la section spécifiée n'est pas trouvée.
*
* Cette fonction ouvre le fichier nommé [<em>scriptName</em>].[SCRIPT_EXT] ou [<em>scriptName</em>]_[<em>language</em>].[SCRIPT_EXT] si un langage est spécifié,
* SCRIPT_EXT étant défini dans le fichier "misc_gen.h".
*/
string readLineAtSection(const string& scriptName, const string& sectionName, const string& language = ConfigSettings::s_language);

/**
* @brief Traduit une chaîne de caractère dans la langue spécifiée au moyen d'un script de traduction.
* @param s La chaîne à traduire, exprimée dans le DEFAULT_LANGUAGE (fichier "misc_gen.h").
* @param language Le langage de traduction.
* @return La chaîne traduite.
* @throw FileException si le fichier de traduction ne peut pas être ouvert.
*
* Le fichier de traduction utilisé est [TRANSLATION_SCRIPT_NAME]_[<em>language</em>].[SCRIPT_EXT],
* TRANSLATION_SCRIPT_NAME et SCRIPT_EXT étant définis dans le fichier "misc_gen.h".
*
* La chaîne <em>s</em> est cherchée dans le fichier de traduction, à gauche des signes '='. Dès qu'elle est trouvée, le mot à droite du signe
* '=' est retourné. Si la chaîne n'est pas trouvée, elle n'est pas traduite et est retournée telle quelle.
*/
string translate(const string& s, const string& language = ConfigSettings::s_language);

/**
* @brief Traduit l'ensemble des chaînes de caractères contenues dans le vecteur passé en paramètre.
* @param stringList La liste des chaînes à traduire, exprimées dans le DEFAULT_LANGUAGE (fichier "misc_gen.h").
* @param language Le langage de traduction.
* @return La liste des chaînes traduites.
* @throw FileException si le fichier de traduction ne peut pas être ouvert.
*
* Le fichier de traduction utilisé est [TRANSLATION_SCRIPT_NAME]_[<em>language</em>].[SCRIPT_EXT],
* TRANSLATION_SCRIPT_NAME et SCRIPT_EXT étant définis dans le fichier "misc_gen.h".
*
* Les chaînes sont cherchées dans le fichier de traduction comme dans la fonction précédente.
*/
vector<string> translate(const vector<string>& stringList, const string& language = ConfigSettings::s_language);

/**
* @brief Retourne la liste des noms de fichiers dans un répertoire.
* @param dirName Le nom du répertoire.
* @param ext L'extension des fichiers à retourner, si "", alors tout fichier est retourné.
* @param prefix Le préfixe des fichiers qu'il faut ignorer, "" pour tout conserver.
* @param recursive Vrai si le contenu des sous-répertoires doit également être retourné.
* @return La liste de noms de fichiers.
* @throw FileException si le dossier ne peut pas être ouvert.
*
* "." et ".." sont omis.
*
* L'argument <em>ext</em> peut être fourni sans le point, par exemple, "txt" ou "wad". (en réalité peu importe, avec ou sans le ".")
*
* Les noms de fichiers retournés sont sans le chemin complet.
* Dans le cas d'une recherche récursive, les fichiers des sous-répertoires sont retournés avec le chemin relatif.
*
* @remarks
* Même dans le cas non récursif, les répertoires ne sont pas retournés.
*
* @warning
* Pour déterminer si un fichier est un répertoire ou non, dans le cas d'une recherche récursive, le test est simplement
* fait sur la présence d'un "." dans le nom. C'est très Windows, je sais, mais c'est comme ça pour l'instant,
* donc pas de fichier sans extension, please.
*/
vector<string> filesInDir(const string& dirName, const string& ext = "", const string& prefix = "", bool recursive = false);

// Fonctions de conversion

/**
* @brief Convertit un entier en chaîne de caractères.
* @param i L'entier à convertir.
* @return La chaîne de caractères créée.
*/
string itostr(int i);

/**
* @brief Convertit un nombre flottant en chaîne de caractères.
* @param f Le nombre flottant à convertir.
* @return La chaîne de caractères créée.
*/
string ftostr(float f);

/**
* @brief Convertit une énumération à 8 valeurs max en pfflag.
* @param i Un entier compris entre 0 et 7.
* @return Le flag correspondant (0 pour 0 et 128 pour 7).
*
* Si i n'est pas compris entre 0 et 7, alors 0 est retourné.
*/
pfflag enumtopfflag(int i);

/**
* @brief Convertit une énumération à 32 valeurs max en pfflag32.
* @param i Un entier compris entre 0 et 31.
* @return Le flag correspondant (0 pour 0 et 2 147 483 648 pour 31).
*
* Si i n'est pas compris entre 0 et 31, alors 0 est retourné.
*/
pfflag32 enumtopfflag32(int i);

/**
* @brief Convertit un code unicode en ASCII.
* @param code Le code unicode.
* @return Le code ASCII, ou 0 par défaut.
*/
unsigned char unicodeToAscii(unsigned int code);

/**
* @brief Convertit une chaîne de caractères unicode en ASCII.
* @param rc_text Le texte à convertir.
* @return Le texte converti, sous forme d'un vecteur d'unsigned char.
*/
vector<unsigned char> unicodeToAscii(const string& rc_text);

// Fonctions de gestion des flux

/**
* @brief Ecrit une chaîne de caractères dans un flux en écriture, en incluant '\0'.
* @param r_ofs Le flux en écriture, prépositionné.
* @param str La chaîne de caractère à écrire.
*
* @remarks
* Le flux n'est pas rembobiné en fin de fonction.
*/
void writeString(ofstream& r_ofs, const string& str);

/**
* @brief Lit une chaîne de caractères dans un flux en lecture, jusqu'à trouver le premier '\0'.
* @param r_ifs Le flux en lecture, prépositionné.
* @param delim Un caractère de fin de chaîne servant de délimitation, en plus de '\0'.
* @return La chaîne de caractères.
*
* Dès que '\0' ou <em>delim</em> est rencontré, la chaîne de caractère est retournée.
*
* @remarks
* Le flux n'est pas rembobiné en fin de fonction.
*/
string readString(ifstream& r_ifs, char delim = '\0');

/**
* @brief Lit une chaîne de caractères dans un flux en lecture, jusqu'à trouver le premier '\0'.
* @param r_ifs Le flux en lecture, prépositionné.
* @param delim Une chaîne de caractères de fin de ligne servant de délimitation, en plus de '\0'.
* @return La chaîne de caractères.
*
* Si une erreur de lecture du flux <em>r_ifs</em>, la chaîne est retournée en son état avant erreur (donc potentiellement partielle).
*
* Dès que '\0' ou <em>delim</em> est rencontré, la chaîne de caractère est retournée (délimiteur non inclus).
*
* @remarks
* Le flux n'est pas rembobiné en fin de fonction.
*/
string readString(ifstream& r_ifs, const string& delim);

/**
* @brief Copie le contenu d'un flux dans un autre.
* @param r_ofs Le flux en écriture.
* @param r_ifs Le flux en lecture.
*
* Le fichier en lecture est intégralement recopié, quelque soit la position de son curseur.
* La copie se fait à partir du curseur du fichier en écriture.
* En fin de méthode, le curseur lecture est positionné en fin de fichier et le flux retourne <em>true</em> au test <em>fail</em>,
* et le curseur écriture après la copie.
*
* Un groupe de quatre octets (unsigned int) est écrit afin d'indiquer la taille du fichier en nombre d'octets.
* Le fichier est ensuite recopié de manière binaire.
*/
void writeBinaryFile(ofstream& r_ofs, ifstream& r_ifs);

// Fonctions d'organisation

/**
* @brief Retourne la valeur minimale d'un vecteur.
* @param rc_v Le vecteur à parcourir.
* @param p_index Un pointeur retournant l'indice concerné (omis si 0).
* @return La valeur minimale.
* @throw ArgumentException si le vecteur est vide.
*
* L'indice retourné est celui de la dernière valeur minimale trouvée dans le vecteur.
*
* @warning
* La classe T doit définir l'opérateur <=.
*
* @bug
* Cette fonction n'est ni testée ni utilisée.
*/
template<class T>
T minIn(const vector<T>& rc_v, unsigned int* p_index = 0)
{
    if (rc_v.size() == 0)
        throw ArgumentException(__LINE__, __FILE__, "Vecteur vide.", "rc_v", "minIn");

    T x;
    bool first = true;

    for (unsigned int i=0, size=rc_v.size();i<size;i++)
    {
        if (first || rc_v[i] <= x)
        {
            x = rc_v[i];
            if (p_index)
                *p_index = i;
            first = false;
        }
    }

    return x;
}

/**
* @brief Retourne la valeur minimale d'un vecteur comprise strictement au-dessus d'une limite.
* @param rc_v Le vecteur à parcourir.
* @param limMin La valeur limite.
* @param p_index Un pointeur retournant l'indice concerné (omis si 0).
* @return La valeur minimale.
* @throw ArgumentException si le vecteur est vide.
*
* L'indice retourné est celui de la dernière valeur minimale trouvée dans le vecteur.
*
* @warning
* La classe T doit définir l'opérateur <=.
*
* @remarks
* La valeur rc_v[0] est retournée dans le cas où toutes les valeurs du vecteur soient au-dessus de la limite.
*
* @bug
* Cette fonction n'est ni testée ni utilisée.
*/
template<class T>
T minIn(const vector<T>& rc_v, const T& limMin, unsigned int* p_index = 0)
{
    if (rc_v.size() == 0)
        throw ArgumentException(__LINE__, __FILE__, "Vecteur vide.", "rc_v", "minIn");

    T x;
    bool first = true;

    for (unsigned int i=0, size=rc_v.size();i<size;i++)
    {
        if (rc_v[i] <= limMin)
            continue;
        if (first || rc_v[i] <= x)
        {
            x = rc_v[i];
            if (p_index)
                *p_index = i;
            first = false;
        }
    }

    if (first)
    {
        if (p_index)
            *p_index = 0;
        x = rc_v[0];
    }

    return x;
}

/**
* @brief Trie un vecteur de la valeur la plus petite à la plus grande.
* @param r_v Le vecteur à classer.
* @param v_p Un vecteur à ranger "dans le même ordre" que <em>r_v</em>, en fonction du classement de r_v (omis si 0).
* @throw ArgumentException si le premier vecteur est vide ou si le deuxième vecteur n'a pas la même taille que le premier.
*
* Si par exemple, dans le cadre du classement par ordre croissant la 3° valeur de <em>r_v</em> passe en 5° position,
* il en ira de même pour <em>v_p</em> sans respecter cette fois l'ordre croissant.
* Cette fonctionnalité est utile pour associer les valeurs d'un autre vecteur à celui qui doit être classé et conserver cette association.
*
* @warning
* La classe T doit définir les opérateurs ==, < et <=.
*
* @bug
* Cette fonction n'est ni testée ni utilisée.
*/
template<class T>
void sortFromMinToMax(vector<T>& r_v, vector<T>* v_p = 0)
{
    if (r_v.size() == 0)
        throw ArgumentException(__LINE__, __FILE__, "Vecteur vide.", "r_v", "sortFromMinToMax");
    if (v_p != 0 && v_p->size() != r_v.size())
        throw ArgumentException(__LINE__, __FILE__, "Vecteurs de tailles différentes.", "r_v/v_p", "sortFromMinToMax");

    T tmp, lim;
    bool firstWhile = true;
    unsigned int i = 0, index = 0;
    vector<T> temp_v, temp2_v;
    while (temp_v.size() < r_v.size())
    {
        if (firstWhile)
        {
            tmp = minIn(r_v, &index);
            firstWhile = false;
        }
        else
            tmp = minIn(r_v, lim, &index);
        temp_v.push_back(tmp);
        if (v_p)
            temp2_v.push_back((*v_p)[index]);
        // on ajoute toutes les autres valeurs égales
        bool firstFor = true;
        for (unsigned int k=0, size=r_v.size();k<size;k++)
        {
            if (r_v[k] == tmp)
            {
                if (firstFor)
                    firstFor = false;
                else
                {
                    temp_v.push_back(tmp);
                    if (v_p)
                        temp2_v.push_back((*v_p)[index]);
                }
            }
        }
        // puis on passe aux valeurs supérieures
        lim = tmp;
        i++;
    }

    r_v = temp_v;
    if (v_p)
        *v_p = temp2_v;
}

/**
* @brief Réduit un vecteur en fixant sa valeur minimale à la valeur spécifiée.
* @param r_v Le vecteur à réduire.
* @param limMin La valeur minimale.
* @throw ArgumentException si le vecteur est vide.
*
* La réduction consiste à "tasser" le vecteur de sorte que toutes ses valeurs soient consécutives (mais potentiellement en désordre).
* La valeur minimale sera ramenée à la valeur spécifiée en paramètre et les autres suivront.
*
* @warning
* La classe T doit définir l'opérateur <=.
*
* @bug
* Cette fonction n'est ni testée ni utilisée.
* En faisant un rapide test, je remarque que cet algorithme ne doit pas fonctionner... mais je ne suis pas sure de ce qu'il était censé faire à la base,
* c'est loin d'être clair. Que mon jeune esprit était embrumé à cet âge-là !
*/
template <class T>
void reduce(vector<T>& r_v, T limMin)
{
    if (r_v.size() == 0)
        throw ArgumentException(__LINE__, __FILE__, "Vecteur vide.", "r_v", "reduce");

    unsigned int* p_index = new unsigned int;
    T tmp;

    for (unsigned int i=0, size=r_v.size();i<size;i++)
    {
        if (i == 0)
        {
            tmp = minIn(r_v, p_index);
            r_v[*p_index] = limMin;
        }
        else
        {
            tmp = minIn(r_v, tmp, p_index);
            r_v[*p_index] = limMin + i;
        }
    }

    delete p_index;
}

// Fonctions de manipulation des conteneurs

/**
* @brief Petite fonction pratique pour supprimer proprement un objet d'une map de pointeurs.
* @param p_param_map_r La map.
* @param key La clé de l'objet à supprimer.
*
* Si la clé n'est pas trouvée, rien n'est fait.
*
* La clé n'est pas fournie par référence pour éviter une erreur de segmentation.
*
* @warning
* Ne fonctionne que sur des maps de pointeurs simples, et non de pointeurs vers des vecteurs de pointeurs eux-mêmes alloués dynamiquement par exemple.
*/
template<class T1, class T2>
void removeObjectFromMap(map<T1, T2*>& p_param_map_r, T1 key)
{
    if (p_param_map_r.find(key) != p_param_map_r.end())
    {
        T2* p_tmp = p_param_map_r[key];
        if (p_tmp != 0)
        {
            delete p_tmp;
            p_tmp = 0;
        }
        p_param_map_r.erase(key);
    }
}

/**
* @brief Fonction ajoutant une valeur à un vecteur uniquement si cette valeur n'est pas déjà présente.
* @param val La valeur à ajouter.
* @param r_v Le vecteur.
* @return L'indice auquel une éventuelle valeur identique est trouvée, ou le dernier indice du vecteur après ajout s'il a lieu.
*
* L'indice retourné par cette fonction pointe au final toujours sur la valeur passée en paramètre.
* Si elle est présente plusieurs fois, l'indice est celui de la première valeur rencontrée.
*
* @warning
* Ne fonctionne qu'avec une classe implémentant l'opérateur ==.
*/
template<class T>
unsigned int addOnlyIfNew(T val, vector<T>& r_v)
{
    unsigned int x = r_v.size();

    bool ok = true;
    for (unsigned int i=0, size=r_v.size();i<size;i++)
    {
        if (r_v[i] == val)
        {
            ok = false;
            x = i;
            break;
        }
    }
    if (ok)
        r_v.push_back(val);

    return x;
}

/**
* @brief Fonction ajoutant les valeurs d'un vecteur à un autre vecteur en évitant les doublons.
* @param vect Le vecteur à ajouter.
* @param r_v Le vecteur à agrandir.
*
* Les valeurs de <em>vect</em> ne sont ajoutées à <em>r_v</em> que si ce dernier ne les contient pas déjà.
*
* @warning
* Ne fonctionne qu'avec une classe implémentant l'opérateur ==.
*/
template<class T>
void addOnlyNewValues(const vector<T>& vect, vector<T>& r_v)
{
    for (unsigned int i=0, size=vect.size();i<size;i++)
    {
        T val = vect[i];
        bool ok = true;
        for (unsigned int j=0, size2=r_v.size();j<size2;j++)
        {
            if (r_v[j] == val)
            {
                ok = false;
                break;
            }
        }
        if (ok)
            r_v.push_back(val);
    }
}

#endif // MISC_H_INCLUDED
