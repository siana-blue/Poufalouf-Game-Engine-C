/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la fonction <em>main</em>.
* @date 13/02/2016
*/

/**
* @mainpage
* Ce programme convertit des fichiers textes en fichier WAD compatibles avec les jeux <em>Poufalouf Games</em>.
*
* @section TxtFiles Structure d'un fichier txt
*
* La structure du fichier texte à convertir est la suivante :
* <ul><li>Pour chaque objet du wad, <em>$OBJECT_SLOT</em> précède la section le définissant.
* OBJECT_SLOT est le texte correspondant à un objet de l'énumération WADObjectSlot (fichier "enum.h").</li>
* <li>Avant les animations de l'objet, il est possible d'insérer des lignes pour spécifier des propriétés de l'objet.
* Ces lignes commencent par un texte correspondant à l'énumération PfWadObjProperty (fichier "enum.h"), précédé du symbole ':' et suivi
* du signe '=' et de la valeur associée. Par exemple, :colHeight=0.25.
* Le détail des options disponibles est énuméré ci-après.</li>
* <li>Pour chaque animation de l'objet, [ANIMATION_SLOT] précède la section la définissant.
* ANIMATION_SLOT est le texte correspondant à une animation de l'énumération AnimationSlot (fichier "enum.h").</li>
* <li>Contenu de l'animation.</li></ul>
*
* @subsection Propriété d'un objet
*
* Les propriétés d'un objet sont définies sous la forme d'une ligne
*
* <code>:text_propriété=valeur</code>
*
* Pour chaque type de propriété, la valeur doit être choisie comme indiqué ci-dessous :
* <ul><li>colHeight : fraction de la hauteur carrée d'une case sur une map.</li>
* <li>box : définition d'une zone. Les paramètres suivants sont le type de box (texte de l'énumération PfBoxType, fichier "enum.h"),
* puis les quatre coordonnées sous forme de float fractions de la box MAIN, le tout séparé par des ';'.</li></ul>
*
* @subsection Animation
*
* Une animation est composée de plusieurs frames.
* Chaque frame est définie par au moins un fichier texture et une série de coordonnées.
*
* Les coordonnées d'une frame sont les coordonnées à utiliser sur le fichier texture pour déterminer la partie du fichier à afficher.
* Elles s'écrivent sous la forme ligne;colonne;nombre de lignes;nombre de colonnes.
*
* Par exemple, pour un fichier divisé en quatre lignes et quatre colonnes, pour afficher le carré en haut à gauche : 1;1;4;4.
*
* Pour un fichier divisé en 3 par 3, pour afficher le carré inférieur droit : 3;3;3;3.
*
* Si aucune frame n'est spécifiée, alors 1;1;1;1 est pris par défaut.
*
* Une animation dans le script est définie par une série de lignes :
*
* <code>option=contenu</code>
*
* Ces options peuvent être les suivantes :
* <ul><li>img : définition du fichier texture (PNG) à utiliser pour les frames à venir. Le contenu est le chemin relatif à l'application du fichier.
* Cette ligne doit être présente avant déclaration de la première frame, puis peut être à nouveau spécifiée pour changer de fichier.</li>
* <li>snd : définition du fichier son (WAV) à utiliser pour les frames à venir. Le contenu est le chemin relatif à l'application du fichier.</li>
* <li>vid : définition de l'ensemble des frames à partir d'images contenues dans le répertoire indiqué.
* Ajoute autant de frames que de fichiers présents dans le répertoire.
* Le format est le suivant : vid=[nom du répertoire relatif à cette application].</li>
* <li>msc : options diverses. Chaque option est séparée par un ";".
* Les options disponibles sont : loop (animation en boucle).</li>
* <li>frm : la frame elle-même, avec les quatre coordonnées tel qu'indiqué ci-dessus, suivies éventuellement d'un '#' et d'une option indiquée dans la suite.
* 'frm=' peut être omis, une ligne sans spécification d'option étant considérée comme une déclaration de frame.</li>
* <li>slw : ralentissement d'une animation (répétition de chaque frame le nombre de fois indiqué à la suite de cette option).</li>
* <li>box : déclaration d'un indice de zone. Le format est le suivant : box=[énumération PfBoxType];[indice].
* L'indice commence à zéro. Il s'agit, pour le type de zone concerné, de sélectionner la zone correspondante dans l'ordre des déclarations
* de zone dans les propriétés générales de l'objet.</li></ul>
*
* Une frame peut être suivie d'un caractère # pour déterminer une action spéciale.
* <ul><li>Pour jouer un son, utiliser #snd. Le son dont le fichier a été défini est joué à la frame indiquée.</li></ul>
*
* @section WadFiles Structure d'un fichier wad
*
* Le fichier wad créé à partir du script txt est un fichier binaire.
* Il est structuré de la manière suivante :
* <ul><li>Quatre octets définissent la version du wad (int).
* Cette valeur permet de tester que le wad utilisé est compatible avec la version du jeu en cours.
* Il s'agit de la valeur PFGAME_VERSION (fichier "gen.h").</li>
* <li>Quatre octets définissent le nombre de fichiers textures disponibles dans ce wad (unsigned int).</li>
* <li>Les fichiers textures PNG sont copiés à la suite les uns des autres, chacun précédé de quatre octets définissant la taille du fichier (unsigned int).
* Les fichiers sont stockés dans l'ordre de lecture du fichier TXT.</li>
* <li>Quatre octets définissent le nombre de fichiers sons disponibles dans ce wad.</li>
* <li>Les noms des fichiers sons sont alors stockés, chacun terminé par le caractère '\0'.
* Il s'agit des noms de fichier sans leur chemin, le chemin étant fixé par les programmes utilisateurs.</li></ul>
*
* Chaque objet est ensuite stocké selon l'organisation suivante :
*
* Quatre octets de valeur NEW_SLOT_FLAG (fichier "script.h") indiquent la présence d'un nouveau slot.
* Quatre octets définissent le slot de l'objet (énumération PfWadSlot, fichier "enum.h").
*
* Quatre octets précisent ensuite le nombre de propriétés de l'objet.
* Chaque propriété est stockée sous forme d'un groupe de quatre octets pour le type de propriété, puis d'un ensemble de données dépendant de celui-ci :
* <ul><li>WADOBJ_COLHEIGHT : un groupe de quatre octets pour une valeur flottante,</li>
* <li>WADOBJ_BOX : un groupe de quatre octets pour le type de zone, puis quatre groupes de quatre octets pour les coordonnées flottantes.</li>
* <li>WADOBJ_CENTER : deux groupes de quatre octets pour deux valeurs flottantes représentant les coordonnées du centre.</li></ul>
*
* Quatre octets précisent ensuite le nombre d'animations pour cet objet.
*
* Chaque animation est ensuite stockée en commençant par quatre octets définissant le statut d'animation (énumération PfAnimationStatus, fichier "enum.h").
* Quatre octets définissent alors le nombre de lignes à lire pour cette animation.
*
* Les lignes d'options et de frames du script sont stockées en commençant par quatre octets définissant l'option (énumération PfWadScriptOption, fichier "enum.h").
*
* <ul><li>WADTXT_IMG et WADTXT_SND : quatre octets suivent, spécifiant l'image ou le son à considérer.
* Le nombre correspond à l'ordre de chargement des fichiers en début de fichier wad, en commençant par 1.</li>
* <li>WADTXT_VID : quatre octets indiquent le nombre de frames de la vidéo.
* Des instructions WADTXT_IMG et WADTXT_FRM sont ajoutées pour chaque frame de la vidéo.</li>
* <li>WADTXT_MSC : un groupe de quatre octets indique les options retenues (32 flags).</li>
* <li>WADTXT_FRM : ajout d'une frame.</li>
* <li>WADTXT_SLW : un groupe de quatre octets (unsigned int) indique le facteur de ralentissement,</li>
* <li>WADTXT_BOX : un groupe de quatre octets (PfBoxType) indique le type de zone, puis un autre groupe de quatre octets (unsigned int)
* indique l'index.</li></ul>
*
* Les frames sont écrites de la manière suivante : les quatre coordonnées sont stockées sous forme d'un octet chacune.
* Pour considérer la frame comme totalement chargée, les quatre octets END_OF_LIST_INT (fichier "gen.h") doivent être lus.
* S'ils ne sont pas présents, alors les quatre octets lus sont considérés comme une option (énumération PfWadScriptOption, fichier "enum.h").
* Les options d'une frame peuvent ainsi s'enchaîner jusqu'à une fin de liste :
* <ul><li>WADTXT_SND : jouer le son actuellement défini.</li></ul>
*
* Chaque section (correspondant à un objet) se termine par les quatre octets END_OF_SECTION (fichier "gen.h").
*
* Le fichier est terminé par les quatre octets END_OF_LIST_INT (fichier "gen.h").
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
* @brief Fonction principale du programme <em>TxtToPfWad</em>.
* @param argc le nombre d'arguments fournis au programme, de 1 (pas d'argument) à l'infini (enfin presque).
* @param argv les arguments fournis représentent les fichiers à convertir.
*/
int main(int argc, char* argv[])
{
	#ifndef NDEBUG
	LOG(__DATE__ << " " << __TIME__ << "\nTxtToPfWad log\n\n");
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
			convertTxtToWad(files_v[i]);
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

