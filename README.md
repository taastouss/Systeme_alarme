#  Système de Détection Précoce d’Incendies Forestiers par Satellite

##  Description

FireScan est un système embarqué autonome conçu pour la détection précoce d’incendies forestiers à l’aide d’un capteur infrarouge et d’une communication satellite.

Le projet combine électronique embarquée, transmission satellite et traitement IoT via Node-RED afin de détecter rapidement une anomalie thermique et d’envoyer une alerte en temps réel.


##  Objectifs

- Détection thermique par balayage angulaire  
-  Transmission d’alerte en temps réel via satellite EchoStar  
-  Optimisation énergétique via panneaux solaires  
-  Adaptation saisonnière du système  
- Supervision et alertes intelligentes via Node-RED  

## Architecture du Système

MLX90614 → RAK3172 → EM2050 → Satellite → MQTT → Node-RED → Dashboard + Email

##  Matériel Utilisé

- Carte ApolloEcho (RAK3172 + EM2050)  
- Capteur infrarouge MLX90614  
- Servomoteur 360°  
- Antenne S-Band  
- Panneau solaire  
- Batterie 5V  

## ⚙ Principe de Fonctionnement

1. Le servomoteur balaie l’environnement.
2. Le capteur MLX90614 mesure la température.
3. En cas d’anomalie thermique :
   - Une trame est générée.
   - Elle est transmise au module satellite EM2050.
   - L’alerte est envoyée via satellite.
4. Les données sont reçues par Node-RED.
5. Node-RED analyse et déclenche une alerte si nécessaire.

#  Intégration Node-RED

## Rôle de Node-RED

Node-RED assure :

- Réception des trames via MQTT  
- Décodage Base64 des données satellite  
- Interprétation des événements (ALIVE / SOS / FEU)  
- Validation temporelle intelligente  
- Envoi automatique d’alertes email  
- Supervision via Dashboard  

##  Logique d’Alerte Implémentée

###  FEU
- Température > 32°C  
- Condition maintenue pendant ≥ 3 secondes  
- Un seul mail envoyé par événement confirmé  

###  SOS
- Envoi d’un seul mail par déclenchement  
- Anti-spam grâce à la gestion d’état (context Node-RED)  

---

## Dashboard Node-RED

Le dashboard affiche en temps réel :

-  Température  
-  Angle du servomoteur  
-  État système (ALIVE / SOS / FEU)  
-  Indicateur visuel d’alerte  

Accès local :
http://localhost:1880/ui  

Accès distant possible via tunnel sécurisé (ngrok).

---

##  Optimisation Énergétique

- Activation du satellite uniquement en cas d’alerte  
- Mode veille entre les scans  
- Adaptation saisonnière de la fréquence de balayage  
- Gestion intelligente de la consommation  


##  Améliorations Futures

- Détection multi-capteurs (gaz, fumée)  
- Filtrage thermique avancé  
- Hébergement cloud (AWS / Azure)  
- Cartographie des alertes  
- Déploiement à grande échelle  

##  Technologies Utilisées

- Node-RED  
- MQTT  
- JavaScript  
- Gmail SMTP  
- LoRa  
- Satellite EchoStar  
- MLX90614  


##  Auteurs

Projet réalisé par :

- Hakim Grissi
- Tasnim Kachradi  
- Idris  

---

## 📌 Statut du Projet

Prototype fonctionnel avec détection intelligente, validation temporelle et alertes automatisées.
