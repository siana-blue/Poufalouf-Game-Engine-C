/**
* @file
* @author Anaïs Vernet
* @brief Fichier regroupant les classes d'utilité générale liées à la géométrie.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Ce fichier regroupe les classes définissant des formes simples (PfPoint, PfPolygon et PfRectangle),
* ainsi que les classes PfColor et PfOrientation.
*/

#ifndef GEOMETRY_H_INCLUDED
#define GEOMETRY_H_INCLUDED

#include "media_gen.h"

#include <vector>

/**
* @brief Objet représentant une orientation, une direction.
*
* Une PfOrientation est composée de deux coordonnées entières x et y et peut être construite directement à partir de valeurs prédéfinies.
*
* Une abscisse positive oriente cette PfOrientation vers l'est. Une ordonnée positive l'oriente vers le nord.
*
* Deux énumérations permettent la simplification de la définition d'une orientation. PfOrientation::PfCardinalPoint est une énumération simple regroupant
* les 4 points cardinaux ainsi que leurs combinaisons. PfOrientation::PfOrientationValue se présente sous forme de flags, et,
* bien que permettant à peu près la même chose au final, se manie différemment par combinaisons de valeurs.
*
* Il est également possible de construire une orientation directement à partir de pfflag (flags 8 bits, bibliothèque PfMisc), ce qui équivaut à une
* combinaison de PfOrientation::PfOrientationValue.
*
* Les coordonnées d'une PfOrientation sont des entiers. Libre à l'utilisateur de définir une échelle.
*/
class PfOrientation
{
public:
    /**
    * @brief Retourne une roue des points cardinaux dans le sens des aiguilles d'une montre, sous forme de flags 8 bits.
    * @return La liste des huit points cardinaux en partant du nord-ouest, compatibles avec l'énumération PfOrientation::PfOrientationValue.
    */
    static vector<pfflag> cardinalPoints();
    /**
    * @brief Enumération des points cardinaux.
    *
    * Cette énumération peut notamment servir à parcourir la roue retournée par la méthode statique PfOrientation::cardinalPoints.
    *
    * Pour définir une orientation, mieux vaut utiliser une combinaison de flags PfOrientation::PfOrientationValue.
    */
    enum PfCardinalPoint
    {
        CARDINAL_NW, //!< Nord-ouest.
        CARDINAL_N, //!< Nord.
        CARDINAL_NE, //!< Nord-est.
        CARDINAL_E, //!< Est.
        CARDINAL_SE, //!< Sud-est.
        CARDINAL_S, //!< Sud.
        CARDINAL_SW, //!< Sud-ouest.
        CARDINAL_W //!< Ouest.
    };
    /**
    * @brief Enumération des valeurs que peut prendre une PfOrientation.
    *
    * Les valeurs composées sont très peu utilisées, et ignorées par la majorité des méthodes de la classe PfOrientation.
    * A la place, c'est généralement une combinaison d'orientations simples (par le système de flags) qui est préférée.
    *
    * @remarks
    * Cette énumération prend des valeurs compatibles avec le système de flags.
    */
    enum PfOrientationValue
    {
        NO_ORIENTATION = 0, //!< Aucune orientation.
        SOUTH = 1, //!< Sud.
        WEST = 2, //!< Ouest.
        NORTH = 4, //!< Nord.
        EAST = 8, //!< Est.
        SOUTH_WEST = 16, //!< Sud-ouest.
        NORTH_WEST = 32, //!< Nord-ouest.
        NORTH_EAST = 64, //!< Nord-est.
        SOUTH_EAST = 128 //!< Sud-est.
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
    * @param x L'abscisse de cette PfOrientation.
    * @param y L'ordonnée de cette PfOrientation.
    */
    PfOrientation(int x, int y);
    /**
    * @brief Constructeur PfOrientation 2.
    * @param orientation La valeur permettant de générer une orientation prédéfinie.
    *
    * Les coordonnées sont chacune égale à 1, -1 ou 0.
    */
    PfOrientation(PfOrientationValue orientation);
    /**
    * @brief Constructeur PfOrientation 3.
    * @param orientation La combinaison de PfOrientationValue permettant de générer une orientation prédéfinie.
    *
    * Les coordonnées sont chacune égale à 1, -1 ou 0.
    */
    explicit PfOrientation(pfflag orientation);
    /**
    * @brief Constructeur PfOrientation 4.
    * @param orientation Le point cardinal servant d'orientation.
    *
    * Les coordonnées sont chacune égale à 1, -1 ou 0.
    */
    PfOrientation(PfCardinalPoint orientation);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Retourne la direction correspondant à cette orientation.
    * @return La direction correspondante.
    *
    * En cas de direction non simple, le point cardinal suivant dans le sens des aiguilles d'une montre est choisi.
    */
    PfOrientationValue toValue() const;
    /**
    * @brief Retourne la direction correspondant à cette orientation.
    * @return La direction correspondante.
    *
    * En cas de direction non simple, une énumération composée peut être choisie.
    */
    PfOrientationValue toExactValue() const;
    /**
    * @brief Retourne la direction simple opposée à cette orientation.
    * @return La direction simple opposée.
    *
    * La méthode PfOrientation::toValue est appelée pour simplifier cette orientation dans le cas où elle serait composée.
    */
    PfOrientationValue oppositeValue() const;
    /**
    * @brief Retourne le flag d'orientations correspondant à cette orientation.
    * @return La direction correspondante.
    *
    * Cette méthode, similaire à PfOrientation::toExactValue, permet de retourner une orientation composée, directement sous la forme de flags.
    */
    pfflag toFlag() const;
    /**
    * @brief Retourne le point cardinal correspondant à cette orientation.
    * @return La direction correspondante.
    *
    * Cette méthode, similaire à PfOrientation::toFlag, permet de retourner un point cardinal.
    * Si l'orientation est nulle, par défaut PfOrientation::CARDINAL_N est retourné (car on dit bien "toujours au nord", c'est pas ça ?).
    */
    PfCardinalPoint toCardinal() const;
    /**
    * @brief Retourne le point cardinal suivant.
    * @param clockwise <code>true</code> pour une rotation dans l'ordre des aiguilles d'une montre, <code>false</code> pour le sens contraire.
    * @return Un point cardinal calculé en considérant le point suivant à partir d'une conversion par la méthode PfOrientation::toCardinal de cette orientation.
    */
    PfCardinalPoint nextCardinal(bool clockwise = true) const;
    /**
    * @brief Retourne la direction opposée.
    * @return La direction opposée.
    *
    * Cette méthode fonctionne avec les directions composées.
    */
    pfflag opposite() const;
    /**
    * @brief Retourne la direction opposée sur l'axe Nord-Sud.
    * @return La direction opposée.
    *
    * Cette méthode retourne SE pour NE par exemple, ainsi que S pour N, mais retourne E pour E.
    */
    pfflag oppositeNS() const;
    /**
    * @brief Retourne la direction opposée sur l'axe Est-Ouest.
    * @return La direction opposée.
    *
    * Cette méthode retourne SW pour SE par exemple, ainsi que W pour E, mais retourne N pour N.
    */
    pfflag oppositeEW() const;
    /**
    * @brief Retourne un point cardinal opposé selon un axe.
    * @param ori L'orientation permettant de déterminer le côté à considérer.
    * @return La direction opposée.
    *
    * N'est adaptée que pour des points cardinaux composés. L'opposée de nord sera ici nord.
    *
    * Si <em>ori</em> est égal à PfOrientation::NORTH ou PfOrientation::SOUTH, l'opposé sera déterminé de sorte que NE retourne SE par exemple.
    *
    * Si <em>ori</em> est égal à PfOrientation::WEST ou PfOrientation::EAST, l'opposé sera déterminé de sorte que NE retourne NW par exemple.
    */
    PfCardinalPoint oppositeOnSameSide(PfOrientationValue ori) const;
    /**
    * @brief Retourne un entier à partir de cette orientation.
    * @return L'entier correspondant.
    *
    * NORTH | WEST correspond à 0 et WEST correspond à 7, les valeurs
    * intermédiaires étant incrémentées dans le sens des aiguilles d'une montre.
    *
    * Toute autre combinaison d'orientation retourne -1.
    */
    int toInt() const;
    /**
    * @brief Indique si cette orientation est perpendiculaire à une autre.
    * @param ori L'autre orientation à considérer.
    * @return <code>true</code> si les deux orientations sont perpendiculaires.
    */
    bool isPerpendicularTo(const PfOrientation& ori) const;
    /**
    * @brief Indique si cette orientation est parallèle à une autre.
    * @param ori L'autre orientation à considérer.
    * @return <code>true</code> si les deux orientations sont parallèles.
    */
    bool isParallelTo(const PfOrientation& ori) const;
    /**
    * @brief Indique si cette orientation est sur l'axe Nord-Sud.
    * @return <code>true</code> si c'est le cas.
    */
    bool isNS() const;
    /**
    * @brief Indique si cette orientation est sur l'axe Est-Ouest.
    * @return <code>true</code> si c'est le cas.
    */
    bool isEW() const;
    /**
    * @brief Indique si cette orientation est sur l'un des axes Nord-Sud ou Est-Ouest.
    * @return <code>true</code> si c'est le cas.
    */
    bool isNSEW() const;
    /**
    * @brief Indique si cette orientation est voisine du point cardinal spécifié.
    * @param ori Le point cardinal servant à la comparaison.
    * @return <code>true</code> si cette orientation est voisine du point cardinal spécifié.
    *
    * Cette orientation est transformée en point cardinal au moyen de la méthode PfOrientation::toCardinal pour procéder à la comparaison.
    *
    * @remarks
    * Retourne <code>false</code> si les deux orientations sont égales.
    */
    bool isNextTo(PfOrientation::PfCardinalPoint ori) const;
    /*
    * Accesseurs
    * ----------
    */
    int getX() const {return m_x;} //!< Accesseur.
    int getY() const {return m_y;} //!< Accesseur.

private:
    int m_x; //!< L'abscisse de cette orientation.
    int m_y; //!< L'ordonnée de cette orientation.
};
/**
* @brief Opérateur d'égalité de PfOrientation.
* @param a La première orientation.
* @param b La deuxième orientation.
* @return <code>true</code> si les deux orientations sont égales.
*
* Deux orientations sont égales si leurs directions sont égales, quelles que soient leurs normes.
*/
bool operator==(const PfOrientation& a, const PfOrientation& b);
/**
* @brief Opérateur de différence de PfOrientation.
* @param a La première orientation.
* @param b La deuxième orientation.
* @return <code>true</code> si les deux orientations ne sont pas égales.
*/
bool operator!=(const PfOrientation& a, const PfOrientation& b);

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
    * @param x L'abscisse de ce point.
    * @param y L'ordonnée de ce point.
    */
    PfPoint(float x = 0.0, float y = 0.0);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Déplace ce point selon l'orientation passée en paramètre.
    * @param orientation L'orientation dans laquelle déplacer ce point.
    * @param scale L'échelle de déplacement.
    *
    * Une unité d'orientation correspond à un décalage de ce point d'une unité multipliée par l'échelle de déplacement.
    */
    void shift(const PfOrientation& orientation, double scale = 1);
    /*
    * Accesseurs
    * ----------
    */
    float getX() const {return m_x;} //!< Accesseur.
    void setX(float x) {m_x = x;} //!< Accesseur.
    float getY() const {return m_y;} //!< Accesseur.
    void setY(float y) {m_y = y;} //!< Accesseur.

private:
    float m_x; //!< L'abscisse de ce point.
    float m_y; //!< L'ordonnée de ce point.
};
/**
* @brief Opérateur d'égalité de PfPoint.
* @param a Le premier point.
* @param b Le deuxième point.
* @return <code>true</code> si les deux points sont identiques.
*
* Deux points sont identiques s'ils ont les mêmes coordonées, à FLOAT_MARGIN (fichier "misc_gen.h", bibliothèque PfMisc) près.
*/
bool operator==(const PfPoint& a, const PfPoint& b);
/**
* @brief Opérateur de différence de PfPoint.
* @param a Le premier point.
* @param b Le deuxième point.
* @return <code>true</code> si les deux points ne sont pas égaux.
*/
bool operator!=(const PfPoint& a, const PfPoint& b);

