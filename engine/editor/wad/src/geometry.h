/**
* @file
* @author Poufalouf
* @brief Fichier regroupant les classes d'utilité générale liées à la géométrie.
* @version 0.0
* @date 08/08/2014
*/

#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include "gen.h"
#include <vector>

class PfOrientation;

/**
* @brief Point sur un plan bidimensionnel, dont les coordonnées sont repérées par des <em>float</em>.
*/
class PfPoint
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfPoint par défaut.
		* @param x l'abscisse de ce point.
		* @param y l'ordonnée de ce point.
		*/
		PfPoint(float x = 0.0, float y = 0.0);

		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Déplace ce point selon l'orientation passée en paramètre.
		* @param orientation l'orientation dans laquelle déplacer ce point.
		* @param scale l'échelle de déplacement.
		*
		* Une unité d'orientation correspond à un décalage de ce point d'une unité multipliée par l'échelle de déplacement.
		*/
		void shift(const PfOrientation& orientation, double scale = 1);
		/*
		* Accesseurs
		* ----------
		*/
		float getX() const {return m_x;}
		void setX(float x) {m_x = x;}
		float getY() const {return m_y;}
		void setY(float y) {m_y = y;}

	private:
		float m_x; //!< L'abscisse de ce point.
		float m_y; //!< L'ordonnée de ce point.
};

class PfRectangle;

/**
* @brief Polygone constitué d'une série de PfPoint.
*
* @warning
* Pas plus de MAX_VERTICES_PER_POLYGON (fichier "gen.h") points pas polygone pour s'assurer de la compatiblité avec GLImage.
*/
class PfPolygon
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfPolygon par défaut.
		*
		* Le polygone créé n'a aucun point.
		*/
		PfPolygon() {}
		/**
		* @brief Constructeur PfPolygon 1.
		* @param count le nombre de sommets de ce polygone.
		* @throw ConstructorException si le paramètre est supérieur à MAX_VERTICES_PER_POLYGON (fichier "gen.h").
		*
		* Tous les sommets sont à (0.0, 0.0).
		*/
		PfPolygon(int count);
		/**
		* @brief Constructeur PfPolygon 2.
		* @param vertices_v le tableau contenant les sommets de ce polygone.
		* @throw ConstructorException si le paramètre a plus de MAX_VERTICES_PER_POLYGON (fichier "gen.h") points.
		*/
		PfPolygon(const vector<PfPoint>& vertices_v);
		/**
		* @brief Constructeur PfPolygon 3.
		* @param rectangle le rectangle initialisant ce polygone.
		*
		* Le rectangle passé en paramètre est converti en polygone.
		*
		* Le polygone créé a pour points dans l'ordre : (x,y), (x,y+h), (x+w,y+h), (x+w,y) où x, y, w et h sont respectivement
		* l'abscisse, l'ordonnée, la largeur et la hauteur du rectangle.
		*/
		PfPolygon(const PfRectangle& rectangle);
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Ajoute un point à ce polygone.
		* @param point le point à ajouter.
		* @throw PfException si le point ajouté est d'indice supérieur à MAX_VERTICES_PER_POLYGON (fichier "gen.h").
		*
		* Le point est ajouté à la fin.
		*/
		void addPoint(const PfPoint& point);
		/**
		* @brief Ajoute un point à ce polygone.
		* @param x l'abscisse du point à ajouter.
		* @param y l'ordonnée du point à ajouter.
		* @throw PfException si le point ajouté est d'indice supérieur à MAX_VERTICES_PER_POLYGON (fichier "gen.h").
		*
		* Le point est ajouté à la fin.
		*/
		void addPoint(float x, float y);
		/**
		* @brief Déplace le point à l'indice spécifié.
		* @param index l'indice du point à modifier.
		* @param x la nouvelle abscisse du point.
		* @param y la nouvelle ordonnée du point.
		* @throw ArgumentException si l'indice n'est pas valide.
		*/
		void replacePointAt(unsigned int index, float x, float y);
		/**
		* @brief Retourne une copie du PfPoint à l'indice spécifié.
		* @param index l'indice du point à retourner.
		* @return le point à l'indice spécifié.
		* @throw ArgumentException si l'indice n'est pas valide.
		*/
		PfPoint pointAt(unsigned int index) const;
		/**
		* @brief Retourne le nombre de sommets de ce polygone.
		* @return la taille du vecteur PfPolygon::m_vertices_v.
		*/
		int count() const;
		/**
		* @brief Déplace ce polygone selon l'orientation passée en paramètre.
		* @param orientation l'orientation dans laquelle déplacer ce polygone.
		* @param scale l'échelle de déplacement.
		*
		* Une unité d'orientation correspond à un décalage de ce polygone d'une unité multipliée par l'échelle de déplacement.
		*/
		void shift(const PfOrientation& orientation, double scale = 1);
		/**
		* @brief Retourne le centre de ce polygone.
		* @return le point dont les coordonnées sont celles du centre.
		*
		* Le centre calculé est le centre de gravité du polygone, en considérant chaque point de poids égal.
		*/
		PfPoint center() const;
		/**
		* @brief Retourne l'abscisse minimale de ce polygone.
		* @return l'abscisse minimale.
		*/
		float minX() const;
		/**
		* @brief Retourne l'abscisse maximale de ce polygone.
		* @return l'abscisse maximale.
		*/
		float maxX() const;
		/**
		* @brief Retourne l'ordonnée minimale de ce polygone.
		* @return l'ordonnée minimale.
		*/
		float minY() const;
		/**
		* @brief Retourne l'ordonnée maximale de ce polygone.
		* @return l'ordonnée maximale.
		*/
		float maxY() const;

	protected:
		vector<PfPoint> m_vertices_v; //!< Le tableau de sommets.
};

