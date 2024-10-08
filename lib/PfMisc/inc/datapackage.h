/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe DataPackage.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef DATAPACKAGE_H_INCLUDED
#define DATAPACKAGE_H_INCLUDED

#include "misc_gen.h"

#include <vector>
#include <fstream>

#define WRITE_INT(a, b) DataPackage::writeValue<int>(a, b, DataPackage::DT_INT) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_UINT(a, b) DataPackage::writeValue<unsigned int>(a, b, DataPackage::DT_UINT) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_CHAR(a, b) DataPackage::writeValue<char>(a, b, DataPackage::DT_CHAR) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_UCHAR(a, b) DataPackage::writeValue<unsigned char>(a, b, DataPackage::DT_UCHAR) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_FLOAT(a, b) DataPackage::writeValue<float>(a, b, DataPackage::DT_FLOAT) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_DOUBLE(a, b) DataPackage::writeValue<double>(a, b, DataPackage::DT_DOUBLE) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_STRING(a, b) DataPackage::DataType __WS_TYPE = DataPackage::DT_STRING;(a).write((char*) &__WS_TYPE, sizeof(DataPackage::DataType));writeString(a, b) //!< Lien vers la fonction <em>writeString</em> (fichier "misc.h") pour cohérence.
#define WRITE_ENUM(a, b) DataPackage::writeValue<int>(a, b, DataPackage::DT_ENUM) //!< Macro simplifiant l'appel à DataPackage::writeValue.
#define WRITE_END(a) DataPackage::writeEnd(a) //!< Lien vers DataPackage::writeEnd pour cohérence.

/**
* @brief Groupe de données.
*
* Un DataPackage permet de parcourir une liste de données de types élémentaires.
* Cette classe est particulièrement utile dans le cadre de la sérialisation.
*
* Un vecteur membre est présent par type de données, stockant dans le DataPackage les données lues par exemple dans un fichier.
* Les données peuvent également être ajoutées manuellement.
*
* Les méthodes de cette classe, portant le nom des types de données, retournent la première valeur du type sélectionné puis pointent vers la suivante.
*
* Lorsque toutes les valeurs d'une liste ont été retournées et si une autre lecture est tentée,
* une valeur par défaut est retournée pour les appels suivants et le booléen DataPackage::m_over passe à l'état vrai.
* A chaque fois qu'une valeur d'un certain type est demandée, l'indice de lecture correspondant est incrémenté pour retourner la valeur suivante au prochain
* appel.
*
* Il est conseillé de ne pas ajouter de données une fois que la lecture du DataPackage a commencé.
* Si le booléen DataPackage::m_over est à l'état vrai, il ne redeviendra jamais faux.
* Ce booléen indique qu'une liste de données a été entièrement lue et qu'une tentative infructueuse de lecture a été faite.
*
* Si tout se passe bien durant la lecture de données, DataPackage::m_over ne passe jamais à l'état vrai.
* Cela ne se fait que si une demande de lecture conduit au retour d'une valeur par défaut suite à épuisement de la liste.
*
* La valeur DT_END de DataPackage::DataType permet d'indiquer à une méthode lisant un DataPackage que la lecture est finie.
*/
class DataPackage
{
public:
    /**
    * @brief Enumération des types de données pris en charge par un DataPackage.
    */
    enum DataType {DT_END = 0, //!< Fin de série.
                   DT_INT = 1, //!< Type int.
                   DT_UINT = 2, //!< Type unsigned int.
                   DT_CHAR = 3, //!< Type char.
                   DT_UCHAR = 9, //!< Type unsigned char, rajouté tardivement, d'où la valeur 9.
                   DT_FLOAT = 4, //!< Type float.
                   DT_DOUBLE = 5, //!< Type double.
                   DT_STRING = STRING_FLAG, //!< Type string, valeur STRING_FLAG (fichier "misc_gen.h") pour faciliter la recherche de chaînes de caractères dans un fichier binaire.
                   DT_ENUM = 7, //!< Valeur d'énumération, il s'agit en fait d'un int classé dans une autre liste, cette valeur sera retournée sous forme d'int.
                   DT_IGNORE = 8 //!< Indique une valeur à ignorer (suivi du nombre d'octets), valeur d'énumération pour l'instant inutilisée.
                  };

