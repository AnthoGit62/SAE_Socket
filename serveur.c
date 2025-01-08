/*
	##################################################################
	### Project Socket by PREVOST Louis | SEYS Anthony | LIM Julie ###
	##################################################################
*/

#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>		/* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h>	/* pour htons et inet_aton */

#define PORT 5000 //(ports >= 5000 réservés pour usage explicite)

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

void creation_socket(int *socketEcoute)
{

	// Crée un socket de communication
	*socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if (*socketEcoute < 0)
	{
		perror("socket");
		exit(-1); // code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", *socketEcoute); // On prépare l’adresse d’attachement locale
}

void connexion(int *socketEcoute, socklen_t *longueurAdresse, struct sockaddr_in *pointDeRencontreLocal)
{

	// Remplissage de sockaddrDistant
	*longueurAdresse = sizeof(*pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(pointDeRencontreLocal, 0x00, *longueurAdresse);
	pointDeRencontreLocal->sin_family = PF_INET;
	pointDeRencontreLocal->sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal->sin_port = htons(PORT);				// = 5000 ou plus
												   // = 5000 ou plus

	// On demande l’attachement local de la socket
	if ((bind(*socketEcoute, (struct sockaddr *)pointDeRencontreLocal, *longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if (listen(*socketEcoute, 6) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
}

int main(int argc, char *argv[])
{

	int socketEcoute;
	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageEnvoye[LG_MESSAGE];
	int ecrits, lus;			  /* nb d’octets ecrits et lus */
	int retour;

	char grille[9] ;
	init_grille(grille);

	// Gestion des socket

	creation_socket(&socketEcoute);
	connexion(&socketEcoute, &longueurAdresse, &pointDeRencontreLocal);

	// T3N

	while (1) {
		memset(messageRecu, 0, LG_MESSAGE * sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0){
			perror("accept");
			close(socketDialogue);
			close(socketEcoute);
			exit(-4);
		}

		lus = recv(socketDialogue, messageRecu, LG_MESSAGE * sizeof(char), 0);
			switch (lus)
			{
			case -1:
				perror("read");
				close(socketDialogue);
				exit(-5);
			case 0:
				fprintf(stderr, "La socket a été fermée par le client !\n\n");
				close(socketDialogue);
				break;
			default:
				printf("Message reçu : %s (%d octets)\n\n", messageRecu, lus);
			}

			// Réponse selon le message reçu
			if (strncmp(messageRecu, "quit", 4) == 0)
			{
				send(socketDialogue, "Fermeture", strlen("Fermeture") + 1, 0);
				break;
			}
			if (strncmp(messageRecu, "play", 4) == 0)
			{
				send(socketDialogue, "Start", strlen("Start") + 1, 0);
				while (1){

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
			}
			else
			{
				char erreur[] = "Commande non reconnue.";
				send(socketDialogue, erreur, strlen(erreur) + 1, 0);
				printf("Commande inconnue envoyée au client.\n");
			}
			
		close(socketDialogue);
	}

	// On ferme la ressource avant de quitter
	close(socketEcoute);
	return 0;
}

