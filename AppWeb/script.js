// Liste des champs
const fields = ["name", "age", "height", "weight", "blood", "allergy"];

// Dictionnaire des unités associées
const units = {
  age: " ans",
  height: " m",
  weight: " kg",
  blood: "",
  allergy: "",
  name: ""
};

// Fonction d'affichage avec unité
function displayValue(field, value) {
  const unit = units[field] || "";
  document.getElementById(field + "Display").textContent = value + unit;
}

// Charger depuis le localStorage au démarrage
fields.forEach(field => {
  const value = localStorage.getItem(field);
  if (value) displayValue(field, value);
});

// Soumission du formulaire
document.getElementById("updateForm").addEventListener("submit", (e) => {
  e.preventDefault();

  fields.forEach(field => {
    const input = document.getElementById(field + "Input").value;
    if (input) {
      localStorage.setItem(field, input);
      displayValue(field, input);
    }
  });

  alert("✅ Informations mises à jour !");
});
