/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la fonction <em>main</em>.
* @date 15/06/2015
*/

/**
* @mainpage
* Ce programme convertit des fichiers textes en fichier MEN compatibles avec les jeux <em>Poufalouf Games</em>.
*
* Il s'agit de fichiers représentant les menus du jeu.
*
* @section TxtFiles Structure d'un fichier txt
*
* Un fichier TXT est constitué d'une succession de sections, elles-mêmes constituées d'options.
*
* Les sections sont délimitées par une ligne commençant par '$', par exemple :
*
* <code>$PUT</code>
*
* Les noms de sections font 3 lettres.
*
* Toutes les lignes lues dans le fichier à la suite de cette déclaration de section concernent cette section, jusqu'à la fin du fichier ou le prochain début de section.
*
* Dans chaque section, les lignes sont de la forme :
*
* <code>opt=valeur</code>
*
* où opt est une déclaration d'option.
*
* Une déclaration d'option fait 3 lettres.
*
* Les différentes sections sont les suivantes :
* <ul><li>PUT : place un objet à l'écran.
* Les options admises sont 'obj' (obligatoire), 'crd', 'txt', 'lyt', 'eft', 'val' et 'dyn'.</li>
* <li>SEL : sélectionne un objet.
* Les options admises sont 'lyt' (obligatoire).</li>
* <li>MUS : définit une musique.
* Les options admises sont 'txt' (obligatoire).
* L'option 'txt' définit le nom du fichier audio (sans le chemin complet).</li>
* <li>MSC : définit diverses options du menu. Les valeurs de ces options suivent l'option 'txt', et sont séparées par des ";".
* Elles peuvent être :
* rien pour l'instant, cette option n'est plus utile pour le moment.</li></ul>
*
* Les différentes options sont les suivantes :
* <ul><li>obj : définit un objet à partir d'un slot de wad.</li>
* <li>crd : définit les coordonnées x;y;w;h entre 0 et 1.
* Par défaut 0;0;1;1.</li>
* <li>txt : définit un texte.
* \<PFGAME_VERSION\> est remplacé par la valeur PFGAME_VERSION (fichier <em>gen.h</em>).
* \<%xx\> où x est un nombre de 00 à 99 est remplacé par le texte dynamique du menu créé, à l'indice correspondant du vecteur.
* Ceci ne fonctionne que dans la section 'PUT'.
* Ces modifications sont effectives à la création du menu dans le programme principal, rien n'est fait ici.
* L'option 'lyt' doit déjà avoir été définie dans le cas d'utilisation de textes dynamiques, afin de savoir à quel layout lier le texte dynamique.
* Inutile d'écrire du texte avant ou après la balise \<%xx\>, ou d'en mettre deux différentes, la présence d'un texte dynamique annule tout autre texte lors de l'écriture de celui-ci sur l'objet.
* Si deux options 'txt' sont lus dans une même section, chaque texte est stocké dans une chaîne différente au moment de la lecture du fichier par le programme.
* Certains objets acceptent plusieurs textes, tandis que d'autres ignoreront les suivants.</li>
* <li>lyt : associe un layout.
* La valeur est de type id(ligne;colonne) où id est le numéro d'identifiant du layout, ligne la ligne à laquelle associer l'objet et colonne sa colonne.</li>
* <li>eft : définit l'effet de ce layout.
* Les valeurs de l'effet sont séparées par des ";".
* Une valeur peut suivre chaque effet, séparée de celui-ci par ":" (par défaut 0).
* Cette valeur doit être entière et comprise entre 0 et 255.</li>
* <li>val : définit les valeurs utilisés dans la construction de l'objet (méthode PfWad::generateGLitem).
* Obligatoirement 4 valeurs, séparées par des ';'.</li>
* <li>dyn : définit un indice de valeur dynamique à associer au widget (commençant à 0).
* L'option 'lyt' doit déjà avoir été définie afin de savoir à quel layout lier la valeur dynamique.</li></ul>
*
* @warning
* Aucune ligne vide ne doit être présente dans le fichier, ceci afin de forcer une fin de fichier en fin de dernière ligne sans retour à la ligne (problématique pour la détection de fin de fichier).
*
* Objets particuliers :
*
* <ul><li>GUI_LIST : le premier 'txt' indique un dossier à parcourir, chaque bouton de la liste correspondant à un nom de fichier.
* Le deuxième 'txt' indique les extensions à considérer.
* En l'absence de précision, tous les fichiers sont retenus.</li></ul>
*
* @section MenFiles Structure d'un fichier MEN
*
* Le fichier men créé à partir du script txt est un fichier binaire.
*
* Quatre octets définissent la version du menu (int).
* Cette valeur permet de tester que le fichier utilisé est compatible avec la version du jeu en cours.
* Il s'agit de la valeur PFGAME_VERSION (fichier "gen.h").
*
* Pour chaque section, l'identifiant de section est écrit sous la forme de quatre octets (énumération PfMenuScriptSection, fichier "enum.h").
*
* Le nombre de lignes de la section est écrit sous la forme de quatre octets (unsigned int).
*
* Chaque option est écrite en commençant par l'identifiant d'option sous la forme de quatre octets (énumération PfMenuScriptOption, fichier "enum.h").
*
* En fonction de l'options, les données suivantes sont écrites :
* <ul><li>obj : quatre octets pour stocker la valeur NEW_SLOT_FLAG (fichier "gen.h"),
* et quatre octets pour le slot de l'objet (énumération PfWadSlot, fichier "enum.h").</li>
* <li>crd : quatre octets par coordonnées (float).</li>
* <li>txt : quatre octets pour stocker la valeur STRING_FLAG (fichier "gen.h"), et un octet par caractère, puis écriture d'un octet '\0'.</li>
* <li>lyt : quatre octets pour l'identifiant du layout, quatre octets pour la ligne et quatre octets pour la colonne (int pour les 3).</li>
* <li>eft : quatre octets (flags, énumération PfEffect, fichier "enum.h") définissent les effets du widget. Une valeur de 0 à 255 peut être intégrée à ces flags (voir l'énumération).</li>
* <li>val : quatre octets pour chaque valeur.</li>
* <li>dyn : quatre octets pour l'indice de valeur dynamique.</li></ul>
*
* Le fichier est terminé par les quatre octets END_OF_LIST_INT (fichier "gen.h").
*/

#include "gen.h"
#include <vector>
#include "errors.h"
#include "misc.h"
#include "script.h"

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
* @brief Fonction principale du programme <em>TxtToPfMen</em>.
* @param argc le nombre d'arguments fournis au programme, de 1 (pas d'argument) à l'infini (enfin presque).
* @param argv les arguments fournis représentent les fichiers à convertir.
*
* Sans argument, ce programme convertit tous les fichiers du répertoire SCRIPT_DIR (fichier "gen.h") en plaçant les fichiers objets MEN dans le répertoire OBJ_DIR.
*/
int main(int argc, char* argv[])
{
	#ifndef NDEBUG
	LOG(__DATE__ << " " << __TIME__ << "\nTxtToPfMen log\n\n");
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
			convertTxtToMen(files_v[i]);
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
	LOG("\nLe programme s'est terminé normalement.\n");
	if (g_log.is_open())
		g_log.close();
	#endif

	return 0;
}