/**
* @brief Rectangle de Poufalouf.
*
* Un PfRectangle est caractérisé par une abscisse, une ordonnée, une largeur, une hauteur et deux angles (x et y).
*
* La valeur de l'angle x est comprise entre -1 et 1 et correspond au décalage du côté haut en fraction de la largeur de ce rectangle s'il était droit.
* Il en est de même pour l'angle y mais sur le côté droit en fonction de la hauteur.
*
* Il s'agit bien d'un rectangle de Poufalouf car il peut n'être qu'un parallélépipède non droit.
*
* <b>Exemple</b>
*
*                       ||
*                     || |
*    --------       ||   |
*   -      -        |   ||
*  -      -         | ||
* --------          ||
*
* A gauche  : w = 8 ; h = 4 ; angle x = 0.5 ; angle y = 0
*
* A droite : w = 6 ; h = 4 ; angle x = 0 ; angle y = 0.75
*/
class PfRectangle
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfRectangle par défaut.
		*
		* Le rectangle créé a pour coordonnées et pour dimensions 0.
		*/
		PfRectangle();
		/**
		* @brief Constructeur PfRectangle 1.
		* @param w la largeur de ce rectangle.
		* @param h la hauteur de ce rectangle.
		*
		* Le rectangle créé a pour coordonnées (0,0).
		*/
		PfRectangle(float w, float h);
		/**
		* @brief Constructeur PfRectangle 2.
		* @param x l'abscisse de ce rectangle.
		* @param y l'ordonnée de ce rectangle.
		* @param w la largeur de ce rectangle.
		* @param h la hauteur de ce rectangle.
		* @param angleX l'angle x de ce rectangle.
		* @param angleY l'angle y de ce rectangle.
		*/
		PfRectangle(float x, float y, float w, float h, float angleX = 0, float angleY = 0);
		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Crée un polygone à partir de ce rectangle.
		* @return le polygone créé.
		*
		* Le polygone créé a pour points dans l'ordre :
		* (x,y), (x,y+h), (x+w,y+h), (x+w,y) où x, y, w et h sont respectivement l'abscisse, l'ordonnée, la largeur et la hauteur de ce rectangle.
		*
		* @remarks
		* Quelque peu redondant avec le constructeur PfPolygon 3.
		*/
		PfPolygon toPfPolygon() const;
		/**
		* @brief Crée un triangle à partir de ce rectangle.
		* @param edge la combinaison de PfOrientation::PfOrientationValue indiquant le coin à sélectionner.
		* @return le triangle créé.
		* @throw ArgumentException si le paramètre <em>edge</em> n'est pas une combinaison de points cardinaux valide.
		*
		* Les points du triangle sont donnés dans le sens des aiguilles d'une montre en partant du point inférieur gauche.
		*/
		PfPolygon toTriangle(pfflag32 edge) const;
		/**
		* @brief Indique si un point est compris dans ce rectangle.
		* @param rc_point le point à considérer.
		* @param bordersIncluded vrai si les bordures du rectangle doivent être considérées comme partie de l'intérieur.
		*
		* Une marge de FLOAT_MARGIN est ajoutée au test pour valider les points sur la bordure (comparaison par égalite de float nécessitant une marge).
		*/
		bool contains(const PfPoint& rc_point, bool bordersIncluded = true) const;
		/**
		* @brief Indique si un rectangle est inclus (entièrement ou partiellement) dans ce rectangle.
		* @param rc_rect le rectangle à considérer.
		* @param bordersIncluded vrai si les bordures du rectangle doivent être considérées comme partie de l'intérieur.
		*
		* Une marge de FLOAT_MARGIN est ajoutée au test pour valider les points sur la bordure (comparaison par égalite de float nécessitant une marge).
		*
		* @warning
		* Le rectangle <em>rc_rect</em> est considéré droit.
		*/
		bool contains(const PfRectangle& rc_rect, bool bordersIncluded = true) const;
		/**
		* @brief Déplace ce rectangle selon l'orientation passée en paramètre.
		* @param orientation l'orientation dans laquelle déplacer ce rectangle.
		* @param scale l'échelle de déplacement.
		*
		* Une unité d'orientation correspond à un décalage de ce rectangle d'une unité multipliée par l'échelle de déplacement.
		*/
		void shift(const PfOrientation& orientation, double scale = 1);
		/**
		* @brief Redimensionne ce rectangle.
		* @param scale l'échelle de l'homothétie en pourcentage des dimensions initiales de ce rectangle.
		*/
		void resize(double scale);
		/*
		* Accesseurs
		* ----------
		*/
		float getX() const {return m_x;}
		float getY() const {return m_y;}
		float getW() const {return m_w;}
		float getH() const {return m_h;}
		float getAngleX() const {return m_angleX;}
		float getAngleY() const {return m_angleY;}

	private:
		float m_x; //!< L'abscisse de ce rectangle.
		float m_y; //!< L'ordonnée de ce rectangle.
		float m_w; //!< La largeur de ce rectangle.
		float m_h; //!< La hauteur de ce rectangle.
		float m_angleX; //!< L'angle x de ce rectangle.
		float m_angleY; //!< L'angle y de ce rectangle.
};

