/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant la classe PfSpinBox.
* @date xx/xx/xxxx
* @version 0.0.0
*/

#ifndef PFSPINBOX_H_INCLUDED
#define PFSPINBOX_H_INCLUDED

#include "glgame_gen.h"

#include "pfwidget.h"
#include "pfbutton.h"
#include "pftextbox.h"

/**
* @brief Boîte numérique.
*
* Une boîte numérique est composée d'une zone de texte ainsi que de deux boutons servant à l'incrémentation ou la décrémentation de sa valeur.
*
* L'effet des boutons est EFFECT_NEXT ou EFFECT_PREV  pour respectivement le premier et le deuxième bouton, avec pour valeur le nombre de pas à effectuer.
*
* @section AnimDef Définition des animations dans le wad
*
* La texture des flèches est déterminée par le wad.
* La boîte de texte est située dans le même fichier, en haut à gauche.
* L'image des boutons, dans le wad, doit représenter les boutons "flèches" vers le haut.
*
* Pour le calcul des coordonnées de texture des éléments de la boîte, il est nécessaire que l'image des boutons soit de dimensions carrées.
*
* Dans le script du wad, ce sont les textures des boutons flèches (vers le haut) qui sont visées lors de la définition des frames.
* L'image de la boîte de texte est automatiquement prise dans le même fichier image, selon la description du paragraphe ci-dessus.
*
* Pour le bouton orienté vers le bas, c'est le programme qui se charge de la rotation nécessaire.
* Le programme considère toujours que l'image de base pointe vers le haut.
*/
class PfSpinBox : public PfWidget
{
	public:
		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfSpinBox.
		* @param name le nom de cette boîte numérique.
		* @param rect le rectangle de coordonnées de ce widget.
		* @param layer le plan de perspective de ce widget.
		* @param minValue la valeur minimale.
		* @param maxValue la valeur maximale.
		* @param statusMap la valeur d'initialisation de la map d'association animation <-> état de ce widget.
		* @throw ConstructorException si une erreur survient lors de la construction.
		*/
		PfSpinBox(const string& name, const PfRectangle& rect, int layer = MAX_LAYER, unsigned int minValue = 0, unsigned int maxValue = 0,
					PfWidget::PfWidgetStatusMap statusMap = WIDGET_STANDARD_MAP);
		/**
		* @brief Destructeur PfSpinBox.
		*
		* Dértuit les éléments de cette boîte numérique.
		*/
		virtual ~PfSpinBox();
		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Modifie la valeur de cette boîte numérique.
		* @param value la nouvelle valeur.
		*
		* Rien n'est fait si la valeur dépasse le maximum.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void changeValue(unsigned int value);
		/**
		* @brief Ajoute una animation aux boutons de cette boîte de texte.
        * @param p_anim l'animation à ajouter.
        * @param value le statut de l'animation.
        * @throw PfException si une exception survient lors de l'ajout.
		*
		* Si une animation existe déjà pour ce statut, elle est remplacée.
		*
        * Le pointeur à ajouter ne doit pas être nul. Ce pointeur sera
        * détruit par le destructeur du PfAnimationGroup de cette classe.
		*
		* Appelle la méthode AnimatedGLItem::addAnimation sur chaque bouton de cette boîte numérique.
		*
		* L'animation ANIM_IDLE de la boîte de texte est créée si la valeur du paramètre value est ANIM_IDLE.
		* Cette animation est construite avec le même indice de texture, comme indiqué dans la description de cette classe.
		*
		* L'animation passée en paramètre est copiée puis détruite.
		*/
		virtual void addAnimation(PfAnimation* p_anim, PfAnimationStatus value);
		/**
		* @brief Surligne ce widget.
		*
		* Appelle la méthode PfWidget::highlight.
		*
		* Surligne le bouton sous la souris ou les quitte tous.
		*/
		virtual void highlight();
		/**
		* @brief Désurligne ce widget.
		*
		* Appelle la méthode PfWidget::leave.
		*
		* Quitte tous les boutons de ce widget et la boîte de texte.
		* Si le texte de la boîte de texte est vide ou inférieur à la valeur minimale, la valeur minimale est inscrite.
		*/
		virtual void leave();
        /**
		* @brief Sélectionne ce widget.
		*
		* Appelle la méthode PfWidget::select.
		*
		* Les effets de ce widget sont annulés.
		*
		* Sélectionne la boîte de texte et quitte tous les boutons.
		*/
		virtual void select();
		/**
		* @brief Déselectionne ce widget.
		*
		* Appelle la méthode PfWidget::deselect.
		*
		* Quitte tous les boutons de ce widget et la boîte de texte.
		* Si le texte de la boîte de texte est vide ou inférieur à la valeur minimale, la valeur minimale est inscrite.
		*/
		virtual void deselect();
		/**
		* @brief Active ce widget.
		*
		* Appelle la méthode PfWidget::activate.
		*
		* Si le point d'activation est sur une flèche, alors le nombre s'incrémente ou se décrémente.
		* Si c'est un bouton, alors la sélection est faite et l'effet de ce widget prend la valeur EFFECT_SELECT ainsi qu'une valeur correspondant à la nouvelle valeur.
		*
		* @remarks
		* Ce ModelItem est marqué modifié.
		*/
		virtual void activate();
		/**
		* @brief Met à jour ce ModelItem.
		*
		* Appelle la méthode PfWidget::update sur tous les composants de cette boîte numérique et sur elle-même.
		*/
		virtual void update();
        /**
        * @brief Génère un Viewable à partir de ce ModelItem.
        * @return le Viewable créé.
		* @throw ViewableGenerationException si la génération échoue.
        *
        * @warning
        * De la mémoire est allouée pour le pointeur retourné.
        */
        virtual Viewable* generateViewable() const;
		/**
		* @brief Traite l'instruction passée en paramètre.
		* @return RETURN_OK si l'instruction a été traitée, RETURN_NOTHING sinon.
		*
		* Appelle la méthode PfWidget::processInstruction.
		*
		* Traite ensuite les instructions comme suit :
		* <ul><li>INSTRUCTION_MODIFY : si la valeur de l'instruction est comprise entre '0' et '9' (valeur char), et si ce nombre ajouté au texte actuel de cette boîte numérique
		* ne conduit pas à un nombre supérieur à la valeur PfSpinBox::m_maxValue, alors la valeur est modifiée et le texte changé.</li>
		* <li>INSTRUCTION_DELETE : l'instruction est transmise à la boîte de texte et la valeur de cette boîte numérique est mise à jour.</li></ul>
		*/
		virtual PfReturnCode processInstruction();
		/*
		* Accesseurs
		* ----------
		*/
		unsigned int getValue() const {return m_value;}
		unsigned int getMinValue() const {return m_minValue;}
		void setMinValue(unsigned int min) {m_minValue = min;}
		unsigned int getMaxValue() const {return m_maxValue;}
		void setMaxValue(unsigned int max) {m_maxValue = max;}

	private:
		PfTextBox* mp_textBox; //!< La boîte de texte.
		PfWidget* mp_firstButton; //!< Le bouton flèche haute.
		PfWidget* mp_secondButton; //!< Le bouton flèche basse.
		unsigned int m_value; //!< La valeur de cette boîte numérique.
		unsigned int m_minValue; //!< La valeur minimale de cette boîte numérique.
		unsigned int m_maxValue; //!< La valeur maximale de cette boîte numérique.
};

#endif // PFSPINBOX_H_INCLUDED