class PfRectangle;

/**
* @brief Polygone constitué d'une série de PfPoint.
*
* @warning
* Pas plus de MAX_VERTICES_PER_POLYGON (fichier "media_gen.h") points pas polygone pour s'assurer de la compatiblité avec GLImage.
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
    * @param count Le nombre de sommets de ce polygone.
    * @throw ConstructorException si le paramètre est supérieur à MAX_VERTICES_PER_POLYGON.
    *
    * Tous les sommets sont à (0.0, 0.0).
    */
    explicit PfPolygon(int count);
    /**
    * @brief Constructeur PfPolygon 2.
    * @param rc_vertices_v Le tableau contenant les sommets de ce polygone.
    * @throw ConstructorException si le paramètre a plus de MAX_VERTICES_PER_POLYGON points.
    */
    explicit PfPolygon(const vector<PfPoint>& rc_vertices_v);
    /**
    * @brief Constructeur PfPolygon 3.
    * @param rectangle Le rectangle initialisant ce polygone.
    *
    * Le rectangle passé en paramètre est converti en polygone.
    *
    * Le polygone créé a pour points dans l'ordre : (x,y), (x,y+h), (x+w,y+h), (x+w,y) où x, y, w et h sont respectivement
    * l'abscisse, l'ordonnée, la largeur et la hauteur du rectangle.
    */
    PfPolygon(const PfRectangle& rectangle);
    /**
    * @brief Constructeur PfPolygon 4.
    * @param triangle1 Le premier triangle.
    * @param triangle2 Le deuxième triangle.
    * @throw ConstructorException si les triangles n'en sont pas.
    *
    * Constructeur servant à fusionner deux triangles partageant une arête en un quadrilatère.
    *
    * Aucun test n'est réalisé sur le fait que les triangles partagent une arête, mais la fusion considère que c'est le cas.
    *
    * L'algorithme considère que le premier point du premier triangle est situé en bas à gauche du quadrilatère.
    * Les points sont ensuite donnés dans le sens des aiguilles d'une montre.
    *
    * La diagonale entre triangles peut être dans les deux sens, l'algorithme le prend en compte en vérifiant l'abscisse du deuxième point de chaque
    * triangle et en prenant le point d'abscisse la plus faible. Pour le reste de l'algorithme, faire confiance à ce code !
    */
    PfPolygon(const PfPolygon& triangle1, const PfPolygon& triangle2);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Ajoute un point à ce polygone.
    * @param point Le point à ajouter.
    * @throw PfException si le point ajouté est d'indice supérieur à MAX_VERTICES_PER_POLYGON.
    *
    * Le point est ajouté à la fin.
    */
    void addPoint(const PfPoint& point);
    /**
    * @brief Ajoute un point à ce polygone.
    * @param x L'abscisse du point à ajouter.
    * @param y L'ordonnée du point à ajouter.
    * @throw PfException si le point ajouté est d'indice supérieur à MAX_VERTICES_PER_POLYGON.
    *
    * Le point est ajouté à la fin.
    */
    void addPoint(float x, float y);
    /**
    * @brief Déplace le point à l'indice spécifié.
    * @param index L'indice du point à modifier.
    * @param x La nouvelle abscisse du point.
    * @param y La nouvelle ordonnée du point.
    * @throw ArgumentException si l'indice n'est pas valide.
    */
    void replacePointAt(unsigned int index, float x, float y);
    /**
    * @brief Retourne une copie du PfPoint à l'indice spécifié.
    * @param index L'indice du point à retourner.
    * @return Le point à l'indice spécifié.
    * @throw ArgumentException si l'indice n'est pas valide.
    */
    PfPoint pointAt(unsigned int index) const;
    /**
    * @brief Retourne le nombre de sommets de ce polygone.
    * @return La taille du vecteur PfPolygon::m_vertices_v.
    */
    int count() const;
    /**
    * @brief Déplace ce polygone selon l'orientation passée en paramètre.
    * @param orientation L'orientation dans laquelle déplacer ce polygone.
    * @param scale L'échelle de déplacement.
    *
    * Une unité d'orientation correspond à un décalage de ce polygone d'une unité multipliée par l'échelle de déplacement.
    */
    void shift(const PfOrientation& orientation, double scale = 1);
    /**
    * @brief Modifie ce polygone pour le transformer par symétrie miroir.
    * @param ori L'orientation de la symétrie.
    * @param scale Le redimensionnement éventuel.
    *
    * La symétrie se fait par rapport au centre de l'objet retourné par la méthode PfPolygon::center.
    */
    void mirror(PfOrientation::PfCardinalPoint ori, double scale = 1);
    /**
    * @brief Modifie l'ordre des points de ce polygone pour les mettre dans le sens des aiguilles d'une montre en commençant par le point le plus en bas à gauche.
    *
    * Cet algorithme est certainement loin de fonctionner avec tous les polygones, surtout s'ils sont compliqués.
    * Mais pour des rectangles ou des parallélogrammes, c'est très bien.
    *
    * L'algorithme est détaillé ci-après.
    *
    * <ul><li>Prendre comme premier point le point le plus bas à gauche (tous les points de X mini sont sélectionnés et le plus en bas est retenu).</li>
    * <li>Tous les points de Y strictement supérieurs au premier sont sélectionnés, et celui de X mini est retenu.
    * S'il y a égalité, le point de Y le plus faible est pris.</li>
    * <li>Ceci est réalisé pour chaque point par rapport au précédent, jusqu'à ce qu'il n'y ait plus de points de Y supérieur au précédent.</li>
    * <li>Tous les points restants sont sélectionnés, et celui de Y le plus haut est retenu. S'il y a égalité, le point de X le plus faible est pris.</li>
    * <li>Ceci est fait jusqu'à avoir parcouru tous les points de ce polygone.</li></ul>
    */
    void rearrangeClockwise();
    /**
    * @brief Retourne le centre de ce polygone.
    * @return Le point dont les coordonnées sont celles du centre.
    *
    * Le centre calculé est le centre de gravité du polygone, en considérant chaque point de poids égal.
    */
    PfPoint center() const;
    /**
    * @brief Retourne l'abscisse minimale de ce polygone.
    * @return L'abscisse minimale.
    */
    float minX() const;
    /**
    * @brief Retourne l'abscisse maximale de ce polygone.
    * @return L'abscisse maximale.
    */
    float maxX() const;
    /**
    * @brief Retourne l'ordonnée minimale de ce polygone.
    * @return L'ordonnée minimale.
    */
    float minY() const;
    /**
    * @brief Retourne l'ordonnée maximale de ce polygone.
    * @return L'ordonnée maximale.
    */
    float maxY() const;
    /**
    * @brief Retourne la largeur maximale de ce polygone.
    * @return La largeur maximale.
    */
    float maxW() const;
    /**
    * @brief Retourne la hauteur maximale de ce polygone.
    * @return La hauteur maximale.
    */
    float maxH() const;

