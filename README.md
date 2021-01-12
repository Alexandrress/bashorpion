- Compilation :

. Il faut ouvrir un terminal et utiliser la commande "make".<br>
. On peut aussi enlever tous les fichiers .exe et .o du répertoire en utilisant 
la commande "make clean".


- Exécuter le projet Bashorpion :

. Exécuter le serveur avec la commande ./BashorpionServer.exe
. Exécuter ensuite un client avec la commande ./BashorpionClient.exe
. Veuillez entrer un username, par exemple "Alexandre" ou "Nicolas".
. Vous pouvez ensuite utiliser les commandes "lobby de jeu" suivantes :
	"list" (Permet de visualiser les joueurs connectés au lobby)
	"exit" (Permet de terminer la connection au lobby de jeu)
	"battle <nomDuJoueur>" (Permet de défier un autre joueur)
	"accept" (Permet de relever un défi si vous avez été défié au préalable)
	"deny" (Permet de refuser un défi si vous avez été défié au préalable)
. Vous êtes maintenant en partie en train de défier un autre joueur (sous
condition qu'il y ait plus de deux clients dans le lobby, vous pouvez exécuter
le programme client sur la même machine deux fois, pour tester)
	"play <caseDuCoup>" (Envoie le coup à jouer à l'adversaire)
. A la fin de la partie, les clients se terminent.


- Générer la documentation Doxygen :

. Il faut utiliser la commande "doxygen BashorpionServer.c".
