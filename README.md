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
| Couleur du jour     | Bleu, Blanc ou Rouge                             |
| Couleur de demain   | Mise a jour quotidienne via API                  |
| Jours restants      | Decompte par couleur (Bleu/300, Blanc/43, Rouge/22) |
| Date et heure       | Synchronisation NTP automatique                  |

---

## Architecture

```
src/
 ├── main.cpp        # Point d'entree — init hardware + boucle principale
 ├── app.h / app.cpp # Machine a etats (connexion, page principale, reglages)
 ├── tempo.h / .cpp  # Logique metier — fetch API, parsing XML, gestion couleurs
 └── affichage.h/.cpp# Couche UI — rendu TFT avec cartes arrondies et indicateurs
```

### Machine a etats

```
┌─────────────┐    WiFi OK     ┌─────────────┐
│  CONNECTION │ ─────────────► │  MAIN_PAGE  │
│             │ ◄───────────── │             │
└─────────────┘    WiFi perdu  └─────────────┘
                                      │
                                      ▼
                               ┌─────────────┐
                               │SETTING_PAGE │
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

- **WiFiManager** — Configuration WiFi via portail captif (`ESP32_Config`)
- **Mise a jour automatique** — Fetch quotidien entre 11h et 13h avec retry toutes les 5 min
- **Shift a minuit** — Rotation automatique des couleurs jour/demain
- **Indicateur d'incertitude** — Signale visuellement si les donnees n'ont pas pu etre actualisees
- **Mode debug** — Interaction tactile pour forcer shift/update + serveur local

---

## Build & Flash

### Prerequis

- [PlatformIO](https://platformio.org/) (CLI ou extension VS Code)
- Cable USB-C vers l'ESP32-S3

### Compilation

```bash
# Mode release (API production)
pio run -e release

# Mode debug (serveur local + interactions tactiles)
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

Au premier demarrage (ou si le reseau sauvegarde est indisponible) :

1. L'ESP32 cree un point d'acces **`ESP32_Config`**
2. Connectez-vous a ce reseau depuis un telephone/PC
3. Ouvrez **`192.168.4.1`** dans un navigateur
4. Selectionnez votre reseau WiFi et entrez le mot de passe
5. L'appareil redemarrera et se connectera automatiquement

---

## Dependances

| Librairie                              | Version | Usage                    |
|----------------------------------------|---------|--------------------------|
| [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | 2.5.31  | Pilote ecran TFT        |
| [WiFiManager](https://github.com/tzapu/WiFiManager) | latest  | Portail captif WiFi     |

---

## Structure des donnees API

L'appareil interroge une API XML qui retourne :

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

---

## Licence

Projet personnel — usage libre.
