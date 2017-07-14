#!/usr/bin/env python

import random

random.seed()

fname = open('names')

cont = fname.read()

fname.close()

# liste des noms des amis
listName = cont.splitlines()
nbNoms = len(listName)

# creation de la liste adjacence du graph
graph = []
for i in range(0, len(listName)-1):
    graph.append([False] * len(listName))

listNbAmis = [0] * len(listName)

print(listName)

maxCon = 4 # nb d'amis maximal
minCon = 2 # nb d'amis minimal

# ajout d'amis aleatoirement

for i in range(1, 200):
    x = random.randrange(0, nbNoms-1)
    y = random.randrange(0, nbNoms-1)
    if x != y:
        graph[x][y] = True

# affichage du graphe
# for i in range(0, nbNoms-1):
#     for j in range(0, nbNoms-1):
#         if graph[i][j]:
#             print("X", end="")
#         else:
#             print(" ", end="")
#     print("")


# disposition de l'info
info = [False]*nbNoms
for i in range(1, 4):
    num = random.randrange(0, nbNoms-1)
    info[num] = True

# sortie dans un fichier
sortie = open("graphe.dat", "w")

sortie.write(str(nbNoms) + "\n")

for i in range(0, nbNoms-1):
    sortie.write(listName[i])
    if info[i]:
        sortie.write(' i' + '\n')
    else:
        sortie.write(' n' + '\n')

for i in range(0, nbNoms-1):
    sortie.write(listName[i] + ' ')
    for j in range(0, nbNoms-1):
        if graph[i][j]:
            sortie.write(listName[j] + " ")
    sortie.write("\n")

sortie.close()
