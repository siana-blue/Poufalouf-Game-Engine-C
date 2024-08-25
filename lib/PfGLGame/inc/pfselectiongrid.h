/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfSelectionGrid.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFSELECTIONGRID_H_INCLUDED
#define PFSELECTIONGRID_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"
#include "pfbutton.h"

/**
* @brief Grille de sélection, dont les cases contiennent des images.
*
* Les lignes sont notées à partir de 1 en haut.
*
* Cet objet est un simple fond noir et génère des boutons sous forme de cases cliquables.
* L'ajout d'animations à ce widget se traduit par l'ajout de l'animation à chaque case (qui sont en fait des PfWidget).
*
* Dans un wad, cet objet est défini au moyen des animations de ses cases, ce widget en lui-même étant représenté par un simple fond noir.
*
* Une grille de sélection possède une liste d'images.
* Un maximum d'images est représenté grâce aux cases disponibles, le reste de la liste étant masqué.
* Il faut alors faire défiler la grille pour accéder aux images suivantes, ce qui est géré par la variable PfSelectionGrid::m_steps.
*
* Chaque image est stockée dans cette classe au moyen d'une paire (indice de texture;rectangle de coordonnées de la texture).
*
* @warning
* Une grille ne peut avoir plus de 256 cases, afin de pouvoir combiner la valeur de la case à l'effet de cette grille (EFFECT_SELECT).
*
* Cet objet peut typiquement être couplé à une PfScrollBar pour gérer son défilement.
*/
class PfSelectionGrid : public PfWidget
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfSelectionGrid.
    * @param name Le nom de cette grille.
    * @param x L'abscisse de cette grille.
    * @param y L'ordonnée de cette grille.
    * @param w La largeur de cette grille.
    * @param h La hauteur de cette grille.
    * @param rows Le nombre de lignes de cette grille.
    * @param columns Le nombre de colonnes de cette grille.
    * @param layer Le plan de perspective de ce widget.
    * @param statusMap La valeur d'initialisation de la map d'association animation <-> état de ce widget.
    * @throw ConstructorException si une erreur survient lors de la génération de cet objet.
    *
    * @warning
    * Le nombre total de cases ne doit pas dépasser 256, sinon une exception est levée.
    */
    PfSelectionGrid(const string& name, float x, float y, float w, float h, unsigned int rows = 3, unsigned int columns = 3, unsigned int layer = MAX_LAYER,
                    PfWidget::PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
    /**
    * @brief Destructeur PfSelectionGrid.
    *
    * Détruit les boutons de cette grille.
    */
    virtual ~PfSelectionGrid();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Sélectionne la case aux coordonnées spécifiées.
    * @param row La ligne de la case à sélectionner.
    * @param col La colonne de la case à sélectionner.
    * @return RETURN_OK si une nouvelle case a été sélectionnée, RETURN_EMPTY si les coordonnées ne sont pas valides et RETURN_NOTHING si la case à sélectionner l'est déjà.
    * @throw PfException si l'animation ANIM_HIGHLIGHTED de la case n'est pas trouvée.
    *
    * Si les coordonnées ne sont pas valides, toutes les cases sont déselectionnées.
    *
    * Le booléen ModelItem::m_modified passe à l'état <code>true</code>.
    */
    PfReturnCode selectCell(unsigned int row, unsigned int col);
    /**
    * @brief Sélectionne la case aux coordonnées spécifiées, s'il y en a une.
    * @param rc_point Le point de sélection.
    * @return RETURN_OK si la sélection a lieu, RETURN_EMPTY si le point n'est sur aucune case et RETURN_NOTHING si la sélection n'a pas lieu car la case à sélectionner l'est déjà.
    *
    * Le booléen ModelItem::m_modified passe à l'état vrai.
    */
    PfReturnCode selectCellAt(const PfPoint& rc_point);
    /**
    * @brief Ajoute une image à cette grille.
    * @param textureIndex L'indice de texture de l'image.
    * @param rc_texCoordRect Le rectangle des coordonnées de la texture.
    */
    void addCellImage(unsigned int textureIndex, const PfRectangle& rc_texCoordRect);
    /**
    * @brief Modifie le pas d'affichage de cette grille.
    * @param step Le nouveau pas d'affichage.
    *
    * Si le pas d'affichage est trop élevé, alors le maximum possible est pris.
    *
    * Tous les boutons de cette grille sont quittés.
    *
    * @remarks
    * Ce ModelItem est marqué modifié.
    */
    void changeStep(unsigned int step);
    /*
    * Redéfinitions
    * -------------
    */
    /**
    * @brief Active ce widget.
    *
    * Appelle PfWidget::activate.
    *
    * Si le point d'activation n'est pas (0;0), alors appelle PfSelectionGrid::selectCellAt sur ce point.
    *
    * Si l'un des champs PfSelectionGrid::m_currentRow ou PfSelectionGrid::m_currentCol est nul, cette méthode est quittée.
    *
    * Associe à ce widget l'effet EFFECT_SELECT (énumération PfEffect, fichier "enum.h", bibliothèque PfMisc)
    * couplé à la valeur correspondant à la case sélectionnée.
    */
    virtual void activate();
    /**
    * @brief Ajoute une animation aux cases de cette grille.
    * @param p_anim L'animation à ajouter.
    * @param value Le statut de l'animation.
    * @throw PfException si une exception survient lors de l'ajout.
    *
    * Si une animation existe déjà pour ce statut, elle est remplacée.
    *
    * Le pointeur à ajouter ne doit pas être nul. Ce pointeur sera détruit par le destructeur du PfAnimationGroup de cette classe.
    *
    * Appelle la méthode AnimatedGLItem::addAnimation sur chaque case de cette grille.
    *
    * L'animation passée en paramètre est copiée puis détruite.
    */
    virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
    /**
    * @brief Met à jour ce ModelItem.
    *
    * Appelle la méthode PfWidget::update des widgets contenus dans cette grille.
    *
    * @remarks
    * Ce ModelItem est marqué modifié si l'un de ses boutons est modifié.
    */
    virtual void update();
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return Le Viewable créé.
    * @throw ViewableGenerationException si le premier bouton (0;0) est nul.
    *
    * Génère un fond noir, les images des objets contenus puis les Viewable de chaque case.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const;
    /*
    * Accesseurs
    * ----------
    */
    int getCurrentRow() const {return m_currentRow;} //!< Accesseur.
    int getCurrentCol() const {return m_currentCol;} //!< Accesseur.
    unsigned int getStep() const {return m_step;} //!< Accesseur.

private:
    unsigned int m_rowsCount; //!< Le nombre de lignes de cette grille.
    unsigned int m_columnsCount; //!< Le nombre de colonnes de cette grille.
    unsigned int m_currentRow; //!< La ligne de la case sélectionnée.
    unsigned int m_currentCol; //!< La colonne de la case sélectionnée.
    PfWidget*** mp_buttons_t2; //!< Le tableau des boutons de cette grille.
    vector<pair<unsigned int, PfRectangle> > m_images_v; //!< la liste des images de cette grille (indice de texture et rectangle de position).
    unsigned int m_step; //!< Le nombre de lignes d'images défilées vers le haut.
};

#endif // PFSELECTIONGRID_H_INCLUDED
