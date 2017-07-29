#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


int connecterSocket(const char * servername, const char * port);
void dessinerGrille(int n, char grille[n][n]);

int main(int argc, char * argv[]) {

	if( argc != 3){
		fprintf(stdout, "\nSyntax : ./client taille lettre\n");
		return -1;
	}
	//n est la dimention de la grille initialiser par la valeur du premier argument
	int sock, n = atoi(argv[1]);
	//Dimention de la grille dans un tableau de caracteres pour l'envoyer au serveur
	char longueurGrille[2];
	sprintf(longueurGrille,"%d",n);

	fprintf(stdout, "%c\n",longueurGrille[0]);
	//La lettre du client
	char clientLettre[] = {argv[2][0]};
	fprintf(stdout, "%c\n",clientLettre[0]);

	//Socket
    struct sockaddr_in adresse;
    //Grille sous forme d'une chaine de caractere
	char grilleString[n*n];
	//La grille sous forme d'une matrice
	char grille[n][n];

	// Creating Client Socket Stream
	if((sock = connecterSocket("localhost", "12345")) < 0)
		perror("createSocket()");

	//Envoyer la taille de la grille et la lettre du client
	if( send(sock,longueurGrille,sizeof(longueurGrille), 0) < 0 || 
		send(sock,clientLettre,sizeof(clientLettre), 0) < 0 ){
		perror("send(n,clietLettre)");
		return -1;
	}
	
	//Recevoir la grille initiale
	if(recv(sock, grilleString, sizeof(grilleString) ,0) < 0) {
		perror("read()");
        return -1;
	}

	//On convertit la grille à une grille sous forme d'une matrice[][] 
	//afin de l'afficher
	int i = 0, j = 0, k = 0;
		for( i = 0 ; i < n ; i++){
			for( j = 0 ; j < n ; j++ ){
				grille[i][j] = grilleString[k];
				k++;
			}
	}
	//Afficher la grille
	dessinerGrille(n,grille);
	
	//Comencer le jeu
	 while(1){

	 		//x et y sont les condronnées entrer par client
        	char x[2],y[2];
        	//x et y doivent respecter quelque conditions telque 0<x<n ...
        	do{
	        	fprintf(stdout, "\nEntrer x : ");
	        	fscanf(stdin, "%s",x); 
	        	fprintf(stdout, "\nEntrer y : ");
	        	fscanf(stdin, "%s",y); 
	        }while( grille[atoi(x)][atoi(y)] != '-' || atoi(x) < 0 || atoi(x) > n 
	        	|| atoi(y) < 0 || atoi(y) > n ); 

	        //Envoyer les conrdonnées au serveur
	        if(send(sock,x,sizeof(x), 0) < 0 || send(sock,y,sizeof(y), 0) < 0){
	        	perror("Send(x,y)");
	        	return -1;
	        }  
	    	//Recevoir la nouvelle grille
            if(recv(sock, grilleString, sizeof(grilleString) ,0) < 0) {
				perror("recv()");
		        return -1;
			}
	//Tester si la partie est terminié
			if(grilleString[0] == '!'){
				close(sock);
				fprintf(stdout, "Vous avez gagné est gagné \n" );
				break;	
			}else if( grilleString[0] == '?'){
				close(sock);
				fprintf(stdout, "Le serveur est gagné \n" );
				break;
			}
	//Convertir la grille à une grille matricielle
			k = 0;
			for( i = 0 ; i < n ; i++){
				for( j = 0 ; j < n ; j++ ){
					grille[i][j] = grilleString[k];
					k++;
				}
			}  
			//Afficher la grille
			dessinerGrille(n,grille);
     } 

	close(sock);
	return EXIT_SUCCESS;
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
int connecterSocket(const char * servername, const char * port) {
    int sock;
    struct sockaddr_in adresse;
    struct hostent *server;
	//socket stream
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    // Getting The Server's DNS Entry
    if ((server = gethostbyname(servername)) < 0) {
        perror("socket()");
        return -1;
    }
	// Cleaning the SOCKADDR_IN Struct
    memset(&adresse, 0, sizeof (struct sockaddr_in));
	// Filling SOCKADDR_IN Fields
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(atoi(port));
	bcopy((char *)server->h_addr, (char *) &adresse.sin_addr.s_addr, server->h_length);
	// Connecting the Socket Stream to SOCKADDR_IN struct
	if(connect(sock, (struct sockaddr *) & adresse, sizeof(struct sockaddr_in)) < 0) {
        close(sock);
        perror("connect()");
        return -1;
    }
	// Returning the Socket Stream
    return sock;
}
