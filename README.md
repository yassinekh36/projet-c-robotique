# Planificateur de trajectoire avec des priorités

Projet de programmation en C à l’École nationale des sciences de l’informatique (ENSI). Le programme simule un robot qui exécute un trajet, détecte des obstacles et donne la priorité aux manœuvres d’urgence. Il s’exécute dans un terminal et consigne les actions dans un journal.

## Compiler et lancer

Le programme utilise le standard C11. Depuis le dossier qui contient `main.c` et `commande.h`, compilez avec GCC.

```bash
gcc -std=c11 -Wall -Wextra -pedantic 1.c -o robot
```

Lancez la simulation avec un capteur aléatoire.

```bash
./robot
```

Lancez le mode de démonstration pour déclencher un obstacle à un moment connu.

```bash
./robot --demo
```

Si le fichier principal s’appelle `main.c` dans votre copie, utilisez ce nom à la place de `1.c` dans la commande de compilation.

## Organisation des fichiers

| Fichier             | Rôle                                                                                         |
| ------------------- | -------------------------------------------------------------------------------------------- |
| `commande.h`        | Définit les types `Mouvement`, `Priorite` et `Commande`.                                     |
| main.c              | Contient les files chaînées, la simulation du capteur, l’exécution et l’écriture du journal. |
| `journal_robot.txt` | Fichier généré par le programme pour conserver les actions exécutées.                        |

## Les six tâches

1. **Structure des commandes.** Une `Commande` contient un mouvement, une durée en secondes, une vitesse et une priorité (`NORMALE` ou `URGENTE`). Les mouvements possibles sont `AVANCER`, `RECULER`, `TOURNER_GAUCHE`, `TOURNER_DROITE` et `ARRET_URGENCE`.
2. **Double file.** Les commandes normales et urgentes sont stockées dans deux files chaînées. `enfiler` ajoute à la fin et `defiler` retire au début, selon le principe du premier entré, premier sorti (FIFO). Les fonctions `ajouter_commande_normale` et `ajouter_commande_urgente` placent la commande dans la file de la priorité correspondante.
3. **Simulation du capteur.** En mode normal, le programme effectue un tirage aléatoire avant chaque commande normale. Si un obstacle est détecté, il ajoute `ARRET_URGENCE`, puis les mouvements de contournement dans la file urgente.
4. **Exécution.** `prendre_prochaine_commande` retire d’abord une commande de la file urgente. Quand elle est vide, la fonction prend la première commande normale. La boucle affiche chaque action avec sa priorité, sa durée et sa vitesse. Elle simule les actions sans attendre la durée affichée.
5. **Journal des événements.** Après chaque action, le programme ajoute une ligne à `journal_robot.txt` avec la date, l’heure, la priorité, le mouvement, la durée et la vitesse. Le journal conserve les entrées des exécutions précédentes.
6. **Tâche supplémentaire.** En mode `--demo`, le capteur déclenche un obstacle juste avant la deuxième commande normale. Le scénario est reproductible à chaque lancement.

## Exemple de démonstration

```text
Commande normale : Avancer 2 s, vitesse 30
Obstacle detecte !
COMMANDE URGENTE : Arret d'urgence 0 s, vitesse 0
COMMANDE URGENTE : Tourner a gauche 1 s, vitesse 20
COMMANDE URGENTE : Avancer 2 s, vitesse 20
COMMANDE URGENTE : Tourner a droite 1 s, vitesse 20
Commande normale : Tourner a droite 1 s, vitesse 20
Commande normale : Reculer 2 s, vitesse 25
```

Le robot termine le contournement avant de reprendre les deux commandes normales en attente. Pour consulter les dernières actions enregistrées, utilisez `tail -n 7 journal_robot.txt`.

Le trajet initial est défini dans le tableau `trajet` de la fonction `main`.
