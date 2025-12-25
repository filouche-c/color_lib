# ANSI Color Library

[🇺🇸 Read in English](README.md)

Une bibliothèque C complète pour la manipulation du terminal : gestion des couleurs, styles de texte et positionnement du curseur.

Cette bibliothèque intègre un **Garbage Collector (GC)** interne. Ce mécanisme gère automatiquement l'allocation et la libération des chaînes de formatage, permettant une utilisation fluide des couleurs dynamiques sans fuite de mémoire ni besoin de gestion manuelle (pas de `free` nécessaire).

## Fonctionnalités Principales

- **Gestion des Couleurs** : Support complet des couleurs standards (ANSI 4-bit), étendues (8-bit) et TrueColor (RGB 24-bit).
- **Styles de Texte** : Application facile de styles tels que le gras, l'italique, le souligné, le clignotement, etc.
- **Contrôle du Curseur** : Positionnement absolu et relatif, masquage et affichage du curseur.
- **Gestion Automatisée de la Mémoire** : Les chaînes générées dynamiquement sont nettoyées automatiquement.
- **Stabilité et Signaux** : Interception native des signaux système (`SIGINT`, `SIGSEGV`, etc.) pour garantir que le terminal est toujours réinitialisé dans un état propre, même en cas d'arrêt brusque du programme.

## Intégration

La bibliothèque est conçue pour être intégrée directement dans vos sources.

1. Copiez les fichiers `color_lib.c` et `color_lib.h` à la racine de votre projet ou dans votre dossier de sources.
2. Incluez l'en-tête dans vos fichiers :

```c
   #include "color_lib.h"
````

3. Compilez les fichiers avec votre projet :

```sh
gcc main.c color_lib.c -o mon_application
```

## Guide d'Utilisation

L'initialisation de la bibliothèque est automatique. Vous pouvez utiliser les fonctionnalités dès le début de votre fonction `main`.
*Optionnel : Vous pouvez appeler manuellement `init_color` pour personnaliser le comportement (voir la section **Initialisation Avancée**).*

### 1. Styles Statiques

Pour les couleurs et styles standards, utilisez les structures globales `Fore` (premier plan), `Back` (arrière-plan) et `Style`.

```c
printf("%sMessage d'erreur en rouge%s\n", Fore.RED, Style.RESET);
printf("%s%sTexte blanc sur fond bleu%s\n", Back.BLUE, Fore.WHITE, Style.RESET);
printf("%sTexte en gras et souligné%s\n", Style.BOLD, Style.UNDERLINE);

// N'oubliez pas de réinitialiser le style à la fin
```

> ⚠️ `Style.RESET_ALL` videra également le Garbage Collector.

### 2. Couleurs Dynamiques (TrueColor & 8-bit)

Utilisez les fonctions génératrices pour des couleurs spécifiques (ex: RGB).
**Important :** Vous n'avez pas besoin de libérer les chaînes retournées ; le Garbage Collector s'en charge.

```c
// Texte en orange personnalisé (RGB: 255, 165, 0)
printf("%sTitre en couleur personnalisée%s\n", fore_color24(255, 165, 0), Style.RESET);

