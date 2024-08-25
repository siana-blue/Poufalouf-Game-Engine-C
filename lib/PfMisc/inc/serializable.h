/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe Serializable.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef SERIALIZABLE_H_INCLUDED
#define SERIALIZABLE_H_INCLUDED

#include "misc_gen.h"

#include "datapackage.h"

/**
* @brief Interface d'objet sérialisable.
*
* Cette classe est typiquement héritée par héritage multiple.
*
* Un objet sérialisable dispose d'une méthode Serializable::saveData permettant sa sérialisation.
*
* La méthode Serializable::loadData permet d'utiliser les données sauvegardées pour mettre à jour cet objet.
*
* Certaines classes pourront définir un constructeur utilisant directement ces données et ne pas redéfinir Serializable::loadData.
*
* @section EnumSauvegarde Proposition d'un modèle de sérialisation
*
* Typiquement, la sérialisation se fait à l'aide d'une énumération spécifiant différentes sections de sauvegarde.
* Chaque section commence par l'enregistrement de la valeur d'énumération qui lui correspond, sous la forme d'une donnée destinée à un DataPackage.
* Ensuite, toujours selon la structure d'un DataPackage, les données de la section sont sauvegardées dans le fichier.
* Enfin, la dernière valeur de l'énumération, égale à SAVE_END_VALUE (fichier "misc_gen.h") et généralement nommée SAVE_END,
* indique que la sauvegarde est terminée.
*
* <code>enum SaveExample {SAVE_SECTION1, SAVE_SECTION2, SAVE_SECTION3, SAVE_END = SAVE_END_VALUE};<br>
* <br>
* void Example::saveData(ofstream& r_ofs) const<br>
* {<br>
*   WRITE_ENUM(r_ofs, Example::SAVE_SECTION1);<br>
*   WRITE_INT(r_ofs, value1);<br>
* <br>
*   WRITE_ENUM(r_ofs, Example::SAVE_SECTION2);<br>
*   WRITE_FLOAT(r_ofs, value2);<br>
*   WRITE_INT(r_ofs, value3);<br>
* <br>
*   WRITE_ENUM(r_ofs, Example::SAVE_SECTION3);<br>
*   WRITE_STRING(r_ofs, str);<br>
* <br>
*   WRITE_ENUM(r_ofs, Example::SAVE_END);<br>
* }<br>
* <br>
* void Example::loadData(DataPackage& r_data)<br>
* {<br>
* ..bool cnt = true;<br>
* ..int section;<br>
* ..while (cnt && !r_data.isOver())<br>
* ..{<br>
* ......section = r_data.nextEnum();<br>
* ......switch (section)<br>
* ......{<br>
* ..........case Example::SAVE_SECTION1:<br>
* ..............useValue(r_data.nextInt());<br>
* ..............break;<br>
* ..........case Example::SAVE_SECTION2:<br>
* ..............useValue(r_data.nextFloat());<br>
* ..............useValue(r_data.nextInt());<br>
* ..............break;<br>
* ..........case Example::SAVE_SECTION3:<br>
* ..............useValue(r_data.nextString());<br>
* ..............break;<br>
* ..........case Example::SAVE_END:<br>
* ..........default:<br>
* ..............cnt = false;<br>
* ..............break;<br>
* ......}<br>
* ..}<br>
* }</code>
*
* L'exemple de code ci-dessus est une structure minimale typique de sérialisation.
*
* Ainsi, si une classe sérialisable devait évoluer avec le projet, et avoir par exemple un membre supplémentaire à sauvegarder, il suffirait alors
* d'ajouter une valeur d'énumération de sauvegarde, avant SAVE_END, et les anciens fichiers de sauvegarde seraient toujours compatibles,
* car la méthode Serializable::loadData peut être définie de sorte à attribuer une valeur par défaut avant le switch au cas où elle ne soit pas trouvée
* dans le DataPackage.
*/
class Serializable
{
public:
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Enregistre le contenu de cet objet.
    * @param r_ofs Le flux en écriture.
    *
    * Les données devraient être stockées de façon à pouvoir être affectées à un DataPackage (indication du type de valeur avant chaque valeur).
    */
    virtual void saveData(ofstream& r_ofs) const = 0;
    /**
    * @brief Utilise les données sauvegardées.
    * @param r_ifs Le flux en lecture.
    *
    * Par défaut, ne fait rien.
    */
    virtual void loadData(ifstream& r_ifs) {}
    /**
    * @brief Utilise les données sauvegardées.
    * @param r_data Le DataPackage contenant les données à lire.
    *
    * Par défaut, ne fait rien.
    */
    virtual void loadData(DataPackage& r_data) {}
};

#endif // SERIALIZABLE_H_INCLUDED
