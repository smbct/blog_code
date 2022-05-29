import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import Dataset
from torchvision.transforms import ToTensor

import chess

from model import ChessModel

import utils


# Dataset holder
class ChessDataset(Dataset):


    #---------------------------------------------------------------------------
    def __init__(self, dataframe, training = True):

        self._dataframe = dataframe
        self.training = training
        self.transform = ToTensor()

        self.len = 0

        self.samples = []
        self.decoded_samples = []

        self.create()

        return


    #---------------------------------------------------------------------------
    def create(self):

        # input: 1 8*8 board per piece type + 1 board for the piece to play -> 12*8*8 + 8*8 = 832
        # output: 1 board 8*8 with the possible positions of the piece played

        board = chess.Board()

        data, decoded = self.create_move_data(board)

        self.samples += data
        self.decoded_samples += decoded

        for df_index in self._dataframe.index:

            moves = self._dataframe.loc[df_index]['moves']
            moves = moves.split(' ')

            if len(moves) > 0:
                board.reset()
                board.push_san(moves[0])
                ind_move = 1
                while ind_move < len(moves):
                    board.push_san(moves[ind_move])

                    data, decoded = self.create_move_data(board)

                    self.samples += data
                    self.decoded_samples += decoded

                    ind_move += 1

        return


    #---------------------------------------------------------------------------
    def create_move_data(self, board):

        data = []

        decoded = []

        input = utils.board_square_to_tensor(board)

        # fetch all possible moves from the current board
        processed_moves = utils.moves_from_rules(board)


        # print(processed_moves)

        for origin in processed_moves:

            input_move = input.detach().clone()
            input_move[0][12][origin[1]][origin[0]] = 1.0

            output = torch.zeros([1, 8, 8], dtype=torch.float32)
            for dest in processed_moves[origin]:
                output[0][dest[1]][dest[0]] = 1.0

            data.append((input_move, torch.flatten(output)))

            decoded.append((board.fen(), origin, processed_moves[origin]))

        return data, decoded

    #---------------------------------------------------------------------------
    def get_decoded(self, idx):
        return self.decoded_samples[idx]


    #---------------------------------------------------------------------------
    def __len__(self):
        return len(self.samples)


    #---------------------------------------------------------------------------
    def __getitem__(self, idx):

        return self.samples[idx]
