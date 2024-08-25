/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe AbstractModel.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef ABSTRACTMODEL_H_INCLUDED
#define ABSTRACTMODEL_H_INCLUDED

#include "mvc_gen.h"
#include <string>
#include <map>
#include <vector>
#include <typeinfo>
#include "noncopyable.h"

class AbstractView;
class ModelItem;

/**
* @brief Modèle MVC.
*
* Le modèle MVC stocke les données du programme sous forme de ModelItem.
* Ces ModelItem sont stockés dans une liste et envoyés à toutes les vues observant ce modèle.
* Les vues affichent alors les ModelItem visibles.
*
* Toutes les opérations à réaliser sur les objets de ce modèle sont réalisées par les méthodes de cette classe.
* Celles-ci sont appelées par le contrôleur de ce modèle durant son cycle de mise à jour.
*
* Les statuts des ModelItem sont gérés par ce modèle, grâce à la méthode AbstractModel::updateItems, redéfinie pour chaque type de modèle.
* Cette méthode est appelée après la mise à jour des vues.
* En effet, si un objet est ModelItem::DEAD, avant de le supprimer, il faut prévenir la vue MVC que cet objet est mort.
* La vue verra ce statut et supprimera le Viewable correspondant dans sa liste.
* Le modèle reprend ensuite la main et supprime cette fois le ModelItem de sa liste.
*
* @warning
* La liste de modèles AbstractModel::mpn_modelItems_map n'admet pas de pointeurs nuls. Des assertions vérifient ce point. De même pour la liste de vues.
*
* @warning
* Les ModelItem doivent être stockés dans la map avec pour clés leurs noms. Ceci est nécessaire pour la bonne gestion des ModelItem par la suite.
*/
class AbstractModel : private NonCopyable
{
public:
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Destructeur AbstractModel.
    *
    * Détruit tous les ModelItem de la liste.
    */
    virtual ~AbstractModel();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Ajoute une vue observant ce modèle.
    * @param r_view La vue à ajouter.
    *
    * Si la vue est déjà liée à ce modèle, alors rien n'est fait.
    */
    void addView(AbstractView& r_view);
    /**
    * @brief Met à jour toutes les vues observant ce modèle.
    *
    * Ceci se fait en deux étapes : la mise à jour du viewport via AbstractView::updateViewport à qui le résultat de AbstractModel::viewportData est
    * passé en paramètre, puis la mise à jour des Viewable de la vue via la liste AbstractModel::mpn_modelItems_map.
    */
    void notifyAll();
    /**
    * @brief Ajoute un ModelItem à la liste de ce modèle.
    * @param p_item L'objet à ajouter.
    *
    * @warning
    * L'objet ajouté sera détruit par le destructeur de cette classe.
    */
    void addItem(ModelItem* p_item);
    /**
    * @brief Demande à supprimer un ModelItem de la liste.
    * @param keyName La clé de l'objet à supprimer.
    * @throw ArgumentException si la clé n'est pas trouvée.
    *
    * Cette méthode se contente de placer le statut de l'objet à ModelItem::DEAD.
    * C'est la méthode AbstractModel::clearItem qui s'occupe de détruire l'objet, cette dernière méthode devant être appelée dans AbstractModel::updateItems.
    */
    void removeItem(const string& keyName);
    /**
    * @brief Passe le statut d'un objet à ModelItem::VISIBLE.
    * @param keyName La clé de l'objet à rendre visible.
    * @throw ArgumentException si la clé n'est pas trouvée.
    */
    void showItem(const string& keyName);
    /**
    * @brief Passe le statut d'un objet à ModelItem::INVISIBLE.
    * @param keyName La clé de l'objet à rendre invisible.
    * @throw ArgumentException si la clé n'est pas trouvée.
    */
    void hideItem(const string& keyName);
    /**
    * @brief Indique si un objet est présent dans la liste avec la clé spécifiée.
    * @param keyName La clé de l'objet dont la présence doit être testée.
    * @return <code>true</code> si l'objet est présent dans la liste.
    */
    bool contains(const string& keyName) const;
    /**
    * @brief Met à jour chaque ModelItem de la liste de ce modèle.
    *
    * C'est dans cette méthode que doit être gérée la suppression d'objet
    * ou leurs changements de statut.
    */
    virtual void updateItems() = 0;

protected:
    /**
    * @brief Supprime un ModelItem de la liste.
    * @param keyName La clé de l'objet à supprimer.
    * @throw ArgumentException si la clé n'est pas trouvée.
    */
    void clearItem(const string& keyName);
    /**
    * @brief Cherche l'objet à la clé spécifiée, du type spécifié.
    * @param keyName La clé de l'objet.
    * @return Un pointeur sur l'objet, 0 s'il n'est pas trouvé ou d'un autre type.
    */
    template <class T>
    T* findItem(const string& keyName)
    {
        map<string, ModelItem*>::iterator it=mpn_modelItems_map.find(keyName);
        if (it == mpn_modelItems_map.end())
            return 0;
        T* p_return;
        try
        {
            p_return = dynamic_cast<T*>(it->second);
        }
        catch (bad_cast& e)
        {
            p_return = 0;
        }
        return p_return;
    }
    /**
    * @brief Cherche l'objet à la clé spécifiée, du type spécifié.
    * @param keyName La clé de l'objet.
    * @return Un pointeur constant sur l'objet, 0 s'il n'est pas trouvé ou d'un autre type.
    */
    template <class T>
    const T* findConstItem(const string& keyName) const
    {
        map<string, ModelItem*>::const_iterator it=mpn_modelItems_map.find(keyName);
        if (it == mpn_modelItems_map.end())
            return 0;
        const T* pc_return;
        try
        {
            pc_return = dynamic_cast<const T*>(it->second);
        }
        catch (bad_cast& e)
        {
            pc_return = 0;
        }
        return pc_return;
    }
    /**
    * @brief Cherche tous les objets du type spécifié.
    * @return La liste des objets du type spécifié.
    *
    * Aucun élément retourné n'est un pointeur nul.
    */
    template <class T>
    vector<T*> findAllItems()
    {
        vector<T*> p_return_v;
        T* p_m = 0;
        for (map<string, ModelItem*>::iterator it=mpn_modelItems_map.begin();it!=mpn_modelItems_map.end();++it)
        {
            if (it->second == 0)
                p_m = 0;
            else
            {
                try
                {
                    p_m = dynamic_cast<T*>(it->second);
                }
                catch (bad_cast& e)
                {
                    p_m = 0;
                }
            }
            if (p_m != 0)
                p_return_v.push_back(p_m);
        }
        return p_return_v;
    }
    /**
    * @brief Retourne la liste des ModelItem de ce modèle.
    * @return La liste.
    */
    vector<ModelItem*> modelItems();
    /**
    * @brief Retourne une information permettant la mise à jour de la partie visible des vues surveillant ce modèle.
    * @return Une paire de coordonnées.
    */
    virtual pair<float, float> viewportData() const = 0;

private:
    map<string, ModelItem*> mpn_modelItems_map; //!< La liste des ModelItem de ce modèle, mémoire allouée dans cette classe (méthode AbstractModel::addItem).
    vector<AbstractView*> mp_views_v; //!< La liste des vues associées à ce modèle, pointeurs externes, pas d'allocation dans cette classe.
};

#endif // ABSTRACTMODEL_H_INCLUDED