// Arrière-plan utilisant la palette 8-bit (Index 200)
printf("%sFond index 200%s\n", back_color8(200), Style.RESET);
```

### 3. Manipulation du Curseur

Idéal pour créer des interfaces textuelles (TUI) ou des animations simples.

```c
print(Cursor.HIDE);             // Masque le curseur pour une interface propre
print(cursor_cup(10, 20));      // Place le curseur ligne 10, colonne 20
printf("Menu Principal");
print(Screen.CLEAR);            // Efface l'écran entier
```

### 4. Gestion de la Mémoire dans les Boucles

Si votre programme s'exécute dans une boucle infinie (comme un jeu ou un serveur de rendu), la mémoire allouée pour les couleurs dynamiques doit être nettoyée périodiquement.
Utilisez `gc_reset()`. Cette fonction effectue deux actions :

1. Retourne la chaîne de réinitialisation ANSI (`\033[0m`)
2. Marque la mémoire utilisée lors du cycle précédent pour suppression

```c
while (running) {
    // Génération d'une couleur aléatoire à chaque tour
    char *color = fore_color24(rand() % 255, 0, 0);

    // gc_reset() ferme le style et nettoie la mémoire du tour précédent
    printf("%sScore actuel : %d%s\n", color, score, gc_reset());

    sleep(1);
}
```

## Référence API

### Structures Globales

| Structure | Description                             | Exemples d'utilisation                      |
| --------- | --------------------------------------- | ------------------------------------------- |
| `Fore`    | Couleurs de texte (16 couleurs de base) | `Fore.RED`, `Fore.BRIGHT_CYAN`              |
| `Back`    | Couleurs d'arrière-plan                 | `Back.BLACK`, `Back.YELLOW`                 |
| `Style`   | Modificateurs de texte                  | `Style.BOLD`, `Style.DIM`, `Style.RESET`    |
| `Cursor`  | Actions sur le curseur                  | `Cursor.HIDE`, `Cursor.SHOW`, `Cursor.HOME` |
| `Screen`  | Actions sur l'écran                     | `Screen.CLEAR`, `Screen.LINE_ERASE_ALL`     |

### Fonctions Utilitaires

| Fonction                | Description                                                     |
| ----------------------- | --------------------------------------------------------------- |
| `cursor_cup(row, col)`  | Déplace le curseur à la position spécifiée (Ligne, Colonne).    |
| `fore_color24(r, g, b)` | Génère une couleur de texte RGB (TrueColor).                    |
| `back_color24(r, g, b)` | Génère une couleur de fond RGB (TrueColor).                     |
| `gc_reset()`            | Réinitialise le style et nettoie la mémoire du cycle précédent. |

*D'autres fonctions sont disponibles dans `color_lib.h`.*

## Notes Techniques

* **Cycle de vie** : La bibliothèque utilise `atexit` pour garantir que le terminal est restauré (curseur visible, couleurs par défaut) lorsque le programme se termine normalement.
* **Gestion des erreurs** : Un gestionnaire de signaux interne intercepte les interruptions (Ctrl+C) ou les crashs (Segfault) pour restaurer l'état du terminal avant de quitter.
* **Multi-threading** : Le Garbage Collector utilise des ressources globales sans mutex. Pour les applications multi-threadées, restreignez l'utilisation des couleurs dynamiques au thread principal ou implémentez votre propre mécanisme de verrouillage.

## Initialisation Avancée

Par défaut, la bibliothèque s'initialise avant l'exécution du `main`. Pour personnaliser cela, vous pouvez appeler `init_color` manuellement.

**Équivalent de l'appel par défaut :**

```c
init_color(NULL, 1, 1, 1, COLOR_FLAG_INIT_DEFAULT);
```

### Paramètres

1. **`escape_char`** (`char*`)
   Le caractère d'échappement à utiliser.

   * Par défaut `\033` si `NULL`
   * Alternatives : `\x1b` ou `\e`

2. **`auto_show_cursor`** (`bool` / `int`)

   * `1` (true) : Envoie `[?25h` à la sortie pour s'assurer que le curseur est visible
   * `0` (false) : L'état du curseur n'est pas modifié à la sortie

3. **`auto_clean`** (`bool` / `int`)

   * `1` (true) : Appelle `gc_clean_all` à la sortie (`atexit`)
   * `0` (false) : Vous devez manuellement libérer (`free`) la mémoire pour éviter les fuites

4. **`catch_signals`** (`bool` / `int`)

   * `1` (true) : Intercepte les signaux de crash et envoie `\033[0m` pour réinitialiser les couleurs
   * `0` (false) : Ne fait rien en cas de crash

5. **`flags`** (`int`)

   * Définit quelles structures sont initialisées/accessibles
   * Voir le fichier header pour la liste des flags
     Utilisez `COLOR_FLAG_INIT_DEFAULT` pour un usage standard

### Test de Compatibilité

Vous pouvez exécuter `color_support_test()` pour vérifier la compatibilité des fonctionnalités.
Elle ne prend aucun argument et ne renvoie rien ; vérifiez la sortie du terminal pour voir quelles fonctionnalités s'affichent correctement sur votre système.