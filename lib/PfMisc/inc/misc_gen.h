/**
* @file
* @author Anaïs Vernet
* @brief Header inclus dans tous les fichiers header de cette bibliothèque.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Sont à noter les lignes suivantes, qui seront comprises dans tout programme utilisant cette bibliothèque suite à inclusion de ce fichier :
*
* <code>using namespace std;</code>
*
* <code>typedef unsigned char pfflag;</code>
*
* <code>typedef unsigned int pfflag32;</code>
*
* Par mise en commentaire ou non, la macro NDEBUG peut facilement être désactivée ou activée.
* Définie, elle annule les appels à la fonction <em>assert</em>, utilisée ponctuellement par certaines fonctions des jeux Poufalouf, notamment les classes
* du système MVC. L'utilisation des assertions a été abandonnée dans la suite du développement, mais pour maintenir leur fonctionnement, l'utilisation
* de cette macro NDEBUG est toujours nécessaire.
* Elle sert également à définir ou non la macro LOG qui permet le recensement dans un fichier texte des différentes erreurs générées, ou d'espions
* ajoutés dans le code pour cibler différents processus.
* En mode Debug, la macro NDEBUG doit donc être désactivée. Il faudra l'activer avant passage en mode Release pour alléger le code et l'exécution.
*
* Ce fichier propose de définir SYSTEM_WINDOWS ou SYSTEM_LINUX. Si le développement sous Linux a été initié, il a très vite été abandonné.
* Depuis le passage à la version 2 de la SDL, la gestion de l'interface graphique sous Linux est à reprendre entièrement.
* Pour l'instant, seule la version Windows est fonctionnelle.
*
* La macro PFGAME_VERSION est définie dans ce fichier, dans cette bibliothèque. Une modification ici permet donc la mise à jour de toutes les applications
* intégrant cette bibliothèque, sous réserve de les recompiler.
* Il s'agit d'un nombre à incrémenter, indépendamment de la version de chaque logiciel.
* Son usage principal est la vérification de la compatibilité entre une ressource et le jeu. Les fichiers wad, textures, menus ou autres ont pour premier
* groupe de quatre octets un entier égal à la valeur PFGAME_VERSION correspondant au programme qui les a générés.
* Cet octet est lu puis comparé à la valeur PFGAME_VERSION du logiciel de jeu. En cas de non-adéquation, une erreur est traitée.
*
* Les macros DIR_SEPARATOR et END_OF_LINE sont des caractères indiquant respectivement la séparation de dossiers dans un chemin d'accès et la fin
* d'une ligne (retour chariot) dans un fichier texte.
* Ces macros dépendent du système d'exploitation, car la séparation de répertoires dans un chemin d'accès diffère entre Windows et Linux, de même
* que la nature d'un retour à la ligne dans un fichier texte.
*
* La première série de macros, de CONFIG_FILE à TMP_FILE, permet la gestion de fichiers standard, tels que les fichiers temporaires ou de configuration.
*
* La deuxième série de macros, de MAX_NUMBER à SAVE_END_VALUE, sont des définitions à utiliser pour divers usages, dont les principaux sont suggérés
* dans leurs documentations propres. Elles sont notamment utilisées dans la gestion des fichiers binaires, mais pas seulement.
*
* La troisième série de macros, de FLOAT_MARGIN à DECIMAL_MODULO, sont des valeurs ou des calculs permettant de simplifier la réalisation d'opérations
* mathématiques.
*
* La quatrième série de macros gère les casts.
*
* Vient ensuite la gestion des informations de debug, qui est inhibée lorsque la macro NDEBUG est définie.
*
* @section ConfigFile Le fichier de configuration
*
* Le fichier de configuration, défini par la macro CONFIG_FILE, sert à définir un état de fonctionnement du jeu.
*
* Pour l'instant, il ne gère que le langage de l'application, au moyen d'un groupe de deux lettres ('en', 'fr'...). Ces bigrammes sont également
* utilisés en extension des fichiers de traduction, sous la forme TRANSLATION_SCRIPT_NAME_[en, fr...]. Par exemple : 'translation_fr'.
* Ainsi, en fonction du langage choisi, le programme sait quel fichier de traduction ouvrir.
*
* Le fichier de configuration sera probablement amené à s'enrichir par la suite.
* La fonction <em>readConfig</em> (fichier "misc.h") gère la lecture de ce fichier.
*
* @section BinaryFile La lecture de fichiers binaires
*
* Les programmes de jeu sont souvent amenés à lire des fichiers binaires : wad, sauvegardes, maps... autant de fichiers binaires à décortiquer.
* Pour faciliter la gestion de tels flux de données, des flags sont utilisés pour signaler la présence de données particulières.
* La deuxième série de macros de ce fichier est là pour ça.
*
* En fixant un nombre maximal, limite à ne pas dépasser, on peut s'assurer que lorsqu'une valeur supérieure est rencontrée dans un flux binaire, il s'agit
* d'un flag. C'est là le rôle principal des macros MAX_NUMBER et INVALID_INT, ainsi que les flags qui suivent.
*
* Les flux binaires sont souvent lus quatre octets par quatre octets (int). Dès que STRING_FLAG est rencontré par exemple, la lecture peut passer en mode
* octet par octet jusqu'à rencontrer '\0', puis reprendre quatre à quatre.
*
* @section FloatValues La comparaison de nombres flottants
*
* Dans le merveilleux monde de l'informatique, la comparaison d'entiers est simple, mais pour les nombres décimaux, c'est la galère.
* Aussi Poufalouf a-t-il décidé d'introduire FLOAT_MARGIN, une valeur définissant l'infiniment petit selon les programmes Poufalouf.
* Lors d'une comparaison, il est utile de recourir à cette macro pour définir quand deux valeurs sont égales ou non.
*
* Ainsi, si FLOAT_MARGIN = 0.0001, Poufalouf a le plaisir de vous annoncer que 1.000001 = 1.000002 par exemple.
* La limite pourrait être bien plus basse, mais c'est généralement suffisant pour les jeux dérivant de cette bibliothèque.
* Il faut garder en mémoire qu'un simple signe '=' avec des float peut conduire à des surprises. Toujours utiliser '>' et '<', et FLOAT_MARGIN.
* Les macros commençant par DECIMAL, dans ce fichier, simplifient ces comparaisons.
*
* @section DebugLists Les listes de debug
*
* Deux listes sont mises à disposition pour le debug dans ce fichier, valables uniquement quand NDEBUG n'est pas définie.
*
* Ces listes globales sont accessibles partout dans le programme et peuvent être remplies et mises à jour à loisir.
* Elles permettent d'afficher l'état de variables directement à l'écran par exemple, pendant l'exécution du programme, pour un mode de debug bien précis.
*
* Le booléen <em>g_debug</em> permet d'afficher ou non les informations des listes par exemple (c'est une suggestion d'utilisation).
* On peut imaginer que la pression d'une touche 'Fx' affiche ou cache ces informations en superposition du jeu.
*/

