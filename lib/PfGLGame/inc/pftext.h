/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfText.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFTEXT_H_INCLUDED
#define PFTEXT_H_INCLUDED

#include "glgame_gen.h"

#include "geometry.h"
#include "glimage.h"

/**
* @brief Texte représenté par une image.
*
* L'image du texte est un fichier texture de 1024x1024 pixels, contenant 16 lignes et 16 colonnes de caractères dans l'ordre de la table ASCII.
*
* Chaque caractère ASCII est associé à une valeur float du tableau PfText::m_zPos_t.
* Cette valeur indique le décalage vertical à associer à chaque caractère.
* Par exemple, pour décaler un caractère d'un quart de hauteur vers le bas, donner au float correspondant la valeur de -0.25.
* Les valeurs de l'énumération PfText::ZPosValue assignent des tableaux préconstruits au texte.
*
* Une série de GLImage peut être créée à partir de ce texte au moyen de la méthode PfText::toGLImages.
*
* Le caractère entrée peut-être utilisé, le texte rendra alors un retour à la ligne.
*
* Un texte est défini par son rectangle de coordonnées et son nombre de lignes.
* A partir de ces données, la hauteur (et donc la largeur) d'un caractère est définie.
*
* @section Proprietes Propriétés du texte
*
* @subsection Alignement Alignement
*
* Lors de sa mise à jour ou à sa construction, le PfText peut gérer son alignement à gauche, à droite, centré ou justifié, par spécification
* via l'énumération PfText::TextAlignProp du champ PfText::m_alignProp.
*
* @subsection Dynamique Affichage progressif
*
* L'énumération PfText::TextTypingMode permet de paramétrer un affichage caractère par caractère progressif, à différentes vitesses.
* Le champ PfText::m_typingMode permet de faire cette spécification. Dans le cas où il est différent de PfText::TEXT_TYPING_ALLATONCE,
* le champ PfText::m_typingCounter gère l'affichage progressif, en comptant le nombre de caractères à afficher.
*
* Par défaut, tous les caractères sont affichés en une fois. Dans le cas d'un affichage progressif, il faudra appeler la méthode PfText::updateTyping
* pour réactualiser le nombre de caractères à afficher à chaque itération (typiquement une méthode à appeler dans la méthode <em>update</em> d'un ModelItem).
* En fonction du type d'affichage, un ou plusieurs caractères pourront être ajoutés simultanément à chaque itération pour avoir une vitesse plus ou moins
* rapide.
*
* @subsection Partiel Affichage partiel
*
* Les champs PfText::m_offset et PfText::m_displayedLength permettent de gérer l'affichage partiel de ce texte, par exemple dans une boîte de dialogue
* où il s'afficherait au fur et à mesure.
*
* Les contraintes géométriques du rectangle PfText::m_rect et de la taille des caractères peut induire un affichage d'un nombre de caractères inférieur
* à PfText::m_displayedLength, cette valeur spécifiant simplement le nombre maximal demandé.
*
* Si l'affichage est progressif, c'est le champ PfText::m_typingCounter qui détermine au final le nombre total de caractères à afficher, qui arrive à
* une valeur maximale de PfText::m_displayedLength (ou contrainte géométrique) une fois le texte entièrement écrit.
*/
class PfText
{
public:
    /**
    * @brief Enumération des valeurs de tableaux de décalages verticaux préconstruits.
    */
    enum ZPosValue {NO_ZPOS, //!< Toutes les valeurs à 0.
                    ZPOS_STANDARD //!< Les lettres minuscules sont décalées vers le bas d'une valeur de -0.33 pour celles le nécessitant.
                    };
    /**
    * @brief Enumération des méthodes d'alignement du texte.
    */
    enum TextAlignProp {TEXT_ALIGN_LEFT, //!< Garder la marge à gauche et ajouter les lettres à droite.
                        TEXT_ALIGN_CENTER, //!< Repositionner le texte pour garder une position centrale.
                        TEXT_ALIGN_RIGHT, //!< Ajouter les lettres en décalant le texte vers la gauche pour conserver la marge droite.
                        TEXT_ALIGN_JUSTIFIED //!< Texte justifié.
                        };
    /**
    * @brief Enumération des types d'affichage du texte.
    */
    enum TextTypingMode {TEXT_TYPING_ALLATONCE, //!< Tout afficher d'un coup.
                         TEXT_TYPING_SLOW, //!< Affichage progressif caractère par caractère lent.
                         TEXT_TYPING_FAST //!< Affichage progressif caractère par caractère rapide.
                         };

