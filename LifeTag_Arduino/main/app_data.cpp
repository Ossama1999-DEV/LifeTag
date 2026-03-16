#include "app_data.h"
#include "config.h"

WebServer server(80);
DNSServer dnsServer;
Preferences preferences;

const byte DNS_PORT = 53;

String nom = "";
String prenom = "";
String dateNaissance = "";
String sexe = "";
String taille = "";
String poids = "";
String groupeSanguin = "";
String allergies = "";
String maladies = "";
String traitement = "";
String contactUrgence = "";
String personneAPrevenir = "";
String lienParente = "";
String traitementsVitaux = "";

String antecedentsMedicaux = "";
String chirurgie = "";
String medecinTraitant = "";
String numeroSecu = "";
String contreIndications = "";
String pathologiesChroniques = "";
String numeroDossierMedical = "";

const char* adminUser = "admin";
const char* adminPass = "1234";

const char* normalUser = "user";
const char* normalPass = "1234";

const char* ssid = "LifeTag";
const char* password = "123456789";