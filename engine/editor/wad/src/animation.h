/**
* @file
* @author Poufalouf
* @brief Fichier contenant les classes se rapportant aux animations.
* @version 0.0
* @date 26/04/2014
*/

#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include "gen.h"
#include <string>
#include <map>
#include "geometry.h"
#include "serializable.h"
#include "datapackage.h"
#include "noncopyable.h"
#include "enum.h"

/**
* @brief Frame d'une animation.
*
* Une frame contient les informations nécessaires à l'affichage d'une image
* et à la diffusion du son correspondant à une phase d'une animation.
*
* Une frame peut contenir des propriétés représentées sous forme de 8 flags.
*/
class PfAnimationFrame : public Serializable
{
	public:
		/**
		* @brief Enumération des sections de sauvegarde d'une PfAnimationFrame.
		*/
		enum SavePfAnimationFrame {SAVE_COORD, //!< section de sauvegarde des coordonnées
								   SAVE_COLOR, //!< section de sauvegarde de la couleur
								   SAVE_TEXTINDEX, //!< section de sauvegarde de l'indice de texture
								   SAVE_SOUNDINDEX, //!< section de sauvegarde de l'indice de son
								   SAVE_PROP, //!< section de sauvegarde des propriétés
								   SAVE_END //!< fin de sauvegarde
								  };

		enum PfAnimationFrameFlag {NO_FRAME_PROP = 0, //!< aucune propriété
								   FRAME_TURNABLE = 1 //!< propriété orientable, la frame utilise une première colonne (spécifiée) pour l'orientation sud, puis dans le sens des auguilles d'une montre utilise les 3 colonnes suivantes.
								  };

		/*
		* Constructeurs et destructeur
		* ----------------------------
		*/
		/**
		* @brief Constructeur PfAnimationFrame 1.
		* @param textureIndex l'indice de la texture dans le wad.
		* @param textCoordRectangle les coordonnées du fragment de texture.
		* @param soundIndex l'indice du son à associer (0 pour aucun son).
		* @param color la couleur de cette frame.
		*/
		PfAnimationFrame(unsigned int textureIndex = 0, const PfRectangle& textCoordRectangle = PfRectangle(), unsigned int soundIndex = 0, const PfColor& color = PfColor::WHITE);
		/**
		* @brief Constructeur PfAnimationFrame 2.
		* @param data les données de chargement.
		* @throw ConstructorException si les données ne sont pas valides.
		*/
		PfAnimationFrame(DataPackage& data);
		/**
		* @brief Destructeur PfAnimationFrame.
		*/
		virtual ~PfAnimationFrame() {}

		/*
		* Méthodes
		* --------
		*/
		/**
		* @brief Indique si une ou plusieurs propriétés sont vérifiées par cette frame.
		* @return vrai si la propriété est vérifiée.
		*/
		bool prop(PfAnimationFrameFlag flag);
		/**
		* @brief Ajoute une ou plusieurs propriétés à cette frame.
		* @param flag la ou les propriétés à ajouter.
		*/
		void addProperties(PfAnimationFrameFlag flag);
		/**
		* @brief Efface toutes les propriétés de cette frame.
		*/
		void eraseProperties();
		/**
		* @brief Retourne le rectangle de coordonnées de la texture correspondant à cette frame.
		* @param ori l'orientation de la frame (utilisée pour les frames orientables).
		* @return le rectangle de coordonnées.
		*
		* Cette méthode tient compte des propriétés de cette frame, et notamment de son caractère orientable.
		*/
		PfRectangle textCoordRectangle(PfOrientation::PfOrientationValue ori = PfOrientation::NO_ORIENTATION) const;

		/*
		* Redéfinitions
		* -------------
		*/
		/**
		* @brief Sérialise cet objet.
		* @param r_ofs le flux en écriture.
		*
		* Le flux doit être positionné à l'endroit désiré pour l'écriture, il n'est pas rembobiné en fin de méthode.
		*
		* Les données devraient être stockées de façon à pouvoir être affectées à un DataPackage (indication du type de valeur avant chaque valeur).
		*/
		virtual void saveData(ofstream& r_ofs) const;

		/*
		* Accesseurs
		* ----------
		*/
		unsigned int getTextureIndex() const {return m_textureIndex;}
		const PfColor& getColor() const {return m_color;}
		unsigned int getSoundIndex() const {return m_soundIndex;}