    /*
    * Constructeurs et destructeur
    * ----------------------------
    */
    /**
    * @brief Constructeur PfText par défaut.
    *
    * Construit un texte vide avec pour indice de texture FONT_TEXTURE_INDEX (fichier "glgame_gen.h").
    */
    PfText();
    /**
    * @brief Constructeur PfText 1.
    * @param text Le texte à afficher.
    * @param textureIndex L'indice de texture de la police.
    * @param zPos_t Le tableau des décalages verticaux.
    * @param rect Le rectangle des coordonnées de ce texte.
    * @param linesCount Le nombre de lignes affichées.
    * @param align Le type de méthode d'alignement de ce texte dans son rectangle.
    */
    PfText(const string& text, unsigned int textureIndex, float zPos_t[256], const PfRectangle& rect, unsigned int linesCount = 1,
           TextAlignProp align = TEXT_ALIGN_CENTER);
    /**
    * @brief Constructeur PfText 2.
    * @param text Le texte à afficher.
    * @param textureIndex L'indice de texture de la police.
    * @param rect Le rectangle des coordonnées de ce texte.
    * @param linesCount Le nombre de lignes affichées.
    * @param align Le type de méthode d'alignement de ce texte dans son rectangle.
    * @param zPosValue La valeur donnant le tableau de décalages préconstruit.
    */
    PfText(const string& text, unsigned int textureIndex, const PfRectangle& rect, unsigned int linesCount = 1, TextAlignProp align = TEXT_ALIGN_CENTER,
           ZPosValue zPosValue = ZPOS_STANDARD);
    /**
    * @brief Constructeur PfText par copie.
    * @param text Le texte à copier.
    */
    PfText(const PfText& text);
    /**
    * @brief Destructeur PfText.
    *
    * Détruit le tableau contenant le texte.
    */
    ~PfText();
    /*
    * Méthodes
    * --------
    */
    /**
    * @brief Modifie le texte.
    * @param text Le nouveau texte.
    *
    * L'offset et la longueur affichée sont réinitialisés de sorte à afficher le texte complet.
    */
    void changeText(const string& text);
    /**
    * @brief Ajoute du texte à la suite de l'existant.
    * @param text Le texte à ajouter.
    * @param newLine <code>true</code> pour faire un retour à la ligne avant ajout du texte.
    *
    * Si le texte était affiché jusqu'à la fin, le champ PfText::m_displayedLength est modifé pour se poursuivre jusqu'à la fin, sinon il est inchangé.
    *
    * @remarks
    * Cette méthode ne modifie pas le champ PfText::m_linesCount.
    */
    void addText(const string& text, bool newLine = false);
    /**
    * @brief Génère une série de GLImage à partir de ce texte.
    * @param relativeToBorders <code>true</code> si les images doivent être positionnées par rapport aux bordures.
    * @param stat <code>true</code> si ce texte ne doit pas se déplacer avec la caméra.
    * @param cursor Indique si un curseur de fin doit être affiché.
    *
    * @warning
    * De la mémoire est allouée lors de la création du vecteur contenant les GLImage et lors de la création de chaque GLImage.
    *
    * Le curseur est placé en fin de texte et a pour dimension la hauteur d'un caractère et le dixième de sa largeur.
    */
    vector<GLImage*>* toGLImages(bool relativeToBorders = true, bool stat = false, bool cursor = false) const;
    /**
    * @brief Retourne le nombre de caractères par ligne de ce texte, au vu de ses dimensions.
    * @return Le nombre de caractères par ligne de ce texte.
    */
    unsigned int charCountPerLine() const;
    /**
    * @brief Retourne le nombre total de caractères affichés sur ce texte au vu de ses dimensions.
    * @return Le nombre total de caractères affichés.
    *
    * @warning
    * Cette méthode ne retourne pas le nombre de caractères réellement affichés mais la capacité totale du PfText.
    */
    unsigned int displayedCharCount() const;
    /**
    * @brief Spécifie une nouvelle portion de texte à afficher.
    * @param offset La nouvelle position de départ, 0 pour le début du texte.
    * @param length Le nouveau nombre maximal de caractères à afficher.
    * @throw ArgumentException si les paramètres ne sont pas adaptés au contenu du texte.
    */
    void changeDisplay(unsigned int offset, unsigned int length);
    /**
    * @brief Met à jour le nombre de caractères à afficher dans le cadre d'un affichage progressif.
    *
    * En fonction de la valeur du champ PfText::m_typingMode, le compteur PfText::m_typingCounter est incrémenté d'une valeur spécifique.
    */
    void updateTyping();
    /*
    * Accesseurs
    * ----------
    */
    unsigned int getLength() const {return m_length;} //!< Accesseur.
    const PfRectangle& getRect() const {return m_rect;} //!< Accesseur.
    void setRect(const PfRectangle& rect) {m_rect = rect;} //!< Accesseur.
    unsigned int getLinesCount() const {return m_linesCount;} //!< Accesseur.
    void setLinesCount(unsigned int linesCount) {m_linesCount = linesCount;} //!< Accesseur.
    void setAlignProp(TextAlignProp align) {m_alignProp = align;} //!< Accesseur.
    unsigned int getOffset() const {return m_offset;} //!< Accesseur.
    unsigned int getDisplayedLength() const {return m_displayedLength;} //!< Accesseur.
    /*
    * Opérateurs
    * ----------
    */
    /**
    * @brief Opérateur d'affectation.
    * @param text Le PfText à affecter à celui-ci.
    *
    * Le texte actuel est détruit puis copié en profondeur d'après le paramètre.
    */
    PfText& operator=(const PfText& text);

private:
    unsigned char* m_text_t; //!< Le texte à afficher.
    unsigned int m_length; //!< Le nombre de caractères dans ce texte.
    unsigned int m_textureIndex; //!< L'indice de texture de la police.
    PfRectangle m_rect; //!< Le rectangle des coordonnées de ce texte.
    unsigned int m_linesCount; //!< Le nombre de lignes affichées.
    float m_zPos_t[256]; //!< La table de décalage en hauteur des caractères.
    TextAlignProp m_alignProp; //!< Le type d'alignement de ce texte dans son rectangle.
    unsigned int m_offset; //!< L'indice du premier caractère à afficher (0 pour affichage depuis le début).
    unsigned int m_displayedLength; //!< Le nombre de caractères du texte à afficher.
    TextTypingMode m_typingMode; //!< Le mode d'affichage du texte à afficher (caractère par caractère ou en un seul coup).
    unsigned int m_typingCounter; //!< Le nombre de caractères déjà écrits dans le cadre d'un affichage progressif.
};

#endif // PFTEXT_H_INCLUDED
