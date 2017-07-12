#!/usr/bin/env python

class Sommet:

    def __init__(self, num):
        self.num = num
        self.suc = []

    def ajouter_suc(self, num):
        self.suc.append(num)

fichier = open('graphe.dat')

lignes = fichier.read().splitlines()

fichier.close()

nbNoms = int(lignes[0])

info = [False]*nbNoms
noms = []
num = {}

# lecture des noms, association d'un numero a un nom
for i in range(1, nbNoms):
    ligne = lignes[i].split(' ')
    noms.append(ligne[0])
    num[ligne[0]] = i-1
    if(ligne[1] == 'i'):
        info[i-1] = True

#print noms
#print num

liste_suc = []
depart = Sommet(-1)

# constrution du graphe par liste des successeurs
for i in range(nbNoms, len(lignes)-1):
    noms_suc = lignes[i].split(' ')

    sommet = Sommet(num[noms_suc[0]])

    # ajout des successeurs
    for i in range(1, len(noms_suc)-1):
        sommet.ajouter_suc(num[noms_suc[i-1]])
    liste_suc.append(sommet)

    if noms[sommet.num] == 'Moi':
        depart = sommet


# calcul de l'info la plus proche
file_sommet = []
file_sommet.append(depart)
visite = [False]*nbNoms
prec = [None]*nbNoms # liste des sommets du chemin parcouru
dernier = None
continuer = True

# tant que l'info n'a pas ete trouvee, la recherche continue
while continuer and len(file_sommet) > 0:

    # retrait du premier de la file
    tete = file_sommet.pop()

    # si l'info a ete trouvee, l'algo s'arrete
    if info[tete.num]:
        continuer = False
        dernier = tete.num
        print("l'info a ete trouvee")
    else: # sinon les successeurs du sommet sont ajoute a la file s'ils n'ont pas ete visites
        for suc in tete.suc:
            if not visite[suc]:
                file_sommet.insert(0, liste_suc[suc])
                visite[suc] = True
                prec[suc] = tete.num



# affichage du chemin du l'information
num = dernier
nom = noms[num]
chemin = []
while nom != 'Moi':
    chemin.append(nom)
    num = prec[num]
    nom = noms[num]

chemin.append('Moi')
chemin.reverse()

print('chemin : ')
for nom in chemin:
    print nom
