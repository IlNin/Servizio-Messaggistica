#include "ClientUtil.h"

# define PORT 1999
# define HOST "localhost"

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) // assicura compatibilità per bzero

void inizializzaSockAdrrServer() {
 server.sin_family = AF_INET;
 server.sin_port = htons(PORT);
 hp = gethostbyname(HOST);
 memcpy(&server.sin_addr, hp->h_addr_list[0], hp->h_length); }

int connessione() {
 // CREAZIONE SOCKET
 ds_sock = socket(AF_INET, SOCK_STREAM, 0);
 if (ds_sock == -1) {
  printf("\n- errore nella creazione del socket -\n");
  return 0; }
 
 // CONNECT DI SOCKET
 ds_sock_connect = connect(ds_sock, (struct sockaddr*) &server, sizeof(server));
 if (ds_sock_connect == -1) {
  printf("\n- impossibile collegarsi al server -\n");
  return 0; }
 printf("\n- stabilita connessione con il server -\n");
 return 1; }

int disconnessione() {
 if (close(ds_sock) == -1) {
  if (allarme != 1) { // se è già scattato l'allarme, evita di stampare questo su schermo
   printf("- errore nella disconnessione con il server -\n"); }
  else { // ripristina il flag dell'allarme
   allarme = 0; }
  return 0; }
 printf("- disconnessione con il server -\n"); 
 return 1; }
 
int inviaPack() {
 if (write(ds_sock, &pack, sizeof(pack)) == -1) {
  printf("- errore durante la comunicazione con il server -\n");
  return 0; }
 printf("- pacchetto inviato con successo -\n");
 return 1; }
  
int riceviPack() { 
 alarm(5);
 sleep(3); // ferma momentaneamente l'applicazione per dar tempo all'utente di leggere 
 if (read(ds_sock, &pack, sizeof(pack)) == -1) {
  alarm(0);
  if (allarme != 1) { // se l'allarme è scattato, non posta questa stringa
   printf("- errore durante la comunicazione con il server -\n"); }
  return 0; }
 alarm(0);
 printf("- ricevuto pacchetto dal server -\n");
 return 1; }
 
int comunicazioneServer() {
 if (connessione() == 0) {
  return 0; }
 if (inviaPack() == 0) {
  disconnessione();
  return 0; }
 if (riceviPack() == 0) {
  disconnessione();
  return 0; }
 if (disconnessione() == 0) {
  return 0; }
 return 1; }

char* ridimensiona(char c[]) {
 int size = strlen(c);
 c[size-1] = '\0'; 
 return c; }
 
void inserisciDatiPersonali() {
 // RICHIESTA NOME
 while(1) {
  printf("- inserire il nome utente (MAX 14 CARATTERI, MIN 4 CARATTERI)\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
   
  else if ((strlen(input)+1) >= MAX) {
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); }
   
  else if (strcmp(input, "\n") == 0) { // caso in cui l'utente preme solamente invio
   printf("\n- input non riconosciuto -\n"); }
  
  else if (strlen(input) > 15) { // strlen conta anche "\n"
   printf("\n- input troppo grande -\n"); }
  
  else if (strlen(input) < 5) { // strlen conta anche "\n"
   printf("\n- input troppo piccolo -\n"); }
  
  else {
   break; } }
 strcpy(pack.Utente.nome, ridimensiona(input));
 
 
 // RICHIESTA PASSWORD
 while(1) {
  printf("- inserire la password (MAX 14 CARATTERI, MIN 4 CARATTERI)\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
   
  else if ((strlen(input)+1) >= MAX) {
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); }
   
  else if (strcmp(input, "\n") == 0) {
   printf("\n- input non riconosciuto -\n"); }
  
  else if (strlen(input) > 15) {
   printf("\n- input troppo grande -\n"); }
  
  else if (strlen(input) < 5) {
   printf("\n- input troppo piccolo -\n"); }
  
  else {
   break; } }
 strcpy(pack.Utente.password, ridimensiona(input)); }

int registrazione() { 
 pack.istruzione = 'R';
 inserisciDatiPersonali();
 
 // COMUNICAZIONE CON IL SERVER
 if (comunicazioneServer() == 0) {
  return 0; }
 
 // ANALISI RISULTATO
 if (pack.istruzione == '1') { // registrazione eseguita
  return 1; }
 else if (pack.istruzione == '0') { // utente gia' presente
  printf("- utente gia' presente -\n");
  return 0; }
 else if (pack.istruzione == '2') { // errori vari durante creazione e scrittura del file
  printf("- errore durante registrazione utente -\n"); 
  return 0; }
 printf("- errore sconosciuto -\n");
 return 0; }
 
