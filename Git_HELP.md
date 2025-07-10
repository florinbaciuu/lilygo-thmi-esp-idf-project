# 📘 Ghid Git personal – by florinbaciuu (a.k.a. The One)

https://github.com/florinbaciuu/lilygo-thmi-esp-idf-project.git


### 🔧 Configurare Git globală (doar o dată pe sistem)

🔧 # Numele tău pentru toate commiturile
```
git config --global user.name "florinbaciuu"
```
🔧 # Emailul asociat contului GitHub
```
git config --global user.email "baciuaurelflorin@gmail.com"
```
🔧 # Emailul asociat contului GitHub
```
git config --global init.defaultBranch main                       # Branch implicit: main
```

---

## 🆕 Inițializarea unui proiect nou și conectarea la GitHub

```
git init                                                          # Inițializează repo local
git add .                                                         # Adaugă toate fișierele
git commit -m "first commit"                                      # Creează primul commit
git branch -M main                                                # Redenumește branch-ul în 'main'
git remote add origin https://github.com/florinbaciuu/lilygo-thmi-esp-idf-project.git    # Adaugă remote-ul
```
---



### 🔗 Adăugare submodul Git (ex: o librărie în lib/)
### 🔍 Verificare submodule existente
## 🔗 Adăugare, verificare și ștergere submodul Git (ex: o librărie în lib/)

```
git submodule add https://github.com/florinbaciuu/ESP32_Resource_Monitor.git lib/ESP32_Resource_Monitor
git submodule status                  # Afișează commiturile și starea submodulelor
git config -f .gitmodules --list     # Verifică ce submodule sunt înregistrate oficial
cat .gitmodules                      # Afișează configurația direct
```

### ❌ Ștergere completă a unui submodul

```
git submodule deinit -f lib/ESP32_Resource_Monitor
git rm -f lib/ESP32_Resource_Monitor
git rm -f .git/modules/lib/ESP32_Resource_Monitor
rm -rf .git/modules/lib/ESP32_Resource_Monitor
git commit -m "Șters submodulul ESP32_Resource_Monitor"
```
---

## 🚀 Push către GitHub

```
git push -u origin main                                           # Push initial La primul push dintr-un proiect nou:
git push --set-upstream origin main				                  # Push initial La primul push dintr-un proiect nou:
git branch -vv							                          # Verifica ce upstream e
git push 							                              # Dupa ce ai pus "git push -u origin main"
git push --force origin main                                      # Push cu forțare (atenție!)

git fetch                                                         # Doar aduce modificările remote, nu le aplică local
git pull                                                          # Fetch + Merge (sau Rebase) = trage și le bagă direct în branchul tău
git clone                                                         # Fetch complet + creează repo nou local
git push                                                          # Trimite schimbările tale către remote
```

---

## 🔁 Ștergere remote 'origin'

```
git remote remove origin                                          # Șterge remote-ul definit
```

---

## ⬇️ Clonare cu tot cu submodule

```
git clone --recurse-submodules https://github.com/florinbaciuu/lilygo-thmi-esp-idf-project
git submodule status
git submodule update --init --recursive
```

---

## 🛠️ Reparare submodul – commit lipsă / detached HEAD

### 🔹 Varianta 1: Forțezi commit valid

```
cd lib/ESP32_Resource_Monitor
git fetch
git checkout main
cd ../..
git add lib/ESP32_Resource_Monitor
git commit -m "Resetat submodulul la commit valid"
git push
```

### 🔹 Varianta 2: Ștergi submodulul și îl adaugi curat

```
git submodule deinit -f lib/ESP32_Resource_Monitor
git rm -f lib/ESP32_Resource_Monitor
rm .git/modules/lib/ESP32_Resource_Monitor
git commit -m "Șters submodulul buclucaș"
```

Apoi:
```
git submodule add https://github.com/florinbaciuu/ESP32_Resource_Monitor.git lib/ESP32_Resource_Monitor
git commit -m "Adăugat din nou submodulul"
git push
```

### 🔹 Sterge fisier din cache dupa introducerea in .gitignore
```
git rm -r --cached .devcontainer
git rm -r --cached .vscode
git rm -r --cached build

```

