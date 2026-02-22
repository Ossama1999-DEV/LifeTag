## 🔥 BONUS — Exécution automatique au démarrage

Tu peux créer une tâche planifiée Windows :

### Étapes

1. Ouvre **Task Scheduler** → **Create Task**
2. Cocher **Run with highest privileges** ✅
3. **Trigger**: At logon
4. **Action**: Exécute le script PowerShell suivant

```powershell
powershell.exe -ExecutionPolicy Bypass -File "C:\Users\PC\scripts\attach-readpi.ps1"
```
