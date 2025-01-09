/*
	##################################################################
	### Project Socket by PREVOST Louis | SEYS Anthony | LIM Julie ###
	##################################################################
*/

#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */
#include <time.h>

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
	int cases_vides[9];
    int total = 0;

    for (int i = 0 ; i < 9 ; i++) {
        if (grille[i] == ' ') {
            cases_vides[total] = i ;
            total++;
        }
    }

    if (total == 0) {
        printf("Pas de cases vides disponibles.\n");
        return 0;
    }
	else {
    	srand(time(NULL));

    	int choix = cases_vides[rand() % total];

    	grille[choix] = 'O';
	}
}

char win(char grille[9])
{
	// Tableau des combinaisons gagnantes
	int combinaisons[8][3] = {
		{0, 1, 2}, // Ligne supérieure
		{3, 4, 5}, // Ligne centrale
		{6, 7, 8}, // Ligne inférieure
		{0, 3, 6}, // Colonne gauche
		{1, 4, 7}, // Colonne centrale
		{2, 5, 8}, // Colonne droite
		{0, 4, 8}, // Diagonale principale
		{2, 4, 6}  // Diagonale secondaire
	};

	// Vérification des combinaisons gagnantes
	for (int i = 0; i < 8; i++)
	{
		if (grille[combinaisons[i][0]] == grille[combinaisons[i][1]] &&
			grille[combinaisons[i][1]] == grille[combinaisons[i][2]] &&
			grille[combinaisons[i][0]] != ' ')
		{
			return grille[combinaisons[i][0]]; // Retourne 'X' ou 'O'
		}
	}

	// Vérification si toutes les cases sont prises
	for (int i = 0; i < 9; i++)
	{
		if (grille[i] == ' ')
		{
			return 'C'; // Partie en cours
		}
	}

	// Aucun gagnant et grille pleine
	return 'R'; // Match nul
}


int main(int argc, char *argv[]){
	int socketEcoute;

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageEnvoye[LG_MESSAGE];
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;

	char grille[9] ;

	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale
	//setsockopt()


	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
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

		char finJeu = 'N';

		init_grille(grille) ;
		
		while (finJeu != 'Y'){

			// Pour envoyer
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "Numéro de la case que vous souhaitez prendre : (1 à 9) :");
			ecrits = send(socketDialogue, messageEnvoye, strlen(messageEnvoye) + 1, 0);

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
			else if (lus == 0) {
				printf("Le client s'est déconnecté.\n");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	} 
			else if (lus < 0) {
				perror("Erreur lors de la réception du message");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	}

			char winner = win(grille);

				if (winner == 'X')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Client, vainqueur !\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
					finJeu = 'Y';
				}
				else if (winner == 'O')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Bot, vainqueur !\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
					finJeu = 'Y';
				}
				else if (winner == 'R')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Match nul !\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
					finJeu = 'Y';
				}
				else if (winner == 'C')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Tours suivant :\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
				}

			// au tour du serveur de jouer
			bot_player(grille);
			affiche(grille);
			send(socketDialogue, grille, sizeof(grille), 0);

			winner = win(grille);

				if (winner == 'X')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Client, vainqueur !\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
					finJeu = 'Y';
				}
				else if (winner == 'O')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Bot, vainqueur !\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
					finJeu = 'Y';
				}
				else if (winner == 'R')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Match nul !\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
					finJeu = 'Y';
				}
				else if (winner == 'C')
				{
					memset(messageEnvoye, 0x00, LG_MESSAGE);
					snprintf(messageEnvoye, LG_MESSAGE, "Tours suivant :\n");
					send(socketDialogue, messageEnvoye, LG_MESSAGE, 0);
				}
		}

	}
	// On ferme la ressource avant de quitter
   	close(socketEcoute);
	return 0; 
}
