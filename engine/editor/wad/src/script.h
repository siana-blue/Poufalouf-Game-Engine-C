/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions de conversion de script.
* @date 31/07/2016
*/

#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "gen.h"
#include <string>

#define NEW_SLOT_FLAG (INVALID_INT+1) //!< Marque d'un nouvel objet dans le fichier WAD, utilisé par le fichier "launcher.h" du programme principal.

/**
* @brief Ajoute l'ensemble des ressources mentionnées dans un script TXT à un fichier WAD.
* @param r_txtFile le script TXT.
* @param r_wadFile le fichier WAD à remplir.
* @throw FileException si un fichier ressource ne peut pas être ouvert pour copie.
* @throw PfException si une erreur qui ne devrait jamais se produire se produit.
*
* Les flux doivent être ouverts et positionnés en début de fichier, juste après la version du jeu pour le fichier en écriture.
*
* Le flux en lecture est repositionné au début en fin de fonction.
*/
void addResToWad(ifstream& r_txtFile, ofstream& r_wadFile);

/**
* @brief Ajoute l'ensemble des objets trouvés dans un script TXT à un fichier WAD.
* @param r_txtFile le script TXT.
* @param r_wadFile le fichier WAD à remplir.
* @param fileName le nom du fichier TXT (pour les exceptions).
* @throw PfException si une exception survient.
*
* Les flux doivent être ouverts.
* Le flux en lecture doit être positionné en début de fichier ou du moins avant les lignes contenant les objets à lire.
* Le flux en écriture doit être positionné dans le fichier WAD à l'endroit où les objets doivent être ajoutés.
*
* Le flux en lecture est lu jusqu'à trouver un premier symbole $.
* Une fois trouvé, les lignes suivantes sont interprétées comme des données concernant un objet, en changeant d'objet à chaque $.
* Entre chaque $, la fonction <em>addAnimsToObject</em> est appelée.
*
* Les flux ne sont pas rembobinés en fin de fonction.
*/
void addObjectsToWad(ifstream& r_txtFile, ofstream& r_wadFile, const string& fileName);

/**
* @brief Ajoute les propriétés d'un objet dans un fichier wad.
* @param r_txtFile le script TXT.
* @param r_wadFile le fichier WAD à remplir.
* @param fileName le nom du script TXT (pour les exceptions).
* @throw ScriptException si une erreur est présente dans le script.
*
* Les flux doivent être ouverts et correctement positionnés, c'est-à-dire juste après une ligne déclarant un slot de WAD ($NOM_DU_SLOT) en
* ce qui concerne le fichier TXT.
*
* Le flux en lecture est balayé de la position d'entrée jusqu'à la première ligne qui ne commence pas par ':'.
*
* Les flux ne sont pas rembobinés en fin de fonction, le flux en lecture étant positionné au début de ligne de déclaration de la première animation
* ([STATUT]).
*/
void addPropertiesToObject(ifstream& r_txtFile, ofstream& r_wadFile, const string& fileName);

/**
* @brief Ajoute l'ensemble des animations d'un objet dans un fichier wad.
* @param r_txtFile le script TXT.
* @param r_wadFile le fichier WAD à remplir.
* @param fileName le nom du script TXT (pour les exceptions).
* @throw ScriptException si une erreur est présente dans le script.
* @throw PfException si une exception survient.
*
* Les flux doivent être ouverts et correctement positionnés.
* Si la première ligne n'est pas de la forme [STATUT], alors une ScriptException est levée.
*
* Le flux en lecture est balayé de la position d'entrée de fonction à la première ligne commençant par '$'.
*
* La fonction <em>addFrameToAnim</em> est appelée pour lire chaque ligne correspondant à une frame.
*
* Les flux ne sont pas rembobinés en fin de fonction, le flux en lecture étant positionné au début de la section suivante ('$').
*/
void addAnimsToObject(ifstream& r_txtFile, ofstream& r_wadFile, const string& fileName);

/**
* @brief Ajoute une frame à l'animation en cours d'écriture dans le WAD.
* @param str la ligne de script contenant la frame.
* @param r_wadFile le fichier WAD à remplir.
* @param fileName le nom du script TXT (pour les exceptions).
*
* Les flux doivent être ouverts et correctement positionnés.
*
* Une seule ligne est lue.
*
* L'octet correspondant à l'option WADTXT_FRM (fichier "enum.h") est ajoutée dans cette fonction.
*
* Le flux en écriture n'est pas rembobiné en fin de fonction.
*/
void addFrameToAnim(const string& str, ofstream& r_wadFile, const string& fileName);

/**
* @brief Convertit un fichier txt en fichier wad.
* @param fileName le nom du script à convertir.
* @throw PfException si une erreur survient lors de la conversion.
*
* Le fichier wad portera le même nom que le fichier txt, à l'extension près.
*/
void convertTxtToWad(const string& fileName);

#endif // SCRIPT_H_INCLUDED
