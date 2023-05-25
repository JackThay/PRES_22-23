# PRES_22-23
Projet de PRES 22-23 Sorbonne Université 

## Encadrant:
- MALOUCH Naceur

## Membres de l'équipe:
- RECHAK Dorian
- THAY Jack
- UNG Thierry
- ZOUHRI Bilal

### Installation
Instruction pour installer le projet:
* Ouvrez un nouvel espace de travail sous OMNET++
* Créez un nouveau projet vide sous dans votre espace de travail
* Copiez tout les fichiers .h, .cc, .ned et .ini et .msg
* Collez tout les fichiers dans votre nouveau projet vide
* Allez dans l'onglet "Project", et cliquez sur "Build Project"
Les fichiers "Packet_M.h" et "Packet_m.cc" vont être compilé (IMPORTANT: vous devez compiler ces fichiers chez vous, les différences de version OMNET++ risquent de provoquer des erreurs)
* Sélectionnez le fichier "network.ned"
* Allez dans l'onglet "Run", et cliquez sur "Run As>OMNeT++ Simulation"

### Modifier l'algorithme chargé dans l'émetteur
* Ouvrez le fichier "network.ned"
* Ligne 21, "IoT0 : Emitter_XXX", remplacez "XXX" par l'algorithme souhaité

### Modifier la topologie réseau pour 1 client et 1 serveur
* Dans le dossier "option routeur 1v1", vous trouverez un fichier "network.ned"
* Copiez ce fichier et remplacez le "network.ned" présent dans le dossier "projet n4"

### BUGS:
- Il arrive que les émetteurs ont des comportements non programmés, comme par exemple CoAP qui tente de "caster" un self-message, cela arrive quand les émetteurs utilisent des algorithmes différents entre eux