private:
    vector<PfPoint> m_vertices_v; //!< Le tableau de sommets.
};

/**
* @brief Rectangle de Poufalouf.
*
* Un PfRectangle est caractérisé par une abscisse, une ordonnée, une largeur, une hauteur et deux angles (x et y).
*
* La valeur de l'angle x est comprise entre -1 et 1 et correspond au décalage du côté haut vers la droite en fraction de la largeur de ce rectangle
* s'il était droit. Il en est de même pour l'angle y mais sur le côté droit vers le haut en fonction de la hauteur.
*
* Il s'agit bien d'un rectangle de Poufalouf car il peut n'être qu'un parallélépipède non droit.
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
    * @param w La largeur de ce rectangle.
    * @param h La hauteur de ce rectangle.
    *
    * Le rectangle créé a pour coordonnées (0,0).
    */
    PfRectangle(float w, float h);
    /**
    * @brief Constructeur PfRectangle 2.
    * @param x L'abscisse de ce rectangle.
    * @param y L'ordonnée de ce rectangle.
    * @param w La largeur de ce rectangle.
    * @param h La hauteur de ce rectangle.
    * @param angleX L'angle x de ce rectangle.
    * @param angleY L'angle y de ce rectangle.
    */
    PfRectangle(float x, float y, float w, float h, float angleX = 0.0, float angleY = 0.0);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Crée un polygone à partir de ce rectangle.
    * @return Le polygone créé.
    *
    * Le polygone créé a pour points dans l'ordre :
    * (x,y), (x,y+h), (x+w,y+h), (x+w,y) où x, y, w et h sont respectivement l'abscisse, l'ordonnée, la largeur et la hauteur de ce rectangle.
    *
    * @remarks
    * Quelque peu redondant avec le constructeur PfPolygon 3.
    */
    PfPolygon toPfPolygon() const;
    /**
    * @brief Retourne le centre de ce rectangle.
    * @return Le centre de ce rectangle.
    */
    PfPoint center() const;
    /**
    * @brief Crée un triangle à partir de ce rectangle.
    * @param edge L'orientation du coin à sélectionner.
    * @return Le triangle créé.
    * @throw ArgumentException si le paramètre <em>edge</em> est un point cardinal N, S, E ou W. Il faut une direction composée.
    *
    * Les points du triangle sont donnés dans le sens des aiguilles d'une montre en partant du point inférieur gauche.
    */
    PfPolygon toTriangle(PfOrientation::PfCardinalPoint edge) const;
    /**
    * @brief Indique si un point est compris dans ce rectangle.
    * @param rc_point Le point à considérer.
    * @param bordersIncluded <code>true</code> si les bordures du rectangle doivent être considérées comme partie de l'intérieur.
    * @return <code>true</code> si le point est contenu dans ce rectangle.
    */
    bool contains(const PfPoint& rc_point, bool bordersIncluded = true) const;
    /**
    * @brief Indique si un rectangle est inclus (entièrement ou partiellement) dans ce rectangle.
    * @param rc_rect Le rectangle à considérer.
    * @param bordersIncluded <code>true</code> si les bordures du rectangle doivent être considérées comme partie de l'intérieur.
    * @return <code>true</code> si le rectangle passé en paramètre est inclus dans ce rectangle.
    *
    * @warning
    * Le rectangle <em>rc_rect</em> est considéré droit.
    */
    bool contains(const PfRectangle& rc_rect, bool bordersIncluded = true) const;
    /**
    * @brief Déplace ce rectangle selon l'orientation passée en paramètre.
    * @param orientation L'orientation dans laquelle déplacer ce rectangle.
    * @param scale L'échelle de déplacement.
    *
    * Une unité d'orientation correspond à un décalage de ce rectangle d'une unité multipliée par l'échelle de déplacement.
    */
    void shift(const PfOrientation& orientation, double scale = 1);
    /**
    * @brief Redimensionne ce rectangle.
    * @param scale L'échelle de l'homothétie en pourcentage des dimensions initiales de ce rectangle.
    */
    void resize(double scale);
    /*
    * Accesseurs
    * ----------
    */
    float getX() const {return m_x;} //!< Accesseur.
    void setX(float x) {m_x = x;} //!< Accesseur.
    float getY() const {return m_y;} //!< Accesseur.
    void setY(float y) {m_y = y;} //!< Accesseur.
    float getW() const {return m_w;} //!< Accesseur.
    void setW(float w) {m_w = w;} //!< Accesseur.
    float getH() const {return m_h;} //!< Accesseur.
    void setH(float h) {m_h = h;} //!< Accesseur.
    float getAngleX() const {return m_angleX;} //!< Accesseur.
    float getAngleY() const {return m_angleY;} //!< Accesseur.

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
    enum PfColorValue {WHITE, //!< Blanc.
                       BLACK, //!< Noir.
                       RED, //!< Rouge.
                       GREEN, //!< Vert.
                       BLUE, //!< Bleu.
                       YELLOW, //!< Jaune.
                       CYAN, //!< Cyan.
                       MAGENTA, //!< Magenta.
                       GRAY, //!< Gris.
                       BROWN //!< Marron.
                       };
    #define ENUM_COLOR_VALUE_COUNT 10 //!< Le nombre de valeurs pour l'énumération PfColorValue.

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
    * @param r La proportion de rouge.
    * @param g La proportion de vert.
    * @param b La proportion de bleu.
    *
    * Toute valeur supérieure à 1.0 est ramenée à 1.0, et toute valeur inférieure à 0.0 est ramenée à 0.0.
    */
    PfColor(float r, float g, float b);
    /**
    * @brief Constructeur PfColor 2.
    * @param color La couleur à construire.
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
    * @param color La couleur à comparer à celle-ci.
    * @return <code>true</code> si les composantes R, G et B des deux couleurs sont respectivement égales.
    */
    bool operator==(const PfColor& color);
    /**
    * @brief Opérateur de différence PfColor.
    * @param color La couleur à comparer à celle-ci.
    * @return <code>true</code> si les composantes R, G et B des deux couleurs ne sont pas toutes respectivement égales.
    */
    bool operator!=(const PfColor& color);

    /*
    * Accesseurs
    * ----------
    */
    float getR() const {return m_r;} //!< Accesseur.
    float getG() const {return m_g;} //!< Accesseur.
    float getB() const {return m_b;} //!< Accesseur.

private:
    float m_r; //!< la proportion de rouge.
    float m_g; //!< la proportion de vert.
    float m_b; //!< la proportion de bleu.
};

#endif // GEOMETRY_H_INCLUDED

