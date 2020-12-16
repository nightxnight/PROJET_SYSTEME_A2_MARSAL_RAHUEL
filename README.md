# PROJET_SYSTEME_A2_MARSAL_RAHUEL

Réaliser une application comportant deux applications : 

Contraites : 
100 places
L'application doit pouvoir les problèmes d'accès concurentielle

Côté serveur : 
Objectif : Gérer la réservation à distance de billets d'un concert.

Il doit pouvoir gérer plusieurs connexion : création de processus fils / un thread qui gère un client
Il génèrera pour une réservation, un numéro de dossier à 10 chiffres.

Côté client : 
Objectif : Effectuer une réservation en se connectant au serveur.

Doit pouvoir consulter les places disponibles
Pour effectuer une reservation, il faut fournir un nom et un prénom.
Peut annuler une réservation en fournissant son nom et numéro de dossier


Modalités : 
Livrer les programmes sources accompagné d'une documentation. 
DeadLine : ?

Cas d'utilisation, réservation d'un billet : 
  Le client se connecte au serveur.
  Le serveur lui envoie la liste des concerts.
  Le client choisis le concert qu'il souhaite réserver.
  Le serveur lui envoie la liste des places.
  
SélectionPlace:
  Le client choisis sélectionne une place.
  Le client répond face à un message de confirmation.
  Si non -> SélectionPlace

  Le client il envoie le numéro de place qu'il souhaite réserver.
  Le serveur vérifie si elle est bien toujours disponible.
  Si non -> SélectionPlace
  
  Le serveur bloque la place.
  Le serveur informe les autres fils que la place est bloqué.
    Les autres clients recoivent les places actualisé.
  
FournirInformation:
  Le serveur attend les informations.
  Le client fournit des informations.
  Le serveur vérifie qu'elles sont correctes.
  Si non -> FournirInformation
  
  Le serveur enregistre la réservation.
  Le serveur informe les autres fils que la place est réservé.
    Les autres clients recoivent les places actualisé.
  Le serveur envoie un numéro de dossier.
  Le client recoit ce numéro de dossier.

Fin du cas d'utilisation
  
Cas d'annulation d'une réservation :

  Le client fait une demande au serveur pour annuler sa réservation.

FournirNumeroDoss:  
  Le serveur attend un numéro de dossier.
  Le client lui fournit un numéro.
  Le serveur vérifie si il trouve un fichier concernant ce numéro de dossier.
  Si non -> FournirNumeroDoss
  
FournirNom:
  Le serveur demande le nom du client afin de prouver son identité.
  Le client lui fournit son nom.
  Le serveur vérifie si le nom correspondant à ce que contient le fichier.
  Si non -> FournirNom
  
  Le serveur demande au client une confirmation
  Si oui, la reservation est annulé et la place est débloqué
  

Réalisation dans un premier temps : 

- Connecté un client au serveur via TCP/IP
- Envoyé au client une liste de place
- Envoyé au serveur une demande de réservation
- Enregistrement de cette réservation
- Annulation d'une réservation

- Mise en place du parallélisme 
- Gérer l'accès concurentielle
- Rajouter la possibilité de choisir son concert 
- Actualisation en direct des places 

  
  