    /**
    * @brief Ecrit dans un flux en écriture une valeur adaptée à un DataPackage.
    * @param r_ofs Le flux en écriture.
    * @param value La valeur à écrire.
    * @param type Le type de donnée.
    *
    * Le type du template permet à cette méthode d'écrire un nombre d'octets correspondant.
    * Le DataType indique quel marqueur attribuer à cette valeur.
    *
    * Dans la plupart des cas, <em>type</em> aura donc pour valeur l'énumération correspondant au type de valeur.
    *
    * Il est cependant possible d'associer le type DT_IGNORE à une valeur, ce qui indiquera à un DataPackage de ne pas enregistrer les octets suivants
    * d'un fichier en cours de lecture.
    * La taille de la valeur doit alors être celle d'un int, cette valeur indiquant le nombre d'octets à sauter.
    *
    * Les macros de simplification définies en début de fichier permettent d'éviter la répétition de spécification du type.
    *
    * @warning
    * Le flux doit être ouvert en binaire.
    *
    * @warning
    * Ne pas utiliser pour les chaînes de caractères.
    */
    template<class T>
    static void writeValue(ofstream& r_ofs, T value, DataType type)
    {
        r_ofs.write((char*) &type, sizeof(DataType));
        r_ofs.write((char*) &value, sizeof(T));
    }

    /**
    * @brief Ecrit dans un flux en écriture la valeur DT_END.
    * @param r_ofs Le flux en écriture.
    */
    static void writeEnd(ofstream& r_ofs)
    {
        DataType type = DT_END;
        r_ofs.write((char*) &type, sizeof(DataType));
    }

    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur DataPackage par défaut.
    */
    DataPackage();
    /**
    * @brief Constructeur DataPackage 1.
    * @param r_ifs Le flux en lecture contenant les données.
    * @param dataCount Le nombre de valeurs à lire, 0 indique une lecture complète jusqu'à DT_END.
    * @throw ConstructorException si un nombre insuffisant de valeurs est lu ou si des données sont invalides.
    *
    * Les données sont écrites sous la forme suivante, pour chaque élément :
    * <ul><li>Type de l'élément,</li>
    * <li>Valeur de l'élément.</li></ul>
    *
    * Les valeurs lues sont stockées dans les listes adéquates.
    *
    * Dans le cas d'un string, les caractères sont lus jusqu'au premier '\0'.
    *
    * @remarks
    * Le flux n'est pas rembobiné en fin de méthode.
    *
    * @warning
    * Le flux doit être ouvert en binaire.
    */
    DataPackage(ifstream& r_ifs, unsigned int dataCount = 0);
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    int nextInt();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addInt(int val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    unsigned int nextUInt();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addUInt(unsigned int val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    char nextChar();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addChar(char val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    unsigned char nextUChar();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addUChar(unsigned char val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    float nextFloat();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addFloat(float val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    double nextDouble();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addDouble(double val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    string nextString();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addString(const string& val);
    /**
    * @brief Retourne la première valeur non lue de ce type.
    * @return La valeur.
    */
    int nextEnum();
    /**
    * @brief Ajoute une valeur de ce type à cette structure.
    * @param val La valeur à ajouter.
    */
    void addEnum(int val);
    /**
    * @brief Efface toutes les valeurs de ce DataPackage.
    *
    * @warning
    * La valeur du booléen DataPackage::m_over est inchangée.
    */
    void clear();
    /*
    * Accesseurs
    * ----------
    */
    bool isOver() const {return m_over;} //!< Accesseur.

private:
    vector<int> m_int_v; //!< La liste d'int.
    unsigned int m_intIndex; //!< L'indice de lecture des int.
    vector<unsigned int> m_uint_v; //!< La liste d'unsigned int.
    unsigned int m_uintIndex; //!< L'indice de lecture des unsigned int.
    vector<char> m_char_v; //!< La liste de char.
    unsigned int m_charIndex; //!< L'indice de lecture des char.
    vector<unsigned char> m_uchar_v; //!< La liste d'unsigned char.
    unsigned int m_ucharIndex; //!< L'indice de lecture des unsigned char.
    vector<float> m_float_v; //!< La liste de float.
    unsigned int m_floatIndex; //!< L'indice de lecture des float.
    vector<double> m_double_v; //!< La liste de double.
    unsigned int m_doubleIndex; //!< L'indice de lecture des double.
    vector<string> m_string_v; //!< La liste de string.
    unsigned int m_stringIndex; //!< L'indice de lecture des string.
    vector<int> m_enum_v; //!< La liste d'enum.
    unsigned int m_enumIndex; //!< L'indice de lecture des enum.
    bool m_over; //!< Indique si une liste a été épuisée.
};

#endif // DATAPACKAGE_H_INCLUDED
