#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <arpa/inet.h>

#define MAX 100

// CREAZIONE STRUCTS
typedef struct utente { // struct che memorizza le informazioni di un utente
 char nome[MAX];
 char password[MAX]; } utente;
 
typedef struct messaggio { // struct che memorizza le parti costituenti di un'email
 char mittente[MAX];
 char oggetto[MAX];
 char testo[MAX];
 char destinatario[MAX]; } messaggio;

typedef struct pacchetto { // struct che memorizza le informazioni trasferite tra client e server
 char istruzione; // indica al server cosa vuole fare il client
 utente Utente;
 messaggio Messaggio;
 messaggio listaMessaggi[MAX];
 int messaggioDaEliminare; } pacchetto; // contiene la lista di tutti i messaggi di un particolare utente

// DICHIARAZIONI VARIABILI
int ds_sock;
int ds_sock_accept;
struct sockaddr_in server; // contiene le informazioni del server necessarie per il binding
struct hostent *hp;

pacchetto pack;
char path[MAX]; // contiene il percorso per trovare un determinato utente o messaggio
char path2[MAX]; // un secondo percorso. Utile durante il processo di registrazione
int ds_file;
FILE* file; // utile per leggere i messaggi
DIR* directory;
struct dirent* drnt;

sigset_t set;  // set contiene i segnali che il client deve bloccare
struct sigaction act; // associa ad ogni segnale una determinata funzione, che sostuisce quella precedente o quella di default
int allarme = 0; // diventa 1 se si attiva l'alarme per inattività

// FUNZIONI
int creaSocketServer(); // crea un socket per il server ed effettua il binding
int disconnessione(); // disconnette il socket del server
int disconnessioneClient(); // disconnette il server dal client
int accettaClient(); // accetta la chiamata di un client
int inviaPack(); // invia pack al client
int riceviPack(); // attende un pack dal client

void registrazione(); // permetta la registrazione di un utente
void login(); // permette il login di un utente
int contatoreMessaggi(); // conta il numero di messaggi di un client
void salvaMessaggio(); // salva un messaggio di un client nella cartella del destinatario
void visualizzaMessaggi(); // invia al client tutti i suoi messaggi
void eliminaMessaggio(); // elimina il messaggio richiesto dal client

int inizializzaSetSegnali(); // aggiunge a set i segnali che deve bloccare
int associaFunzioni(); // associa ai segnali le funzioni che devono essere eseguite quando sono bloccati
void chiusuraForzata(int signo); // stampa su schermo che il server è stato chiuso a causa di una chiusura forzata
void fineTimer(); // stampa su schermo che la comunicazione con il client è terminata a causa di inattività

void azzeraPack(); // azzera le informazioni di un pack dopo averle lette
void provaStampa(); // stampa i contenuti di un packet (debug)
