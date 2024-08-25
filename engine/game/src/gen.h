/**
* @file
* @author Anaïs Vernet
* @brief Header inclus dans tous les fichiers header de ce programme ainsi que dans le fichier "main.cpp".
* @date 15/07/2016
*
* Ce fichier définit les macros générales, dont NDEBUG qui doit être défini avant la release.
*
* Il contient la ligne :
*
* <code>using namespace std;</code>
*
* qui n'est plus à répéter dans les autres fichiers.
*
* Il déclare la variable globale <em>g_log</em> de type ofstream qui est initialisée dans le fichier "main.cpp".
*/

#ifndef GEN_H_INCLUDED
#define GEN_H_INCLUDED

#include "misc_gen.h"

#ifndef NDEBUG
#define DBG_MOVEZ // MapModel::moveZ
//#define DBG_ADDOBJECTS // MapModel::MapModel | MapEditorModel::MapEditorModel | MapEditorModel::addObject
#endif

#define PFGAME_VERSION 0 //!< L'identifiant de version du jeu.
#define SOUNDS_DIR "./res/sounds/" //!< Le répertoire des fichiers sons.
#define MUSIC_DIR "./res/music/" //!< Le répertoire des musiques.
#define WAD_DIR "./res/wad/" //!< Le répertoire des fichiers wads.
#define WAD_EXT "wad" //!< L'extension des fichiers wads.
#define TEXTURES_DIR "./res/textures/" //!< Le répertoire des fichiers textures.
#define TEXTURES_EXT "tex" //!< L'extension des fichiers textures.
#define MENU_DIR "./res/menu/" //!< Le répertoire des menus.
#define MENU_EXT "men" //!< L'extension des fichiers menus.
#define MAPS_DIR "./res/maps/" //!< Le répertoire des maps.
#define MAPS_EXT "map" //!< L'extension des fichiers map.
#define SYSTEM_FILE_PREFIX "PF_" //!< Le préfixe des fichiers ressources système.

#define MAP_NAME "zzMap2D" //!< Le nom d'un ModelItem Map : zz permet aux cases, à plan égal avec un objet, d'être affichées devant.
#define MAP_LAYER 500 //!< Le plan de perspective le plus en arrière d'une map 2D vue de haut.
#define MAP_MAX_LINES_COUNT 100 //!< Le nombre maximal de lignes ou de colonnes d'une map.
#define MAP_CELL_SIZE 0.08 //!< La taille d'une case de map.
#define TERRAIN_TEXTURE_INDEX 10000000 //!< Index de la texture de terrain.
#define TERRAIN_TEXTURE_INDEX_2 10000001 //!< Index de la texture des reliefs du terrain.
#define TERRAIN_CELLS_COUNT 16 //!< Le nombre de cases par ligne/colonne du fichier de texture du terrain.
#define TERRAIN_PIXEL_SIZE 1./1024 //!< La taille d'un pixel du fichier texture de terrains.
#define MAP_CELL_SQUARE_HEIGHT 16 //!< La hauteur Z d'une case correspondant aux dimensions de sa surface (multiple de 4 !).
#define MAP_MAX_HEIGHT 20*MAP_CELL_SQUARE_HEIGHT //!< La hauteur maximale sur une map.
#define MAP_Z_STEP_SIZE (MAP_CELL_SIZE/MAP_CELL_SQUARE_HEIGHT) //!< La taille d'un pas Z en pixels verticaux.
#define MAP_STEPS_PER_CELL 16 //!< Le nombre de pas par case.
#define MAP_STEP_SIZE (MAP_CELL_SIZE/MAP_STEPS_PER_CELL) //!< La taille d'un pas horizontal en pixels.
#define MAP_GRAVITY 1 //!< L'accélération de la pesanteur.

#endif // GEN_H_INCLUDED
