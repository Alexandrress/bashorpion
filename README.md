- Compilation : <br>

. Il faut ouvrir un terminal et utiliser la commande "make".<br>
. On peut aussi enlever tous les fichiers .exe et .o du répertoire en utilisant 
la commande "make clean". <br>


- Exécuter le projet Bashorpion : <br>

. Exécuter le serveur avec la commande ./BashorpionServer.exe <br>
. Exécuter ensuite un client avec la commande ./BashorpionClient.exe <br>
. Veuillez entrer un username, par exemple "Alexandre" ou "Nicolas". <br>
. Vous pouvez ensuite utiliser les commandes "lobby de jeu" suivantes : <br>
	"list" (Permet de visualiser les joueurs connectés au lobby) <br>
	"exit" (Permet de terminer la connection au lobby de jeu) <br>
	"battle <nomDuJoueur>" (Permet de défier un autre joueur) <br>
	"accept" (Permet de relever un défi si vous avez été défié au préalable) <br>
	"deny" (Permet de refuser un défi si vous avez été défié au préalable) <br>
. Vous êtes maintenant en partie en train de défier un autre joueur (sous <br>
condition qu'il y ait plus de deux clients dans le lobby, vous pouvez exécuter <br>
le programme client sur la même machine deux fois, pour tester) <br>
	"play <caseDuCoup>" (Envoie le coup à jouer à l'adversaire) <br>
. A la fin de la partie, les clients se terminent. <br>


- Générer la documentation Doxygen : <br>

. Il faut utiliser la commande "doxygen BashorpionServer.c". <br>
