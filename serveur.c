/*
Ce programme est la realisation d'un jeu Morpion entre : 
un serveur (Ordinateur qui genere des choix automatiquement)
et un client connecte via socket tpc
le client peut choisir la taille du tableau de jeu ainsi que 
la lettre avec laquelle il a jouer sauf la lettre O qui est 
reservee au serveur. 
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int creerSocket(const char * port);
int estGagnant(char lettre, int n ,char grille[n][n]);
void dessinerGrille(int n, char grille[n][n]);
void initialiserGrille(int n , char grille[n][n]);
int intRandom(int min, int max);

int main(){

	int sockServer; //socket d'ecoute
	int sockClient; //socket de communication
    struct sockaddr_in adresse;
	char buffer[256];

	/*Creation socket*/
	if((sockServer = creerSocket("12345")) < 0)
		perror("createSocket()");
	
    listen(sockServer, 5);
    fprintf(stdout, "le listener est active ... \n");

    int longueur = sizeof(struct sockaddr_in);
    /*Acceptation du client*/
 if((sockClient = accept(sockServer, (struct sockaddr *) & adresse, & longueur)) < 0) {
        perror("accept()");
        return -1;
    }
	printf("Le client est connecte\n");
	//n est la dimention de la grille
	int n;
	char clientLettre; //lettre client
	char serveurLettre = 'O';//lettre serveur par defaut O
	// Recevoir la taille de la grille
	if(read(sockClient, buffer, 256) < 0) {
		perror("read()");
        return -1;
   	}
   	//transformer en integer
	n = atoi(buffer);			
	//Recevoir la letter du client
	if(read(sockClient, buffer, 256) < 0) {
		perror("read()");
        return -1;
	}
	//transformer en majuscule
	clientLettre = toupper(buffer[0]);

	fprintf(stdout,"\nLettre du client est : %c \n", clientLettre);
	fprintf(stdout,"\nLettre du server est : %c \n", serveurLettre);
	
	//La grille sous forme d'une matrice du taille n*n
	char grille[n][n];
	//Initialiser la grille par des caractères '-'
	initialiserGrille(n,grille);
	//Afficher la grille initiale
	dessinerGrille(n,grille);

	//convertir grille en chaine de charactere
	char grilleString[n*n];
	int i = 0, j = 0, k = 0;
	for( i = 0 ; i < n ; i++){
		for( j = 0 ; j < n ; j++ ){
			grilleString[k] = grille[i][j];
			k++;
		}
	}
	//Envoyer la grille
	if(send(sockClient, grilleString,sizeof(grilleString), 0) < 0){
		perror("send(grille)");
		return -1;
	}
	//jeu 
		 while(1){
		 	char x1[2],y1[2];//coordonnees client 
        	int x2,y2; //coordonnees serveur 
 if( recv(sockClient,x1,sizeof(x1), 0) < 0 || recv(sockClient,y1,sizeof(y1), 0) < 0 ){
        		perror("recv(x,y)");
        		return -1;
        	}
 
  			grille[atoi(x1)][atoi(y1)] = clientLettre;

  			//les coordonees aleatoires
  			do{
	        	x2 = intRandom(0,n);
	        	y2 = intRandom(0,n);
	        }while( grille[x2][y2] != '-'  ); 
  			grille[x2][x2] = serveurLettre;

  			
  			dessinerGrille(n,grille);

  			if(estGagnant(clientLettre,n,grille) == 1){
				fprintf(stdout, "Le client a gagne !\n" );
			    grilleString[0] = '!'; //mettre ! si le client a gagne
				if(send(sockClient, grilleString,sizeof(grilleString), 0) < 0){
					perror("send(grille)");
					return -1;
				}
				close(sockClient);
				break;
			}else if(estGagnant(serveurLettre,n,grille) == 1){
				fprintf(stdout, "Le serveur a gagne !\n" );
				grilleString[0] = '?'; // mettre ? si le client a gagner
				if(send(sockClient, grilleString,sizeof(grilleString), 0) < 0){
					perror("send(grille)");
					return -1;
				}
				close(sockClient);
				break;
			}

			
			k = 0;
  			for( i = 0 ; i < n ; i++){
				for( j = 0 ; j < n ; j++ ){
					grilleString[k] = grille[i][j];//convertir en chaine de carac
					k++;
				}
			}
			
			if(send(sockClient, grilleString,sizeof(grilleString), 0) < 0){
				perror("send(grille)");
				return -1;
			}
        
     	} 
   
	return EXIT_SUCCESS;

}

int intRandom(int min, int max)
{
	return (min + (rand () % (max-min+1)));
}
void initialiserGrille(int n , char grille[n][n]){
		int i = 0, j = 0;
		for( i = 0 ; i < n ; i++){
			for( j = 0 ; j < n ; j++ ){
				grille[i][j] = '-';//initialiser par -
			}
		}

}
void dessinerGrille(int n, char grille[n][n]){
fprintf(stdout, "\n*********************************Grille***********************\n");
		int i = 0, j = 0;
		for( i = 0 ; i < n ; i++){
			for( j = 0 ; j <= n ; j++ ){
				if( j == n ) 
					fprintf(stdout, "|");
				else
					fprintf(stdout,"|%2c", grille[i][j]);
			}
			fprintf(stdout, "\n");
		}
fprintf(stdout, "\n**************************************************************\n");
}



int estGagnant(char lettre, int n ,char grille[n][n]){
	int colon = 0,ligne = 0,diagonale = 0;
	int i = 0, j = 0;
		for( i = 0 ; i < n ; i++){
			for( j = 0 ; j <= n ; j++ ){
				if( grille[i][j] == lettre) ligne++;
				if( grille[j][i] == lettre) colon++;
				if( i == j && grille[i][j] == lettre) diagonale++;
			}
			if( ligne == n || colon == n || diagonale == n)  
				return 1;
			else{
				ligne= 0; colon = 0;
			}
		}
	return -1;
}




//Creation d'une socket
int creerSocket(const char * port) {
    int sock;
    struct sockaddr_in adresse;
	// Creating the Socket Stream
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
	
    memset(&adresse, 0, sizeof (struct sockaddr_in));
	
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(atoi(port));
    adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	/* Bind */
   if(bind(sock, (struct sockaddr *) & adresse, sizeof(struct sockaddr_in)) < 0) {
        close(sock);
        perror("bind()");
        return -1;
    }
    return sock;
}

