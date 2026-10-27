# Coding
- La branche `master` est la branche stable
- La branche `dev` est pour le dévellopement
- Tout changement doit faire l’objet d’une [Pull Request](https://docs.github.com/fr/pull-requests/reference/pull-requests)
    - Code review / validation par l’autre systematique
    - Ajout de tests si necessaire
    - CI vert avant de merge
    - merge dans dev sinon main pour hotfix
- Utilisation des conventional commits pour faciliter la review : [conventionalcommits.com](https://www.conventionalcommits.org/en/v1.0.0/)

## Conventions de style

### Nommage

- Classes et structures : `UpperCamelCase` (`MaClasse`).
- Fonctions, méthodes, variables et paramètres : `snake_case` (`ma_fonction`, `ma_variable`).

### Formatage

- Indenter uniquement avec des tabulations, d’une largeur de 4 colonnes.
- Placer les accolades ouvrantes sur une nouvelle ligne, sauf pour les fonctions courtes écrites sur une seule ligne.
- Placer le symbole `*` d’un pointeur à droite : `int *ptr`.
- Placer le symbole `&` d’une référence à gauche : `int& ref`.
- Ne pas laisser de ligne vide au début d’un bloc.

# Utilisation de la POO
- Utilisation des classes de base et abstraites pour les interfaces / contrat API
- Usage des design patterns si possible

# CI
- est trigger pour chaque PR et chaque merge dans `master`
- Effectue :
    - linting
    - compilation
    - run des tests
