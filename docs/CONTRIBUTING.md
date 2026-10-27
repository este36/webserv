# Coding
- La branche `master` est la branche stable
- La branche `dev` est pour le dévellopement
- Tout changement doit faire l’objet d’une [Pull Request](https://docs.github.com/fr/pull-requests/reference/pull-requests)
    - Code review / validation par l’autre systematique
    - Ajout de tests si necessaire
    - CI vert avant de merge
    - merge dans dev sinon main pour hotfix
- Utilisation des conventional commits pour faciliter la review : [conventionalcommits.com](https://www.conventionalcommits.org/en/v1.0.0/)

# Utilisation de la POO
- Utilisation des classes de base et abstraites pour les interfaces / contrat API
- Usage des design patterns si possible

# CI
- est trigger pour chaque PR et chaque merge dans `master`
- Effectue :
    - linting
    - compilation
    - run des tests
