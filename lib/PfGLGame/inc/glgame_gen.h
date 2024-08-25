/**
* @file
* @author Anaïs Vernet
* @brief Header inclus dans tous les fichiers header de cette bibliothèque.
* @date xx/xx/xxxx
* @version 0.0.0
*/

/**
* @mainpage
*
* <b>Bibliothèque contenant les outils nécessaires à la création d'un jeu Poufalouf sous OpenGL</b>
*
* Cette bibliothèque contient :
*
* <ul><li>les éléments d'un système MVC standard pour la gestion d'un jeu Poufalouf sous OpenGL (GLModel, GLController, GLView et GLItem ainsi que
* ses classes dérivées),</li>
* <li>des outils de gestion des animations (PfAnimationFrame, PfAnimation, PfAnimationGroup),</li>
* <li>des outils de gestion de la caméra (PfCamera),</li>
* <li>une série d'objets d'interface graphique type GUI (listés dans le header "pfgui.h").</li></ul>
*
* @see pfgui.h
*
* L'affichage d'un jeu Poufalouf est composé de la fenêtre centrale, sur laquelle se déroule l'action, et de bordures latérales. Celles-ci peuvent être
* vides (cas du jeu ou de menus), ou munies d'objets d'interface graphique GUI.
* Le repère de coordonnées peut être exprimé de manière absolue selon le système OpenGL ou relative aux bordures. Le choix est spécifié dans les différentes
* section de la documentation.
*
* @warning
* Cette bibliothèque dépend des bibliothèques PfMisc, PfMedia et PfMVC.
*
* Pour créer un jeu Poufalouf, il faudra linker dans le programme cette bibliothèque ainsi que les trois citées ci-dessus, car elles sont également
* directement utilisées par la plupart des programmes de jeu.
* L'ordre de dépendance est le suivant : libpfglgame -> libpfmvc -> libpfmedia -> libpfmisc.
*
* @section CreerJeu Les bases de la création d'un jeu Poufalouf sous OpenGL
*
* Cette section décrit les principales étapes à suivre lors de la création d'un jeu utilisant cette bibliothèque.
*
* @subsection MVCJeu Le système MVC
*
* Une redéfinition des trois composants de ce pattern est disponible dans cette bibliothèque. Le système MVC OpenGL pose les bases de la gestion de
* l'image.
*
* Faire hériter des composants plus complexes des classes GLModel, GLView et GLController est la méthode la plus simple de procéder.
* Ces trois composants seront ensuite à combiner pour la création d'un MVCSystem.
*
* @subsection CameraJeu Gérer la caméra
*
* La classe PfCamera est utilisée pour la gestion de la caméra dans les jeux Poufalouf.
*
* Pour définir une caméra par défaut, aucune action n'est à entreprendre autre que l'utilisation d'un GLModel. En effet, celui-ci possède un membre
* GLModel::m_camera. Le constructeur par défaut de PfCamera plaçant une caméra libre de toute cible au coin inférieur gauche de la map (0;0), tout est
* prêt pour ensuite l'utiliser à loisir.
*
* @subsection ObjetJeu Insérer un objet dans le modèle
*
* Les objets utilisés sont de classes héritant de GLItem, classe abstraite.
* Trois classes permettent de définir un ModelItem adapté à un jeu OpenGL :
*
* <ul><li>PolygonGLItem : un objet simple, doté d'un polygone et d'une texture fixe,</li>
* <li>RectangleGLItem : un objet rectangulaire, permettant d'imposer un format plus pratique pour la gestion de sprites,</li>
* <li>AnimatedGLItem : un objet doté d'animations.</li></ul>
*
* @subsection AnimJeu Animer un objet
*
* L'animation est une succession de sprites dans ce jeu.
*
* La classe PfAnimation est utilisée. Trois outils sont utilisés en tout :
*
* <ul><li>PfAnimationFrame : le composant élémentaire d'une animation, défini par une texture et quelques propriétés complémentaires,</li>
* <li>PfAnimation : une série de frames offrant quelques méthodes de parcours du contenu de l'animation,</li>
* <li>PfAnimationGroup : un outil utile pour gérer les animations disponibles d'un objet, et passer de l'une à l'autre rapidement.</li></ul>
*
* L'utilisation d'un AnimatedGLItem permet d'avoir accès à ces outils, car ce dernier possède un membre AnimatedGLItem::m_animationGroup.
* Pour animer un objet, se référer à cette classe.
*
* @subsection InterfaceJeu L'interface utilisateur
*
* En plus d'objets animés répondant ou non aux commandes du joueur, il est fort probable que le jeu nécessite des contrôles de type widgets ça et là,
* notamment dans les menus.
*
* Cette bibliothèque inclut une série de widgets permettant cela. Les PfWidget sont des AnimatedGLItem, orientés vers l'interface graphique
* utilisée dans les menus.
*/

#ifndef GL_GAME_GEN_H_INCLUDED
#define GL_GAME_GEN_H_INCLUDED

#define MAX_LAYER 10000000 //!< Le plan de perspective le plus en avant.
#define MAX_FRAMES 10000 //!< Le nombre maximal de frames par animation.

#define FONT_TEXTURE_INDEX 1000000 //!< Index de la texture de police par défaut.
#define FONT_DEFAULT "./res/font.png" //!< Le fichier de police par défaut.
#define FONT_TEXTURE_INDEX_2 1000001 //!< Index de la texture de la deuxième police par défaut.
#define FONT_DEFAULT_2 "./res/font_2.png" //!< Le deuxième fichier de police par défaut.
#define FONT_SIZE_DEFAULT 0.02 //!< La taille de la police par défaut.

#endif // GL_GAME_GEN_H_INCLUDED