int login() { 
 pack.istruzione = 'L';
 inserisciDatiPersonali();
 
 // COMUNICAZIONE CON IL SERVER
 if (comunicazioneServer() == 0) {
  return 0; }
 
 // ANALISI RISULTATO
 if (pack.istruzione == '1') { // login effettuato
  strcpy(nomeUtente, pack.Utente.nome); // memorizza il nome utente per il resto della sessione
  return 1; }
 else if (pack.istruzione == '0') {
  printf("- nome utente non registrato -\n");
  return 0; }
 else if (pack.istruzione == '2') {
  printf("- errore durante il login utente -\n");
  return 0; }
 else if (pack.istruzione == '3') {
  printf("- password utente errata -\n"); 
  return 0; }
 else {
  printf("- errore sconosciuto -\n");
  return 0; } }
	
int scriviMessaggio() {
 pack.istruzione = 'W';
 strcpy(pack.Messaggio.mittente, nomeUtente);
 
 // RICHIESTA DESTINATARIO
 while(1) {
  printf("- inserire il nome del destinatario (MAX 14 CARATTERI, MIN 4 CARATTERI)\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
   
  else if ((strlen(input)+1) >= MAX) {
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); }
   
  else if (strcmp(input, "\n") == 0) { // caso in cui l'utente preme solamente invio
   printf("\n- input non riconosciuto -\n"); }
  
  else if (strlen(input) > 15) { // strlen conta anche "\n"
   printf("\n- input troppo grande -\n"); }
  
  else if (strlen(input) < 5) { // strlen conta anche "\n"
   printf("\n- input troppo piccolo -\n"); }
  
  else {
   break; } }
 strcpy(pack.Messaggio.destinatario, ridimensiona(input));
 
 // RICHIESTA OGGETTO
 while(1) {
  printf("- inserire l'oggetto del messaggio (MAX 20 CARATTERI, MIN 1 CARATTERE)\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
   
  else if ((strlen(input)+1) >= MAX) {
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); }
   
  else if (strcmp(input, "\n") == 0) { // caso in cui l'utente preme solamente invio
   printf("\n- input non riconosciuto -\n"); }
  
  else if (strlen(input) > 21) { // strlen conta anche "\n"
   printf("\n- input troppo grande -\n"); }
  
  else {
   break; } }
 strcpy(pack.Messaggio.oggetto, ridimensiona(input));
 
 // RICHIESTA TESTO
 while(1) {
  printf("- inserire il testo del messaggio (MAX 80 CARATTERI, MIN 1 CARATTERE)\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
   
  else if ((strlen(input)+1) >= MAX) { // non si aggiunge niente a stlren perchè MAXTESTO tiene gia' conto di "\n" e "\0"
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); }
   
  else if (strcmp(input, "\n") == 0) { // caso in cui l'utente preme solamente invio
   printf("\n- input non riconosciuto -\n"); }
  
  else if (strlen(input) > 81) { // strlen conta anche "\n"
   printf("\n- input troppo grande -\n"); }
  
  else {
   break; } }
 strcpy(pack.Messaggio.testo, ridimensiona(input));
 
 // COMUNICAZIONE CON IL SERVER
 if (comunicazioneServer() == 0) {
  return 0; }
 
 // ANALISI RISULTATO
 if (pack.istruzione == '1') { // messaggio inviato con successo
  return 1; }
 else if (pack.istruzione == '0') {
  printf("- non e' stato possibile trovare il destinatario -\n");
  return 0; }
 else if (pack.istruzione == '2') {
  printf("- errore durante l'invio del messaggio -\n");
  return 0; }
 else {
  printf("- errore: assenza di spazio nella casella email del destinatario -\n"); 
  return 0; } }

