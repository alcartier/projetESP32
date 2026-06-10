<p align="center">
  <img src="https://img.shields.io/badge/ESP32--S3-Firmware-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32-S3"/>
  <img src="https://img.shields.io/badge/PlatformIO-Build-orange?style=for-the-badge&logo=platformio&logoColor=white" alt="PlatformIO"/>
  <img src="https://img.shields.io/badge/Framework-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
</p>

<h1 align="center">Tempo ESP32</h1>

<p align="center">
  <strong>Afficheur connecte des jours Tempo EDF sur ecran TFT ILI9341</strong><br>
  <em>Couleur du jour, couleur de demain, jours restants par couleur — en un coup d'oeil.</em>
</p>

---

## Apercu

Ce projet embarque un **ESP32-S3** equipe d'un ecran **TFT 320x240** (ILI9341) qui affiche en temps reel les informations du calendrier **Tempo EDF** :

| Information         | Description                                      |
|---------------------|--------------------------------------------------|
| Couleur du jour     | Bleu, Blanc, Rouge ou Hors saison                |
| Couleur de demain   | Mise a jour quotidienne via API (ou "?" avant 11h) |
| Jours restants      | Decompte par couleur (Bleu/300, Blanc/43, Rouge/22) |
| Date et heure       | Synchronisation NTP automatique (Europe/Paris)   |

---

## Architecture

```
src/
 ├── main.cpp        # Point d'entree — init hardware + boucle principale
 ├── app.h / app.cpp # Machine a etats (connexion, page principale, reglages)
 ├── tempo.h / .cpp  # Logique metier — fetch API, parsing XML, cache NVS
 └── affichage.h/.cpp# Couche UI — rendu TFT avec themes dark/light
```

### Machine a etats

```
┌─────────────┐    WiFi OK     ┌─────────────┐
│  CONNECTION │ ─────────────► │  MAIN_PAGE  │
│             │ ◄───────────── │             │
└─────────────┘    WiFi perdu  └─────────────┘
                                      │ touch
                                      ▼
                               ┌─────────────┐
                               │SETTING_PAGE │ ← bouton "Retour"
                               └─────────────┘
```

---

## Hardware

| Composant       | Reference                |
|-----------------|--------------------------|
| MCU             | ESP32-S3 DevKitC-1       |
| Ecran           | TFT ILI9341 240x320 SPI  |
| Interface       | Ecran tactile (XPT2046)  |

### Brochage SPI

| Signal   | GPIO |
|----------|------|
| MOSI     | 11   |
| SCLK     | 12   |
| CS       | 10   |
| DC       | 8    |
| RST      | 9    |
| BL       | 14   |
| MISO     | 6    |
| TOUCH_CS | 13   |
| TOUCH_IRQ| 3    |

---

## Fonctionnalites

### Affichage principal
- **Couleurs CDC** — BLEU (#1565C0), BLANC (#E0E0E0), ROUGE (#C62828), INCONNU (#424242)
- **Cas speciaux** — "Hors saison" si TEMPO_INCONNU, "?" + "Vers 11h" si demain non publie
- **Indicateur d'incertitude** — Signale si les donnees n'ont pas pu etre actualisees

### Connectivite
- **WiFiManager** — Portail captif `TEMPO-Config` (timeout 5 min)
- **Fetch API** — Cartelectronic XML, quotidien entre 11h-13h, retry toutes les 5 min
- **Timeout HTTP 10s** — Fallback sur cache NVS si le serveur ne repond pas
- **NTP Europe/Paris** — Fuseau POSIX avec gestion automatique de l'heure d'ete

### Persistance (NVS)
- **Donnees Tempo** — `color_today`, `color_tomorrow`, `days_remaining_*`, `fetch_date`, `fetch_timestamp`
- **Parametres** — Mode sombre, alarme, sombre auto — persistes entre redemarrages

### Page Reglages (tactile)
- **Mode sombre / clair** — Toggle avec application immediate
- **Alarme sonore** — Toggle (preparation future)
- **Sombre automatique** — Toggle (preparation future)
- **WiFi** — Affichage du SSID connecte + bouton deconnexion (efface credentials, relance portail)
- **Navigation** — Touch n'importe ou sur main → settings, bouton "Retour" pour revenir

### Logique temporelle
- **Shift a minuit** — Rotation automatique des couleurs jour/demain
- **Flag incertain** — Active apres 13h si aucun fetch reussi

---

## Build & Flash

### Prerequis

- [PlatformIO](https://platformio.org/) (CLI ou extension VS Code)
- Cable USB-C vers l'ESP32-S3

### Compilation

```bash
# Mode release (API Cartelectronic production)
pio run -e release

# Mode debug (serveur local + interactions tactiles debug)
pio run -e debug
```

### Upload

```bash
pio run -e release --target upload
```

### Moniteur serie

```bash
pio device monitor -b 115200
```

---

## Configuration WiFi

Au premier demarrage (ou apres deconnexion via les reglages) :

1. L'ESP32 cree un point d'acces **`TEMPO-Config`**
2. Connectez-vous a ce reseau depuis un telephone/PC
3. Ouvrez **`192.168.4.1`** dans un navigateur
4. Selectionnez votre reseau WiFi et entrez le mot de passe
5. L'appareil se connecte automatiquement (timeout portail : 5 min)

---

## Dependances

| Librairie                              | Version | Usage                    |
|----------------------------------------|---------|--------------------------|
| [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | 2.5.31  | Pilote ecran TFT        |
| [WiFiManager](https://github.com/tzapu/WiFiManager) | latest  | Portail captif WiFi     |

---

## Structure des donnees API

L'appareil interroge le serveur Cartelectronic (XML) :

```xml
<dateJ0>2026-06-06,BLEU</dateJ0>
<dateJ1>2026-06-07,BLANC</dateJ1>
<dcpt>150,20,10</dcpt>
```

| Champ    | Format                     | Description                        |
|----------|----------------------------|------------------------------------|
| `dateJ0` | `YYYY-MM-DD,COULEUR`       | Date et couleur du jour            |
| `dateJ1` | `YYYY-MM-DD,COULEUR`       | Date et couleur de demain          |
| `dcpt`   | `bleu,blanc,rouge`         | Nombre de jours ecoules par couleur|

Couleurs possibles : `BLEU`, `BLANC`, `ROUGE`, `TEMPO_INCONNU`

---

## Licence

Projet personnel — usage libre.
