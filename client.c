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
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 256

void affiche(char grille[LG_MESSAGE]){
	printf(" %c | %c | %c\n" , grille[0], grille[1], grille[2]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[3], grille[4], grille[5]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[6], grille[7], grille[8]);
	printf("~~~~~~~~~~~~~~~~~~~~\n");
}

int main(int argc, char *argv[]){

	int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char buffer[LG_MESSAGE];
	int nb , choix ; 

	char ip_dest[16];
	int  port_dest;

	// si il y a au moins 2 arguments passés en ligne de commande, récupération ip et port
	if (argc>1) {
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",&port_dest);
	}else{
		printf("USAGE : %s ip port\n",argv[0]);
		exit(-1);
	}

	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);

	longueurAdresse = sizeof(sockaddrDistant);
	
	memset(&sockaddrDistant, 0x00, longueurAdresse);

	sockaddrDistant.sin_family = AF_INET;
	sockaddrDistant.sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);

	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);

 	while(1){
		// Pour recevoir et enregistrer le choix
		memset(buffer, 0x00, LG_MESSAGE);
		nb = recv(descripteurSocket, buffer, LG_MESSAGE, 0);
    	if (nb <= 0) {
			if (nb == 0) {
				printf("Le serveur a fermé la connexion.\n");
			} 
			else {
				perror("Erreur de réception de la grille");
			}
			exit(0);
    	}
		printf("%s",buffer);

		if(strcmp(buffer,"Numéro de la case que vous souhaitez prendre : (1 à 9) :") == 0) {
			scanf("%d", &choix);

			// Pour transformer choix en texte
			sprintf(buffer, "%d", choix) ;

			//Envoie et check les erreurs
			switch(nb = send(descripteurSocket, buffer, strlen(buffer) + 1, 0)){
				case -1 :
						perror("Erreur en écriture...");
						close(descripteurSocket);
						exit(-3);
				case 0 :
					fprintf(stderr, "Le socket a été fermée par le serveur !\n\n");
					return 0;
			}
		}

		// Pour recevoir et afficher la grille
		memset(buffer, 0x00, LG_MESSAGE);
		nb = recv(descripteurSocket, buffer, 9, 0);
    	if (nb <= 0) {
			if (nb == 0) {
				printf("Le serveur a fermé la connexion.\n");
			} 
			else {
				perror("Erreur de réception de la grille");
			}
			exit(0);
    	}
		affiche(buffer);

		// Afficher le gagnant
		memset(buffer, 0x00, LG_MESSAGE);
		nb = recv(descripteurSocket, buffer, LG_MESSAGE,0);
    	if (nb <= 0) {
			if (nb == 0) {
				printf("Le serveur a fermé la connexion.\n");
			} 
			else {
				perror("Erreur de réception de la grille");
			}
			exit(0);
    	}
		printf("%s", buffer);	
	}

	// On ferme la ressource avant de quitter
	close(descripteurSocket);

	return 0;
}