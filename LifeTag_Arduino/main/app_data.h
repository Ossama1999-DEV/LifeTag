#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

extern bool nfcWritePending;
extern unsigned long nfcWriteRequestTime;

extern WebServer server;
extern DNSServer dnsServer;
extern Preferences preferences;

extern const byte DNS_PORT;

extern String nom;
extern String prenom;
extern String dateNaissance;
extern String sexe;
extern String taille;
extern String poids;
extern String groupeSanguin;
extern String allergies;
extern String maladies;
extern String traitement;
extern String contactUrgence;
extern String personneAPrevenir;
extern String lienParente;
extern String traitementsVitaux;
extern String antecedentsMedicaux;
extern String chirurgie;
extern String medecinTraitant;
extern String numeroSecu;
extern String contreIndications;
extern String pathologiesChroniques;
extern String adresse;
extern String dispositifMedical;