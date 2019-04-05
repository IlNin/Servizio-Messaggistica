#include "ClientUtil.c"

int main() {	
 inizializzaSockAdrrServer();
 
 // GESTIONE SEGNALI
 if (inizializzaSetSegnali() == 0) {
  printf("- errore durante la creazione del set segnali -\n");
  exit(0); }
 if (associaFunzioni() == 0) {
  printf("- errore durante l'associazione dei segnali con le funzioni -\n");
  exit(0); }
  
 printf("Benvenuto al servizio online di messaggistica!\n");
 
 // AUTENTICAZIONE UTENTE
 printf("Per continuare effettuare l'accesso:\n");
 while(1) {
  azzeraPack();
  printf("- premere 1 per registrarsi\n- premere 2 per il login\n- premere 0 per uscire\n");
  if (fgets(input, MAX, stdin) == NULL) {
   printf("\n- input non riconosciuto -\n"); }
   
  else if ((strlen(input)+1) >= MAX) { // (stlen(input+1)) perchè strlen non conta "/0"
   printf("\n- input troppo grande -\n");
   while (getchar() != '\n'); } // scorro i caratteri in eccesso in input fino al "punto di invio"
   
  else if (strcmp(input, "0\n") == 0) { // l'utente esce
   printf("\nGrazie per aver utilizzato i nostri servizi. Arrivederci!\n");
   exit(0); }
   
  else if (strcmp(input, "1\n") == 0) { // l'utente si registra
   printf("\n- inizio registrazione -\n");
   if (registrazione() == 0) {
	printf("\n- registrazione fallita -\n");
	continue; }
   printf("\n- registrazione effettuata con successo -\n"); }
   
  else if (strcmp(input, "2\n") == 0) { // l'utente effettua il login
   printf("\n- inizio login -\n");
   if (login() == 0) { // login fallito
    printf("\n- login fallito -\n");
	continue; }
   
   // GESTIONE POSTA
   printf("\n- login effettuato con successo - \n");
   printf("Benvenuto alla tua casella di posta, %s:\n", nomeUtente);
   while(1) {
	printf("- premere 1 per visualizzare i tuoi messaggi\n- premere 2 per scrivere un nuovo messaggio\n- premere 0 per effettuare il logout\n");
	if (fgets(input, MAX, stdin) == NULL) {
	 printf("\n- input non riconosciuto - \n"); }
	 
	else if ((strlen(input)+1 >= MAX)) {
	 printf("\n- input troppo grande - \n");
	 while (getchar() != '\n'); }
	 
	else if (strcmp(input, "0\n") == 0) {
	  printf("\n- logout eseguito con successo - \n");
	  break; }
	  
	else if (strcmp(input, "1\n") == 0) {
	  printf("\n- lettura messaggi -\n");
	  visualizzaMessaggi(); }
	  
	else if (strcmp(input, "2\n") == 0) {
	  printf("\n- scrittura messaggio -\n");
	  if (scriviMessaggio() == 0) {
	   printf("\n- messaggio non inviato -\n");
	   continue; }
	  printf("\n- messaggio inviato correttamente -\n"); }
	  
	else {
	 printf("\n- input non valido -\n"); } } }

  else {
   printf("\n- input non valido -\n"); } } }
