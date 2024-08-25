/**
* @file
* @author Anaïs Vernet
* @brief Fichier regroupant les classes liées à la gestion d'un set de textures.
* @date 17/06/2015
*/

#ifndef TEXTURES_H_INCLUDED
#define TEXTURES_H_INCLUDED

#include "gen.h"
#include <map>
#include <vector>
#include <string>
#include "geometry.h"

/**
* @brief Retourne un rectangle de coordonnées à partir d'un indice de case d'une texture composée.
* @param index le numéro de la case.
* @param rowsCount le nombre de lignes dans le fichier texture.
* @param columnsCount le nombre de colonnes dans le fichier texture.
* @param xFraction la fraction horizontale de la case à retourner, 1.0 pour une case entière.
* @param yFraction la fraction verticale de la case à retourner, 1.0 pour une case entière.
* @param xOffset le décalage horizontal de la case à retourner, 0.0 pour commencer en début de case, 1.0 pour la case suivante.
* @param yOffset le décalage vertical de la case à retourner, 0.0 pour commencer en début de case, 1.0 pour la case suivante.
* @return un rectangle de coordonnées.
*
* Si l'argument <em>index</em> est négatif, un rectangle (0, 0, 0, 0) est retourné. Cela permet de traiter le retour -1 des méthodes <em>[x]Index</em> de la classe PfMapTextureSet.
*
* Les cases sont numérotées de 0 à (rowsCount*columnsCount-1), en ligne de gauche à droite de la ligne la plus haute à la ligne la plus basse.
*
* @warning
* En l'état actuel du programme, la taille des pixels utilisés par OpenGL est fixée à TERRAIN_PIXEL_SIZE pour cette fonction.
*/
PfRectangle rectFromTextureIndex(int index, unsigned int rowsCount = TERRAIN_CELLS_COUNT, unsigned int columnsCount = TERRAIN_CELLS_COUNT,
								float xFraction = 1.0, float yFraction = 1.0, float xOffset = 0.0, float yOffset = 0.0);


