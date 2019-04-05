#include "ServerUtil.h"

#define PORT 1999
#define HOST "localhost"
#define LISTENERS 100

#define bzero(b,len) (memset((b), '\0', (len)), (void) 0) // assicura compatibilità per bzero

int creaSocketServer() {
 // CREAZIONE SOCKET	
 if ((ds_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  printf("\n- errore nella creazione del socket -\n");
  return 0; }
 
 // BINDING SOCKET
 server.sin_family = AF_INET;
 server.sin_port = htons(PORT);
 hp = gethostbyname(HOST);
 memcpy(&server.sin_addr, hp->h_addr_list[0], hp->h_length); 
 
 if (bind(ds_sock, (struct sockaddr*) &server, sizeof(server)) == -1) {
  disconnessione();
  printf("\n- errore nel binding del socket -\n"); 
  return 0; }
 
 // PREPARA IL SERVER A RICEVERE CHIAMATE
 if (listen(ds_sock, LISTENERS) == -1) {
  disconnessione();
  printf("\n- errore nel listening del socket -\n");
  return 0; }
 
 return 1; }

int disconnessione() {
 if (close(ds_sock) == -1) {
  printf("\n- errore nella disconnessione del socket -\n");
  return 0; }
 return 1; }

int disconnessioneClient() {
 if (close(ds_sock_accept) == -1) {
  if (allarme != 1) {
   printf("\n- errore nella disconnessione del client -\n"); }
  else {
   allarme = 0; }
  return 0; }
 printf("- disconnesso dal client -\n");
 return 1; }

int accettaClient() {
  ds_sock_accept = accept(ds_sock, NULL, NULL);
  if (ds_sock_accept == -1) {
   printf("\n- fallimento nell'accettare un nuovo client -\n");
   return 0; }
  printf("\n- trovato nuovo client -\n");
  return 1; }
 
int inviaPack() {
 if (write(ds_sock_accept, &pack, sizeof(pack)) == -1) {
  printf("- errore durante la comunicazione con il client -\n");
  return 0; }
 printf("- pacchetto inviato con successo -\n");
 return 1; }
  
int riceviPack() {
 alarm(5);
 sleep(3);
 if (read(ds_sock_accept, &pack, sizeof(pack)) == -1) {
  alarm(0);
  if (allarme != 1) { // se l'allarme è scattato, non posta questa stringa
   printf("- errore durante la comunicazione con il client -\n"); }
  return 0; }
 alarm(0);
 printf("- ricevuto pacchetto dal client -\n");
 return 1; }

void registrazione() {
 printf("- registrazione client in corso -\n");
 
 // CREAZIONE PERCORSO
 strcpy(path, "./Utenti/"); // path ora punta al percorso dove sarà salvato il nome
 strcat(path, pack.Utente.nome);
 printf("- creazione nuovo utente -\n");
 sigprocmask(SIG_BLOCK, &set, NULL); // blocca i segnali per tutta la durata della funzione per garantire l'integrità dei dati
 ds_file = open(path, O_CREAT|O_EXCL|O_WRONLY, 0666);
 
 // ANALISI DS_FILE
 if (errno == EEXIST) { // l'utente già esiste
  errno = 0; // ripristino errno
  printf("- utente gia' esistente -\n");
  pack.istruzione = '0'; }
  
 else if (ds_file == -1) { // errore nella creazione del file
  printf("- errore nella creazione del file -\n");
  pack.istruzione = '2'; }
  
 else { // tutto ok
  printf("- scrittura password -\n");
  int esito = write(ds_file, pack.Utente.password, strlen(pack.Utente.password));
  if (esito == -1) {
   printf("- errore nella scrittura password -\n");
   remove(path); // rimuove il file dato che non è stata scritta alcuna password
   pack.istruzione = '2'; }
  else {
   printf("- registrazione nuovo utente avvenuta con successo -\n");
   
   // CREAZIONE CARTELLA PER I MESSAGGI
   printf("- creazione cartella messaggi per il nuovo utente -\n");
   strcpy(path2, "./Messaggi/");
   strcat(path2, pack.Utente.nome);
   if (mkdir(path2, 0777) == -1) {
	printf("- creazione cartella fallita -\n");
	remove(path); // rimuove il file con password
	pack.istruzione = '2'; }
   else {
	printf("- cartella creata con successo -\n");
	pack.istruzione = '1'; } } }
 
 close(ds_file);  
 inviaPack();
 sigprocmask(SIG_UNBLOCK, &set, NULL);  }

void login() {
 printf("- login client in corso -\n");
 
 // APERTURA PERCORSO
 strcpy(path, "./Utenti/"); // path ora punta al percorso dove sarà aperto il file con nome e password
 strcat(path, pack.Utente.nome);
 printf("- ricerca utente in corso -\n");
 file = fopen(path, "r2");
 
 // ANALISI DS_FILE
 if (errno == ENOENT) { // l'utente non risulta registrato
  errno = 0; // ripristino errno
  printf("- utente non registrato -\n");
  pack.istruzione = '0';
  inviaPack();
  return; }
 
 else if (file == NULL) {
  printf("- errore nell'apertura del file -\n");
  pack.istruzione = '2';
  inviaPack();
  return; }
 
 else { // l'utente esiste e legge la password
  printf("- ricerca password in corso -\n");
  char password[MAX];
  
  // LETTURA PASSWORD
  fgets(password, MAX, file);
  printf("- confronto password in corso -\n");
  if (strcmp(password, pack.Utente.password) == 0) {
   printf("- le password corrispondono-\n");
   pack.istruzione = '1'; }
  else {
   // printf("%s, %d\n", password, strlen(password)); debug prova password));
   // printf("%s, %d\n", pack.Utente.password, strlen(pack.Utente.password)); 
   printf("- le password non corrispondono-\n");
   pack.istruzione = '3'; }
  fclose(file); }
 
 
 inviaPack(); }

void salvaMessaggio() {
 // CONTROLLO DESTINATARIO
 strcpy(path, "./Messaggi/");
 strcat(path, pack.Messaggio.destinatario);
 printf("- controllo esistenza destinatario -\n");
 directory = opendir(path);
 
 // ANALISI DIRECTORY
 if (errno == ENOENT) {
  errno = 0; // ripristino errno
  printf("- destinatario inesistente -\n");
  pack.istruzione = '0'; }
 
 else { // il destinatario esiste
  printf("- destinatario trovato -\n");
  
  // ANALISI NOME MESSAGGIO
  int numero = contatoreMessaggi();
  numero = numero+1; // titolo del nuovo messaggio
  if (numero == 101) { // un client può avere solo 100 messaggi
   printf("- il destinatario ha la casella piena -\n");
   pack.istruzione = '3';
   inviaPack();
   return; }
  
  // SCRITTURA MESSAGGIO
  char nomeMessaggio[MAX];
  sprintf(nomeMessaggio, "%d", numero);  
  strcat(path, "/");
  strcat(path, nomeMessaggio);
  
  sigprocmask(SIG_BLOCK, &set, NULL); // blocca i segnali per tutta la durata della funzione per garantire l'integrità dei dati
  ds_file = open(path, O_CREAT|O_EXCL|O_WRONLY, 0666);
  
  if (ds_file == -1) {
   printf("- errore durante smistamento messaggio -\n");
   pack.istruzione = '2'; }
  else { // file aperto correttamente
    printf("- scrittura oggetto -\n");
    char* separa = "OBJECT:\n";
    write(ds_file, separa, strlen(separa));
	write(ds_file, pack.Messaggio.oggetto, strlen(pack.Messaggio.oggetto));
	
	printf("- scrittura corpo -\n");
	separa = "\nTEXT:\n";
	write(ds_file, separa, strlen(separa));
	write(ds_file, pack.Messaggio.testo, strlen(pack.Messaggio.testo));
	
	printf("- scrittura mittente -\n");
	separa = "\nSENDER:\n";
	write(ds_file, separa, strlen(separa));
	write(ds_file, pack.Messaggio.mittente, strlen(pack.Messaggio.mittente));
	
	printf("- messaggiato smistato correttamente -\n");
	close(ds_file);
	pack.istruzione = '1'; } }
 
 inviaPack();
 sigprocmask(SIG_UNBLOCK, &set, NULL); }

int contatoreMessaggi() {
 strcpy(path, "./Messaggi/");
 strcat(path, pack.Messaggio.destinatario);
 directory = opendir(path); // apre la directory con i messaggi dell'utente
 int contatore = 0; // conta i messaggi

 while(1) {
  drnt = readdir(directory);
  if (drnt == NULL) { // raggiunta fine directory
   break; }
  contatore = contatore+1; }
 
 contatore = contatore-2; // toglie "." dal conto
 return contatore; }

void visualizzaMessaggi() {
 // CONTATORE MESSAGGI
 printf("- conta dei messaggi client in corso -\n");
 int contatore = contatoreMessaggi();
 
 if (contatore == 0) {
  printf("- nessun messaggio trovato -\n");
  pack.istruzione = '0';
  inviaPack();
  return; }
 
 printf("- sono stati trovati %d messaggi-\n", contatore);
 
 // LETTURA MESSAGGI
 char contatoreB[MAX]; 
 printf("- lettura messaggi client in corso -\n");
 
 for (int i = 1; i <= contatore; i++) { 
  // CREAZIONE PATH	 
  strcpy(path, "./Messaggi/");
  strcat(path, pack.Messaggio.destinatario);
  strcat(path, "/");
  sprintf(contatoreB, "%d", i);
  strcat(path, contatoreB);
  
  // APERTURA PATH
  file = fopen(path, "r");
  if (file == NULL) { 
   printf("- errore durante lettura messaggio n.%d -\n", i);
   pack.istruzione = '2'; 
   inviaPack();
   return; }
  
  // LEGGE OGGETTO
  char buffer[MAX];
  fgets(buffer, MAX, file); // legge "OBJECT:\n";
  fgets(pack.listaMessaggi[i-1].oggetto, MAX, file);
 
  // LEGGE TESTO
  fgets(buffer, MAX, file); // legge "TEXT:\n";
  fgets(pack.listaMessaggi[i-1].testo, MAX, file); 
  
  // LEGGE MITTENTE
  fgets(buffer, MAX, file); // legge "SENDER:\n" 
  fgets(pack.listaMessaggi[i-1].mittente, MAX, file);
  
  // CHIUSURA PATH
  fclose(file); }
 
 inviaPack(); }

void eliminaMessaggio() {
 printf("- eliminazione messaggio in corso -\n");
 
 // CANCELLAZIONE MESSAGGIO
 char numero[MAX];
 sprintf(numero, "%d", pack.messaggioDaEliminare);
 strcpy(path, "./Messaggi/");
 strcat(path, pack.Messaggio.destinatario);
 strcat(path, "/");
 strcat(path, numero);
 
 sigprocmask(SIG_BLOCK, &set, NULL); // blocca i segnali per tutta la durata della funzione per garantire l'integrità dei dati
 if (remove(path) == -1) {
  printf("- errore eliminazione messaggio -\n");
  pack.istruzione = '0';
  inviaPack();
  return; }
 printf("- messaggio eliminato con successo -\n");
 
 // RINOMINAZIONE FILE SUCCESSIVI A QUELLO ELIMINATO
 printf("- rinominazione dei messaggi in corso -\n");
 int contatore = contatoreMessaggi(); 
 for (int i = pack.messaggioDaEliminare+1; i <= contatore+1; i++) {
  sprintf(numero, "%d", i);
  strcpy(path, "./Messaggi/");
  strcat(path, pack.Messaggio.destinatario);
  strcat(path, "/");
  strcat(path, numero);
  
  char nuovaPath[MAX];
  sprintf(numero, "%d", i-1);
  strcpy(nuovaPath, "./Messaggi/");
  strcat(nuovaPath, pack.Messaggio.destinatario);
  strcat(nuovaPath, "/");
  strcat(nuovaPath, numero);
  
  rename(path, nuovaPath); }
 
 pack.istruzione = '1';
 inviaPack();
 sigprocmask(SIG_UNBLOCK, &set, NULL); }
 
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
  printf("\n - CATTURATO SIGNALE SIGTERM -\n"); }
 printf("\\\\\\\\ CHIUSURA FORZATA DEL SERVER ////////\n");
 close(ds_sock_accept);
 close(ds_sock);
 exit(0); }
 
void fineTimer() {
 allarme = 1;
 printf("- il client impiega troppo tempo ad inviare la sua richiesta -\n");
 close(ds_sock_accept); 
 return; }

void azzeraPack() {
 bzero(&pack, sizeof(pack)); }

void provaStampa() {
 printf("istruzione:\n");
 printf("%c\n", pack.istruzione);
 printf("Utente.nome:\n");
 printf("%s\n", pack.Utente.nome);
 printf("Utente.password:\n");
 printf("%s\n", pack.Utente.password);
 printf("Messaggio.mittente:\n");
 printf("%s\n", pack.Messaggio.mittente);
 printf("Messaggio.oggetto:\n");
 printf("%s\n", pack.Messaggio.oggetto);
 printf("Messaggio.testo:\n");
 printf("%s\n", pack.Messaggio.testo);
 printf("Messaggio.destinatario:\n");
 printf("%s\n", pack.Messaggio.destinatario);
 printf("messaggioDaEliminare:\n");
 printf("%d\n", pack.messaggioDaEliminare); }
