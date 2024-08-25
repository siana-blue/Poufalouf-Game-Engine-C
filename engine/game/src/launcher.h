/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions nécessaires au lancement du jeu.
* @date 12/02/2016
*/

#ifndef LAUNCHER_H_INCLUDED
#define LAUNCHER_H_INCLUDED

#include "gen.h"
#include <vector>
#include <string>

/**
* @brief Exécute les actions nécessaires au lancement du jeu.
* @param wadName le nom du wad à charger pour le menu du launcher (sans extension).
* @param menuName le nom du fichier menu à utiliser pour le menu du launcher (sans extension).
* @return vrai si l'utilisateur choisit de lancer le jeu, faux si le programme doit quitter.
*
* Cette fonction appelle la fonction <em>outdatedFiles</em>.
* Si des fichiers à convertir sont trouvés, alors le menu de conversion s'affiche, et le jeu
* ne peut pas être lancé tant que la conversion n'est pas réalisée.
*
* La fonction <em>upgradeFile</em> est appelée pour chaque fichier à convertir.
*/
bool executeLauncher(const string& wadName, const string& menuName);

/**
* @brief Retourne la liste des fichiers dont la version est antérieure à la version actuelle de ce programme.
* @return la liste des noms des fichiers, avec chemin relatif à l'application et extension.
* @throw FileException si un répertoire ne peut pas être ouvert, ou si un fichier ne peut pas être ouvert.
*
* Les fichiers scannés sont les fichiers maps, wad et menus, d'extensions respectives MAPS_EXT, WAD_EXT et MENU_EXT (fichier "gen.h").
* Ces fichiers sont recherchés dans les répertoires MAPS_DIR, WAD_DIR et MENU_DIR (fichier "gen.h").
*
* La version actuelle du jeu est indiquée par PFGAME_VERSION (fichier "gen.h").
*
* @warning
* Les fichiers de versions postérieures ne sont pas retournés.
*/
vector<string> outdatedFiles();

/**
* @brief Met à jour le fichier dont le nom est passé en paramètre.
* @param fileName le nom du fichier à mettre à jour, avec le chemin relatif à l'application et l'extension.
* @throw FileException si le fichier ne peut pas être ouvert.
*
* L'extension du fichier détermine son contenu.
*
* @remarks
* Crée un fichier temporaire TMP_FILE (fichier "gen.h").
*/
void upgradeFile(const string& fileName);

/**
* @brief Méthode appelée par <em>upgradeFile</em> pour mettre à jour un fichier WAD si des slots ont été ajoutés dans le fichier "enum.h".
* @param c l'octet en cours de lecture.
* @param byteCount le nombre d'octets parcourus dans le fichier WAD.
*
* Tant que <em>byteCount</em> n'atteint pas la zone de déclaration des slots du fichier, cette fonction ne fait rien.
*
* Dès que cette zone est atteinte, l'ensemble des données stockées dans le fichier sont considérées être des groupes multiples de quatre octets.
* Cette fonction utilise alors la fonction <em>readIntFrom4Chars</em> et dès que la variable globale <em>g_readInt_v</em> à l'indice utilisé
* par cette fonction est égale à NEW_SLOT_FLAG (fichier "gen.h"),
* le groupe de 4 octets suivants servira à déterminer l'ancien slot à convertir à sa nouvelle valeur.
*
* Avant d'utiliser cette fonction, la variable globale g_upgradeFileAfterAddedSlot_slots_v (fichier "launcher.cpp") doit être initialisée.
* Ce vecteur contient la liste des nouvelles valeurs de l'énumération PfWadSlot.
*/
void upgradeWadAfterAddedSlot(char c, int byteCount);

/**
* @brief Méthode appelée par <em>upgradeFile</em> pour mettre à jour un fichier MEN si des slots ont été ajoutés dans le fichier "enum.h".
* @param c l'octet en cours de lecture.
* @param byteCount le nombre d'octets parcourus dans le fichier MEN.
*
* Cette fonction étudie chaque groupe de quatre octets jusqu'à rencontrer des flags spéciaux auxquels cas, selon le flag :
* <ul><li>NEW_SLOT_FLAG : elle modifie le slot selon la variable globale g_upgradeFileAfterAddedSlot_slots_v (fichier "launcher.cpp"),</li>
* <li>STRING_FLAG : elle liste chaque octet jusqu'à rencontrer '\0' avant de reprendre une lecture octet par octet.</li></ul>
*
* Avant d'utiliser cette fonction, la variable globale g_upgradeFileAfterAddedSlot_slots_v (fichier "launcher.cpp") doit être initialisée.
* Ce vecteur contient la liste des nouvelles valeurs de l'énumération PfWadSlot.
*/
void upgradeMenAfterAddedSlot(char c, int byteCount);

/**
* @brief Méthode appelée par <em>upgradeFile</em> pour mettre à jour un fichier MAP si des slots ont été ajoutés dans le fichier "enum.h".
* @param c l'octet en cours de lecture.
* @param byteCount le nombre d'octets parcourus dans le fichier MAP.
*
* Cette fonction étudie chaque groupe de quatre octets jusqu'à rencontrer des flags spéciaux auxquels cas, selon le flag :
* <ul><li>NEW_SLOT_FLAG : elle modifie le slot selon la variable globale g_upgradeFileAfterAddedSlot_slots_v (fichier "launcher.cpp"),</li>
* <li>STRING_FLAG : elle liste chaque octet jusqu'à rencontrer '\0' avant de reprendre une lecture octet par octet.</li></ul>
*
* Avant d'utiliser cette fonction, la variable globale g_upgradeFileAfterAddedSlot_slots_v (fichier "launcher.cpp") doit être initialisée.
* Ce vecteur contient la liste des nouvelles valeurs de l'énumération PfWadSlot.
*/
void upgradeMapAfterAddedSlot(char c, int byteCount);

/**
* @brief Convertit 4 char en un int, fonction adaptée à la lecture octet par octet de la fonction <em>upgradeFile</em>.
* @param c le char juste lu par la fonction <em>upgradeFile</em>.
* @param index l'indice des vecteurs de stockage à utiliser.
* @return INVALID_INT si l'entier n'est pas entièrement lu, la valeur <em>g_readInt</em> sinon.
*
* A chaque appel, cette fonction vérifie la valeur globale <em>g_readCharsCount_v</em> à l'indice spécifié (fichier "launcher.cpp").
* En fonction de cette valeur, la variable globale <em>g_readInt_v</em> à l'indice spécifié est mise à jour grâce au char passé en paramètre.
* Puis, la variable <em>g_readCharsCount_v</em> à l'indice spécifié est incrémentée.
*
* Quand <em>g_readCharsCount_v</em> à l'indice spécifié est égale à 4, elle repasse à 0 et la valeur <em>g_readInt_v</em> à l'indice spécifié
* est retournée par cette fonction avant de repasser nulle.
* Sinon, INVALID_INT (fichier "gen.h") est retournée.
*
* L'utilisation de vecteurs pour les variables globales permet d'utiliser cette fonction pour plusieurs cas en parallèle.
*/
int readIntFrom4Chars(char c, unsigned int index);

#endif // LAUNCHER_H_INCLUDED