/**
* @brief Jeu de textures utilisables sur une map 2D vue de haut.
*
* Un jeu de textures contient un fichier image principal et un ou plusieurs fichiers images annexes.
* Ces fichiers sont tous contenus dans la liste PfMapTextureSet::m_terrainIDs.
* Chaque fichier est constitué de cases repérées par des indices numérotés à partir de 0,
* partant du bord supérieur gauche pour se terminer au bord inférieur droit, en parcourant colonnes puis lignes.
*
* La première valeur du champ PfMapTextureSet::m_terrainIDs_v indique l'indice de texture du terrain de base,
* les terrains annexes étant précisés dans la suite de la liste.
*
* Des liens peuvent être faits entre le fichier de terrain principal et les autres terrains, comme indiqués dans les descriptions des méthodes concernées.
*
* Le champ statique PfMapTextureSet::s_newTerrainIndex a pour première valeur TERRAIN_TEXTURE_INDEX (fichier "gen.h").
* Ce champ est ensuite incrémenté d'un nombre d'unités égal aux nombres d'indices de terrain disponibles par PfMapTextureSet créé.
*
* La map g_texIndexes_map du fichier "textures.cpp" recense les indices de textures affectés
* aux terrains de base pour chaque nom de fichier texture chargé auparavant.
* Ainsi, si la texture a déjà été ajoutée au contexte OpenGL, le champ statique
* PfMapTextureSet::s_newTerrainIndex n'est pas utilisé ni incrémenté, les textures ne sont pas rechargées.
*
* Les textures du terrain de base ont chacune une priorité en cas de recouvrement. Par défaut, cette priorité est 0.
* La map PfMapTextureSet::m_spreadingLayers_map associe à chaque case une priorité.
* Si deux priorités sont égales, l'indice de la case détermine la priorité, l'indice le plus élevé étant au plan le plus en avant.
*/
class PfMapTextureSet
{
	public:
		static unsigned int s_newTerrainIndex; //!< Le nouvel indice de texture terrain à utiliser.

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfMapTextureSet.
		* @param texName le nom du fichier texture à charger (sans l'extension).
		* @throw ConstructorException si le fichier n'est pas valide.
		*
		* Si le nom de fichier passé en paramètres est vide, alors le jeu de texture est créé avec un nom vide et aucune valeur.
		* Ceci est nécessaire pour un constructeur par défaut.
		*
		* Le fichier n'est pas considéré valide si ce jeu de texture n'a pas au moins un fichier de terrain chargé.
		*/
		explicit PfMapTextureSet(const string& texName = "");
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Retourne l'indice de terrain à l'index spécifié.
		* @param index l'indice dans la liste des fichiers de terrain.
		* @return l'indice de terrain.
		* @throw ArgumentException si l'indice n'est pas valide.
		*/
		unsigned int terrainId(unsigned int index) const;
		/**
		* @brief Retourne la priorité d'une case de texture concernant les débordements.
		* @param index l'indice de la case du terrain principal à considérer.
		* @return la priorité.
		*/
		int spreadingLayer(unsigned int index) const;
		/**
		* @brief Retourne l'indice de texture correspondant au débordement lié à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @return l'indice du fichier texture, 0 si aucun lien n'est fait à l'indice spécifié.
		*
		* Le débordement est la texture qui sera appliquée à une case voisine d'une case d'une autre texture, afin de faire un transition douce.
		*/
		unsigned int spreadingTexture(unsigned int index) const;
		/**
		* @brief Retourne l'indice d'une case de débordement liée à une texture du fichier principal.
		* @param index l'indice de la case du terrain principal à considérer.
		* @param ori l'orientation du débordement.
		* @return l'indice de la case de texture du débordement, -1 si aucun lien n'est fait à l'indice spécifié ou si l'orientation spécifiée conduit à sortir du fichier texture.
		*
		* Le débordement est la texture qui sera appliquée à une case voisine d'une case d'une autre texture, afin de faire un transition douce.
		*
		* Les débordements dans le fichier texture sont toujours présentés sous forme d'un anneau entourant la case centrale.
		* L'indice lié à la case du fichier principal est l'indice de la case centrale de cet anneau, et l'orientation permet de retourner la partie de l'anneau adaptée par simple calcul.
		* S'il se trouve par erreur que l'indice de case du terrain principal soit lié à une case qui soit sur un bord du fichier, -1 est retourné pour les orientations non couvertes.
		*
		* @remarks
		* Cette méthode est à utiliser en lien avec PfMapTextureSet::spreadingTexture.
		*/
		int spreadingIndex(unsigned int index, PfOrientation::PfCardinalPoint ori) const;
		/**
		* @brief Retourne l'indice de texture correspondant à la bordure liée à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @return l'indice du fichier texture, 0 si aucun lien n'est fait à l'indice spécifié.
		*
		* Les bordures d'une case sont les textures appliquées sur l'intérieur d'une case avant une falaise.
		*/
		unsigned int borderTexture(unsigned int index) const;
		/**
		* @brief Retourne l'indice de texture correspondant à la bordure liée à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @param ori l'orientation de la bordure.
		* @return l'indice de la case de texture de la bordure, -1 si aucun lien n'est fait à l'indice spécifié ou si l'orientation spécifiée conduit à sortir du fichier texture.
		*
		* Les bordures d'une case sont les textures appliquées sur l'intérieur d'une case avant une falaise.
		*
		* Les bordures dans le fichier texture sont présentées en ligne sur quatre cases successives dans l'ordre N-E-S-O.
		*
		* @remarks
		* Cette méthode est à utiliser en lien avec PfMapTextureSet::borderTexture.
		*/
		int borderIndex(unsigned int index, PfOrientation::PfOrientationValue ori) const;
		/**
		* @brief Retourne l'indice de texture correspondant à la falaise liée à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @return l'indice du fichier texture, 0 si aucun lien n'est fait à l'indice spécifié.
		*
		* Il s'agit des textures représentant la falaise sous une case.
		*/
		unsigned int cliffTexture(unsigned int index) const;
		/**
		* @brief Retourne l'indice de texture correspondant à la falaise liée à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @param ori l'orientation de l'ombre dans le cas d'une ombre.
		* @return l'indice de la case de texture de la falaise, -1 si aucun lien n'est fait à l'indice spécifié ou si l'orientation spécifiée conduit à sortir du fichier texture.
		*
		* Il s'agit des textures représentant la falaise sous une case.
		*
		* Les textures de falaises dans le fichier texture sont présentées en ligne sur trois cases successives dans l'ordre falaise-ombre ouest-ombre est.
		*
		* Si le paramètre <em>ori</em> est PfOrientation::WEST ou PfOrientation::EAST, l'ombre correspondante est retournée, sinon, la texture principales de la falaise est retournée.
		*
		* @remarks
		* Cette méthode est à utiliser en lien avec PfMapTextureSet::cliffTexture.
		*/
		int cliffIndex(unsigned int index, PfOrientation::PfOrientationValue ori = PfOrientation::NO_ORIENTATION) const;
		/**
		* @brief Retourne l'indice de texture correspondant au débordement à-pic au-dessus d'une falaise liée à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @return l'indice du fichier texture, 0 si aucun lien n'est fait à l'indice spécifié.
		*
		* Il s'agit des textures représentant par exemple l'herbe qui surplombe la falaise.
		*/
		unsigned int cliffBorderTexture(unsigned int index) const;
		/**
		* @brief Retourne l'indice de texture correspondant au débordement à-pic lié à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @param ori l'orientation du débordement.
		* @return l'indice de la case de texture du débordement à-pic, -1 si aucun lien n'est fait à l'indice spécifié ou si l'orientation spécifiée conduit à sortir du fichier texture.
		*
		* Il s'agit des textures représentant par exemple l'herbe qui surplombe la falaise.
		*
		* Les débordements à-pic dans le fichier texture sont toujours présentés sous forme d'un anneau entourant la case centrale.
		* L'indice lié à la case du fichier principal est l'indice de la case centrale de cet anneau, et l'orientation permet de retourner la partie de l'anneau adaptée par simple calcul.
		* S'il se trouve par erreur que l'indice de case du terrain principal soit lié à une case qui soit sur un bord du fichier, -1 est retourné pour les orientations non couvertes.
		*
		* @remarks
		* Cette méthode est à utiliser en lien avec PfMapTextureSet::cliffBorderTexture.
		*/
		int cliffBorderIndex(unsigned int index, PfOrientation::PfCardinalPoint ori) const;
		/**
		* @brief Retourne l'indice de texture correspondant au débordement au sol sous une falaise liée à l'indice de case du terrain principal spécifié.
		* @param index l'indice de la case du terrain principal à considérer.
		* @return l'indice du fichier texture, 0 si aucun lien n'est fait à l'indice spécifié.
		*
		* Il s'agit des textures représentant par exemple l'herbe qui recouvre le bas d'une falaise.
		*/
		unsigned int cliffFloorTexture(unsigned int index) const;
		/**
		* @brief Retourne l'indice d'une case de débordement liée à une texture du fichier principal.
		* @param index l'indice de la case du terrain principal à considérer.
		* @param ori l'orientation du débordement.
		* @return l'indice de la case de texture du débordement, -1 si aucun lien n'est fait à l'indice spécifié ou si l'orientation spécifiée conduit à sortir du fichier texture.
		*
		* Il s'agit des textures représentant par exemple l'herbe qui recouvre le bas d'une falaise.
		*
		* Les débordements dans le fichier texture sont présentés en ligne, la première texture étant le débordement au coin ouest de la falaise, puis sur la falaise et enfin à l'est.
		* Si <em>ori</em> est égal à PfOrientation::WEST ou PfOrientation::EAST, les débordements correspondants sont retournés, sinon le recouvrement central est retourné.
		*
		* @remarks
		* Cette méthode est à utiliser en lien avec PfMapTextureSet::cliffFloorTexture.
		*/
		int cliffFloorIndex(unsigned int index, PfOrientation::PfOrientationValue ori = PfOrientation::NO_ORIENTATION) const;
		/*
		* Accesseurs
		* ----------
		*/
		const string& getName() const {return m_name;}

	private:
		string m_name; //!< Le nom du fichier de textures utilisé pour ce jeu de textures.
		vector<unsigned int> m_terrainIDs_v; //!< La liste des indices de texture des images du terrain.
		map<unsigned int, int> m_spreadingLayers_map; //!< La map des priorités de recouvrement <indice de case sur le terrain principal, priorité>.
		map<unsigned int, pair<unsigned int, int> > m_spreadingLinks_map; //!< La map des liens vers les débordements de cases entre les deux fichiers terrains <indice texture, case>.
		map<unsigned int, pair<unsigned int, int> > m_bordersLinks_map; //!< La map des liens vers les bordures des cases <indice texture, case>.
		map<unsigned int, pair<unsigned int, int> > m_cliffsLinks_map; //!< La map des liens vers les textures de falaise des cases <indice texture, case>.
		map<unsigned int, pair<unsigned int, int> > m_cliffBordersLinks_map; //!< La map des liens vers les bordures de falaises des cases <indice texture, case>.
		map<unsigned int, pair<unsigned int, int> > m_cliffFloorLinks_map; //!< La map des liens vers le recouvrement en bas de falaise <indice texture, case>.
};

#endif // TEXTURES_H_INCLUDED