/**
* @mainpage
*
* <b>Bibliothèque contenant divers outils pouvant être utilisés pour les jeux Poufalouf</b>
*
* Cette bibliothèque contient une série de macros mathématiques, des outils pour la manipulation de données, la déclaration de variables générales
* et bien d'autres choses détaillées ci-dessous :
* <ul><li>la définition de valeurs par défaut pour divers systèmes numériques, notamment pour le stockage de données dans des fichiers binaires,
* fichier "misc_gen.h",</li>
* <li>un système d'énumérations communes aux programmes utilisant cette bibliothèque, fournissant un texte associé à chaque valeur,
* fichier "enum.h",</li>
* <li>des classes pour gérer les exceptions, toutes héritant de PfException, l'exception de base d'un jeu Poufalouf,</li>
* <li>des fonctions génériques de manipulation de fichiers, et d'intérêt général telles que, par exemple, la conversion de texte en nombre,
* fichier "misc.h",</li>
* <li>une classe facilitant la gestion de données sous forme de paquets (DataPackage),</li>
* <li>une interface pour la gestion de phases de jeu (MultiPhases),</li>
* <li>une interface pour les objets pouvant répondre à des instructions (InstructionReader),</li>
* <li>une interface interdisant la copie d'une instance de classe (NonCopyable),</li>
* <li>une interface pour les objets pouvant être sérialisés (Serializable).</li></ul>
*
* @see misc_gen.h, misc.h, enum.h
*
* Les éléments qui suivent sont valables pour tous les programmes réalisés par Poufalouf.
* C'est lui le cerveau, je ne fais qu'humblement retranscrire.
*
* @section ReglesCode Règles d'écriture du code
*
* @subsection ReglesCodeVariables Nommage des variables
*
* Les variables sont nommées de la manière suivante : [préfixe]_[nom]_[suffixe].
* Si plusieurs préfixes se superposent, ils sont tous regroupés avant le premier '_'.
* Si plusieurs suffixes se superposent, ils sont chacun séparés par un '_'.
*
* Les préfixes sont les suivants :
* <ul><li>pointeur : p_,</li>
* <li>pointeur vers une valeur constante : q_ (ou pc_),</li>
* <li>membre privé d'une classe : m_,</li>
* <li>variable globale : g_,</li>
* <li>variable statique : s_,</li>
* <li>variable prenant une valeur allouée dynamiquement en mémoire : n_,</li>
* <li>référence : r_,</li>
* <li>objet constant : c_ (à ajouter à r_ ou p_, inutile sur les variables simples),</li>
* <li>variable retournée en fin de méthode : x_.</li></ul>
*
* @remarks
* Cas particulier : x peut être utilisé comme nom de variable seul dans une fonction pour indiquer par exemple un entier de retour.
*
* @warning
* Les membres de structures (définies par le mot-clé <code>struct</code>) ne sont pas précédés du suffixe m_.
*
* Les suffixes qualifient des conteneurs, ils sont les suivants :
* <ul><li>tableau : _t (_t2 pour 2 dimensions etc...),</li>
* <li>vecteur : _v,</li>
* <li>liste : _l,</li>
* <li>map : _map,</li>
* <li>set : _set,</li>
* <li>pair : _pair.</li></ul>
*
* Mis à part les préfixes m_ et g_ qui se situent toujours en tout début de variable (premier préfixe), les autres préfixes changent de place
* s'ils qualifient le conteneur lui-même.
* <ul><li>Si un préfixe qualifie le contenu d'un conteneur, il se place avant le nom de la variable.</li>
* <li>Si un préfixe qualifie le conteneur lui-même, il se situe après le suffixe qualifiant le conteneur.</li></ul>
*
* Les suffixes se succèdent du conteneur le plus profond au conteneur le plus à l'extérieur dans le cas de conteneurs imbriqués.
* Dans le cas des maps, les variables qualifiées sont les valeurs et non les clés.
*
* <b>Exemples</b>
*
* <ul><li>Membre d'une classe : m_name</li>
* <li>Vecteur de pointeurs : p_name_v</li>
* <li>Membre pointeur vers un vecteur de pointeurs : mp_name_v_p</li>
* <li>Pointeur global : gp_name</li>
* <li>Pointeur vers une map de pointeurs constants vers des vecteurs de pointeurs alloués dynamiquement, le pointeur étant membre d'une classe :
* mpn_name_v_pc_map_p</li></ul>
*
* @remarks
* Des exceptions sont tolérées pour les références constantes, notamment vers des chaînes de caractères, pour lesquelles le préfixe rc_ est omis.
*
* @subsection ReglesCodeAccesseurs Gestion des accesseurs
*
* Les accesseurs sont nommés getNomVariable et setNomVariable. Aucune autre méthode ne peut s'appeler get[...] ou set[...].
*
* Les accesseurs ne font rien d'autre que de retourner une variable ou la mettre à jour. Ils sont écrits dans le header sur une ligne.
*
* Les getters sont constants et retournent des références constantes, des pointeurs constants ou des copies pour les types élémentaires.
*
* Les tests de booléen sont classés parmi les accesseurs et sont appelés isNomVariable.
* A la différence de 'get', d'autres méthodes peuvent être nommées en commençant par 'is'.
*
* @section ReglesDocumentation Règles concernant la documentation
*
* Toutes les variables, fonctions, classes, attributs de classe et méthodes de classe sont documentés selon la norme du programme <em>doxygen</em>.
*
* Les noms de variables et de fonctions sont écrits en <em>italique</em>.
* Les noms des attributs et les méthodes sont écrits sans style particulier mais systématiquement précédés de l'opérateur de portée et de la classe les
* contenant.
*
* Les noms de fichier sont écrits entre guillemets.
*
* @subsection ReglesDocConstructeur Nommage des constructeurs
*
* Les constructeurs sont nommés de la manière suivante :
* <ul><li>constructeur par défaut : il n'a aucun paramètre ou uniquement des paramètres optionnels,</li>
* <li>constructeur 1, 2, 3... : il a au moins un paramètre obligatoire. Le nommage est libre et peut être décliné au moyen d'indices 1a, 2c etc...,</li>
* <li>constructeur par copie.</li></ul>
*
* @section PointsDivers Fonctionnalités d'utilité générale
*
* @subsection NDEBUG Macro La macro NDEBUG
*
* Tirée de la fonction <em>assert</em> proposée par la bibliothèque standard cassert, cette macro, une fois définie, sert à indiquer que le mode Debug
* est terminé et qu'on passe en Release.
*
* Cette macro est notamment utilisée par les assertions, qui sont inhibées une fois NDEBUG définie.
*
* Les assertions sont utilisées par certaines classes des jeux Poufalouf, bien qu'abandonnées par la suite. Cependant, NDEBUG reste et est utilisée
* pour les méthodes de debug personnalisées.
*
* La macro LOG, par exemple, qui permet d'écrire dans un fichier texte pendant l'exécution du programme, est annulée quand NDEBUG est défini.
* Cela permet de ne plus générer ce fichier texte en mode Release.
*/

