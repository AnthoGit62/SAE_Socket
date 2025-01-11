# DOC du projet T3N Socket V2

#### Par PREVOST Louis & SEYS Anthony & LIM Julie

#### Projet Universitaire - Janvier 2025


### 1. Compiler le jeu :

Si vous ne disposez pas des fichiers exécutables, vous pouvez les générer ainsi :

Dans un terminal Linux :

```bash
gcc serveur.c -o serveur
gcc client.c -o client
```

Dans un compilateur dédié sous Windows :

```bash
gcc serveur.c -o serveur
gcc client.c -o client
```

### 2. Lancer le jeu :

Exécutez d'abord le serveur dans un terminal ou un compilateur dédié :

```bash
./serveur
```

Ensuite, exécutez les clients et les spectateurs dans 2 terminaux différent :

```bash
./client
```

### 3. Jouer :

Le jeu se lance directement sur une partie.
À la fin de votre partie, une nouvelle se lance automatiquement.
Pour quitter, il vous suffit d'utiliser Ctrl + C.

Les cases sont numérotées de cette manière :

```markdown
 1 | 2 | 3
------------
 4 | 5 | 6
------------
 7 | 8 | 9
```

Le premier joueurs utilise le caractère X et le second le O.
Une fois les deux joueurs connectés, vous disposez de 5secondes pour connecter jusqu'à 9 spectateurs.
Le premier à réaliser l'une des 8 combinaisons gagnantes remporte la partie.