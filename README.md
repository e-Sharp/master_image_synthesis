# Projet MIF27

- Tom Montauriol (11916410)
- Willy Jacquet (11806811)

# Fonctionnalités

- Génération du tube
    - Génération procédurale
    - Eclairage des fragments (Blinn-Phong)
    - Texture qui se répète
- Contrôle du joueur
    - Vitesse latérale progressive
    - Accélération et décéleration
    - Modèle personnalisé
    - Animation en fonction de la velocité
- Placement d'obstables et de bonus
    - Placement procédural
    - Animation des obstacles (radiale et axiale)
    - Modèle personnalisé
- Collisions
    - Disparition des bonus et augmentation du score
    - Perte de la partie avec les obstacles
- Animation de fond
    - Effet hypervitesse
    - Réactif avec la vitesse du joueur
- Gestion du score
    - Persistence
    - Table des meilleurs scores

# Compilation et exécution

La compilation nécessite un compilateur C++14.

Le projet est testé et fonctionnel sous Linux.
Les bibliothèques Windows ne sont pas incluses.

Pour lancer le programme, il faut exécuter les commandes suivantes: 
```bash
premake4 gmake
make runner
./bin/runner
```