	private:
		unsigned int m_textureIndex; //!< L'indice de la texture de l'image.
		PfRectangle m_textCoordRectangle; //!< Les coordonnées du fragment de texture.
		unsigned int m_soundIndex; //!< L'indice du son associé à cette frame.
		PfColor m_color; //!< La couleur de l'image.
		pfflag m_properties; //!< les propriétés de cette frame.
};

/**
* @brief Animation d'un objet affichable à l'écran.
*
* Une animation est composée d'une série de PfAnimationFrame.
*
* Les PfAnimationFrame de cette animation sont détruites par le destructeur de cette classe.
*
* La frame en cours est repérée par un indice PfAnimation::m_currentFrame dont
* la valeur est 1 pour la première frame. 0 indique une frame invalide ou
* une animation vide.
*
* Si PfAnimation::m_loop est vrai, alors l'animation sera jouée en boucle.
* Ceci est géré dans la méthode PfAnimation::increaseFrame.
*/
class PfAnimation : public Serializable
{
	public:
    	/*
	    * Constructeurs et destructeur
	    * ----------------------------
	    */
   	 	/**
  		* @brief Constructeur PfAnimation par défaut.
    	*
    	* L'animation crée n'a aucune frame et son indice de frame est à 0.
    	*/
    	PfAnimation();
    	/**
    	* @brief Constructeur PfAnimation 1.
    	* @param textureIndex l'indice de la texture.
    	* @param textCoordRectangle les coordonnées du fragment de texture.
		* @param loop vrai si l'animation tourne en boucle.
		* @param soundIndex l'indice du son associé à cette frame (0 pour aucun son).
    	* @param color la couleur de la frame.
    	*
    	* Construit une animation possédant une PfAnimationFrame construite à
    	* partir des paramètres. L'indice de frame est à 1.
    	*/
    	PfAnimation(unsigned int textureIndex, const PfRectangle& textCoordRectangle, bool loop = false, unsigned int soundIndex = 0, const PfColor& color = PfColor::WHITE);
		/**
		* @brief Constructeur PfAnimation 2.
		* @param data les données de chargement.
		* @throw ConstructorException si les données ne sont pas valides.
		*/
		PfAnimation(DataPackage& data);
		/**
		* @brief Constructeur PfAnimation par copie.
		* @param anim l'animation à copier.
		*/
		PfAnimation(const PfAnimation& anim);
    	/**
    	* @brief Destructeur PfAnimationFrame.
    	*
    	* Détruit les frames de la liste.
    	*/
    	virtual ~PfAnimation();

    	/*
    	* Méthodes
    	* --------
    	*/
    	/**
    	* @brief Passe à la frame suivante.
    	* @return vrai si la frame a été incrémentée.
    	* @throw PfException si la frame actuelle est 0.
    	*
    	* Si la frame en cours est la dernière, rien n'est fait.
		* Le booléen PfAnimation::m_over passe à l'état vrai.
    	*/
    	bool increaseFrame();
		/**
		* @brief Remet cette animation dans son état initial.
		* @throw PfException si cette animation n'a pas de frame.
		*
		* La frame actuelle de l'animation passe à 1, et le booléen PfAnimation::m_over passe à l'état faux.
		*/
		void reset();
    	/**
    	* @brief Ajoute une frame à cette animation.
    	* @param p_frame la frame à ajouter.
    	* @param index l'indice à laquelle ajouter la frame.
    	* @return vrai si la frame a été ajoutée.
    	*
    	* Si l'indice d'ajout est plus élevé que la taille de la liste de frames,
    	* alors la frame est ajoutée à la fin de la liste.
    	*
    	* Si le pointeur passé en paramètre est nul, alors rien n'est fait et
    	* faux est retourné.
    	*
    	* @warning
    	* La frame ajoutée sera détruite par le destructeur de cette classe.
    	*/
    	bool addFrame(PfAnimationFrame* p_frame, unsigned int index = MAX_FRAMES);
    	/**
    	* @brief Retourne la frame en cours.
    	* @return Une référence constante vers la frame en cours.
    	* @throw PfException si la frame actuelle n'est pas définie ou invalide.
   		*/
    	const PfAnimationFrame& currentFrame() const;

		/*
		* Redéfinitions
		* -------------
		*/
		/**
   		* @brief Sérialise cet objet.
	    * @param r_ofs le flux en écriture.
	    *
	    * Le flux doit être positionné à l'endroit désiré pour l'écriture, il n'est pas rembobiné en fin de méthode.
		*
	   	* Les données devraient être stockées de façon à pouvoir être affectées à un DataPackage (indication du type de valeur avant chaque valeur).
	    */
    	virtual void saveData(ofstream& r_ofs) const;

