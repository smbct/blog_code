import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import Dataset
from torchvision.transforms import ToTensor

import chess

from interface import gui
from interface import gboard

from dataset import ChessDataset

from model import ChessModel



import utils
import model_inference

import random


#-------------------------------------------------------------------------------
# generate board configurations from multiple games
def create_boards_dataset(games):

    # generate all the possible boards from the games
    boards_str = []
    board = chess.Board()


    print('creating the dataset...')
    for game_index in games.index:
        moves = games.loc[game_index]['moves'].split(' ')
        board.reset()
        for move in moves:
            board.push_san(move)
            boards_str.append(board.fen())

    boards_str = list(set(boards_str))

    random.shuffle(boards_str)

    return boards_str


#-------------------------------------------------------------------------------
# plot a comparison of the board with moves from the rules moves predicted by the model
def plot_comparison(model, moves):

    fig, axs = plt.subplots(3, 2)

    selected_indexes = list(range(len(moves)))
    random.shuffle(selected_indexes)
    selected_indexes = selected_indexes[:3]

    board = chess.Board()
    ind2 = 0
    for ind in range(len(moves)):
        board.push_san(moves[ind])
        if ind in selected_indexes:
            mv = utils.moves_from_rules(board)
            mv2 = utils.moves_from_model(device, model, board, ind%2==0)

            gboard.draw_board(axs[ind2][0], board)
            gboard.draw_moves(axs[ind2][0], mv)

            gboard.draw_board(axs[ind2][1], board)
            gboard.draw_moves(axs[ind2][1], mv2, True)

            ind2 += 1

    return

#-------------------------------------------------------------------------------
# create the dataset and train the model
def training(games, device, model):

    # games = games.iloc[:5]

    boards_str = create_boards_dataset(games)

    training_boards = boards_str[:8000]
    test_boards = boards_str[3000:5000]

    ################################################################################
    # dataset creation
    training_dataset = ChessDataset(training_boards)
    test_dataset = ChessDataset(test_boards)

    print('size of the training set: ', len(training_dataset))
    print('size of the test set: ', len(test_dataset))

    batch_size = 150
    train_dataloader = DataLoader(training_dataset, batch_size, shuffle=True)
    test_dataloader = DataLoader(test_dataset, batch_size, shuffle=True)

    ################################################################################
    # model inference
    model_inference.train_model(device, model, train_dataloader, test_dataloader)
    torch.save(model.state_dict(), 'chess_model.pth')



################################################################################
# generation of board configurations

games = pd.read_csv('games.csv', index_col=0)
games.drop_duplicates(inplace=True, subset=['moves']) # duplicated rows
games.sample(frac=1)


device = "cuda" if torch.cuda.is_available() else "cpu"
print(f"Using {device} device")

model = ChessModel().to(device)



################################################################################
# training the model
# training(games, device, model)
# 97.289479% accuracy



################################################################################
# illustration of the trained model
model.load_state_dict(torch.load('chess_model.pth'))
model.eval()

game_index = 4
moves = games.iloc[game_index]['moves'].split(' ')

# generate a big graph
plot_comparison(model, moves)

# GUI setup
window = gui(moves, model)
window.update_board()

plt.show()
