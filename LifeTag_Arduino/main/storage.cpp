#include "storage.h"
#include "app_data.h"

void loadAllData() {
    nom = preferences.getString("nom", "");
    prenom = preferences.getString("prenom", "");
    dateNaissance = preferences.getString("date", "");
    sexe = preferences.getString("sexe", "");
    taille = preferences.getString("taille", "");
    poids = preferences.getString("poids", "");
    groupeSanguin = preferences.getString("groupe", "");
    allergies = preferences.getString("allergies", "");
    maladies = preferences.getString("maladies", "");
    traitement = preferences.getString("traitement", "");
    contactUrgence = preferences.getString("contact", "");
    personneAPrevenir = preferences.getString("personne", "");
    lienParente = preferences.getString("lien", "");
    traitementsVitaux = preferences.getString("vitaux", "");

    antecedentsMedicaux = preferences.getString("antecedents", "");
    chirurgie = preferences.getString("chirurgie", "");
    medecinTraitant = preferences.getString("medecin", "");
    numeroSecu = preferences.getString("secu", "");
    contreIndications = preferences.getString("contre", "");
    pathologiesChroniques = preferences.getString("pathologies", "");
    numeroDossierMedical = preferences.getString("dossier", "");
}

void saveBaseData() {
    preferences.putString("nom", nom);
    preferences.putString("prenom", prenom);
    preferences.putString("date", dateNaissance);
    preferences.putString("sexe", sexe);
    preferences.putString("taille", taille);
    preferences.putString("poids", poids);
    preferences.putString("groupe", groupeSanguin);
    preferences.putString("allergies", allergies);
    preferences.putString("maladies", maladies);
    preferences.putString("traitement", traitement);
    preferences.putString("contact", contactUrgence);
    preferences.putString("personne", personneAPrevenir);
    preferences.putString("lien", lienParente);
    preferences.putString("vitaux", traitementsVitaux);
}

void saveAdminData() {
    preferences.putString("antecedents", antecedentsMedicaux);
    preferences.putString("chirurgie", chirurgie);
    preferences.putString("medecin", medecinTraitant);
    preferences.putString("secu", numeroSecu);
    preferences.putString("contre", contreIndications);
    preferences.putString("pathologies", pathologiesChroniques);
    preferences.putString("dossier", numeroDossierMedical);
}