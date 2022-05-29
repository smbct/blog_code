
import numpy as np

import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import Dataset
from torchvision.transforms import ToTensor

import chess

#---------------------------------------------------------------------------
def square_to_coord(square):

    # (0,0) is the upper left corner of the board
    # this is more practical to write into the tensors

    # first coord is the column, second coord is the row

    name = chess.square_name(square)
    return (int(ord(name[0])-ord('a')), 8-int(name[1]))



#------------------------------------------------------------------------------
# model encoding constants
_layer_to_piece = ['p', 'b', 'n', 'r', 'k', 'q', 'P', 'B', 'N', 'R', 'K', 'Q']
_piece_to_layer = {}
for ind in range(len(_layer_to_piece)):
    _piece_to_layer[_layer_to_piece[ind]] = ind

# first layers: black pieces
# followinf layers: white pieces

#------------------------------------------------------------------------------
def layer_to_piece(layer):
    return _layer_to_piece[layer]

#------------------------------------------------------------------------------
def piece_to_layer(piece):
    return _piece_to_layer[piece]

#---------------------------------------------------------------------------
def board_square_to_tensor(board):

    input = torch.zeros([1,13, 8, 8], dtype=torch.float32)

    for i in range(8): # rows
        for j in range(8): # columns
            piece = board.piece_at((7-i)*8+j)
            if not piece is None:
                symbol = piece.symbol()
                layer_id = piece_to_layer(symbol)
                input[0][layer_id][i][j] = 1.0

    return input




#---------------------------------------------------------------------------
def moves_from_model(device, model, board, color):

    # False -> white moves
    # True -> black moves

    moves = {}

    for i in range(8): # rows
        for j in range(8): # columns
            piece = board.piece_at((7-i)*8+j)

            if not piece is None:
                symbol = piece.symbol()

                if (ord(symbol) >= ord('a')) == color:

                    x = board_square_to_tensor(board).to(device)
                    x = x[None,:]
                    # print(x, x.shape)
                    x[0][0][12][i][j] = 1.

                    # print('test', (j,i), x, x.shape)

                    y = model(x).detach()

                    y = y.reshape((8,8))
                    # print(y)

                    for k in range(8): # rows
                        for l in range(8): # columns
                            if y[k][l] >= 0.5: # move predicted
                                if (j,i) in moves:
                                    moves[(j,i)].append((l,k))
                                else:
                                    moves[(j,i)] = [(l,k)]
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