/**
* @brief Couleur RGB.
*/
class PfColor
{
	public:
		/**
		* @brief Enumération de valeurs prédéfinies de couleur.
		*/
		enum PfColorValue {WHITE, //!< blanc
						   BLACK, //!< noir
						   RED, //!< rouge
						   GREEN, //!< vert
						   BLUE //!< bleu
						  };
		#define ENUM_COLOR_VALUE_COUNT 5

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfColor par défaut.
		*
		* Construit la couleur blanche.
		*/
		PfColor();
		/**
		* @brief Constructeur PfColor 1.
		* @param r la proportion de rouge.
		* @param g la proportion de vert.
		* @param b la proportion de bleu.
		*
		* Toute valeur supérieure à 1.0 est ramenée à 1.0, et toute valeur inférieure à 0.0 est ramenée à 0.0.
		*/
		PfColor(float r, float g, float b);
		/**
		* @brief Constructeur PfColor 2.
		* @param color la couleur à construire.
		*
		* Construit une couleur selon des paramètres prédéfinis.
		*/
		PfColor(PfColorValue color);

		/*
		* Opérateurs
		* ----------
		*/
		/**
		* @brief Opérateur d'égalité PfColor.
		* @param color la couleur à comparer à celle-ci.
		* @return vrai si les composantes R, G et B des deux couleurs sont respectivement égales.
		*/
		bool operator==(const PfColor& color);
		/**
		* @brief Opérateur de différence PfColor.
		* @param color la couleur à comparer à celle-ci.
		* @return vrai si les composantes R, G et B des deux couleurs ne sont pas toutes respectivement égales.
		*/
		bool operator!=(const PfColor& color);

		/*
		* Accesseurs
		* ----------
		*/
		float getR() const {return m_r;}
		float getG() const {return m_g;}
		float getB() const {return m_b;}

	private:
		float m_r; //!< la proportion de rouge.
		float m_g; //!< la proportion de vert.
		float m_b; //!< la proportion de bleu.
};

