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
#include <unistd.h> // Pour la fonction sleep()

#define PORT 5000
#define LG_MESSAGE 256

void init_grille(char grille[9]){
	for(int i = 0 ; i < 9 ; i++){
			grille[i] = ' ' ;
		}
}

void affiche(char grille[9]){
	printf(" %c | %c | %c\n" , grille[0], grille[1], grille[2]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[3], grille[4], grille[5]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[6], grille[7], grille[8]);
	printf("~~~~~~~~~~~~~~~~~~~~\n");
}

void prendre_case(char grille[9], char *messageRecu , char player) {
    int caseNum;

    sscanf(messageRecu, "%d", &caseNum);

	if(grille[caseNum - 1] == ' '){
		switch (caseNum) {
			case 1:  
				grille[0] = player; 
				break;
			case 2:  
				grille[1] = player; 
				break;
			case 3:  
				grille[2] = player; 
				break;
			case 4:  
				grille[3] = player; 
				break ; 
			case 5:  
				grille[4] = player; 
				break;
			case 6:  
				grille[5] = player; 
				break;
			case 7:  
				grille[6] = player; 
				break;
			case 8:  
				grille[7] = player; 
				break;
			case 9:  
				grille[8] = player; 
				break;
			default:
				printf("Option invalide.\n");
				return;
    	}
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
	int socketDialogueCli1;
	int socketDialogueCli2;

	struct sockaddr_in pointDeRencontreLocal;
	struct sockaddr_in pointDeRencontreDistant;
	socklen_t longueurAdresse;

	char messageRecu[LG_MESSAGE];
	char messageEnvoye[LG_MESSAGE];

	int ecrits, lus, retour, wait ;
	char winner ;

	int spectateurs[10];
	int nb_spectateurs = 0;

	char grille[9] ;

	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0); 

	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("socket"); // Affiche le message d’erreur 
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute);

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);

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
	
	printf("En attente de deux connexions clients...\n");

// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 
	while(1){

	//memset(messageRecu, 'a', LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n");
		
	// Accepter la première connexion client
		socketDialogueCli1 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogueCli1 < 0) {
			perror("accept (Client 1)");
			close(socketEcoute);
			exit(-4);
		}
		printf("Client 1 connecté !\n");

	// Accepter la deuxième connexion client
		socketDialogueCli2 = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogueCli2 < 0) {
			perror("accept (Client 2)");
			close(socketDialogueCli2);
			close(socketEcoute);
			exit(-5);
		}
		printf("Client 2 connecté !\n");

		wait = 0 ;

	// Cette boucle attend 5 secondes que des spectateurs se connectent
		while (wait < 5) {

		// On attend 5 secondes que les spectateurs se connectent  
			if (select(socketEcoute + 1, &(fd_set){{1U << socketEcoute}}, NULL, NULL, &(struct timeval){5, 0}) > 0) {
				if(nb_spectateurs < 9){
					spectateurs[nb_spectateurs] = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
					if (spectateurs[nb_spectateurs] >= 0) {
						printf("Spectateur connecté !\n");
						nb_spectateurs++ ;
					} else {
						perror("accept (spectateur)");
					}
				}
			} 

			// Je ne fait le test que tt les secondes pour ne pas abuser sur les calculs
			sleep(1);
			wait ++ ;
			printf("attente ...%d\n", wait);
		}

	// Pour set la grille vide en début de partie
		init_grille(grille) ;
		
	// Boucle du jeu :
		while (1){

		//--------------------------------------------- Joueur 1 ---------------------------------------------

		// Parler au client 1
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "Numéro de la case que vous souhaitez prendre : (1 à 9) :");
			ecrits = send(socketDialogueCli1, messageEnvoye, strlen(messageEnvoye) + 1, 0);

			if (ecrits == 0) {
				printf("Le client s'est déconnecté.\n");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	} 
			else if (ecrits < 0) {
				perror("Erreur lors de la réception du message");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	}

		// Faire attendre le client 2
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "Votre adversaire joue ...\n");
			ecrits = send(socketDialogueCli2, messageEnvoye, strlen(messageEnvoye) + 1, 0);

			if (ecrits == 0) {
				printf("Le client s'est déconnecté.\n");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	} 
			else if (ecrits < 0) {
				perror("Erreur lors de la réception du message");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	}

		// Envoyer un message au spectateur 
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "A\n");
			for (int i = 0; i < nb_spectateurs; i++){
				send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
			}	

		// Choix du client 1
			memset(messageRecu, 0x00, LG_MESSAGE);
			lus = recv(socketDialogueCli1, messageRecu, LG_MESSAGE, 0);

		// On verifie , on claim la case et on envoie au joueurs + specs
			if (lus > 0) {
				prendre_case(grille, messageRecu, 'X');
				affiche(grille);
				send(socketDialogueCli1, grille, sizeof(grille), 0);
				send(socketDialogueCli2, grille, sizeof(grille), 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], grille, sizeof(grille), 0);
				}
			}
			else if (lus == 0) {
				printf("Le client 1 s'est déconnecté.\n");
				break;
        	} 
			else if (lus < 0) {
				perror("Erreur lors de la réception du message");
				break;
        	}

		// On verifie si un joueurs à gagné et on envoie l'avancé a tout le monde
			winner = win(grille);

			if (winner == 'X')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Les X gagnent !\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
				break;
			}
			else if (winner == 'O')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Les O gagnent !\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
				break;
			}
			else if (winner == 'R')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Match nul !\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
				break;
			}
			else if (winner == 'C')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Tours suivant :\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
			}

	//--------------------------------------------- Joueur 2 ---------------------------------------------

		// Pour parler au client 2
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "Numéro de la case que vous souhaitez prendre : (1 à 9) :");
			ecrits = send(socketDialogueCli2, messageEnvoye, strlen(messageEnvoye) + 1, 0);

			if (ecrits == 0) {
				printf("Le client s'est déconnecté.\n");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	} 
			else if (ecrits < 0) {
				perror("Erreur lors de la réception du message");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	}

		// Faire attendre le client 1
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "Votre adversaire joue ...\n");
			ecrits = send(socketDialogueCli1, messageEnvoye, strlen(messageEnvoye) + 1, 0);

			if (ecrits == 0) {
				printf("Le client s'est déconnecté.\n");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	} 
			else if (ecrits < 0) {
				perror("Erreur lors de la réception du message");
				break; // Sortir de la boucle et fermer le socket de dialogue
        	}

		// Envoyer un message au spectateur 
			memset(messageEnvoye, 0x00, LG_MESSAGE);
			snprintf(messageEnvoye, LG_MESSAGE, "A\n");
			for (int i = 0; i < nb_spectateurs; i++){
				send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
			}

		// Choix du client 2
			memset(messageRecu, 0x00, LG_MESSAGE);
			lus = recv(socketDialogueCli2, messageRecu, LG_MESSAGE, 0);

		// On verifie , on claim la case et on envoie au joueur
			if (lus > 0) {
				prendre_case(grille, messageRecu, 'O');
				affiche(grille);
				send(socketDialogueCli2, grille, sizeof(grille), 0);
				send(socketDialogueCli1, grille, sizeof(grille), 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], grille, sizeof(grille), 0);
				}
			}
			else if (lus == 0) {
				printf("Le client s'est déconnecté.\n");
				break;
        	} 
			else if (lus < 0) {
				perror("Erreur lors de la réception du message");
				break;
        	}

		// On verifie si un joueurs à gagné et on envoie l'avancé a tout le monde
			winner = win(grille);

			if (winner == 'X')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Les X gagnent !\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
				break;
			}
			else if (winner == 'O')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Les O gagnent !\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
				break;
			}
			else if (winner == 'R')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Match nul !\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
				break;
			}
			else if (winner == 'C')
			{
				memset(messageEnvoye, 0x00, LG_MESSAGE);
				snprintf(messageEnvoye, LG_MESSAGE, "Tours suivant :\n");
				send(socketDialogueCli1, messageEnvoye, LG_MESSAGE, 0);
				send(socketDialogueCli2, messageEnvoye, LG_MESSAGE, 0);
				for (int i = 0; i < nb_spectateurs; i++){
					send(spectateurs[i], messageEnvoye, LG_MESSAGE, 0);
				}
			}
		}
	}
   	close(socketDialogueCli1);
    close(socketDialogueCli2);
    close(socketEcoute);
	return 0; 
}
