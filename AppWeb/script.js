// Chargement des données initiales
const fields = ["name", "age", "height", "weight", "blood", "allergy"];

// Charger depuis le localStorage
fields.forEach(field => {
  const value = localStorage.getItem(field);
  if (value) document.getElementById(field + "Display").textContent = value + (field === "age" ? " ans" : "");
});

// Soumission du formulaire
document.getElementById("updateForm").addEventListener("submit", (e) => {
  e.preventDefault();
  fields.forEach(field => {
    const input = document.getElementById(field + "Input").value;
    if (input) {
      localStorage.setItem(field, input);
      document.getElementById(field + "Display").textContent = 
        input + (field === "age" ? " ans" : "");
    }
  });
  alert("✅ Informations mises à jour !");
});
