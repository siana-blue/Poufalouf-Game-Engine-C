/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les énumérations générales.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Chacune de ces énumérations est accompagnée d'une fonction associant une chaîne de caractères (string) à ses valeurs.
* Cette fonction, pour chaque énumération, est appelée <em>textFrom</em>.
*
* Chacune de ces énumérations est accompagnée d'une macro ENUM_[NOM_D_ENUMERATION]_COUNT contenant le nombre d'éléments dans l'énumération.
*/

#ifndef ENUM_H_INCLUDED
#define ENUM_H_INCLUDED

#include "misc_gen.h"

#include <string>

/**
* @brief Enumération des différents codes de retour de fonctions ou méthodes.
*
* Ces codes peuvent être utilisés par diverses fonctions ou méthodes du programme.
*
* Cette énumération prend des valeurs compatibles avec le système de flags.
* Les huit premiers bits sont inutilisés pour permettre à un code de retour d'être complété par une valeur.
*
* Pour le bon fonctionnement de <em>enumtopfflag32</em> (fichier "misc.h"), il est nécessaire de compléter l'énumération par des empty entre 0 et 0x100.
*/
enum PfReturnCode
{
    RETURN_NULL = 0x0, //!< Valeur nulle.
    RETURN_VALUE1 = 0x1, //!< Emplacement pour valeur associée.
    RETURN_VALUE2 = 0x2, //!< Emplacement pour valeur associée.
    RETURN_VALUE3 = 0x4, //!< Emplacement pour valeur associée.
    RETURN_VALUE4 = 0x8, //!< Emplacement pour valeur associée.
    RETURN_VALUE5 = 0x10, //!< Emplacement pour valeur associée.
    RETURN_VALUE6 = 0x20, //!< Emplacement pour valeur associée.
    RETURN_VALUE7 = 0x40, //!< Emplacement pour valeur associée.
    RETURN_VALUE8 = 0x80, //!< Emplacement pour valeur associée.
    RETURN_OK = 0x100, //!< Fonctionnement normal.
    RETURN_FAIL = 0x200, //!< Erreur de fonctionnement.
    RETURN_NOTHING = 0x400, //!< Aucune erreur, mais rien n'est fait.
    RETURN_EMPTY = 0x800 //!< Aucune erreur, mais pas d'objet.
};
#define ENUM_PF_RETURN_CODE_COUNT 13 //!< Le nombre de valeurs pour l'énumération PfReturnCode.

