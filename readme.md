# Jeu Poufalouf, version C++

Il s'agit d'un moteur de jeu, davantage qu'un jeu, c'est pourquoi j'avais fini par l'appeler "Engine".
Le jeu n'est pas terminé, il n'y a pas d'objectif en tant que tel, mais la conception des maps fonctionne bien.

Le dossier est séparé en plusieurs parties :

- engine : c'est le coeur du programme, divisé en deux sous-parties, game (le jeu) et editor (les éditeurs de fichiers de configuration)
- lib : des librairies .a, modules utilisés par engine

L'éditeur n'est pas l'éditeur de map qui est directement intégré à game, il s'agit plutôt de l'éditeur de fichiers config tels que wad, textures, et menu. Le concept est qu'à partir d'un fichier texte, un fichier binaire est créé par exécution du script de chaque éditeur, fichier binaire qui, lui, est lu par l'engine. Voilà, quand on peut faire simple...

Je stocke ce programme ici pour l'instant, mais il faut que je le reprenne, c'est dommage de l'avoir abandonné comme ça. Pour l'instant je ne m'attarde pas plus dessus et je le mets tel quel sur ce repository.

Musique et sound design, textures, tout est maison.

POUR JOUER, c'est game.exe dans /engine/game. La compilation est hasardeuse, je n'y arrive pas sous VS Code mais ça fonctionnait sous Codeblocks, je me repencherai sur tout ça avec une doc adaptée pour un futur commit.

## Documentation

Il y en a une, format Doxygen. Reprenant le projet plusieurs années après, c'est quand même compliqué de s'y retrouver.
Ce que je comprends en reprenant le code et en testant, pour l'éditeur :

- F4 : sauvegarder
- F9 : accès aux propriétés de la map
- Alt : en cliquant sur une case en mode Relief, orienter la flèche, puis avec + et - en maintenant Alt enfoncé, changement du relief selon la flèche.

Ce sont les seuls instructions que je vais écrire pour l'instant, le reste est intuitif en essayant.

## Oupsies

A priori il y aurait eu un chercher/remplacer douteux qui aurait ajouter "ANIM_SELECTED" dans certains champs de classes, mais comme c'est fait de manière homogène dans tout le code, cela ne pose pas de problème de fonctionnement.
