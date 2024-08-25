/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la fonction <em>main</em>.
* @version 0.0.0
* @date 15/06/2015
*/

/**
* @mainpage
* Ce programme convertit des fichiers textes en fichier TEX compatibles avec les jeux <em>Poufalouf Games</em>.
*
* @section TxtFiles Structure d'un fichier txt
*
* La structure du fichier texte à convertir est la suivante :
* <ul><li>Les premières lignes précédant la première section indiquent les chemins des fichiers de texture de terrain à utiliser.</li>
* <li>S'ensuivent ensuite des groupes de lignes commençant par une ligne de section au format $xxx,
* et continuant avec un nombre variable de lignes au format n:i;j
* où n indique l'indice de texture du terrain, 0 étant le fichier de terrain principal, puis les nombres suivants désignant les indices de la liste de terrains annexes,
* i indique un indice de case du terrain à lier à l'indice j du terrain annexe.</li></ul>
*
* Les sections disponibles sont les suivantes :
* <ul><li>SPR : lie un indice de terrain de base à un indice de terrain annexe définissant un débordement.
* La case pointée doit être le centre d'un anneau contenant les différentes textures de débordement.
* Dans cette section, la ligne n:i;j peut être suivie par un #p où p est un nombre positif ou négatif.
* Ce nombre indique la priorité de la texture en cas de débordement sur une autre case (au-dessus ou en dessous de la texture voisine).
* Par défaut, le niveau de priorité est 0, il est donc possible d'affecter une priorité inférieure ou supérieure.</li>
* <li>BRD : utilisé pour les bordures de cases, pointant vers la texture nord. Les suivantes s'enchaînent de gauche à droite dans le sens des aiguilles d'une montre.</li>
* <li>CLF : pointe vers la texture de falaise à utiliser. Les bordures gauches puis droites de la falaise suivent.</li>
* <li>CLB : pointe vers une texture de débordement au niveau d'un à-pic avant falaise, au nord-ouest puis dans le sens des aiguilles d'une montre.
* La case pointée doit être le centre d'un anneau contenant les différentes textures de débordement.</li>
* <li>FLC : pointe vers la texture de recouvrement en bas de falaise.
* La case pointée est le recouvrement sur un coin gauche, puis le milieu et le coin droits suivent.</li></ul>
*
* @section TexFiles Structure d'un fichier tex
*
* Le fichier tex créé à partir du script txt est un fichier binaire.
* Il est structuré de la manière suivante :
* <ul><li>Quatre octets définissent la version du wad (int).
* Cette valeur permet de tester que le wad utilisé est compatible avec la version du jeu en cours.
* Il s'agit de la valeur PFGAME_VERSION (fichier "gen.h").</li>
* <li>Quatre octets précisant le nombre de fichiers terrains.</li>
* <li>Quatre octets définissent la taille du fichier de texture de terrain.</li>
* <li>L'ensemble du fichier PNG est stocké.</li>
* <li>Les deux dernières étapes sont réalisées pour chaque fichier terrain.</li>
* <li>Quatre octets définissent une section.</li>
* <li>Quatre octets définissent le nombre de lignes à lire dans la section.</li>
* <li>Une suite de groupes de trois fois quatre octets lient ensuite un indice de texture du fichier terrain et la case associée à un indice de texture de terrain annexe.</li>
* <li>Dans le cas d'une section SPR, quatre octets indiquent la priorité de la texture pour les débordements.</li>
* <li>Les trois dernières étapes sont répétées autant de fois qu'il y a de sections.</li>
* <li>Le groupe de quatre octets END_OF_LIST_INT (fichier "gen.h") est présent en fin de fichier.</li></ul>
*/

#include "gen.h"
#include <vector>
#include "errors.h"
#include "script.h"
#include "misc.h"

#ifndef NDEBUG
/**
* @brief Initialisation du flux en écriture stockant les informations relatives au déroulement du programme.
*
* @remarks
* Ce fichier est créé dans le répertoire d'exécution du programme, sous le nom "log.txt.". Si le fichier existe déjà, il est remplacé. En mode release, ce fichier n'est pas créé.
*/
ofstream g_log("log.txt", ios::trunc);
#endif

/**
* @brief Fonction principale du programme <em>TxtToPfTex</em>.
* @param argc le nombre d'arguments fournis au programme, de 1 (pas d'argument) à l'infini (enfin presque).
* @param argv les arguments fournis représentent les fichiers à convertir.
*/
int main(int argc, char* argv[])
{
	#ifndef NDEBUG
	LOG(__DATE__ << " " << __TIME__ << "\nTxtToPfTex log\n\n");
	#endif

	try
	{
		vector<string> files_v;
		if (argc > 1)
		{
			for (int i=1;i<argc;i++)
				files_v.push_back(argv[i]);
		}
		else
			files_v = filesInDir(SCRIPT_DIR, ".txt", "", true);

		for (int i=0, size=files_v.size();i<size;i++)
			convertTxtToTex(files_v[i]);
	}
	catch (PfException& e)
	{
		#ifndef NEDBUG
		LOG("Une erreur est survenue:\n" << e.what() << "\n");
		if (g_log.is_open())
			g_log.close();
		#endif
		return 1;
	}

	#ifndef NEDBUG
	if (g_log.is_open())
		g_log.close();
	#endif

	return 0;
}

