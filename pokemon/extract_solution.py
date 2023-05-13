file = open('sol.txt', 'r')
lines = file.read().splitlines()
lines = [line[2:] for line in lines if line[0] == 'v']
file.close()

file = open('sol.txt', 'w')
for line in lines:
    file.write(line)
    file.write('\n')
file.close()