#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>


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
int ds_sock_connect;
struct sockaddr_in server; // indirizzo e porta del server a cui collegarsi
struct hostent *hp;

char input[MAX]; // input da tastiera, ottenuto con fgets
pacchetto pack; // usato per scambiare informazioni tra client e server
char nomeUtente[MAX]; // il nome dell'utente della sessione attuale
 
sigset_t set;  // set contiene i segnali che il client deve bloccare
struct sigaction act; // associa ad ogni segnale una determinata funzione, che sostuisce quella precedente o quella di default
int allarme = 0; // diventa 1 se si attiva l'alarme per inattività

// FUNZIONI
void inizializzaSockAdrrServer(); // prepara sockaddr_in server per la connessione
int connessione(); // si connette con il server
int disconnessione(); // si disconnette con il server
int inviaPack(); // invia pack al server
int riceviPack(); // attende un pack dal server
int comunicazioneServer(); // esegue tutte le funzioni precedenti

char* ridimensiona(char c[]); //ridimensiona char[] alla dimensione del suo input
void inserisciDatiPersonali(); // accetta come input i dati dell'utente
int registrazione(); // permette ad un nuovo utente di registrarsi
int login(); // permette ad un vecchio utente di fare il login
int scriviMessaggio(); // permette all'utente di scrivere un nuovo messaggio
void visualizzaMessaggi(); // permette all'utente di visualizzare i suoi messaggi e eventualmente di cancellarli

int inizializzaSetSegnali(); // aggiunge a set i segnali che deve bloccare
int associaFunzioni(); // associa ai segnali le funzioni che devono essere eseguite quando sono bloccati
void chiusuraForzata(int signo); // stampa su schermo che il client è stato chiuso a causa di una chiusura forzata
void fineTimer(); // stampa su schermo che la comunicazione con il server è terminata a causa di inattività

void azzeraPack(); // azzera le informazioni di un pack dopo averle lette
