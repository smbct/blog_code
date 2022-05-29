
import numpy as np

import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import Dataset
from torchvision.transforms import ToTensor

import chess

#---------------------------------------------------------------------------
def square_to_coord(square):
    name = chess.square_name(square)
    return (int(ord(name[0])-ord('a')), int(name[1])-1)


#------------------------------------------------------------------------------
def layer_to_piece(layer):
    return _layer_to_piece[layer]

#------------------------------------------------------------------------------
def piece_to_layer(piece):
    return _piece_to_layer[piece]


#------------------------------------------------------------------------------
# model encoding constants
_layer_to_piece = ['p', 'b', 'n', 'r', 'k', 'q', 'P', 'B', 'N', 'R', 'K', 'Q']
_piece_to_layer = {}
for ind in range(len(_layer_to_piece)):
    _piece_to_layer[_layer_to_piece[ind]] = ind



#---------------------------------------------------------------------------
def board_square_to_tensor(board):

    input = torch.zeros([1,13, 8, 8], dtype=torch.float32)

    for i in range(8):
        for j in range(8):
            piece = board.piece_at(i*8+j)
            if not piece is None:
                symbol = piece.symbol()
                layer_id = piece_to_layer(symbol)
                input[0][layer_id][i][j] = 1.0

    return input




#---------------------------------------------------------------------------
def moves_from_model(device, model, board, color):

    moves = {}

    for i in range(8):
        for j in range(8):
            piece = board.piece_at(i*8+j)

            if not piece is None:
                symbol = piece.symbol()
                if (ord(symbol) >= ord('a')) == color:
                    x = board_square_to_tensor(board).to(device)
                    x = x[None,:]
                    # print(x, x.shape)
                    x[0][0][12][j][i] = 1.

                    print('test', (i,j), x, x.shape)

                    y = model(x).detach()

                    print(y.reshape((8,8)))

                    for k in range(8):
                        for l in range(8):
                            if y[0][k*8+l] >= 0.5:
                                if (i,j) in moves:
                                    moves[(i,j)].append((l,k))
                                else:
                                    moves[(i,j)] = [(l,k)]
    return moves

#---------------------------------------------------------------------------
def moves_from_rules(board):

    # fetch all possible moves from the current board

    moves = {}

    for move in board.legal_moves:

        first = square_to_coord(move.from_square)
        second = square_to_coord(move.to_square)

        if first in moves:
            moves[first].append(second)
        else:
            moves[first] = [second]

    return moves


#-------------------------------------------------------------------------------
def go_to_move(board, moves, move_index):

    # go to one specific configuration of a particular game

    board.reset()
    for i in range(move_index):
        board.push_san(moves[i])

    return
