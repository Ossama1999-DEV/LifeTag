/**
 * @file main.ino
 * @brief ESP32 - Login + menu lecture/ecriture + formulaire + affichage des donnees
 */

// http://192.168.4.1

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Adafruit_GFX.h>
#include <Preferences.h>

// Identifiants de connexion
const char* validUser = "admin";
const char* validPass = "1234";

// Parametres WiFi
const char* ssid = "LucasEsp32";
const char* password = "123456789";

// LED
#define LED_PIN 19
bool etatLed = false;

// Serveur Web
WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

// Memoire persistante
Preferences preferences;

// Variables de stockage
String nom = "";
String prenom = "";
String dateNaissance = "";
String groupeSanguin = "";
String allergies = "";
String maladies = "";
String traitement = "";
String contactUrgence = "";

// Pin Relay
#define RELAY_PIN 27

// Page HTML de connexion
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
            max-width: 300px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        h1 {
            color: #007BFF;
        }

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

        .button:hover {
            background-color: #0056b3;
        }

        .error {
            color: red;
            font-weight: bold;
            display: none;
        }
    </style>

    <script>
        function login() {
            var username = document.getElementById("username").value;
            var password = document.getElementById("password").value;

            fetch("/login?user=" + encodeURIComponent(username) + "&pass=" + encodeURIComponent(password))
            .then(response => response.text())
            .then(data => {
                if (data === "SUCCESS") {
                    localStorage.setItem("esp32_auth", "true");
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
        <h1>🔒 HOME Login</h1>
        <input type="text" id="username" placeholder="Nom d'utilisateur">
        <input type="password" id="password" placeholder="Mot de passe">
        <p class="error" id="error-message">Login ou mot de passe incorrect</p>
        <button class="button" onclick="login()">Se connecter</button>
    </div>
</body>
</html>
)rawliteral";

// Page menu
const char menu_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Menu bracelet</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            color: #333;
        }

        .container {
            margin-top: 80px;
            max-width: 340px;
            margin-left: auto;
            margin-right: auto;
            background: white;
            padding: 25px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        h1 {
            color: #007BFF;
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

        .write {
            background-color: #28a745;
        }

        .write:hover {
            background-color: #218838;
        }

        .read {
            background-color: #007BFF;
        }

        .read:hover {
            background-color: #0056b3;
        }

        .logout {
            background-color: #dc3545;
        }

        .logout:hover {
            background-color: #b02a37;
        }
    </style>

    <script>
        function goWrite() {
            window.location.href = "/control";
        }

        function goRead() {
            window.location.href = "/read";
        }

        function logout() {
            localStorage.removeItem("esp32_auth");
            window.location.href = "/";
        }

        if (localStorage.getItem("esp32_auth") !== "true") {
            window.location.href = "/";
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Bracelet médical</h1>
        <button class="button write" onclick="goWrite()">Écrire les données</button>
        <button class="button read" onclick="goRead()">Lire les données</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>
    </div>
</body>
</html>
)rawliteral";

// Page formulaire
const char control_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bracelet médical</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            color: #333;
        }

        .container {
            margin-top: 30px;
            max-width: 380px;
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

        .validate {
            background-color: #28a745;
        }

        .validate:hover {
            background-color: #218838;
        }

        .back {
            background-color: #6c757d;
        }

        .back:hover {
            background-color: #565e64;
        }

        .logout {
            background-color: #dc3545;
        }

        .logout:hover {
            background-color: #b02a37;
        }

        .result {
            margin-top: 20px;
            font-weight: bold;
            color: #333;
            text-align: center;
        }
    </style>

    <script>
        function afficherDonnees() {
            let nom = document.getElementById("nom").value;
            let prenom = document.getElementById("prenom").value;
            let dateNaissance = document.getElementById("date_naissance").value;
            let groupeSanguin = document.getElementById("groupe_sanguin").value;
            let allergies = document.getElementById("allergies").value;
            let maladies = document.getElementById("maladies").value;
            let traitement = document.getElementById("traitement").value;
            let contactUrgence = document.getElementById("contact_urgence").value;

            fetch("/save?nom=" + encodeURIComponent(nom) +
                  "&prenom=" + encodeURIComponent(prenom) +
                  "&date=" + encodeURIComponent(dateNaissance) +
                  "&groupe=" + encodeURIComponent(groupeSanguin) +
                  "&allergies=" + encodeURIComponent(allergies) +
                  "&maladies=" + encodeURIComponent(maladies) +
                  "&traitement=" + encodeURIComponent(traitement) +
                  "&contact=" + encodeURIComponent(contactUrgence))
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
            window.location.href = "/";
        }

        if (localStorage.getItem("esp32_auth") !== "true") {
            window.location.href = "/";
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Écrire les données</h1>

        <label for="nom">Nom</label>
        <input type="text" id="nom" placeholder="Nom">

        <label for="prenom">Prénom</label>
        <input type="text" id="prenom" placeholder="Prénom">

        <label for="date_naissance">Date de naissance</label>
        <input type="date" id="date_naissance">

        <label for="groupe_sanguin">Groupe sanguin</label>
        <select id="groupe_sanguin">
            <option value="">Choisir</option>
            <option value="A+">A+</option>
            <option value="A-">A-</option>
            <option value="B+">B+</option>
            <option value="B-">B-</option>
            <option value="AB+">AB+</option>
            <option value="AB-">AB-</option>
            <option value="O+">O+</option>
            <option value="O-">O-</option>
        </select>

        <label for="allergies">Allergies</label>
        <textarea id="allergies" placeholder="Ex : pénicilline, arachides"></textarea>

        <label for="maladies">Maladies / antécédents</label>
        <textarea id="maladies" placeholder="Ex : diabète, épilepsie, asthme"></textarea>

        <label for="traitement">Traitement en cours</label>
        <textarea id="traitement" placeholder="Ex : insulinothérapie, anticoagulants"></textarea>

        <label for="contact_urgence">Contact d'urgence</label>
        <input type="text" id="contact_urgence" placeholder="Nom du contact">

        <button class="button validate" onclick="afficherDonnees()">Valider</button>
        <button class="button back" onclick="goMenu()">Retour au menu</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>

        <p class="result" id="result"></p>
    </div>
</body>
</html>
)rawliteral";

// Gestion de la page principale
void handleRoot() {
    server.send(200, "text/html", login_page);
}

// Gestion de la connexion
void handleLogin() {
    if (server.hasArg("user") && server.hasArg("pass")) {
        String user = server.arg("user");
        String pass = server.arg("pass");

        if (user == validUser && pass == validPass) {
            server.send(200, "text/plain", "SUCCESS");
        } else {
            server.send(401, "text/plain", "ERROR");
        }
    } else {
        server.send(400, "text/plain", "Missing parameters");
    }
}

// Page menu
void handleMenu() {
    server.send(200, "text/html", menu_page);
}

// Page ecriture
void handleControl() {
    server.send(200, "text/html", control_page);
}

// Page lecture
void handleRead() {
    nom = preferences.getString("nom", "");
    prenom = preferences.getString("prenom", "");
    dateNaissance = preferences.getString("date", "");
    groupeSanguin = preferences.getString("groupe", "");
    allergies = preferences.getString("allergies", "");
    maladies = preferences.getString("maladies", "");
    traitement = preferences.getString("traitement", "");
    contactUrgence = preferences.getString("contact", "");

    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Lecture des données</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            color: #333;
            text-align: center;
        }

        .container {
            margin-top: 30px;
            max-width: 420px;
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

        .back {
            background-color: #6c757d;
        }

        .back:hover {
            background-color: #565e64;
        }

        .logout {
            background-color: #dc3545;
        }

        .logout:hover {
            background-color: #b02a37;
        }
    </style>
    <script>
        function goMenu() {
            window.location.href = "/menu";
        }

        function logout() {
            localStorage.removeItem("esp32_auth");
            window.location.href = "/";
        }

        if (localStorage.getItem("esp32_auth") !== "true") {
            window.location.href = "/";
        }
    </script>
</head>
<body>
    <div class="container">
        <h1>Lire les données</h1>
)rawliteral";

    html += "<div class='field'><span class='label'>Nom</span><div class='value'>" + nom + "</div></div>";
    html += "<div class='field'><span class='label'>Prénom</span><div class='value'>" + prenom + "</div></div>";
    html += "<div class='field'><span class='label'>Date de naissance</span><div class='value'>" + dateNaissance + "</div></div>";
    html += "<div class='field'><span class='label'>Groupe sanguin</span><div class='value'>" + groupeSanguin + "</div></div>";
    html += "<div class='field'><span class='label'>Allergies</span><div class='value'>" + allergies + "</div></div>";
    html += "<div class='field'><span class='label'>Maladies / antécédents</span><div class='value'>" + maladies + "</div></div>";
    html += "<div class='field'><span class='label'>Traitement en cours</span><div class='value'>" + traitement + "</div></div>";
    html += "<div class='field'><span class='label'>Contact d'urgence</span><div class='value'>" + contactUrgence + "</div></div>";

    html += R"rawliteral(
        <button class="button back" onclick="goMenu()">Retour au menu</button>
        <button class="button logout" onclick="logout()">Déconnexion</button>
    </div>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

// Sauvegarde des donnees
void handleSave() {
    if (server.hasArg("nom") &&
        server.hasArg("prenom") &&
        server.hasArg("date") &&
        server.hasArg("groupe") &&
        server.hasArg("allergies") &&
        server.hasArg("maladies") &&
        server.hasArg("traitement") &&
        server.hasArg("contact")) {

        nom = server.arg("nom");
        prenom = server.arg("prenom");
        dateNaissance = server.arg("date");
        groupeSanguin = server.arg("groupe");
        allergies = server.arg("allergies");
        maladies = server.arg("maladies");
        traitement = server.arg("traitement");
        contactUrgence = server.arg("contact");

        preferences.putString("nom", nom);
        preferences.putString("prenom", prenom);
        preferences.putString("date", dateNaissance);
        preferences.putString("groupe", groupeSanguin);
        preferences.putString("allergies", allergies);
        preferences.putString("maladies", maladies);
        preferences.putString("traitement", traitement);
        preferences.putString("contact", contactUrgence);

        String nomLu = preferences.getString("nom", "");
        String prenomLu = preferences.getString("prenom", "");
        String dateLu = preferences.getString("date", "");
        String groupeLu = preferences.getString("groupe", "");
        String allergiesLu = preferences.getString("allergies", "");
        String maladiesLu = preferences.getString("maladies", "");
        String traitementLu = preferences.getString("traitement", "");
        String contactLu = preferences.getString("contact", "");

        Serial.println("===== DONNEES ENREGISTREES =====");
        Serial.println("Nom : " + nomLu);
        Serial.println("Prenom : " + prenomLu);
        Serial.println("Date de naissance : " + dateLu);
        Serial.println("Groupe sanguin : " + groupeLu);
        Serial.println("Allergies : " + allergiesLu);
        Serial.println("Maladies / antecedents : " + maladiesLu);
        Serial.println("Traitement : " + traitementLu);
        Serial.println("Contact urgence : " + contactLu);
        Serial.println("================================");

        server.send(200, "text/plain", "Données médicales enregistrées avec succès");
    } else {
        server.send(400, "text/plain", "Paramètres manquants");
    }
}

// Setup
void setup() {
    Serial.begin(115200);
    delay(1000);

    preferences.begin("formulaire", false);

    nom = preferences.getString("nom", "");
    prenom = preferences.getString("prenom", "");
    dateNaissance = preferences.getString("date", "");
    groupeSanguin = preferences.getString("groupe", "");
    allergies = preferences.getString("allergies", "");
    maladies = preferences.getString("maladies", "");
    traitement = preferences.getString("traitement", "");
    contactUrgence = preferences.getString("contact", "");

    WiFi.softAP(ssid, password);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/login", handleLogin);
    server.on("/menu", handleMenu);
    server.on("/control", handleControl);
    server.on("/read", handleRead);
    server.on("/save", handleSave);

    server.begin();

    Serial.println("Serveur demarre");
    Serial.print("IP ESP32 : ");
    Serial.println(WiFi.softAPIP());

    Serial.println("===== DONNEES DEJA SAUVEGARDEES =====");
    Serial.println("Nom : " + nom);
    Serial.println("Prenom : " + prenom);
    Serial.println("Date de naissance : " + dateNaissance);
    Serial.println("Groupe sanguin : " + groupeSanguin);
    Serial.println("Allergies : " + allergies);
    Serial.println("Maladies / antecedents : " + maladies);
    Serial.println("Traitement : " + traitement);
    Serial.println("Contact urgence : " + contactUrgence);
    Serial.println("====================================");
}

// Loop
void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}