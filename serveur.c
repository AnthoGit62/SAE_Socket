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

int socketEcoute;
struct sockaddr_in pointDeRencontreLocal;
socklen_t longueurAdresse;

void lire_heure(char *heure)
{
	FILE *fpipe;
	fpipe = popen("date '+%X'", "r");
	if (fpipe == NULL)
	{
		perror("popen");
		exit(-1);
	}
	fgets(heure, LG_MESSAGE, fpipe);
	pclose(fpipe);
}

void lire_date(char *date)
{
	FILE *fpipe;
	fpipe = popen("date '+%A %d %B %Y'", "r");
	if (fpipe == NULL)
	{
		perror("popen");
		exit(-1);
	}
	fgets(date, LG_MESSAGE, fpipe);
	pclose(fpipe);
}

void creation_socket()
{
	// Crée un socket de communication
	socketEcoute = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if (socketEcoute < 0)
	{
		perror("socket");
		exit(-1); // code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale
															   // setsockopt()
}

void connexion()
{

	// Remplissage de sockaddrDistant
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT);			   // = 5000 ou plus

	// On demande l’attachement local de la socket
	if ((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if (listen(socketEcoute, 6) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
}

int main(int argc, char *argv[])
{

	int socketDialogue;
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus;			  /* nb d’octets ecrits et lus */
	int retour;

	// Socket Part

	creation_socket();

	connexion();

	// Game

	while (1)
	{
		memset(messageRecu, 0, LG_MESSAGE * sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");

		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0)
		{
			perror("accept");
			close(socketDialogue);
			close(socketEcoute);
			exit(-4);
		}
		while (1)
		{
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
			if (strncmp(messageRecu, "heure", 4) == 0)
			{
				char heure[LG_MESSAGE];
				lire_heure(heure);
				send(socketDialogue, heure, strlen(heure) + 1, 0);
				printf("Heure envoyée au client : %s\n", heure);
			}
			else if (strncmp(messageRecu, "date", 5) == 0)
			{
				char date[LG_MESSAGE];
				lire_date(date);
				send(socketDialogue, date, strlen(date) + 1, 0);
				printf("Date envoyée au client : %s\n", date);
			}
			else if (strncmp(messageRecu, "quit", 4) == 0)
			{
				send(socketDialogue, "shutdown", strlen("shutdown") + 1, 0);
				break;
			}
			else
			{
				char erreur[] = "Commande non reconnue. Veuillez envoyer 'heure' ou 'date'.";
				send(socketDialogue, erreur, strlen(erreur) + 1, 0);
				printf("Commande inconnue envoyée au client.\n");
			}
		}

		close(socketDialogue);
	}

	// On ferme la ressource avant de quitter
	close(socketEcoute);
	return 0;
}