#ifndef MISC_GEN_H_INCLUDED
#define MISC_GEN_H_INCLUDED

//#define NDEBUG

#define SYSTEM_WINDOWS //!< Indique que le système d'exploitation est Windows.
//#define SYSTEM_LINUX //!< Indique que le système d'exploitation est Linux.

#define PFGAME_VERSION 0 //!< La version actuelle du jeu. Nombre utilisé pour vérifier la compatibilité d'un fichier ressource avec le jeu.

#ifdef SYSTEM_LINUX
#define DIR_SEPARATOR '/' //!< Le caractère de séparation des répertoires dans le système de fichier du système d'exploitation.
#define END_OF_LINE '\n' //!< Le caractère de retour chariot dans les fichiers textes gérés par le système d'exploitation.
#endif // SYSTEM_LINUX
#ifdef SYSTEM_WINDOWS
#define DIR_SEPARATOR '\\' //!< Le caractère de séparation des répertoires dans le système de fichier du système d'exploitation.
#define END_OF_LINE "\r\n" //!< Le caractère de retour chariot dans les fichiers textes gérés par le système d'exploitation.
#endif // SYSTEM_WINDOWS

// Macros pour la gestion des fichiers textes

#define CONFIG_FILE "config.txt" //!< Le nom du fichier de configuration.
#define SCRIPT_EXT "txt" //!< L'extension des fichiers script.
#define DEFAULT_LANGUAGE "en" //!< L'extension du langage par défaut.
#define TRANSLATION_SCRIPT_NAME "./script/translation" //!< Le nom des fichiers de traduction (sans l'extension de langage).
#define TMP_FILE "./res/.temp" //!< Le nom des fichiers temporaires créés.

