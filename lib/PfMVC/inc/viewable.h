/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Viewable.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef VIEWABLE_H_INCLUDED
#define VIEWABLE_H_INCLUDED

#include "mvc_gen.h"
#include <string>
#include <vector>
#include "glimage.h"
#include "geometry.h"
#include "noncopyable.h"

/**
* @brief Objet généré par un ModelItem pour être représenté sur une vue MVC.
*
* Le Viewable répartit les informations d'un ModelItem selon différentes variables, et c'est la vue qui décide comment afficher chacune de ces variables.
* Un même Viewable peut être affiché différemment en fonction de la vue qui l'affiche.
*
* Le nom d'un Viewable créé lui sert d'identifiant. Il n'est pas modifiable.
*
* Un Viewable peut être visible ou invisible (champ Viewable::m_visible).
* S'il est invisible, la vue ne devrait pas l'afficher.
*
* Un Viewable possède une liste de GLImage affichées dans l'ordre du vecteur.
*
* Un Viewable possède un indice de son à jouer lors de son affichage.
*
* Un Viewable possède un plan de perspective Viewable::m_layer. Plus le plan de perspective est faible, plus le Viewable sera en arrière sur l'image.
* La vue affiche en effet les images dans l'ordre de leurs plans de perspective, recouvrant les plans les plus faibles avec les plans les plus élevés.
*
* Un Viewable peut posséder des liens vers d'autres Viewable.
* Ces autres Viewable ne sont normalement pas ajoutés directement à la vue, et ce afin de créer des groupes de Viewable qui naissent et disparaissent en même temps.
* Le nom des Viewables liés n'a pas d'importance.
*
* Si un Viewable possède plusieurs images sur le même plan, mieux vaut utiliser la liste d'images, car plus rapide à rendre.
* Pour avoir un Viewable constitué de plusieurs images sur des plans de perspective différents, alors le lien vers d'autre Viewable doit être utilisé.
*
* Tout ce qui est écrit ci-dessus dépend bien sûr de la façon dont la vue est codée, mais il s'agit de la conception imaginée.
*
* Le destructeur de cette classe détruit également tous les Viewable liés.
*/
class Viewable : private NonCopyable
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur Viewable par défaut.
    * @param name Le nom de ce Viewable.
    *
    * L'image de ce Viewable est créée par défaut. Il n'est pas visible et son plan de perspective est 0.
    */
    explicit Viewable(const string& name = "");
    /**
    * @brief Constructeur Viewable 1.
    * @param name Le nom de ce Viewable.
    * @param glImage La GLImage de ce Viewable.
    * @param layer Le plan de perspective de ce Viewable, correspondant à l'ordre d'affichage sur la vue (0 : premier affiché, arrière-plan...).
    * @param soundIndex L'indice de son (0 pour aucun son).
    *
    * Le Viewable construit est visible.
    */
    Viewable(const string& name, const GLImage& glImage, int layer = 0, unsigned int soundIndex = 0);
    /**
    * @brief Constructeur Viewable 2.
    * @param name Le nom de ce Viewable.
    * @param polygon Le polygone de l'image à créer.
    * @param layer Le plan de perspective de ce Viewable, correspondant à l'ordre d'affichage sur la vue (0 : premier affiché, arrière-plan...).
    * @param soundIndex L'indice de son (0 pour aucun son).
    * @param color La couleur de ce Viewable.
    * @param line Indique si l'image ajoutée est rendue sous forme de lignes.
    * @param coordRelativeToBorder <code>true</code> si le polygone est centré entre les bordures de la vue.
    * @param stat <code>true</code> pour une image ne dépendant pas de la caméra.
    * @throw ConstructorException si une erreur survient lors de la génération de la GLImage.
    *
    * L'image de ce Viewable n'est pas texturée.
    *
    * Le Viewable construit est visible.
    */
    Viewable(const string& name, const PfPolygon& polygon, int layer = 0, unsigned int soundIndex = 0, const PfColor& color = PfColor::WHITE, bool line = false,
            bool coordRelativeToBorder = true, bool stat = false);
    /**
    * @brief Constructeur Viewable 3.
    * @param name Le nom de ce Viewable.
    * @param polygon Le polygone de l'image à créer.
    * @param layer Le plan de perspective de ce Viewable, correspondant à l'ordre d'affichage sur la vue (0 : premier affiché, arrière-plan...).
    * @param textureIndex L'indice de texture de cette image, 0 pour une image non texturée. Il s'agit de l'indice correspondant au wad actuellement utilisé.
    * @param soundIndex L'indice de son (0 pour aucun son).
    * @param coordPolygon Le polygone des coordonnées du fragment de texture à utiliser.
    * @param color La couleur de ce Viewable.
    * @param coordRelativeToBorder <code>true</code> si le polygone est centré entre les bordures de la vue.
    * @param stat <code>true</code> pour une image ne dépendant pas de la caméra.
    * @throw ConstructorException si une erreur survient lors de la génération de la GLImage.
    *
    * Le Viewable construit est visible.
    */
    Viewable(const string& name, const PfPolygon& polygon, int layer, unsigned int textureIndex, unsigned int soundIndex,
            const PfPolygon& coordPolygon = PfPolygon(PfRectangle(1.0, 1.0)), const PfColor& color = PfColor::WHITE, bool coordRelativeToBorder = true, bool stat = false);
    /**
    * @brief Destructeur Viewable.
    *
    * Les GLImage de la liste et les Viewable liés à celui-ci sont détruits.
    */
    ~Viewable();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Ajoute une image à la liste des GLImage de ce Viewable.
    * @param p_glImage L'image à ajouter.
    *
    * Un pointeur nul n'est pas ajouté.
    *
    * @warning
    * L'image n'est pas copiée, c'est un pointeur qui est utilisé.
    * Elle sera détruite par le destructeur Viewable.
    */
    void addImage(GLImage* p_glImage);
    /**
    * @brief Ajoute des images à la liste des GLImage de ce Viewable.
    * @param p_glImages_v_r La liste des images à ajouter.
    *
    * Les images sont ajoutées dans l'ordre du vecteur passé en paramètre, à la suite des GLImage déjà présentes.
    *
    * Les images nulles ne sont pas ajoutées.
    *
    * @warning
    * Les images ne sont pas copiées, ce sont des pointeurs qui sont utilisés.
    * Elles seront détruites par le destructeur Viewable.
    */
    void addImages(vector<GLImage*>& p_glImages_v_r);
    /**
    * @brief Ajoute les images d'un Viewable à celui-ci.
    * @param viewable Le Viewable dont les images doivent être copiées.
    *
    * Les images du Viewable passé en paramètre sont copiées et ajoutées à la liste des GLImage de ce Viewable.
    * Les images nulles ne sont pas copiées.
    */
    void addImages(const Viewable& viewable);
    /**
    * @brief Lie un Viewable à celui-ci.
    * @param p_viewable le Viewable à lier.
    *
    * Si le paramètre est un pointeur nul, alors rien n'est fait.
    *
    * @warning
    * Le Viewable lié sera détruit par le destructeur de ce Viewable.
    */
    void addViewable(Viewable* p_viewable);
    /**
    * @brief Retourne le nombre de GLImage de ce Viewable.
    * @return Le nombre d'images.
    */
    unsigned int imagesCount() const;
    /**
    * @brief Retourne la GLImage à l'indice spécifié.
    * @param index L'indice de l'image à retourner.
    * @return L'image à l'indice spécifié.
    * @throw ArgumentException si l'indice n'est pas valide.
    */
    const GLImage& imageAt(unsigned int index) const;
    /**
    * @brief Retourne le nombre de Viewable liés à celui-ci.
    * @return Le nombre de Viewable liés.
    */
    unsigned int viewablesCount() const;
    /**
    * @brief Retourne le Viewable à l'indice spécifié.
    * @param index L'indice du Viewable à retourner.
    * @return Le Viewable à l'indice spécifié.
    * @throw ArgumentException si l'indice n'est pas valide.
    */
    const Viewable* viewableAt(unsigned int index) const;
    /**
    * @brief Modifie le plan de perspective de ce Viewable.
    * @param layer Le nouveau plan de perspective du Viewable.
    *
    * Si des Viewable sont liés à celui-ci avec des plans de perspective différents, alors la différence est conservée.
    *
    * Par exemple :
    *
    * Viewable de layer 0, deux Viewable liés avec pour plans de perspective respectifs 2 et 3. Paramètre <em>layer</em> = 4.
    *
    * Après exécution de cette méthode, le Viewable principal a pour plan de perspective 4, et les deux Viewable liés ont pour plans de perspective
    * respectifs 6 et 7.
    */
    void changeLayer(int layer);
    /**
    * @brief Modifie l'angle des images et des Viewable liés à ce Viewable.
    * @param angle Le nouvel angle.
    */
    void changeAngle(double angle);
    /*
    * Accesseurs
    * ----------
    */
    const string& getName() const {return m_name;} //!< Accesseur.
    bool isVisible() const {return m_visible;} //!< Accesseur.
    void setVisible(bool visible) {m_visible = visible;} //!< Accesseur.
    int getLayer() const {return m_layer;} //!< Accesseur.
    unsigned int getSoundIndex() const {return m_soundIndex;} //!< Accesseur.
    void setSoundIndex(unsigned int soundIndex) {m_soundIndex = soundIndex;} //!< Accesseur.
    double getAngle() const {return m_angle;} //!< Accesseur.

private:
    string m_name; //!< Le nom de ce Viewable.
    bool m_visible; //!< Indique si ce Viewable est visible à l'écran.
    int m_layer; //!< Le plan de perspective de ce Viewable.
    vector<GLImage*> mp_glImages_v; //!< La liste des images de ce Viewable.
    vector<Viewable*> mp_viewables_v; //!< La liste des Viewable liés à celui-ci.
    unsigned int m_soundIndex; //!< L'indice du son à jouer.
    double m_angle; //!< L'angle de l'image de ce Viewable.
};

#endif // VIEWABLE_H_INCLUDED

