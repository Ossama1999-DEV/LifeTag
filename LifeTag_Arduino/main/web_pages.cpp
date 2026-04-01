#include "web_pages.h"

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
