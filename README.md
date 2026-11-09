# webserv

## Instructions

### Compiler :

```sh
make
```

### Rebuild complet :

```sh
make re
```

### Lancer les tests :

```sh
make run_tests
```

### Ajouter un nouveau test :
- Créer un fichier `test_X.cpp` dans le dossier `tests` qui contient une fonction `main()`. Si le process retourne 0 le test est considéré comme passé.
-  Mettre à jour la variable `TESTS_SRCS` dans le `Makefile` avec le nouveau fichier.

### Contribuer

Voir [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) pour plus de détails.
