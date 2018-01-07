using JuMP
using GLPK,GLPKMathProgInterface

using Cbc
using CPLEX

include("parser.jl")

# model = Model(solver=GLPKSolverMIP())
# model = Model(solver=CbcSolver())
model = Model(solver=CplexSolver())

sommeL = sum(inst.rect[i][1] for i in 1:inst.nb)*2
sommeH = sum(inst.rect[i][2] for i in 1:inst.nb)*2

##############################################################
####################### Variables ############################
##############################################################

# x[i,j] = rectangle i, coordonnée j (1 ou 2)

@variable(model, x[i in 1:inst.nb, j in 1:2] >= 0)

# ox : à gauche ou à droite d'un autre rectangle
# ox = 0 : à gauche
# oy : en haut ou en bas d'un autre rectangle
# oy = 0 : en haut
@variable(model, ox[i in 1:inst.nb, j in (i+1):inst.nb], Bin)
@variable(model, oy[i in 1:inst.nb, j in (i+1):inst.nb], Bin)

# largeur et hauteur max
@variable(model, lmax >= 0)
@variable(model, hmax >= 0)

##############################################################
####################### Contraintes ##########################
##############################################################

# les rectangles ne se chevauchent pas

for i in 1:inst.nb
    for j in i+1:inst.nb

        # en abscisse
        @constraint(model, x[i,1] <= x[j,1] - inst.rect[i][1] + (1-ox[i,j])*sommeL)
        @constraint(model, x[i,1] >= x[j,1] + inst.rect[j][1] - ox[i,j]*sommeL)

        # en ordonnée
        @constraint(model, x[i,2] <= x[j,2] - inst.rect[i][2] + (1-oy[i,j])*sommeH)
        @constraint(model, x[i,2] >= x[j,2] + inst.rect[j][2] - oy[i,j]*sommeH)

    end
end

# cadre à minimiser

@constraint(model, [i in 1:inst.nb], lmax >= x[i,1]+inst.rect[i][1])
@constraint(model, [i in 1:inst.nb], hmax >= x[i,2]+inst.rect[i][2])

##############################################################
##################### Fonction objectif ######################
##############################################################

@objective(model, Min, hmax+lmax)

@time solve(model)

X = getvalue(x)
