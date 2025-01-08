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
	printf("~~~~~~~~~~~~~~~~~~~~\n");
	printf(" %c | %c | %c\n" , grille[0], grille[1], grille[2]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[3], grille[4], grille[5]);
	printf("------------\n");
	printf(" %c | %c | %c\n" , grille[6], grille[7], grille[8]);
}

void creation_socket_client(int argc, char *argv[], char *ip_dest, int *port_dest, int *descripteurSocket) {
	if (argc>1) { // si il y a au moins 2 arguments passés en ligne de commande, récupération ip et port
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",port_dest);
	}else{
		printf("USAGE : %s ip port\n",argv[0]);
		exit(-1);
	}

	// Crée un socket de communication
	*descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(*descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", *descripteurSocket);
}

void attente_client(char *ip_dest, int port_dest, int descripteurSocket, struct sockaddr_in *sockaddrDistant, socklen_t *longueurAdresse) {
	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	*longueurAdresse = sizeof(struct sockaddr_in);
	// Initialise à 0 la structure sockaddr_in
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &sockaddrDistant
	memset(sockaddrDistant, 0, *longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrDistant->sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrDistant->sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant->sin_addr);

	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)sockaddrDistant, *longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);
}

int main(int argc, char *argv[]){

	char buffer[LG_MESSAGE]; // buffer stockant le message
	int nb; /* nb d’octets écrits et lus */	
	char ip_dest[16];
	int  port_dest;
	int descripteurSocket;
	socklen_t longueurAdresse;
	struct sockaddr_in sockaddrDistant;

	int grille[9] ;

	// Pour pouvoir contacter le serveur, le client doit connaître son adresse IP et le port de comunication
	// Ces 2 informations sont passées sur la ligne de commande
	// Si le serveur et le client tournent sur la même machine alors l'IP locale fonctionne : 127.0.0.1
	// Le port d'écoute du serveur est 5000 dans cet exemple, donc en local utiliser la commande :
	// ./client_base_tcp 127.0.0.1 5000
	

	// Gestion de la connexion du client > serveur

	creation_socket_client(argc, argv, ip_dest, &port_dest, &descripteurSocket);
	// longueurAdresse = sizeof(struct sockaddr_in); 
	attente_client(ip_dest, port_dest, descripteurSocket, &sockaddrDistant, &longueurAdresse);


	// Gestion intéractions

	while(1)
	{
		printf("\nChoisissez l'une des options suivantes : \n");
		printf("° Tapez 'quit', si vous souhaitez arrêter l'applications ! \n");
		printf("° Tapez 'play' pour jouer ! \n");
		printf("\n> ");
		fgets(buffer, LG_MESSAGE, stdin);
		buffer[strcspn(buffer, "\n")] = '\0'; 

		// Envoi du message
		//switch(nb = write(descripteurSocket, buffer, strlen(buffer))){
		switch(nb = send(descripteurSocket, buffer, strlen(buffer)+1,0)){
			case -1 : /* une erreur ! */
					perror("Erreur en écriture...");
					close(descripteurSocket);
					exit(-3);
			case 0 : /* le socket est fermée */
				fprintf(stderr, "Le socket a été fermée par le serveur !\n\n");
				return 0;
			default: /* envoi de n octets */
				printf("Message %s envoyé! (%d octets)\n\n", buffer, nb);
		}
		nb = recv(descripteurSocket, buffer, LG_MESSAGE, 0);
		if(nb < 0){
			perror("Erreur de réception...");
			close(descripteurSocket);
			exit(-4);
		}
		printf("Réponse du serveur : %s\n", buffer);

		if(strcmp(buffer, "Fermeture") == 0) {
			// On ferme la ressource avant de quitter
			close(descripteurSocket);
			return 0;
		} 

		if(strcmp(buffer, "Start") == 0) {
			while(1){
				printf("A votre tour :\n");

				// Pour recevoir et enregistrer le choix
				memset(buffer, 0x00, LG_MESSAGE);
				recv(descripteurSocket, buffer, LG_MESSAGE, 0);
				printf("%s",buffer);

				int choix;
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

				// Pour recevoir et afficher
				memset(buffer, 0x00, LG_MESSAGE);
				
				recv(descripteurSocket, buffer, 9, 0);
				affiche(buffer);
				

				printf("L'ordinateur va jouer :\n");

				// Pour recevoir et afficher
				memset(buffer, 0x00, LG_MESSAGE);
				recv(descripteurSocket, buffer, 9, 0);
				affiche(buffer);
			}
			return 0;
		} 
	}
	return 0 ;
}

