# Plus court chemin et graphe

Les programmes de ce dossier accompagnent [ce billet](http://smbct.github.io/2017/07/court-chemin) de blog. Le programme *resol.py* permet de calculer le chemin le plus court dans le graphe *graphe.dat* pour parvenir à un sommet marqué (par exemple contenant une information particulière). Le fichier *names* contient une liste de noms qui constituent les sommets du graphe. Le fichier *graphe.dat* contient un graphe d'amis fait à partir des noms. Ce graphe peut être généré aléatoirement à partir du programme *generation.py*.

## Structure du fichier de graphe

La première ligne du fichier de graphe contient le nombre de sommets (personnes) du graphe. Les lignes suivantes sont les noms, suivi de la lettre *i* si le sommet l'info recherchée, *n* sinon.

Les lignes suivantes indique les amis ou arêtes du graphe. Le premier nom de la ligne est le nom du sommet et les noms suivants sont les noms de ses amis (ou sommets connectés).

## Exécution des programmes

Pour exécuter le programme de génération de graphe, il suffit d'utiliser la commande *python3 generation.py*. Le programme va créer un graphe aléatoirement à partir du ficher *names* et l'enregistrer dans le fichier *graphe.dat*.

Pour exécuter le programme de recherche du plus court chemin, il faut utiliser la commande *python3 resol.py*. Le programme afficher alors le plus court chemin du graphe *graphe.dat*.