/**
* @brief Retourne un texte représentant un code de retour.
* @param code Le code de retour.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de la valeur énumérée sans 'RETURN' (par exemple, 'OK').
*
* Pour les 8 premières valeurs suivant RETURN_NULL, 'NULL' est également retourné.
*/
inline string textFrom(PfReturnCode code)
{
    string x_str;

    switch (code)
    {
        case RETURN_NULL:
        case RETURN_VALUE1:
        case RETURN_VALUE2:
        case RETURN_VALUE3:
        case RETURN_VALUE4:
        case RETURN_VALUE5:
        case RETURN_VALUE6:
        case RETURN_VALUE7:
        case RETURN_VALUE8:
            x_str = "NULL";
            break;
        case RETURN_OK:
            x_str = "OK";
            break;
        case RETURN_FAIL:
            x_str = "FAIL";
            break;
        case RETURN_NOTHING:
            x_str = "NOTHING";
            break;
        case RETURN_EMPTY:
            x_str = "EMPTY";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des différentes animations.
*/
enum PfAnimationStatus
{
    ANIM_NONE, //!< Aucune animation.
    ANIM_IDLE, //!< Immobile.
    ANIM_HIGHLIGHTED, //!< Surligné, mis en emphase.
    ANIM_SELECTED, //!< Sélectionné.
    ANIM_ACTIVATED, //!< Activé.
    ANIM_DEACTIVATED, //!< Désactivé.
    ANIM_DISABLED, //!< Désactivé, inerte.
    ANIM_WALKING, //!< Marche.
    ANIM_JUMPING, //!< Saut.
    ANIM_FALLING1, //!< Chute 1.
    ANIM_FALLING2, //!< Chute 2.
    ANIM_GETUP1, //!< Redressement 1.
    ANIM_GETUP2, //!< Redressement 2.
    ANIM_DROWNING, //!< Noyade.
    ANIM_MISC1, //!< Animations diverses valeur 1.
    ANIM_MISC2 //!< Animations diverses valeur 2.
};
#define ENUM_PF_ANIMATION_STATUS_COUNT 16 //!< Le nombre de valeurs pour l'énumération PfAnimationStatus.

/**
* @brief Retourne un texte représentant une animation.
* @param status Le statut d'animation.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de l'élément de l'énumération, sans le préfixe 'ANIM'.
*/
inline string textFrom(PfAnimationStatus status)
{
    string x_str;

    switch (status)
    {
        case ANIM_NONE:
            x_str = "NONE";
            break;
        case ANIM_IDLE:
            x_str = "IDLE";
            break;
        case ANIM_HIGHLIGHTED:
            x_str = "HIGHLIGHTED";
            break;
        case ANIM_SELECTED:
            x_str = "SELECTED";
            break;
        case ANIM_ACTIVATED:
            x_str = "ACTIVATED";
            break;
        case ANIM_DEACTIVATED:
            x_str = "DEACTIVATED";
            break;
        case ANIM_DISABLED:
            x_str = "DISABLED";
            break;
        case ANIM_WALKING:
            x_str = "WALKING";
            break;
        case ANIM_JUMPING:
            x_str = "JUMPING";
            break;
        case ANIM_FALLING1:
            x_str = "FALLING1";
            break;
        case ANIM_FALLING2:
            x_str = "FALLING2";
            break;
        case ANIM_GETUP1:
            x_str = "GETUP1";
            break;
        case ANIM_GETUP2:
            x_str = "GETUP2";
            break;
        case ANIM_DROWNING:
            x_str = "DROWNING";
            break;
        case ANIM_MISC1:
            x_str = "MISC1";
            break;
        case ANIM_MISC2:
            x_str = "MISC2";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des slots d'objets dans un wad.
*
* Laisser toujours les 'GUI' en fin de liste.
*/
enum PfWadSlot
{
    WAD_NULL_OBJECT, //!< Slot vide.
    WAD_MOB, //!< Mob.
    WAD_JUNGLE1, //!< Jungle d'objets 1.
    WAD_JUNGLE2, //!< Jungle d'objets 2.
    WAD_GRASS, //!< Etendue d'herbe.
    WAD_FENCE1, //!< Barrière 1.
    WAD_FENCE2, //!< Barrière 2.
    WAD_SIGN1, //!< Pancarte 1.
    WAD_SIGN2, //!< Pancarte 2.
    WAD_STATIC1, //!< Objet statique 1.
    WAD_STATIC2, //!< Objet statique 2.
    WAD_ANIMATED1, //!< Objet animé 1.
    WAD_ANIMATED2, //!< Objet animé 2.
    WAD_PORTAL1, //!< Portail 1.
    WAD_BACKGROUND, //!< Décor de fond.
    WAD_GUI_BORDER, //!< Bordure d'écran.
    WAD_GUI_BUTTON1, //!< Bouton cliquable 1.
    WAD_GUI_BUTTON2, //!< Bouton cliquable 2.
    WAD_GUI_SELECTIONGRID, //!< Grille de sélection.
    WAD_GUI_SCROLLBAR, //!< Barre de défilement.
    WAD_GUI_LIST1, //!< Liste de boutons 1.
    WAD_GUI_LIST2, //!< Liste de boutons 2.
    WAD_GUI_LABEL1, //!< Label 1.
    WAD_GUI_LABEL2, //!< Label 2.
    WAD_GUI_TEXTBOX, //!< Boîte de texte.
    WAD_GUI_SPINBOX, //!< Boîte numérique.
    WAD_GUI_SLIDINGCURSOR, //!< Curseur glissant.
    WAD_GUI_PROGRESSBAR //!< Barre de progression.
};
#define ENUM_PF_WAD_SLOT_COUNT 28 //!< Le nombre de valeurs pour l'énumération PfWadSlot.
#define ENUM_PF_WAD_SLOT_MAP_BEG WAD_MOB //!< Le slot marquant le début de la section des objets d'une map.
#define ENUM_PF_WAD_SLOT_MAP_END WAD_PORTAL1 //!< Le slot marquant la fin de la section des objets d'une map.
#define ENUM_PF_WAD_SLOT_WIDGETS_BEG WAD_GUI_BUTTON1 //!< Le slot marquant le début de la section des objets d'interface utilisateur.
#define ENUM_PF_WAD_SLOT_WIDGETS_END WAD_GUI_PROGRESSBAR //!< Le slot marquant la fin de la section des objets d'interface utilisateur.

/**
* @brief Retourne un texte représentant un slot de wad.
* @param slot Le slot de wad.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de l'élément de l'énumération, sans le préfixe 'WAD'.
*/
inline string textFrom(PfWadSlot slot)
{
    string x_str;

    switch (slot)
    {
        case WAD_NULL_OBJECT:
            x_str = "NULL_OBJECT";
            break;
        case WAD_MOB:
            x_str = "MOB";
            break;
        case WAD_JUNGLE1:
            x_str = "JUNGLE1";
            break;
        case WAD_JUNGLE2:
            x_str = "JUNGLE2";
            break;
        case WAD_GRASS:
            x_str = "GRASS";
            break;
        case WAD_FENCE1:
            x_str = "FENCE1";
            break;
        case WAD_FENCE2:
            x_str = "FENCE2";
            break;
        case WAD_SIGN1:
            x_str = "SIGN1";
            break;
        case WAD_SIGN2:
            x_str = "SIGN2";
            break;
        case WAD_STATIC1:
            x_str = "STATIC1";
            break;
        case WAD_STATIC2:
            x_str = "STATIC2";
            break;
        case WAD_ANIMATED1:
            x_str = "ANIMATED1";
            break;
        case WAD_ANIMATED2:
            x_str = "ANIMATED2";
            break;
        case WAD_PORTAL1:
            x_str = "PORTAL1";
            break;
        case WAD_BACKGROUND:
            x_str = "BACKGROUND";
            break;
        case WAD_GUI_BORDER:
            x_str = "GUI_BORDER";
            break;
        case WAD_GUI_BUTTON1:
            x_str = "GUI_BUTTON1";
            break;
        case WAD_GUI_BUTTON2:
            x_str = "GUI_BUTTON2";
            break;
        case WAD_GUI_SELECTIONGRID:
            x_str = "GUI_SELECTIONGRID";
            break;
        case WAD_GUI_SCROLLBAR:
            x_str = "GUI_SCROLLBAR";
            break;
        case WAD_GUI_LIST1:
            x_str = "GUI_LIST1";
            break;
        case WAD_GUI_LIST2:
            x_str = "GUI_LIST2";
            break;
        case WAD_GUI_LABEL1:
            x_str = "GUI_LABEL1";
            break;
        case WAD_GUI_LABEL2:
            x_str = "GUI_LABEL2";
            break;
        case WAD_GUI_TEXTBOX:
            x_str = "GUI_TEXTBOX";
            break;
        case WAD_GUI_SPINBOX:
            x_str = "GUI_SPINBOX";
            break;
        case WAD_GUI_SLIDINGCURSOR:
            x_str = "GUI_SLIDINGCURSOR";
            break;
        case WAD_GUI_PROGRESSBAR:
            x_str = "GUI_PROGRESSBAR";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des options trouvées dans un script de wad.
*
* @warning
* Les textes des instructions doivent faire 3 lettres.
*/
enum PfWadScriptOption
{
    WADTXT_IMG, //!< Option img (chargement d'image).
    WADTXT_SND, //!< Option snd (chargement de son).
    WADTXT_VID, //!< Option vid (chargement d'une video).
    WADTXT_MSC, //!< Option msc (options diverses).
    WADTXT_FRM, //!< Option frm (déclaration de frame).
    WADTXT_SLW, //!< Option slw (ralentissement d'animation).
    WADTXT_BOX //!< Option box (affectation d'une zone).
};
#define ENUM_PF_WAD_SCRIPT_OPTION_COUNT 7 //!< Le nombre de valeurs pour l'énumération PfWadScriptOption.

/**
* @brief Retourne un texte représentant une option de script de wad.
* @param option L'option de script de wad.
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de l'option dans le script, indiquée dans la documentation de l'énumération elle-même (option xxx).
*/
inline string textFrom(PfWadScriptOption option)
{
    string x_str;

    switch (option)
    {
        case WADTXT_IMG:
            x_str = "img";
            break;
        case WADTXT_SND:
            x_str = "snd";
            break;
        case WADTXT_VID:
            x_str = "vid";
            break;
        case WADTXT_MSC:
            x_str = "msc";
            break;
        case WADTXT_FRM:
            x_str = "frm";
            break;
        case WADTXT_SLW:
            x_str = "slw";
            break;
        case WADTXT_BOX:
            x_str = "box";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des valeurs prises par les arguments de l'option "msc".
*
* Cette énumération prend des valeurs compatibles avec le système de flags.
*/
enum PfMscValue
{
    WADMSC_NONE = 0x0, //!< Valeur none, aucune instruction.
    WADMSC_LOOP = 0x1, //!< Valeur loop, jouer l'animation en boucle.
    WADMSC_TURNABLE = 0x2 //!< Valeur turnable, animation dépendant de l'orientation de l'objet.
};
#define ENUM_PF_MSC_VALUE_COUNT 3 //!< Le nombre de valeurs pour l'énumération PfMscValue.

/**
* @brief Retourne un texte représentant une valeur de l'option "msc".
* @param value La valeur de l'option "msc".
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de la valeur dans le script, indiquée dans la documentation de l'énumération elle-même (valeur xxx).
*/
inline string textFrom(PfMscValue value)
{
    string x_str;

    switch (value)
    {
        case WADMSC_NONE:
            x_str = "none";
            break;
        case WADMSC_LOOP:
            x_str = "loop";
            break;
        case WADMSC_TURNABLE:
            x_str = "turnable";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des propriétés d'un objet définies dans un WAD.
*/
enum PfWadObjProperty
{
    WADOBJ_COLHEIGHT, //!< Valeur colHeight, hauteur de collision.
    WADOBJ_BOX, //!< Valeur box, définition d'une zone.
    WADOBJ_CENTER //!< Valeur center, définition du centre d'un objet.
};
#define ENUM_PF_WAD_OBJ_PROPERTY_COUNT 3 //!< Le nombre de valeurs pour l'énumération PfWadObjProperty.

/**
* @brief Retourne un texte représentant une propriété d'objet.
* @param value La propriété concernée.
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de la propriété dans le script, indiquée dans la documentation de l'énumération elle-même (valeur xxx).
*/
inline string textFrom(PfWadObjProperty value)
{
    string x_str;

    switch (value)
    {
        case WADOBJ_COLHEIGHT:
            x_str = "colHeight";
            break;
        case WADOBJ_BOX:
            x_str = "box";
            break;
        case WADOBJ_CENTER:
            x_str = "center";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des sections trouvées dans un script de menu.
*
* @warning
* Les textes des sections doivent faire 3 lettres.
*/
enum PfMenuScriptSection
{
    MENSCT_PUT, //!< Section PUT (placer un objet).
    MENSCT_SEL, //!< Section SEL (sélection d'un widget).
    MENSCT_MUS, //!< Section MUS (définition d'une musique).
    MENSCT_MSC //!< Section MSC (options diverses).
};
#define ENUM_PF_MENU_SCRIPT_SECTION_COUNT 4 //!< Le nombre de valeurs pour l'énumération PfMenuScriptSection.

/**
* @brief Retourne un texte représentant une section de script de menu.
* @param section La section de script de menu.
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de la section dans le script, indiquée dans la documentation de l'énumération elle-même (section XXX).
*/
inline string textFrom(PfMenuScriptSection section)
{
    string x_str;

    switch (section)
    {
        case MENSCT_PUT:
            x_str = "PUT";
            break;
        case MENSCT_SEL:
            x_str = "SEL";
            break;
        case MENSCT_MUS:
            x_str = "MUS";
            break;
        case MENSCT_MSC:
            x_str = "MSC";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des options trouvées dans un script de menu.
*
* @warning
* Les textes des options doivent faire 3 lettres.
*/
enum PfMenuScriptOption
{
    MENOPT_OBJ, //!< Option obj (définition d'objet wad).
    MENOPT_CRD, //!< Option crd (coordonnées).
    MENOPT_TXT, //!< Option txt (texte d'un objet).
    MENOPT_LYT, //!< Option lyt (identifiant de layout).
    MENOPT_EFT, //!< Option eft (effet d'un widget).
    MENOPT_VAL, //!< Option val (valeurs de construction).
    MENOPT_DYN //!< Option dyn (association à une valeur dynamique).
};
#define ENUM_PF_MENU_SCRIPT_OPTION_COUNT 7 //!< Le nombre de valeurs pour l'énumération PfMenuScriptOption.

/**
* @brief Retourne un texte représentant une option de script de menu.
* @param option L'option de script de menu.
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de l'option dans le script, indiquée dans la documentation de l'énumération elle-même (option xxx).
*/
inline string textFrom(PfMenuScriptOption option)
{
    string x_str;

    switch (option)
    {
        case MENOPT_OBJ:
            x_str = "obj";
            break;
        case MENOPT_CRD:
            x_str = "crd";
            break;
        case MENOPT_TXT:
            x_str = "txt";
            break;
        case MENOPT_LYT:
            x_str = "lyt";
            break;
        case MENOPT_EFT:
            x_str = "eft";
            break;
        case MENOPT_VAL:
            x_str = "val";
            break;
        case MENOPT_DYN:
            x_str = "dyn";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des options trouvées dans un script de textures.
*
* @warning
* Les textes des sections doivent faire 3 lettres.
*/
enum PfTextureScriptSection
{
    TEXSCT_SPR, //!< Section SPR, débordements.
    TEXSCT_BRD, //!< Section BRD, bordures.
    TEXSCT_CLF, //!< Section CLF, falaises.
    TEXSCT_CLB, //!< Section CLB, bordures de falaises.
    TEXSCT_FLC //!< Section FLC, recouvrement en bas de falaises.
};
#define ENUM_PF_TEXTURE_SCRIPT_SECTION_COUNT 5 //!< Le nombre de valeurs pour l'énumération PfTextureScriptSection.

/**
* @brief Retourne un texte représentant une section de script de textures.
* @param section La section de script de textures.
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de la section dans le script, indiquée dans la documentation de l'énumération elle-même (section XXX).
*/
inline string textFrom(PfTextureScriptSection section)
{
    string x_str;

    switch (section)
    {
        case TEXSCT_SPR:
            x_str = "SPR";
            break;
        case TEXSCT_BRD:
            x_str = "BRD";
            break;
        case TEXSCT_CLF:
            x_str = "CLF";
            break;
        case TEXSCT_CLB:
            x_str = "CLB";
            break;
        case TEXSCT_FLC:
            x_str = "FLC";
            break;
    }

    return x_str;
}

/**
* @brief Enumération d'effets d'éléments de jeu.
*
* Cette énumération correspond à des valeurs prises par des flags 32 bits.
*
* Les 8 premiers bits sont associés à une valeur complétant l'effet de l'énumération.
* L'énumération commence donc à partir de 0x100.
*
* Il sera possible par la suite d'associer des effets aux 8 premières valeurs, effets qui appartiendront à un autre "groupe" d'effets.
*
* Pour le bon fonctionnement de <em>enumtopfflag32</em> (fichier "misc.h"), il est nécessaire de compléter l'énumération par des empty entre 0 et 0x100.
*/
enum PfEffect
{
    EFFECT_NONE = 0x0, //!< Effet none, aucun effet.
    EFFECT_EMPTY1 = 0x1, //!< Vide.
    EFFECT_EMPTY2 = 0x2, //!< Vide.
    EFFECT_EMPTY3 = 0x4, //!< Vide.
    EFFECT_EMPTY4 = 0x8, //!< Vide.
    EFFECT_EMPTY5 = 0x10, //!< Vide.
    EFFECT_EMPTY6 = 0x20, //!< Vide.
    EFFECT_EMPTY7 = 0x40, //!< Vide.
    EFFECT_EMPTY8 = 0x80, //!< Vide.
    EFFECT_NEW = 0x100, //!< Effet new, nouveau (jeu, fichier...).
    EFFECT_NEXT = 0x200, //!< Effet next, étape suivante, menu suivant...
    EFFECT_QUIT = 0x400, //!< Effet quit, quitter.
    EFFECT_PREV = 0x800, //!< Effet prev, précédent.
    EFFECT_SWITCH = 0x1000, //!< Effet switch, changer de mode...
    EFFECT_SELECT = 0x2000, //!< Effet select, sélection...
    EFFECT_SLEEP = 0x4000, //!< Effet sleep, mise en veille, en attente...
    EFFECT_DELETE = 0x8000, //!< Effet delete, suppression.
    EFFECT_PRINT = 0x10000 //!< Effet print, affichage.
};
#define ENUM_PF_EFFECT_COUNT 18 //!< Le nombre de valeurs pour l'énumération PfEffect.

/**
* @brief Retourne un texte représentant un effet.
* @param effect L'effet.
* @return Le texte correspondant.
*
* Le texte retourné correspond à l'écriture de l'effet dans le script, indiquée dans la documentation de l'énumération elle-même (effet xxx).
*
* Le texte 'empty' est retourné pour les valeurs comprises entre 0x1 et 0x80.
*/
inline string textFrom(PfEffect effect)
{
    string x_str;

    switch (effect)
    {
        case EFFECT_NONE:
            x_str = "none";
            break;
        case EFFECT_EMPTY1:
        case EFFECT_EMPTY2:
        case EFFECT_EMPTY3:
        case EFFECT_EMPTY4:
        case EFFECT_EMPTY5:
        case EFFECT_EMPTY6:
        case EFFECT_EMPTY7:
        case EFFECT_EMPTY8:
            x_str = "empty";
            break;
        case EFFECT_NEW:
            x_str = "new";
            break;
        case EFFECT_NEXT:
            x_str = "next";
            break;
        case EFFECT_QUIT:
            x_str = "quit";
            break;
        case EFFECT_PREV:
            x_str = "prev";
            break;
        case EFFECT_SWITCH:
            x_str = "switch";
            break;
        case EFFECT_SELECT:
            x_str = "select";
            break;
        case EFFECT_SLEEP:
            x_str = "sleep";
            break;
        case EFFECT_DELETE:
            x_str = "delete";
            break;
        case EFFECT_PRINT:
            x_str = "print";
            break;
    }

    return x_str;
}

/**
* @brief Enumération représentant les instructions pouvant être adressées aux objets du jeu.
*/
enum PfInstruction
{
    INSTRUCTION_NONE, //!< Instruction none, aucune instruction.
    INSTRUCTION_LEFT, //!< Instruction left, direction gauche.
    INSTRUCTION_UP, //!< Instruction up, direction haut.
    INSTRUCTION_RIGHT, //!< Instruction right, direction droite.
    INSTRUCTION_DOWN, //!< Instruction down, direction bas.
    INSTRUCTION_ACTIVATE, //!< Instruction activate, activer.
    INSTRUCTION_SWITCH, //!< Instruction switch, changer de mode...
    INSTRUCTION_SELECT, //!< Instruction select, sélection...
    INSTRUCTION_DELETE, //!< Instruction delete, effacer...
    INSTRUCTION_MODIFY, //!< Instruction modify, modifier...
    INSTRUCTION_VALIDATE, //!< Instruction validate, valider...
    INSTRUCTION_STOP, //!< Instruction stop, arrêt...
    INSTRUCTION_JUMP, //!< Instruction jump, saut...
    INSTRUCTION_CHECK //!< Instruction check, demande de vérification d'état...
};
#define ENUM_PF_INSTRUCTION_COUNT 14 //!< Le nombre de valeurs pour l'énumération PfInstruction.

/**
* @brief Retourne un texte représentant une instruction.
* @param instruction L'instruction.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de la valeur énumérée sans 'INSTRUCTION', et en minuscules (par exemple, 'up').
*/
inline string textFrom(PfInstruction instruction)
{
    string x_str;

    switch (instruction)
    {
        case INSTRUCTION_NONE:
            x_str = "none";
            break;
        case INSTRUCTION_LEFT:
            x_str = "left";
            break;
        case INSTRUCTION_UP:
            x_str = "up";
            break;
        case INSTRUCTION_RIGHT:
            x_str = "right";
            break;
        case INSTRUCTION_DOWN:
            x_str = "down";
            break;
        case INSTRUCTION_ACTIVATE:
            x_str = "activate";
            break;
        case INSTRUCTION_SWITCH:
            x_str = "switch";
            break;
        case INSTRUCTION_SELECT:
            x_str = "select";
            break;
        case INSTRUCTION_DELETE:
            x_str = "delete";
            break;
        case INSTRUCTION_MODIFY:
            x_str = "modify";
            break;
        case INSTRUCTION_VALIDATE:
            x_str = "validate";
            break;
        case INSTRUCTION_STOP:
            x_str = "stop";
            break;
        case INSTRUCTION_JUMP:
            x_str = "jump";
            break;
        case INSTRUCTION_CHECK:
            x_str = "check";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des différents types de jeu.
*/
enum PfGameMode
{
    GAME_NONE, //!< Valeur par défaut.
    GAME_MENU, //!< Mode menu.
    GAME_MAP //!< Mode map.
};
#define ENUM_PF_GAME_MODE_COUNT 3 //!< Le nombre de valeurs pour l'énumération PfGameMode.

/**
* @brief Retoure un texte représentant un mode de jeu.
* @param mode Le mode de jeu.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de la valeur énumérée sans 'GAME', et en minuscules (par exemple, 'map').
*/
inline string textFrom(PfGameMode mode)
{
    string x_str;

    switch (mode)
    {
        case GAME_NONE:
            x_str = "none";
            break;
        case GAME_MENU:
            x_str = "menu";
            break;
        case GAME_MAP:
            x_str = "map";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des différents types de zone.
*/
enum PfBoxType
{
    BOX_TYPE_MAIN, //!< Zone principale.
    BOX_TYPE_COLLISION, //!< Zone de collision.
    BOX_TYPE_TRIGGER, //!< Zone d'activation.
    BOX_TYPE_DOOR, //!< Zone portail.
    BOX_TYPE_ACTION //!< Zone d'action.
};
#define ENUM_PF_BOX_TYPE_COUNT 5 //!< Le nombre de valeurs pour l'énumération PfBoxType.

/**
* @brief Retourne un texte représentant une zone.
* @param box La zone.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de la valeur énumérée sans 'BOX_TYPE'.
*/
inline string textFrom(PfBoxType box)
{
    string x_str;

    switch (box)
    {
        case BOX_TYPE_MAIN:
            x_str = "MAIN";
            break;
        case BOX_TYPE_COLLISION:
            x_str = "COLLISION";
            break;
        case BOX_TYPE_TRIGGER:
            x_str = "TRIGGER";
            break;
        case BOX_TYPE_DOOR:
            x_str = "DOOR";
            break;
        case BOX_TYPE_ACTION:
            x_str = "ACTION";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des différents états possibles d'un objet.
*
* Cette énumération correspond à des valeurs prises par des flags 32 bits.
*/
enum PfObjectStatus
{
    OBJSTAT_NONE = 0x0, //!< Aucun statut particulier.
    OBJSTAT_JUMPING = 0x1, //!< En train de sauter.
    OBJSTAT_LANDING = 0x2, //!< Atterrissage.
    OBJSTAT_ONTHEFLOOR = 0x4, //!< Tranquillement sur terre.
    OBJSTAT_FALLING = 0x8, //!< En train de tomber.
    OBJSTAT_STANDBY = 0x10, //!< En standby (diverses applications).
    OBJSTAT_STOPPED = 0x20, //!< Arrêté.
    OBJSTAT_ONWATER = 0x40, //!< Sur une surface d'eau.
    OBJSTAT_DEAD = 0x80, //!< Mort, terminé, supprimé.
    OBJSTAT_ACTIVATED = 0x100 //!< Activé.
};
#define ENUM_PF_OBJECT_STATUS_COUNT 10 //!< Le nombre de valeurs pour l'énumération PfObjectStatus.

/**
* @brief Retourne un texte représentant un état d'objet.
* @param objStat L'état d'objet.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de la valeur énumérée sans 'OBJSTAT'.
*/
inline string textFrom(PfObjectStatus objStat)
{
    string x_str;

    switch (objStat)
    {
        case OBJSTAT_NONE:
            x_str = "NONE";
            break;
        case OBJSTAT_JUMPING:
            x_str = "JUMPING";
            break;
        case OBJSTAT_LANDING:
            x_str = "LANDING";
            break;
        case OBJSTAT_ONTHEFLOOR:
            x_str = "ONTHEFLOOR";
            break;
        case OBJSTAT_FALLING:
            x_str = "FALLING";
            break;
        case OBJSTAT_STANDBY:
            x_str = "STANDBY";
            break;
        case OBJSTAT_STOPPED:
            x_str = "STOPPED";
            break;
        case OBJSTAT_ONWATER:
            x_str = "ONWATER";
            break;
        case OBJSTAT_DEAD:
            x_str = "DEAD";
            break;
        case OBJSTAT_ACTIVATED:
            x_str = "ACTIVATED";
            break;
    }

    return x_str;
}

/**
* @brief Enumération des codes d'activation.
*
* Cette énumération correspond à des valeurs prises par des flags 32 bits.
*/
enum PfActivationCode
{
    ACTIVCODE_NONE = 0x0, //!< Aucune activation.
    ACTIVCODE_ANY = 0x1, //!< Activation par tout objet potentiel.
    ACTIVCODE_USER = 0x2, //!< Activation par une entrée utilisateur.
    ACTIVCODE_SOUTH = 0x4, //!< Activation par un objet orienté sud.
    ACTIVCODE_WEST = 0x8, //!< Activation par un objet orienté ouest.
    ACTIVCODE_NORTH = 0x10, //!< Activation par un objet orienté nord.
    ACTIVCODE_EAST = 0x20 //!< Activation par un objet orienté est.
};
#define ENUM_PF_ACTIVATION_CODE_COUNT 7 //!< Le nombre de valeurs pour l'énumération PfActivationCode.

/**
* @brief Retourne un texte représentant un code d'activation.
* @param code Le code d'activation.
* @return Le texte correspondant.
*
* Le texte retourné correspond au nom de la valeur énumérée sans 'ACTIVCODE'.
*/
inline string textFrom(PfActivationCode code)
{
    string x_str;

    switch (code)
    {
        case ACTIVCODE_NONE:
            x_str = "NONE";
            break;
        case ACTIVCODE_ANY:
            x_str = "ANY";
            break;
        case ACTIVCODE_USER:
            x_str = "USER";
            break;
        case ACTIVCODE_SOUTH:
            x_str = "SOUTH";
            break;
        case ACTIVCODE_WEST:
            x_str = "WEST";
            break;
        case ACTIVCODE_NORTH:
            x_str = "NORTH";
            break;
        case ACTIVCODE_EAST:
            x_str = "EAST";
            break;
    }

    return x_str;
}

#endif // ENUM_H_INCLUDED

