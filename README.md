- Compilation : <br>

. Il faut ouvrir un terminal et utiliser la commande "make". <br>
. On peut aussi enlever tous les fichiers .exe et .o du répertoire en utilisant 
la commande "make clean". <br>


- Exécuter le projet Bashorpion (SUR LE MEME ORDINATEUR) : <br>

. Exécuter le serveur avec la commande ./BashorpionServer.exe <br>
. Exécuter ensuite un client avec la commande ./BashorpionClient.exe <br>
. Veuillez rentrer un port 60001 pour le client qui se fera défié.<br>
. Veuillez rentrer l'adresse du serveur Bashorpion càd 127.0.0.1 en local.<br>
. Veuillez entrer un username, par exemple "Alexandre" ou "Nicolas". <br>
. Vous pouvez ensuite utiliser les commandes "lobby de jeu" suivantes : <br>
&nbsp;&nbsp;"list" (Permet de visualiser les joueurs connectés au lobby) <br>
&nbsp;&nbsp;"exit" (Permet de terminer la connection au lobby de jeu) <br>
&nbsp;&nbsp;"battle <nomDuJoueur>" (Permet de défier un autre joueur) <br>
&nbsp;&nbsp;"accept" (Permet de relever un défi si vous avez été défié au préalable) <br>
&nbsp;&nbsp;"deny" (Permet de refuser un défi si vous avez été défié au préalable) <br>
. Exécuter ensuite un second client avec la commande ./BashorpionClient.exe <br>
. Veuillez rentrer un port différent de 60001 et 60002, et non assigné. <br>
. Veuillez rentrer l'adresse du serveur Bashorpion càd 127.0.0.1 en local. <br>
. Vous pouvez maintenant faire "list" et défier le premier client avec le port 60001 d'ouvert. <br>
. Si l'autre joueur accepte vous êtes maintenant en duel. <br>
&nbsp;&nbsp;"play <caseDuCoup>" (Envoie le coup à jouer à l'adversaire) <br>
. A la fin de la partie, les clients peuvent se redéfier ou revenir en lobby. <br>
. Vous pouvez maintenant utiliser la commande "leave" pour quitter le lobby.


- Exécuter le projet Bashorpion (EN LAN) : <br>

. Exécuter le serveur sur un premier PC avec la commande ./BashorpionServer.exe <br>
. Exécuter ensuite un client sur le même PC avec la commande ./BashorpionClient.exe <br>
. Veuillez rentrer un port 60002 pour le client.<br>
. Veuillez rentrer l'adresse du serveur Bashorpion càd 127.0.0.1 pour celui qui host le serveur (le premier PC).<br>
. Veuillez entrer un username, par exemple "Alexandre" ou "Nicolas". <br>
. Vous pouvez ensuite utiliser les commandes "lobby de jeu" suivantes : <br>
&nbsp;&nbsp;"list" (Permet de visualiser les joueurs connectés au lobby) <br>
&nbsp;&nbsp;"exit" (Permet de terminer la connection au lobby de jeu) <br>
&nbsp;&nbsp;"battle <nomDuJoueur>" (Permet de défier un autre joueur) <br>
&nbsp;&nbsp;"accept" (Permet de relever un défi si vous avez été défié au préalable) <br>
&nbsp;&nbsp;"deny" (Permet de refuser un défi si vous avez été défié au préalable) <br>
. Exécuter ensuite un second client sur un second PC avec la commande ./BashorpionClient.exe <br>
. Veuillez rentrer un port 60002 pour le second client.<br>
. Veuillez rentrer l'adresse du serveur Bashorpion càd l'adresse IP du PC du premier client en local. <br>
. Un des deux joueurs peut maintenant faire "list" et défier l'autre joueur. <br>
. Si l'autre joueur accepte vous êtes maintenant en duel. <br>
&nbsp;&nbsp;"play <caseDuCoup>" (Envoie le coup à jouer à l'adversaire) <br>
. A la fin de la partie, les clients peuvent se redéfier ou revenir en lobby. <br>
. Vous pouvez maintenant utiliser la commande "leave" pour quitter le lobby.


- Générer la documentation Doxygen : <br>

. Il faut utiliser la commande "doxygen BashorpionServer.c". <br>
