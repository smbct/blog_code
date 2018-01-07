#!/usr/bin/env python

print('test')

fic = open('train-images.idx3-ubyte', 'rb')


lu = fic.read(4) # magic number
print(int(lu))

lu = fic.read(4) #nimages


str = fic.read(2) #hauteur
str = fic.read(2) #largeur


fic.close()