/**
* @brief Orientation.
*
* Une PfOrientation est composée de deux coordonnées entières x et y et peut être construite directement à partir de valeurs prédéfinies.
*
* Une abscisse positive oriente cette PfOrientation vers l'est. Une ordonnée positive l'oriente vers le nord.
*/
class PfOrientation
{
	public:
		/**
		* @brief Retourne une "roue" des points cardinaux dans le sens des aiguilles d'une montre.
		* @return la liste des huit points cardinaux en partant du nord-ouest.
		*/
		static vector<pfflag32> cardinalPoints();
		/**
		* @brief Enumération des valeurs que peut prendre une PfOrientation.
		*
		* @remarks
		* Les différentes valeurs sont choisies comme pour des flags dans l'idée future de pouvoir combiner des orientations.
		*/
		enum PfOrientationValue {NO_ORIENTATION = 0, //!< aucune orientation
								 SOUTH = 1, //!< sud
								 WEST = 2, //!< ouest
								 NORTH = 4, //!< nord
								 EAST = 8 //!< est
								};

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfOrientation par défaut.
		*
		* Construit une orientation nulle (0;0).
		*/
		PfOrientation();
		/**
		* @brief Constructeur PfOrientation 1.
		* @param x l'abscisse de cette PfOrientation.
		* @param y l'ordonnée de cette PfOrientation.
		*/
		PfOrientation(int x, int y);
		/**
		* @brief Constructeur PfOrientation 2.
		* @param orientation la valeur permettant de générer une orientation
		* prédéfinie.
		*
		* Les coordonnées sont chacune égale à 1, -1 ou 0.
		*/
		PfOrientation(PfOrientationValue orientation);
		/**
		* @brief Constructeur PfOrientation 3.
		* @param orientation la combinaison de PfOrientationValue permettant de générer une orientation prédéfinie.
		*
		* Les coordonnées sont chacune égale à 1, -1 ou 0.
		*/
		PfOrientation(pfflag32 orientation);

		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Retourne la direction correspondant à cette orientation.
		* @return la direction correspondante.
		*
		* En cas de direction non simple, le premier point cardinal dans le sens des aiguilles d'une montre est choisi.
		*/
		PfOrientationValue toValue() const;
		/**
		* @brief Retourne la direction simple opposée à cette orientation.
		* @return la direction simple opposée.
		*
		* La méthode PfOrientation::toValue est appelée pour simplifier cette orientation dans le cas où elle serait composée.
		*/
		PfOrientationValue oppositeValue() const;
		/**
		* @brief Retourne le flag d'orientations correspondant à cette orientation.
		* @return la direction correspondante.
		*
		* Cette méthode, similaire à PfOrientation::toValue, permet de retourner une orientation composée.
		*/
		pfflag32 toFlag() const;
		/**
		* @brief Retourne la direction opposée.
		* @return la direction opposée.
		*
		* Cette méthode fonctionne avec les directions composées.
		*/
		pfflag32 opposite() const;
		/**
		* @brief Retourne la direction opposée sur l'axe Nord-Sud.
		* @return la direction opposée.
		*
		* Cette méthode retourne SE pour NE par exemple, ainsi que S pour N, mais retourne E pour E.
		*/
		pfflag32 oppositeNS() const;
		/**
		* @brief Retourne la direction opposée sur l'axe Est-Ouest.
		* @return la direction opposée.
		*
		* Cette méthode retourne SW pour SE par exemple, ainsi que W pour E, mais retourne N pour N.
		*/
		pfflag32 oppositeEW() const;
		/**
		* @brief Retourne un entier à partir de cette orientation.
		* @return l'entier correspondant.
		*
		* NORTH | WEST correspond à 0 et WEST correspond à 7, les valeurs
		* intermédiaires étant incrémentées dans le sens des aiguilles d'une montre.
		*
		* Toute autre combinaison d'orientation retourne -1.
		*/
		int toInt() const;
		/**
		* @brief Indique si cette orientation est perpendiculaire à une autre.
		* @param ori l'autre orientation à considérer.
		* @return vrai si les deux orientations sont perpendiculaires.
		*/
		bool isPerpendicularTo(const PfOrientation& ori) const;
		/**
		* @brief Indique si cette orientation est parallèle à une autre.
		* @param ori l'autre orientation à considérer.
		* @return vrai si les deux orientations sont parallèles.
		*/
		bool isParallelTo(const PfOrientation& ori) const;
		/**
		* @brief Indique si cette orientation est sur l'axe Nord-Sud.
		* @return vrai si c'est le cas.
		*/
		bool isNS() const;
		/**
		* @brief Indique si cette orientation est sur l'axe Est-Ouest.
		* @return vrai si c'est le cas.
		*/
		bool isEW() const;

		/*
		* Accesseurs
		* ----------
		*/
		int getX() const {return m_x;}
		int getY() const {return m_y;}

	private:
		int m_x; //!< L'abscisse de cette orientation.
		int m_y; //!< L'ordonnée de cette orientation.
};
/**
* @brief Opérateur d'égalité de PfOrientation.
* @param a la première orientation.
* @param b la deuxième orientation.
* @return vrai si les deux orientations sont égales.
*
* Deux orientations sont égales si leurs directions sont égales, quelles que soient leurs normes.
*/
bool operator==(const PfOrientation& a, const PfOrientation& b);
/**
* @brief Opérateur de différence de PfOrientation.
* @param a la première orientation.
* @param b la deuxième orientation.
* @return vrai si les deux orientations ne sont pas égales.
*/
bool operator!=(const PfOrientation& a, const PfOrientation& b);

#endif // TOOLS_H_INCLUDED

