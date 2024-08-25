/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe ModelItem.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef MODELITEM_H_INCLUDED
#define MODELITEM_H_INCLUDED

#include "mvc_gen.h"
#include <string>
#include "noncopyable.h"

class Viewable;

/**
* @brief Objet utilisé pour stocker les informations d'un modèle MVC.
*
* Cet objet est capable de générer un Viewable en vue d'être affiché par une vue MVC.
*
* Il s'agit d'une classe abstraite qui ne définit ici qu'un constructeur standard.
*
* Les statuts des ModelItem sont censés définir leur comportement vis-à-vis du modèle lors de la mise à jour de la vue à laquelle il est lié.
*
* Lors de la mise à jour des Viewable de la vue, si le statut est ModelItem::UNCHANGED, rien n'est
* fait à propos de l'objet dans le modèle. Si le Viewable correspondant dans la vue est visible, alors il reste visible, et de même s'il
* est invisible. Les statuts ModelItem::VISIBLE et ModelItem::INVISIBLE servent à dire à la vue de changer la visibilité de cet objet. Enfin,
* le statut ModelItem::DEAD prépare la suppression de cet objet. Voir la documentation des éléments du MVCSystem pour le détail du fonctionnement
* de ces mises à jour d'objets.
*
* Si cet objet est marqué modifié, il générera un nouveau Viewable lors de la mise à jour de la vue. Ce Viewable sera alors visible sauf si
* le statut actuel de cet objet est ModelItem::INVISIBLE.
*
* Il est nécessaire de passer le booléen ModelItem::m_modified à <code>true</code> pour chaque modification, autrement il ne sera pas pris en compte
* lors de l'affichage par la vue, qui ne génèrera pas de nouveau Viewable pour refléter le nouvel état.
*/
class ModelItem : private NonCopyable
{
public:
    /**
    * @brief Enumération des statuts d'un ModelItem.
    */
    enum ModelItemStatus {UNCHANGED, //!< ModelItem inchangé.
                          VISIBLE, //!< ModelItem visible.
                          INVISIBLE, //!< ModelItem invisible.
                          DEAD //!< ModelItem à supprimer.
                          };
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur ModelItem par défaut.
    * @param name le nom de ce ModelItem.
    *
    * Ce ModelItem est modifié et visible.
    */
    explicit ModelItem(const string& name = "");
    /**
    * @brief Destructeur ModelItem.
    */
    virtual ~ModelItem() {}
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Met à jour ce ModelItem.
    */
    virtual void update() = 0;
    /**
    * @brief Génère un Viewable à partir de ce ModelItem.
    * @return le Viewable créé.
    *
    * @warning
    * De la mémoire est allouée pour le pointeur retourné.
    */
    virtual Viewable* generateViewable() const = 0;
    /*
    * Accesseurs
    * ----------
    */
    const string& getName() const {return m_name;} //!< Accesseur.
    void setName(const string& name) {m_name = name;} //!< Accesseur.
    ModelItemStatus getStatus() const {return m_status;} //!< Accesseur.
    void setStatus(ModelItemStatus status) {m_status = status;} //!< Accesseur.
    bool isModified() const {return m_modified;} //!< Accesseur.
    void setModified(bool modified) {m_modified = modified;} //!< Accesseur.

protected:
    string m_name; //!< Le nom de ce ModelItem.
    ModelItemStatus m_status; //!< Le statut de ce ModelItem.
    bool m_modified; //!< Indique si ce ModelItem a été modifié.
};

#endif // MODELITEM_H_INCLUDED