// Macros pour la gestion de fichiers binaires

#define MAX_NUMBER 2000000000 //!< Le nombre qu'il ne faut JAMAIS dépasser.
#define INVALID_INT (MAX_NUMBER+1) //!< Entier invalide.
#define NEW_SLOT_FLAG (INVALID_INT+1) //!< Marqueur d'un nouveau slot dans un fichier.
#define STRING_FLAG (INVALID_INT+2) //!< Marqueur d'une chaîne de caractère dans un fichier, placé avant la chaîne. Peut servir pour une recherche en lecture binaire.
#define END_OF_LIST_CHAR 4 //!< Fin de liste adaptée notamment aux caractères.
#define END_OF_LIST_INT -999999999 //!< Fin de liste de groupes de quatre octets.
#define END_OF_SECTION -999999998 //!< Fin de section.
#define SAVE_END_VALUE 1000 //!< La valeur du champ SAVE_END des énumérations de sauvegarde.

// Macros mathématiques

#define FLOAT_MARGIN 0.0001 //!< La marge utilisée pour la comparaison de nombres flottants.

#define MAX(a, b) (((a) > (b))?(a):(b)) //!< Macro calculant le maximum de deux valeurs.
#define MIN(a, b) (((a) < (b))?(a):(b)) //!< Macro calculant le minimum de deux valeurs.
#define ABS(a) (((a) > -(a))?(a):(-(a))) //!< Macro calculant la valeur absolue d'une valeur.
#define SGN(a) (((a)==0)?0:(((a) > 0)?1:-1)) //!< Macro donnant le signe d'une valeur.
#define ROUND_FLOAT(nb, rUp) ((ABS((nb)-(int)((nb)+FLOAT_MARGIN))>FLOAT_MARGIN)?round((nb)+(rUp?1:-1)*FLOAT_MARGIN):((nb)+FLOAT_MARGIN)) //!< Macro arrondissant un float uniquement s'il n'est pas entier, et permettant de choisir comment arrondir 0,5 (nécessite la bibliothèque cmath).
#define RAND(min, max) (rand()%((max)-(min)+1)+(min)) //!< Macro simplifiant l'appel à la fonction <em>rand</em> (nécessite la bibliothèque cstdlib).
#define DECIMAL_EQUAL(a, b) ((ABS((a)-(b))>FLOAT_MARGIN)?false:true) //!< Macro simplifiant la comparaison a == b de nombres flottants, en utilisant FLOAT_MARGIN.
#define DECIMAL_SUP(a, b) ((a)-(b)>FLOAT_MARGIN?true:false) //!< Macro simplifiant la comparaison a > b de nombre flottants, en utilisant FLOAT_MARGIN.
#define DECIMAL_INF(a, b) ((a)-(b)<-FLOAT_MARGIN?true:false) //!< Macro simplifiant la comparaison a < b de nombre flottants, en utilisant FLOAT_MARGIN.
#define DECIMAL_SUPEQUAL(a, b) ((DECIMAL_SUP(a, b) || DECIMAL_EQUAL(a, b))?true:false) //!< Macro simplifiant la comparaison a >= b de nombre flottants, en utilisant FLOAT_MARGIN.
#define DECIMAL_INFEQUAL(a, b) ((DECIMAL_INF(a, b) || DECIMAL_EQUAL(a, b))?true:false) //!< Macro simplifiant la comparaison a <= b de nombre flottants, en utilisant FLOAT_MARGIN.
#define DECIMAL_MODULO(val, div) ((val) / (div) - (int) ((val) / (div))) //!< Macro retournant la partie après la virgule d'une division de float.

