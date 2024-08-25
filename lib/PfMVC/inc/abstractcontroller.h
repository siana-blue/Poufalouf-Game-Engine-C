/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe AbstractController.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef ABSTRACTCONTROLLER_H_INCLUDED
#define ABSTRACTCONTROLLER_H_INCLUDED

#include "mvc_gen.h"
#include "datapackage.h"

class AbstractModel;

/**
* @brief Contrôleur MVC.
*
* Le contrôleur possède un pointeur vers un modèle qu'il contrôle.
* Le rôle du contrôleur est de traiter les entrées utilisateur et de mettre à jour le modèle MVC en appelant ses diverses méthodes.
*
* Le contrôleur définit le statut du système MVC auquel il appartient.
* Ce statut définit si le système est en fonctionnement ou s'il doit être mis en pause ou détruit.
*
* Le contrôleur possède une liste de valeurs pouvant être mises à jour pour donner une information à la fonction appelant le système MVC auquel il appartient.
*
* @warning
* Un pointeur nul pour le modèle n'est pas toléré, des assertions vérifient ce point.
*/
class AbstractController
{
public:
    /**
    * @brief Enumération des différents statuts d'un système MVC.
    */
    enum MVCStatus {ALIVE, //!< Système MVC en fonctionnement.
                    ASLEEP, //!< Système MVC en pause.
                    DEAD //!< Système MVC à détruire.
                    };
    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur AbstractController.
    * @param r_model Le modèle à observer.
    */
    explicit AbstractController(AbstractModel& r_model);
    /**
    * @brief Destructeur AbstractController.
    */
    virtual ~AbstractController() {}
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Lit les entrées de l'utilisateur et effectue les traitements nécessaires.
    */
    virtual void pollInput() = 0;
    /**
    * @brief Efface toute entrée utilisateur.
    *
    * Cette méthode est appelée par le système MVC à la place de AbstractController::pollInput quand AbstractController::isAvailable retourne <code>false</code>.
    */
    virtual void flushInput() = 0;
    /**
    * @brief Cycle de mise à jour du contrôleur : le modèle est mis à jour par appel à ses différentes méthodes.
    */
    virtual void update() = 0;
    /**
    * @brief Indique si ce contrôleur accepte les entrées utilisateurs.
    * @return <code>true</code> si les entrées utilisateur sont admises.
    */
    virtual bool isAvailable() const = 0;
    /**
    * @brief Passe le statut du MVCSystem à ALIVE s'il n'était pas DEAD.
    */
    void wakeUp();
    /*
    * Accesseurs
    * ----------
    */
    MVCStatus getStatus() const {return m_status;} //!< Accesseur.
    void setStatus(MVCStatus status) {m_status = status;} //!< Accesseur.
    const DataPackage& getValues() const {return m_values;} //!< Accesseur.

protected:
    AbstractModel* mp_model; //!< Le modèle lié à ce contrôleur.
    MVCStatus m_status; //!< Le statut du système MVC de ce contrôleur.
    DataPackage m_values; //!< La liste de valeurs permettant un transfert d'information vers la fonction appelante.
};

#endif // ABSTRACTCONTROLLER_H_INCLUDED

