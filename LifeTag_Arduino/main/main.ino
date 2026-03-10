/**
 * @file main.ino
 * @brief ESP32 - Bracelet médical avec rôles user/admin, lecture séparée,
 * préremplissage du formulaire et fiche d'urgence
 */

// http://192.168.4.1

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Adafruit_GFX.h>
#include <Preferences.h>
#include <ESPmDNS.h>

// Identifiants
const char* adminUser = "admin";
const char* adminPass = "1234";

const char* normalUser = "user";
const char* normalPass = "1234";

// WiFi AP
const char* ssid = "LifeTag";
const char* password = "123456789";

WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

Preferences preferences;

// Données de base
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

// Données médecin
String antecedentsMedicaux = "";
String chirurgie = "";
String medecinTraitant = "";
String numeroSecu = "";
String contreIndications = "";
String pathologiesChroniques = "";
String numeroDossierMedical = "";

// ---------- PAGE LOGIN ----------
const char login_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Connexion</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            color: #333;
        }
        .container {
            margin-top: 100px;
            max-width: 320px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 { color: #007BFF; }
        input {
            width: 100%;
            padding: 10px;
            margin: 10px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-sizing: border-box;
        }
        .button {
            width: 100%;
            padding: 10px;
            border: none;
            cursor: pointer;
            border-radius: 5px;
            transition: 0.3s;
            background-color: #007BFF;
            color: white;
            font-size: 16px;
        }
        .button:hover { background-color: #0056b3; }
        .error {
            color: red;
            font-weight: bold;
            display: none;
        }
        .hint {
            margin-top: 15px;
            color: #666;
            font-size: 13px;
        }
        .access-type{
            margin-top:20px;
            padding-top:15px;
            border-top:1px solid #ddd;
            font-size:14px;
            color:#666;
        }
        footer{
            margin-top:40px;
            font-size:12px;
            color:#aaa;
        }
    </style>

    <script>
        function login() {
            var username = document.getElementById("username").value;
            var password = document.getElementById("password").value;

            fetch("/login?user=" + encodeURIComponent(username) + "&pass=" + encodeURIComponent(password))
            .then(response => response.text())
            .then(data => {
                if (data === "ADMIN") {
                    localStorage.setItem("esp32_auth", "true");
                    localStorage.setItem("esp32_role", "admin");
                    window.location.href = "/menu";
                } else if (data === "USER") {
                    localStorage.setItem("esp32_auth", "true");
                    localStorage.setItem("esp32_role", "user");
                    window.location.href = "/menu";
                } else {
                    document.getElementById("error-message").style.display = "block";
                }
            });
        }

        if (localStorage.getItem("esp32_auth") === "true") {
            window.location.href = "/menu";
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>SAFYRA SYSTEMS</h1>
        <input type="text" id="username" placeholder="Nom d'utilisateur">
        <input type="password" id="password" placeholder="Mot de passe">
        <p class="error" id="error-message">Login ou mot de passe incorrect</p>
        <button class="button" onclick="login()">Se connecter</button>
    </div>
    <div class="access-type">
        Accès: 👤 Utilisateur | 👨‍⚕️ Personnel médical
    </div>
</body>
<footer>
    © 2026 Safyra Systems
</footer>
</html>
)rawliteral";

// ---------- PAGE MENU ----------
const char menu_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Menu</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            color: #333;
        }
        .container {
            margin-top: 80px;
            max-width: 360px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 25px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        h1 { color: #007BFF; }
        .role {
            margin-top: 10px;
            font-weight: bold;
            color: #555;
        }
        .button {
            width: 100%;
            padding: 12px;
            border: none;
            cursor: pointer;
            border-radius: 5px;
            transition: 0.3s;
            color: white;
            font-size: 16px;
            margin-top: 12px;
        }
        .write { background-color: #28a745; }
        .write:hover { background-color: #218838; }
        .read { background-color: #007BFF; }
        .read:hover { background-color: #0056b3; }
        .logout { background-color: #dc3545; }
        .logout:hover { background-color: #b02a37; }
    </style>

    <script>
        function goWrite() {
            window.location.href = "/control";
        }

        function goRead() {
            let role = localStorage.getItem("esp32_role");
            if (role === "admin") {
                window.location.href = "/read_medecin";
            } else {
                window.location.href = "/read_patient";
            }
        }

        function logout() {
            localStorage.removeItem("esp32_auth");
            localStorage.removeItem("esp32_role");
            window.location.href = "/";
        }

        window.onload = function() {
            if (localStorage.getItem("esp32_auth") !== "true") {
                window.location.href = "/";
                return;
            }

            let role = localStorage.getItem("esp32_role");
            let roleText = document.getElementById("roleText");
            let writeBtn = document.getElementById("writeBtn");

            if (role === "admin") {
                roleText.innerText = "Connecté en tant que : Médecin";
                writeBtn.innerText = "Écrire les données médecin";
            } else {
                roleText.innerText = "Connecté en tant que : Utilisateur";
                writeBtn.innerText = "Écrire les données de base";
            }
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Bracelet médical</h1>
        <p class="role" id="roleText"></p>
        <button class="button write" id="writeBtn" onclick="goWrite()">Écrire les données</button>
        <button class="button read" onclick="goRead()">Lire les données</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>
    </div>
</body>
</html>
)rawliteral";

// ---------- FONCTIONS UTILES ----------
String htmlEscape(String s) {
    s.replace("&", "&amp;");
    s.replace("<", "&lt;");
    s.replace(">", "&gt;");
    s.replace("\"", "&quot;");
    s.replace("'", "&#39;");
    return s;
}

String inputText(const String& id, const String& label, const String& value, const String& placeholder = "") {
    return "<label for='" + id + "'>" + label + "</label>"
           "<input type='text' id='" + id + "' value='" + htmlEscape(value) + "' placeholder='" + htmlEscape(placeholder) + "'>";
}

String inputDate(const String& id, const String& label, const String& value) {
    return "<label for='" + id + "'>" + label + "</label>"
           "<input type='date' id='" + id + "' value='" + htmlEscape(value) + "'>";
}

String textArea(const String& id, const String& label, const String& value, const String& placeholder = "") {
    return "<label for='" + id + "'>" + label + "</label>"
           "<textarea id='" + id + "' placeholder='" + htmlEscape(placeholder) + "'>" + htmlEscape(value) + "</textarea>";
}

String optionSelected(const String& current, const String& value) {
    return current == value ? " selected" : "";
}

// ---------- PAGE PRINCIPALE ----------
void handleRoot() {
    server.send(200, "text/html", login_page);
}

// ---------- LOGIN ----------
void handleLogin() {
    if (server.hasArg("user") && server.hasArg("pass")) {
        String user = server.arg("user");
        String pass = server.arg("pass");

        if (user == adminUser && pass == adminPass) {
            server.send(200, "text/plain", "ADMIN");
        } else if (user == normalUser && pass == normalPass) {
            server.send(200, "text/plain", "USER");
        } else {
            server.send(401, "text/plain", "ERROR");
        }
    } else {
        server.send(400, "text/plain", "Missing parameters");
    }
}

// ---------- MENU ----------
void handleMenu() {
    server.send(200, "text/html", menu_page);
}

// ---------- PAGE ECRITURE PRE-REMPLIE ----------
void handleControl() {
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

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Écrire les données</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            color: #333;
        }
        .container {
            margin-top: 30px;
            max-width: 460px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 25px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: left;
        }
        h1 {
            color: #007BFF;
            text-align: center;
        }
        label {
            font-weight: bold;
        }
        input, textarea, select {
            width: 100%;
            padding: 10px;
            margin: 10px 0 15px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-sizing: border-box;
            font-family: Arial, sans-serif;
        }
        textarea {
            resize: vertical;
            min-height: 70px;
        }
        .section-title {
            margin-top: 20px;
            margin-bottom: 10px;
            color: #007BFF;
            font-weight: bold;
            text-align: center;
        }
        .button {
            width: 100%;
            padding: 10px;
            border: none;
            cursor: pointer;
            border-radius: 5px;
            transition: 0.3s;
            color: white;
            font-size: 16px;
            margin-top: 10px;
        }
        .validate { background-color: #28a745; }
        .validate:hover { background-color: #218838; }
        .back { background-color: #6c757d; }
        .back:hover { background-color: #565e64; }
        .logout { background-color: #dc3545; }
        .logout:hover { background-color: #b02a37; }
        .result {
            margin-top: 20px;
            font-weight: bold;
            color: #333;
            text-align: center;
        }
    </style>

    <script>
        function afficherDonnees() {
            let role = localStorage.getItem("esp32_role");

            let nom = document.getElementById("nom").value;
            let prenom = document.getElementById("prenom").value;
            let dateNaissance = document.getElementById("date_naissance").value;
            let sexe = document.getElementById("sexe").value;
            let taille = document.getElementById("taille").value;
            let poids = document.getElementById("poids").value;
            let groupeSanguin = document.getElementById("groupe_sanguin").value;
            let allergies = document.getElementById("allergies").value;
            let maladies = document.getElementById("maladies").value;
            let traitement = document.getElementById("traitement").value;
            let contactUrgence = document.getElementById("contact_urgence").value;
            let personnePrevenir = document.getElementById("personne_prevenir").value;
            let lienParente = document.getElementById("lien_parente").value;
            let traitementsVitaux = document.getElementById("traitements_vitaux").value;

            let url = "/save?nom=" + encodeURIComponent(nom) +
                      "&prenom=" + encodeURIComponent(prenom) +
                      "&date=" + encodeURIComponent(dateNaissance) +
                      "&sexe=" + encodeURIComponent(sexe) +
                      "&taille=" + encodeURIComponent(taille) +
                      "&poids=" + encodeURIComponent(poids) +
                      "&groupe=" + encodeURIComponent(groupeSanguin) +
                      "&allergies=" + encodeURIComponent(allergies) +
                      "&maladies=" + encodeURIComponent(maladies) +
                      "&traitement=" + encodeURIComponent(traitement) +
                      "&contact=" + encodeURIComponent(contactUrgence) +
                      "&personne=" + encodeURIComponent(personnePrevenir) +
                      "&lien=" + encodeURIComponent(lienParente) +
                      "&vitaux=" + encodeURIComponent(traitementsVitaux) +
                      "&role=" + encodeURIComponent(role);

            if (role === "admin") {
                let antecedents = document.getElementById("antecedents_medicaux").value;
                let chirurgie = document.getElementById("chirurgie").value;
                let medecinTraitant = document.getElementById("medecin_traitant").value;
                let numeroSecu = document.getElementById("numero_secu").value;
                let contreIndications = document.getElementById("contre_indications").value;
                let pathologiesChroniques = document.getElementById("pathologies_chroniques").value;
                let numeroDossier = document.getElementById("numero_dossier").value;

                url += "&antecedents=" + encodeURIComponent(antecedents) +
                       "&chirurgie=" + encodeURIComponent(chirurgie) +
                       "&medecin=" + encodeURIComponent(medecinTraitant) +
                       "&secu=" + encodeURIComponent(numeroSecu) +
                       "&contre=" + encodeURIComponent(contreIndications) +
                       "&pathologies=" + encodeURIComponent(pathologiesChroniques) +
                       "&dossier=" + encodeURIComponent(numeroDossier);
            }

            fetch(url)
            .then(response => response.text())
            .then(data => {
                document.getElementById("result").innerHTML = data;
            });
        }

        function goMenu() {
            window.location.href = "/menu";
        }

        function logout() {
            localStorage.removeItem("esp32_auth");
            localStorage.removeItem("esp32_role");
            window.location.href = "/";
        }

        window.onload = function() {
            if (localStorage.getItem("esp32_auth") !== "true") {
                window.location.href = "/";
                return;
            }

            let role = localStorage.getItem("esp32_role");

            if (role === "admin") {
                document.getElementById("adminFields").style.display = "block";
                document.getElementById("pageTitle").innerText = "Écrire les données médecin";
            } else {
                document.getElementById("adminFields").style.display = "none";
                document.getElementById("pageTitle").innerText = "Écrire les données de base";
            }
        }
    </script>
</head>
<body>
    <div class="container">
        <h1 id="pageTitle">Écrire les données</h1>
)rawliteral";

    html += "<div class='section-title'>Informations de base</div>";
    html += inputText("nom", "Nom", nom, "Nom");
    html += inputText("prenom", "Prénom", prenom, "Prénom");
    html += inputDate("date_naissance", "Date de naissance", dateNaissance);

    html += "<label for='sexe'>Sexe</label>";
    html += "<select id='sexe'>";
    html += "<option value=''" + optionSelected(sexe, "") + ">Choisir</option>";
    html += "<option value='Homme'" + optionSelected(sexe, "Homme") + ">Homme</option>";
    html += "<option value='Femme'" + optionSelected(sexe, "Femme") + ">Femme</option>";
    html += "<option value='Autre'" + optionSelected(sexe, "Autre") + ">Autre</option>";
    html += "</select>";

    html += inputText("taille", "Taille", taille, "Ex : 1.78 m");
    html += inputText("poids", "Poids", poids, "Ex : 74 kg");

    html += "<label for='groupe_sanguin'>Groupe sanguin</label>";
    html += "<select id='groupe_sanguin'>";
    html += "<option value=''" + optionSelected(groupeSanguin, "") + ">Choisir</option>";
    html += "<option value='A+'" + optionSelected(groupeSanguin, "A+") + ">A+</option>";
    html += "<option value='A-'" + optionSelected(groupeSanguin, "A-") + ">A-</option>";
    html += "<option value='B+'" + optionSelected(groupeSanguin, "B+") + ">B+</option>";
    html += "<option value='B-'" + optionSelected(groupeSanguin, "B-") + ">B-</option>";
    html += "<option value='AB+'" + optionSelected(groupeSanguin, "AB+") + ">AB+</option>";
    html += "<option value='AB-'" + optionSelected(groupeSanguin, "AB-") + ">AB-</option>";
    html += "<option value='O+'" + optionSelected(groupeSanguin, "O+") + ">O+</option>";
    html += "<option value='O-'" + optionSelected(groupeSanguin, "O-") + ">O-</option>";
    html += "</select>";

    html += textArea("allergies", "Allergies", allergies, "Ex : pénicilline, arachides");
    html += textArea("maladies", "Maladies / antécédents", maladies, "Ex : diabète, épilepsie, asthme");
    html += textArea("traitement", "Traitement en cours", traitement, "Traitement");
    html += textArea("traitements_vitaux", "Traitements vitaux", traitementsVitaux, "Traitements indispensables");
    html += inputText("contact_urgence", "Contact d'urgence", contactUrgence, "Nom du contact");
    html += inputText("personne_prevenir", "Personne à prévenir", personneAPrevenir, "Nom");
    html += inputText("lien_parente", "Lien de parenté", lienParente, "Ex : mère, frère");

    html += "<div id='adminFields' style='display:none;'>";
    html += "<div class='section-title'>Informations réservées au médecin</div>";
    html += textArea("antecedents_medicaux", "Antécédents médicaux détaillés", antecedentsMedicaux, "Antécédents détaillés");
    html += textArea("chirurgie", "Chirurgies / hospitalisations", chirurgie, "Chirurgies, hospitalisations");
    html += inputText("medecin_traitant", "Médecin traitant", medecinTraitant, "Nom du médecin");
    html += inputText("numero_secu", "Numéro de sécurité sociale", numeroSecu, "Numéro de sécurité sociale");
    html += textArea("contre_indications", "Contre-indications", contreIndications, "Contre-indications");
    html += textArea("pathologies_chroniques", "Pathologies chroniques", pathologiesChroniques, "Pathologies chroniques");
    html += inputText("numero_dossier", "Numéro de dossier médical", numeroDossierMedical, "Numéro dossier");
    html += "</div>";

    html += R"rawliteral(
        <button class="button validate" onclick="afficherDonnees()">Valider</button>
        <button class="button back" onclick="goMenu()">Retour au menu</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>
        <p class="result" id="result"></p>
    </div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

// ---------- FICHE URGENCE PATIENT ----------
void handleReadPatient() {
    nom = preferences.getString("nom", "");
    prenom = preferences.getString("prenom", "");
    dateNaissance = preferences.getString("date", "");
    sexe = preferences.getString("sexe", "");
    taille = preferences.getString("taille", "");
    poids = preferences.getString("poids", "");
    groupeSanguin = preferences.getString("groupe", "");
    allergies = preferences.getString("allergies", "");
    traitement = preferences.getString("traitement", "");
    traitementsVitaux = preferences.getString("vitaux", "");
    contactUrgence = preferences.getString("contact", "");
    personneAPrevenir = preferences.getString("personne", "");
    lienParente = preferences.getString("lien", "");

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fiche d'urgence</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            color: #333;
            text-align: center;
        }
        .container {
            margin-top: 25px;
            max-width: 460px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 25px;
            border-radius: 14px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: left;
        }
        .main-name {
            text-align: center;
            font-size: 32px;
            font-weight: bold;
            margin-bottom: 18px;
            color: #1b1b1b;
        }
        .blood {
            text-align: center;
            font-size: 28px;
            font-weight: bold;
            color: #b00020;
            margin-bottom: 20px;
            padding: 12px;
            border: 2px solid #b00020;
            border-radius: 10px;
            background: #fff5f6;
        }
        .card {
            margin-bottom: 15px;
            padding: 14px;
            border-radius: 10px;
            border: 1px solid #ddd;
            background: #f8f9fa;
        }
        .danger {
            background: #fff3f3;
            border: 2px solid #d32f2f;
        }
        .warning {
            background: #fff8e1;
            border: 2px solid #f0ad4e;
        }
        .label {
            font-weight: bold;
            margin-bottom: 6px;
            display: block;
        }
        .button {
            width: 100%;
            padding: 10px;
            border: none;
            cursor: pointer;
            border-radius: 5px;
            color: white;
            font-size: 16px;
            margin-top: 10px;
        }
        .edit { background-color: #28a745; }
        .edit:hover { background-color: #218838; }
        .back { background-color: #6c757d; }
        .back:hover { background-color: #565e64; }
        .logout { background-color: #dc3545; }
        .logout:hover { background-color: #b02a37; }
    </style>
    <script>
        function goMenu() { window.location.href = "/menu"; }
        function goEdit() { window.location.href = "/control"; }
        function logout() {
            localStorage.removeItem("esp32_auth");
            localStorage.removeItem("esp32_role");
            window.location.href = "/";
        }
        if (localStorage.getItem("esp32_auth") !== "true") {
            window.location.href = "/";
        }
    </script>
</head>
<body>
    <div class="container">
)rawliteral";

    html += "<div class='main-name'>" + htmlEscape(prenom + " " + nom) + "</div>";
    html += "<div class='blood'>Groupe sanguin : " + htmlEscape(groupeSanguin) + "</div>";

    html += "<div class='card'><span class='label'>Date de naissance</span>" + htmlEscape(dateNaissance) + "</div>";
    html += "<div class='card'><span class='label'>Sexe</span>" + htmlEscape(sexe) + "</div>";
    html += "<div class='card'><span class='label'>Taille / Poids</span>" + htmlEscape(taille + " / " + poids) + "</div>";

    html += "<div class='card danger'><span class='label'>Allergies</span>" + htmlEscape(allergies) + "</div>";
    html += "<div class='card warning'><span class='label'>Traitements vitaux</span>" + htmlEscape(traitementsVitaux) + "</div>";
    html += "<div class='card'><span class='label'>Traitement en cours</span>" + htmlEscape(traitement) + "</div>";

    html += "<div class='card'><span class='label'>Contact d'urgence</span>" + htmlEscape(contactUrgence) + "</div>";
    html += "<div class='card'><span class='label'>Personne à prévenir</span>" + htmlEscape(personneAPrevenir) + "</div>";
    html += "<div class='card'><span class='label'>Lien de parenté</span>" + htmlEscape(lienParente) + "</div>";

    html += R"rawliteral(
        <button class="button edit" onclick="goEdit()">Modifier</button>
        <button class="button back" onclick="goMenu()">Retour au menu</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>
    </div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

// ---------- FICHE COMPLETE MEDECIN ----------
void handleReadMedecin() {
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

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Lecture médecin</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            color: #333;
            text-align: center;
        }
        .container {
            margin-top: 25px;
            max-width: 520px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 25px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: left;
        }
        h1 {
            color: #007BFF;
            text-align: center;
        }
        .field {
            margin-bottom: 14px;
        }
        .label {
            font-weight: bold;
            display: block;
            margin-bottom: 4px;
        }
        .value {
            background: #f8f9fa;
            padding: 10px;
            border-radius: 5px;
            border: 1px solid #ddd;
            word-wrap: break-word;
        }
        .section {
            margin-top: 20px;
            margin-bottom: 10px;
            color: #007BFF;
            font-weight: bold;
            text-align: center;
        }
        .button {
            width: 100%;
            padding: 10px;
            border: none;
            cursor: pointer;
            border-radius: 5px;
            color: white;
            font-size: 16px;
            margin-top: 10px;
        }
        .edit { background-color: #28a745; }
        .edit:hover { background-color: #218838; }
        .back { background-color: #6c757d; }
        .back:hover { background-color: #565e64; }
        .logout { background-color: #dc3545; }
        .logout:hover { background-color: #b02a37; }
    </style>
    <script>
        function goMenu() { window.location.href = "/menu"; }
        function goEdit() { window.location.href = "/control"; }
        function logout() {
            localStorage.removeItem("esp32_auth");
            localStorage.removeItem("esp32_role");
            window.location.href = "/";
        }
        if (localStorage.getItem("esp32_auth") !== "true") {
            window.location.href = "/";
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Fiche médecin</h1>
)rawliteral";

    html += "<div class='section'>Informations de base</div>";
    html += "<div class='field'><span class='label'>Nom</span><div class='value'>" + htmlEscape(nom) + "</div></div>";
    html += "<div class='field'><span class='label'>Prénom</span><div class='value'>" + htmlEscape(prenom) + "</div></div>";
    html += "<div class='field'><span class='label'>Date de naissance</span><div class='value'>" + htmlEscape(dateNaissance) + "</div></div>";
    html += "<div class='field'><span class='label'>Sexe</span><div class='value'>" + htmlEscape(sexe) + "</div></div>";
    html += "<div class='field'><span class='label'>Taille</span><div class='value'>" + htmlEscape(taille) + "</div></div>";
    html += "<div class='field'><span class='label'>Poids</span><div class='value'>" + htmlEscape(poids) + "</div></div>";
    html += "<div class='field'><span class='label'>Groupe sanguin</span><div class='value'>" + htmlEscape(groupeSanguin) + "</div></div>";
    html += "<div class='field'><span class='label'>Allergies</span><div class='value'>" + htmlEscape(allergies) + "</div></div>";
    html += "<div class='field'><span class='label'>Maladies / antécédents</span><div class='value'>" + htmlEscape(maladies) + "</div></div>";
    html += "<div class='field'><span class='label'>Traitement en cours</span><div class='value'>" + htmlEscape(traitement) + "</div></div>";
    html += "<div class='field'><span class='label'>Traitements vitaux</span><div class='value'>" + htmlEscape(traitementsVitaux) + "</div></div>";
    html += "<div class='field'><span class='label'>Contact d'urgence</span><div class='value'>" + htmlEscape(contactUrgence) + "</div></div>";
    html += "<div class='field'><span class='label'>Personne à prévenir</span><div class='value'>" + htmlEscape(personneAPrevenir) + "</div></div>";
    html += "<div class='field'><span class='label'>Lien de parenté</span><div class='value'>" + htmlEscape(lienParente) + "</div></div>";

    html += "<div class='section'>Informations réservées au médecin</div>";
    html += "<div class='field'><span class='label'>Antécédents médicaux détaillés</span><div class='value'>" + htmlEscape(antecedentsMedicaux) + "</div></div>";
    html += "<div class='field'><span class='label'>Chirurgies / hospitalisations</span><div class='value'>" + htmlEscape(chirurgie) + "</div></div>";
    html += "<div class='field'><span class='label'>Médecin traitant</span><div class='value'>" + htmlEscape(medecinTraitant) + "</div></div>";
    html += "<div class='field'><span class='label'>Numéro sécurité sociale</span><div class='value'>" + htmlEscape(numeroSecu) + "</div></div>";
    html += "<div class='field'><span class='label'>Contre-indications</span><div class='value'>" + htmlEscape(contreIndications) + "</div></div>";
    html += "<div class='field'><span class='label'>Pathologies chroniques</span><div class='value'>" + htmlEscape(pathologiesChroniques) + "</div></div>";
    html += "<div class='field'><span class='label'>Numéro de dossier médical</span><div class='value'>" + htmlEscape(numeroDossierMedical) + "</div></div>";

    html += R"rawliteral(
        <button class="button edit" onclick="goEdit()">Modifier</button>
        <button class="button back" onclick="goMenu()">Retour au menu</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>
    </div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

// ---------- SAUVEGARDE ----------
void handleSave() {
    if (server.hasArg("nom") &&
        server.hasArg("prenom") &&
        server.hasArg("date") &&
        server.hasArg("sexe") &&
        server.hasArg("taille") &&
        server.hasArg("poids") &&
        server.hasArg("groupe") &&
        server.hasArg("allergies") &&
        server.hasArg("maladies") &&
        server.hasArg("traitement") &&
        server.hasArg("contact") &&
        server.hasArg("personne") &&
        server.hasArg("lien") &&
        server.hasArg("vitaux") &&
        server.hasArg("role")) {

        nom = server.arg("nom");
        prenom = server.arg("prenom");
        dateNaissance = server.arg("date");
        sexe = server.arg("sexe");
        taille = server.arg("taille");
        poids = server.arg("poids");
        groupeSanguin = server.arg("groupe");
        allergies = server.arg("allergies");
        maladies = server.arg("maladies");
        traitement = server.arg("traitement");
        contactUrgence = server.arg("contact");
        personneAPrevenir = server.arg("personne");
        lienParente = server.arg("lien");
        traitementsVitaux = server.arg("vitaux");

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

        String role = server.arg("role");

        if (role == "admin") {
            antecedentsMedicaux = server.arg("antecedents");
            chirurgie = server.arg("chirurgie");
            medecinTraitant = server.arg("medecin");
            numeroSecu = server.arg("secu");
            contreIndications = server.arg("contre");
            pathologiesChroniques = server.arg("pathologies");
            numeroDossierMedical = server.arg("dossier");

            preferences.putString("antecedents", antecedentsMedicaux);
            preferences.putString("chirurgie", chirurgie);
            preferences.putString("medecin", medecinTraitant);
            preferences.putString("secu", numeroSecu);
            preferences.putString("contre", contreIndications);
            preferences.putString("pathologies", pathologiesChroniques);
            preferences.putString("dossier", numeroDossierMedical);
        }
        server.send(200, "text/plain", "Données enregistrées avec succès");
    } else {
        server.send(400, "text/plain", "Paramètres manquants");
    }
}

void handleRedirect() {
    server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
    server.send(302, "text/plain", "");
}

// ---------- SETUP ----------
void setup() {
    Serial.begin(115200);
    delay(1000);

    preferences.begin("formulaire", false);

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

    WiFi.softAP(ssid, password);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    MDNS.begin("safyra");

    server.on("/", handleRoot);
    server.on("/login", handleLogin);
    server.on("/menu", handleMenu);
    server.on("/control", handleControl);
    server.on("/read_patient", handleReadPatient);
    server.on("/read_medecin", handleReadMedecin);
    server.on("/save", handleSave);

    server.on("/generate_204", handleRedirect);      // Android
    server.on("/fwlink", handleRedirect);            // Microsoft
    server.on("/hotspot-detect.html", handleRedirect); // Apple
    server.on("/library/test/success.html", handleRedirect); // Apple
    server.on("/ncsi.txt", handleRedirect);          // Windows
    server.onNotFound(handleRedirect);               // tout le reste

    server.begin();
    }

// ---------- LOOP ----------
void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}