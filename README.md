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
* Ouvrez un nouvel espace de travail
* Créez un nouveau projet vide sous dans votre espace de travail
* Copiez tout les fichiers .h, .cc, .ned et .ini et .msg
* Collez tout les fichiers dans votre nouveau projet vide
* Allez dans l'onglet "Project", et cliquez sur "Build Project"
Les fichiers "Packet_M.h" et "Packet_m.cc" vont être compilé (IMPORTANT: vous devez compiler ces fichiers chez vous, les différences de version OMNET++ risquent de provoquer des erreurs)
* Sélectionnez le fichier "network.ned"
* Allez dans l'onglet "Run", et cliquez sur "Run As>OMNeT++ Simulation"

### TO DO:
A faire, dans l'immédiat:
- Créer dans le routeur un temps de traitements qui correspond à sendingTime = (***Packet->getBitLength() / bandwidth)
- Améliorer le routeur
- Corriger les timeouts CoAP pour le client (j'ai beau regardé, je vois pas où était le problème)
- Recréer (AIMD)
- Adapter le BBR de Thierry a notre architecture de code
- Faire en sorte que bandwidth modifie VRAIMENT la bande passante dans "network.ned"

A faire, dans un avenir proche:
- Mettre que 1 seul serveur
- Mettre les identifiants de paquets dans les "bubbles"
- Limiter les temps à des ms