		/*
		* Opérateurs
		* ----------
		*/
    	/**
        * @brief Opérateur d'affectation.
        * @param anim l'animation à affecter à celle-ci.
        */
		PfAnimation& operator=(const PfAnimation& anim);

		/*
		* Accesseurs
		* ----------
		*/
		bool isLoop() const {return m_loop;}
		void setLoop(bool loop) {m_loop = loop;}
		bool isOver() const {return m_over;}

	private:
		bool m_loop; //!< Indique que l'animation tourne en boucle.
		bool m_over; //!< Indique que l'animation a été entièrement jouée.
    	vector<PfAnimationFrame*> mp_frames_v; //!< La liste de frames.
    	unsigned int m_currentFrame; //!< Le numéro de la frame en cours.
};

/**
* @brief Groupe d'animations.
*
* Un groupe d'animations est une map d'animations dont les clés sont les
* valeurs de l'énumération PfAnimationGroup::PfAnimationValue.
*
* A chaque clé correspond une animation valide. Si la map ne contient aucune
* animation, alors la valeur de l'animation courante est NO_ANIMATION.
*
* Les animations ajoutées dans ce groupe (en tant que pointeurs) sont détruites
* par le destructeur de ce PfAnimationGroup.
*
* @warning
* Tout groupe d'animation doit contenir une animation STILL, utilisée par défaut par différents processus du programme.
*/
class PfAnimationGroup : private NonCopyable
{
	public:
	    /*
	   	* Constructeurs et destructeur
	    * ----------------------------
	    */
	    /**
	    * @brief Constructeur PfAnimationGroup par défaut.
	    *
	    * La valeur de l'animation en cours est NO_ANIMATION.
	    */
	    PfAnimationGroup();
	    /**
	    * @brief Destructeur PfAnimationGroup.
	    *
	    * Détruit les animations de ce groupe.
	    */
	    virtual ~PfAnimationGroup();

	    /*
	    * Méthodes
	    * --------
	    */
	    /**
	    * @brief Ajoute une animation à ce groupe.
	    * @param p_anim l'animation à ajouter.
	    * @param status le statut de l'animation.
	    * @throw ArgumentException si l'animation est un pointeur nul ou si
	    * la valeur d'animation est NO_ANIMATION.
	    *
	    * Si une animation existe déjà pour la valeur spécifiée, alors celle-ci
	    * est détruite et remplacée.
	    */
	    void addAnimation(PfAnimation* p_anim, PfAnimationStatus status);
	    /**
	    * @brief Change l'animation en cours.
	    * @param value le statut de la nouvelle animation à jouer.
	    * @return vrai si l'animation a été changée.
	    *
	    * Si l'animation demandée est déjà en cours, alors rien n'est fait et
	    * faux est retourné.
	    *
	    * Si l'animation demandée n'est pas valide,
		* alors rien n'est fait et faux est retourné.
		*
		* L'animation quittée est remise à zéro au moyen de la méthode PfAnimation::reset.
	   	*/
	    bool changeAnimation(PfAnimationStatus value);
	    /**
	    * @brief Retourne l'animation en cours.
	    * @return Un pointeur vers l'animation en cours.
	    * @throw PfException si l'animation actuelle est NO_ANIMATION ou si à l'animation actuelle correspond un pointeur nul.
	    */
	    PfAnimation* currentAnimation();
	    /**
	    * @brief Retourne l'animation en cours.
	    * @return Une référence constante vers l'animation en cours.
	    * @throw PfException si l'animation actuelle est NO_ANIMATION ou si à l'animation actuelle correspond un pointeur nul.
	    */
		const PfAnimation& currentConstAnim() const;
	    /**
	    * @brief Retourne la frame actuelle de l'animation en cours.
	    * @return Une référence constante vers la frame en cours.
	    * @throw PfException si la frame actuelle n'est pas définie.
	    */
	    const PfAnimationFrame& currentFrame() const;

		/*
		* Accesseurs
		* ----------
		*/
		PfAnimationStatus getCurrentAnimation() const {return m_currentAnimation;}

	private:
	    map<PfAnimationStatus, PfAnimation*> mp_animations_map; //!< La map d'animations.
	    PfAnimationStatus m_currentAnimation; //!< Le statut de l'animation en cours.
};

#endif // ANIMATION_H_INCLUDED
