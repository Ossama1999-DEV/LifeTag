const fields = ["name", "age", "height", "weight", "blood", "allergy"];
const units = { age: " ans", height: " m", weight: " kg", blood: "", allergy: "", name: "" };

// Affiche la valeur avec unité
function displayValue(field, value) {
  const unit = units[field] || "";
  document.getElementById(field + "Display").textContent = value + unit;
}

// Charger les infos stockées
fields.forEach(f => {
  const v = localStorage.getItem(f);
  if (v) displayValue(f, v);
});

// Charger la photo si elle existe
const profilePhoto = document.getElementById("profilePhoto");
const savedPhoto = localStorage.getItem("profilePhoto");
if (savedPhoto) profilePhoto.src = savedPhoto;

// Gestion du formulaire
document.getElementById("updateForm").addEventListener("submit", e => {
  e.preventDefault();
  fields.forEach(f => {
    const input = document.getElementById(f + "Input").value;
    if (input) {
      localStorage.setItem(f, input);
      displayValue(f, input);
    }
  });
  alert("✅ Informations mises à jour !");
});

// --- Photo de profil ---
const changePhotoBtn = document.getElementById("changePhotoBtn");
const modal = document.getElementById("photoModal");
const importPhotoBtn = document.getElementById("importPhotoBtn");
const takePhotoBtn = document.getElementById("takePhotoBtn");
const closeModal = document.getElementById("closeModal");
const fileInput = document.getElementById("fileInput");
const cameraContainer = document.getElementById("cameraContainer");
const cameraStream = document.getElementById("cameraStream");
const captureBtn = document.getElementById("captureBtn");
const cancelCameraBtn = document.getElementById("cancelCameraBtn");
const photoCanvas = document.getElementById("photoCanvas");

// Ouvrir le menu
changePhotoBtn.addEventListener("click", () => modal.style.display = "block");

// Fermer le menu
closeModal.addEventListener("click", () => modal.style.display = "none");

// Importer depuis la galerie
importPhotoBtn.addEventListener("click", () => {
  fileInput.click();
});

// Gérer l’importation
fileInput.addEventListener("change", event => {
  const file = event.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = e => {
    profilePhoto.src = e.target.result;
    localStorage.setItem("profilePhoto", e.target.result);
    modal.style.display = "none";
  };
  reader.readAsDataURL(file);
});

// Prendre une photo
takePhotoBtn.addEventListener("click", async () => {
  modal.style.display = "none";
  cameraContainer.style.display = "block";
  try {
    const stream = await navigator.mediaDevices.getUserMedia({ video: true });
    cameraStream.srcObject = stream;
  } catch (err) {
    alert("Erreur : impossible d'accéder à la caméra.");
  }
});

// Capturer la photo
captureBtn.addEventListener("click", () => {
  const ctx = photoCanvas.getContext("2d");
  photoCanvas.width = cameraStream.videoWidth;
  photoCanvas.height = cameraStream.videoHeight;
  ctx.drawImage(cameraStream, 0, 0);
  const dataURL = photoCanvas.toDataURL("image/png");
  profilePhoto.src = dataURL;
  localStorage.setItem("profilePhoto", dataURL);
  stopCamera();
  cameraContainer.style.display = "none";
});

// Fermer la caméra
cancelCameraBtn.addEventListener("click", () => {
  stopCamera();
  cameraContainer.style.display = "none";
});

// Stop le flux caméra
function stopCamera() {
  const stream = cameraStream.srcObject;
  if (stream) {
    stream.getTracks().forEach(track => track.stop());
  }
  cameraStream.srcObject = null;
}
