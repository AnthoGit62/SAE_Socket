// Branche de dev louis, code sale mais qui marche

#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <time.h>

#define PORT 5000
#define LG_MESSAGE 256

void init_grille(char grille[9]){
	for(int i = 0 ; i < 9 ; i++){
			grille[i] = ' ' ;
		}
}

void affiche(char grille[9]){
	printf("~~~~~~~~~~~~~~~~~~~~\n");
	printf(" %c | %c | %c\n" , grille[0], grille[1], grille[2]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[3], grille[4], grille[5]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[6], grille[7], grille[8]);
}

void prendre_case(char grille[9], char *messageRecu) {
    int caseNum;

    sscanf(messageRecu, "%d", &caseNum);

	if(grille[caseNum - 1] == ' '){
		switch (caseNum) {
			case 1:  
				grille[0] = 'X'; 
				break;
			case 2:  
				grille[1] = 'X'; 
				break;
			case 3:  
				grille[2] = 'X'; 
				break;
			case 4:  
				grille[3] = 'X'; 
				break ; 
			case 5:  
				grille[4] = 'X'; 
				break;
			case 6:  
				grille[5] = 'X'; 
				break;
			case 7:  
				grille[6] = 'X'; 
				break;
			case 8:  
				grille[7] = 'X'; 
				break;
			case 9:  
				grille[8] = 'X'; 
				break;
			default:
				printf("Option invalide.\n");
				return;
    	}
	}
}

int bot_player(char grille[9]) {
	for(int i = 0 ; i < 8 ; i++){
		if(grille[i] == ' '){
			grille[i] = 'O';
			return 0;
		}
	}
	return 1 ;
}



int main(int argc, char *argv[]){

	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;

	char messageRecu[LG_MESSAGE];
	char messageEnvoye[LG_MESSAGE];

	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;

	char grille[9] ;


// Crée un socket de communication

	socketEcoute = socket(AF_INET, SOCK_STREAM, 0);

	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket");
		exit(-1);
	}

	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);

	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); 
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT); // = 5000 ou plus
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2); 
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
	 
// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 

	while(1){

		memset(messageRecu, 'a', LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
		
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);

		if (socketDialogue < 0) {
   			perror("accept");
			close(socketDialogue);
   			close(socketEcoute);
   			exit(-4);
		}

		memset(messageEnvoye, 0x00, LG_MESSAGE);
		memset(messageRecu, 0x00, LG_MESSAGE);
		init_grille(grille) ;

	// Jeu :
		while(1){
			// Pour envoyer
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "Numéro de la case que vous souhaitez prendre : (1 à 9) :");
			send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);

			// Pour recup
			memset(messageRecu, 0x00, LG_MESSAGE);
			lus = recv(socketDialogue, messageRecu, LG_MESSAGE, 0);

			// On verifie , on claim la case et on envoie au joueur
			if (lus > 0) {
				printf("Message reçu : %s\n", messageRecu);
				prendre_case(grille, messageRecu);
				affiche(grille);
				send(socketDialogue, grille, sizeof(grille), 0);
			}

			// au tour du serveur de jouer
			bot_player(grille);
			affiche(grille);
			send(socketDialogue, grille, sizeof(grille), 0);
		}

        close(socketDialogue);
	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