// Macros de casts

#define CAST(a, type) *(dynamic_cast<type*>(a)) //!< Macro simplifiant l'appel à dynamic_cast, et retournant une référence (a = pointeur, type sans pointeur).
#define P_CAST(a, type) dynamic_cast<type*>(a) //!< Macro simplifiant l'appel à dynamic_cast, et retournant un pointeur (a = pointeur, type sans pointeur).

using namespace std;

typedef unsigned char pfflag; //!< Octet, type utilisé pour définir un système de flag sur 8 bits.
typedef unsigned int pfflag32; //!< Quadruple octet, type utilisé pour définir un système de flag sur 32 bits.

#ifndef NDEBUG

/**
* @brief Indique si les informations de debug sont affichées à l'écran.
*
* @remarks
* Non déclaré si NDEBUG est définie.
*/
extern bool g_debug;
#include <vector>
#include <map>
#include <string>
/**
* @brief Liste de valeurs entières pour debug, repérées par leurs noms associés.
*
* Cette liste peut être utilisée partout dans le programme, afin de consulter l'état de variables préalablement stockées ici.
*
* @remarks
* Non déclaré si NDEBUG est définie.
*/
extern map<string, vector<int> > g_dbInt_v_map;
/**
* @brief Liste de valeurs flottantes pour debug, repérées par leurs noms associés.
*
* Cette liste peut être utilisée partout dans le programme, afin de consulter l'état de variables préalablement stockées ici.
*
* @remarks
* Non déclaré si NDEBUG est définie.
*/
extern map<string, vector<float> > g_dbFloat_v_map;

#include <fstream>
/**
* @brief Variable globale repérant un flux en écriture.
*
* Ce fichier stocke les informations correspondant au déroulement du programme, notamment les erreurs qui peuvent survenir.
*
* @warning
* Une fois NDEBUG déclaré, en Release, cette variable n'est ni déclarée ni utilisée par ce programme. Plutôt que d'écrire directement
* dans cette variable avec l'opérateur <<, utiliser la macro LOG de ce fichier. Le test de définition de NDEBUG y est intégré.
*/
extern ofstream g_log;

/**
* @brief Macro permettant d'afficher un message sur le fichier log (variable de flux <em>g_log</em>).
*
* Cette macro s'utilise de la manière suivante :
*
* <code>LOG([texte1] << [texte2] << ...);</code>
*
* Si NDEBUG est définie, alors cette macro ne fait rien.
*/
#define LOG(s) g_log << s

#else
#define LOG(s) ;

#endif // ifndef NDEBUG

#endif // MISC_GEN_H_INCLUDED
