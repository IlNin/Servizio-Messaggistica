#include "ServerUtil.c"

int main() {
 // GESTIONE SEGNALI
 if (inizializzaSetSegnali() == 0) {
  printf("- errore durante la creazione del set segnali -\n");
  exit(0); }
 if (associaFunzioni() == 0) {
  printf("- errore durante l'associazione dei segnali con le funzioni -\n");
  exit(0); }	
	
 // CREAZIONE SERVER
 printf("- creazione server in corso -\n");
 if (creaSocketServer() == 0) {
  printf("\n- creazione server fallita -\n");
  exit(0); }
 printf("\n- creazione server eseguita con successo -\n");
 
 // ACCETTAZIONE RICHIESTE SERVER
 while(1) {	 
  azzeraPack();
  if (accettaClient() == 0) {
   continue; }
  if (riceviPack() == 0) { // se non riceve un pack, continua a ciclare
   disconnessioneClient();
   continue; }
  
  /* PROVA STAMPA PACK (DEBUG)
  provaStampa(); */
  
  // ANALISI RICHIESTA CLIENT
  printf("- analizzo richiesta client -\n");
  if (pack.istruzione == 'R') { // il client desidera registrarsi
   printf("- il client desidera registrarsi -\n");
   registrazione();
   disconnessioneClient(); }
   
  else if (pack.istruzione == 'L') { // il cliente desidera loggarsi
   printf("- il client desidera loggarsi -\n");
   login();
   disconnessioneClient(); }
   
  else if (pack.istruzione == 'W') { // il cliente desidera inviare un messaggio
   printf("- il cliente desidera inviare un messaggio -\n");
   salvaMessaggio();
   disconnessioneClient(); }
  
  else if (pack.istruzione == 'V') { // il client desidera visualizzare i suoi messaggi
   printf("- il cliente desidera visualizzare i suoi messaggi -\n");
   visualizzaMessaggi();
   disconnessioneClient(); }
  
  else if (pack.istruzione == 'D') { // il client desidera eliminare un messaggio
   printf("- il cliente desidera eliminare un messaggio -\n");
   eliminaMessaggio();
   disconnessioneClient(); }
   
  else {
   printf("- richiesta del client non riconosciuta -\n");
   disconnessioneClient(); } }
  
 disconnessione(); }
