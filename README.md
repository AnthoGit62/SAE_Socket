# 🎮 T3N Socket V3

> **Jeu de Morpion multijoueur en réseau avec système de spectateurs**

[![Projet Universitaire](https://img.shields.io/badge/Projet-Universitaire-blue.svg)](https://github.com)
[![Date](https://img.shields.io/badge/Date-Janvier%202025-green.svg)](https://github.com)
[![C](https://img.shields.io/badge/Language-C-orange.svg)](https://github.com)

## 👥 Équipe de développement

- **SEYS Anthony** [@Anthony](https://github.com/AnthoGit62)
- **PREVOST Louis** [@Louis Prévost](https://github.com/louisprvst)
- **LIM Julie** 

---

## 📋 Table des matières

- [Description](#-description)
- [Fonctionnalités](#-fonctionnalités)
- [Prérequis](#-prérequis)
- [Installation](#-installation)
- [Utilisation](#-utilisation)
- [Règles du jeu](#-règles-du-jeu)

---

## 🎯 Description

T3N Socket V3 est une implémentation en C d'un jeu de Morpion multijoueur utilisant les sockets réseau. Le projet permet à deux joueurs de s'affronter en temps réel, tandis que jusqu'à 9 spectateurs peuvent observer la partie.

## ✨ Fonctionnalités

- 🎮 **Mode multijoueur** : Deux joueurs en simultané
- 👀 **Système de spectateurs** : Jusqu'à 9 spectateurs par partie
- 🔄 **Parties automatiques** : Une nouvelle partie se lance dès la fin de la précédente
- 🌐 **Architecture client-serveur** : Communication via sockets TCP/IP
- ⚡ **Temps réel** : Synchronisation instantanée entre tous les participants

## 🔧 Prérequis

- **Compilateur GCC** installé sur votre système
- **Système d'exploitation** : Linux, macOS ou Windows (avec un environnement de compilation C)
- Connaissances de base en utilisation du terminal

## 🚀 Installation

### 1️⃣ Compiler le projet

#### Sous Linux / macOS

Ouvrez un terminal et exécutez :

```bash
gcc serveur.c -o serveur
gcc client.c -o client
```

#### Sous Windows

Dans un terminal compatible (MinGW, WSL, etc.) :

```bash
gcc serveur.c -o serveur.exe
gcc client.c -o client.exe
```

---

## 🎲 Utilisation

### 1️⃣ Démarrer le serveur

Dans un premier terminal :

```bash
./serveur
```

Le serveur attend maintenant les connexions des joueurs.

### 2️⃣ Connecter les joueurs

Dans deux terminaux différents, lancez les clients :

**Terminal 2 (Joueur 1)** :
```bash
./client
```

**Terminal 3 (Joueur 2)** :
```bash
./client
```

### 3️⃣ Ajouter des spectateurs (optionnel)

Une fois les deux joueurs connectés, vous disposez de **5 secondes** pour connecter jusqu'à 9 spectateurs.

Dans des terminaux supplémentaires :
```bash
./client
```

### 4️⃣ Quitter le jeu

Pour fermer proprement un client ou le serveur :
```
Ctrl + C
```

---

## 📖 Règles du jeu

### Plateau de jeu

Les cases sont numérotées de 1 à 9 :

```
 1 | 2 | 3
-----------
 4 | 5 | 6
-----------
 7 | 8 | 9
```

### Symboles

- **Joueur 1** : `X`
- **Joueur 2** : `O`

### Objectif

Le premier joueur à aligner **3 symboles identiques** (horizontalement, verticalement ou en diagonale) remporte la partie.

### Combinaisons gagnantes

Il existe **8 combinaisons** possibles :
- **Lignes** : 1-2-3, 4-5-6, 7-8-9
- **Colonnes** : 1-4-7, 2-5-8, 3-6-9
- **Diagonales** : 1-5-9, 3-5-7

### Déroulement

1. Les joueurs jouent à tour de rôle
2. Entrez le numéro de la case où vous souhaitez placer votre symbole
3. La partie se termine dès qu'un joueur gagne ou que le plateau est plein (match nul)
4. Une nouvelle partie démarre automatiquement

---

## 📝 Notes techniques

- **Protocole** : TCP/IP via sockets BSD
- **Architecture** : Client-Serveur
- **Langage** : C standard (C99)
- **Limite de connexions** : 2 joueurs + 9 spectateurs maximum

---

## 📄 Licence

Projet universitaire - Janvier 2025

---

## 🤝 Contribution

Ce projet est un travail universitaire. Pour toute question ou suggestion, contactez les membres de l'équipe.

---