void visualizzaMessaggi() {
 pack.istruzione = 'V';
 strcpy(pack.Messaggio.destinatario, nomeUtente);
 
 // COMUNICAZIONE CON IL SERVER
 if (comunicazioneServer() == 0) {
  return; }
  
 // STAMPA MESSAGGI
 if (pack.istruzione == '0') {
  printf("\n- non hai nessun messaggio -\n");
  return; }
 
 if (pack.istruzione == '2') {
  printf("\n- errore durante lettura messaggi -\n"); }
 
 printf("\n- lista dei tuoi messaggi -\n");
 int contatore = 0;
 for (int i = 1; i <= MAX; i++) { // ci sono dei messaggi da leggere
  if (strcmp(pack.listaMessaggi[i-1].oggetto, "\0") == 0) {
    printf("\n- fine lista messaggi -\n");
    contatore = i-1;
    break; }
  printf("\nMessaggio n.%d:\n", i);
  printf("OGGETTO: %s", pack.listaMessaggi[i-1].oggetto);
  printf("MITTENTE: %s\n", pack.listaMessaggi[i-1].mittente);
  printf("TESTO: %s", pack.listaMessaggi[i-1].testo); }
 
 
 // CANCELLAZIONE MESSAGGI
 while(1) {
  printf("\n- premere 1 per eliminare un messaggio\n- premere 0 per tornare indietro\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
  
  else if ((strlen(input)+1) >= MAX) {
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); }
  
  else if (strcmp(input, "0\n") == 0) {
   printf("\n");
   return; }
  
  else if (strcmp(input, "1\n") == 0) { // l'utente desidera cancellare un messaggio
   pack.istruzione = 'D';
   while(1) {
    printf("\n- inserisci il numero del messaggio che vuoi eliminare\n");
    if (fgets(input, MAX, stdin) == NULL) {
     printf("\n- input non riconosciuto -\n"); }
  
    else if ((strlen(input)+1) >= MAX) {
     printf("\n- input troppo grande -\n");
     while (getchar() != '\n'); }
    
    else { // input riconosciuto
     char* input2 = ridimensiona(input); // toglie "\n" dall'input
     int n = atoi(input2); // converte l'input in int
     if (n == 0) {
      printf("\n- input non riconosciuto -\n"); }
     else if (n < 1 || n > contatore) { // caso in cui il messaggio n non esiste
      printf("\n- non esiste alcun messaggio con tale numero -\n"); }
     else {
      pack.messaggioDaEliminare = n;
      if (comunicazioneServer() == 0) {
       return; }
	  
      if (pack.istruzione == '0') {
       printf("\n- errore durante rimozione messaggio -\n");
       return; }
 
      if (pack.istruzione == '1') {
       printf("\n- messaggio rimosso con successo -\n");
       bzero(pack.listaMessaggi, sizeof(pack.listaMessaggi));
       visualizzaMessaggi();
       return; } } } } }
  
  else {
   printf("\n- input non riconosciuto -\n"); } } }

int inizializzaSetSegnali() {
 if (sigemptyset(&set) == -1) {
  return 0; }
 if (sigaddset(&set, SIGHUP) == -1) { // SIGHUP: chiusura del terminale associato al processo
  return 0; }
 if (sigaddset(&set, SIGINT) == -1) { // SIGINT: chiusura del processo quando l'user preme CTRL+C
  return 0; }
 if (sigaddset(&set, SIGQUIT) == -1) { // SIGQUIT: come SIGINT, ma con un "core dump" generato dal sistema
  return 0; }
 if (sigaddset(&set, SIGILL) == -1) { // SIGILL: tentativo di eseguire un'istruzione proibita
  return 0; }
 if (sigaddset(&set, SIGSEGV) == -1) { // SIGEGV: tentativo di accedere ad un indirizzo di memoria proibito
  return 0; }
 if (sigaddset(&set, SIGTERM) == -1) { // SIGTERM: richiesta non forzata di terminazione
  return 0; }
 if (sigaddset(&set, SIGALRM) == -1) { // SIGALRM: inviato allo scadere del timer di alarm()
  return 0; }
 if (sigaddset(&set, SIGPIPE) == -1) { // SIGPIPE: il server si sconnette durante la conversazione
  return 0; }
  
 act.sa_mask = set;
 return 1; }

int associaFunzioni() { 
 act.sa_handler = chiusuraForzata;
 if (sigaction(SIGHUP, &act, NULL) == -1) {
  return 0; }
 if (sigaction(SIGINT, &act, NULL) == -1) {
  return 0; }
 if (sigaction(SIGQUIT, &act, NULL) == -1) {
  return 0; }
 if (sigaction(SIGILL, &act, NULL) == -1) {
  return 0; }
 if (sigaction(SIGSEGV, &act, NULL) == -1) {
  return 0; }
 if (sigaction(SIGTERM, &act, NULL) == -1) {
  return 0; }
 
 act.sa_handler = fineTimer;
 if (sigaction(SIGALRM, &act, NULL) == -1) {
  return 0; }
 
 act.sa_handler = SIG_IGN;
 if (sigaction(SIGPIPE, &act, NULL) == -1) {
  return 0; }
 
 return 1; }

void chiusuraForzata(int signo) {
 if (signo == SIGHUP) {
  printf("\n - CATTURATO SIGNALE SIGHUP -\n"); } 
 if (signo == SIGINT) {
  printf("\n - CATTURATO SIGNALE SIGINT -\n"); }
 if (signo == SIGQUIT) {
  printf("\n - CATTURATO SIGNALE SIGQUIT -\n"); }
 if (signo == SIGILL) {
  printf("\n - CATTURATO SIGNALE SIGILL -\n"); }
 if (signo == SIGSEGV) {
  printf("\n - CATTURATO SIGNALE SIGSEGV -\n"); }
 if (signo == SIGTERM) {
  printf("\n - CATTURATO SIGNALE SIGINT -\n"); }
 printf("\\\\\\\\ CHIUSURA FORZATA DEL CLIENT ////////\n");
 close(ds_sock);
 exit(0); }

void fineTimer() {
 allarme = 1;
 printf("- il server impiega troppo tempo a rispondere -\n");
 close(ds_sock);
 return; }
 
void azzeraPack() {
 bzero(&pack, sizeof(pack)); }
