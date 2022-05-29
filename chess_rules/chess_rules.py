import pandas as pd
import numpy as np
from matplotlib import pyplot as plt
from matplotlib.widgets import Slider, Button
import matplotlib.patches as mpatches

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



print('hello')

games = pd.read_csv('games.csv', index_col=0)
games.drop_duplicates(inplace=True, subset=['moves']) # duplicated rows
games.sample(frac=1)

# select a subset of the games
# games = games.iloc[:40]
games = games.iloc[:70]

# print(games.head())
#
# print(games[:10])

game_index = 3

print(games.iloc[game_index]['moves'])
moves = games.iloc[game_index]['moves'].split(' ')
move_index = 0






# GUI setup
window = gui(moves)
window.update_board()


board = chess.Board()





# dataset creation

# training_set = 80%
# test_set = 20%
stopping_index = int(games.shape[0]*0.7)
training_df = games.iloc[:stopping_index]
test_df = games.iloc[stopping_index:]

# print(training_df)
# print(test_df)

training_dataset = ChessDataset(training_df)
test_dataset = ChessDataset(test_df)


print('size of the training set: ', len(training_dataset))

batch_size = 30
train_dataloader = DataLoader(training_dataset, batch_size, shuffle=True)
test_dataloader = DataLoader(test_dataset, batch_size, shuffle=True)

device = "cuda" if torch.cuda.is_available() else "cpu"
# device='cpu'
print(f"Using {device} device")


model = ChessModel().to(device)
model.load_state_dict(torch.load('chess_model.pth'))



# model inference
# model_inference.train_model(device, model, train_dataloader, test_dataloader)
# torch.save(model.state_dict(), 'chess_model.pth')



# model.eval()
# it = iter(train_dataloader)
# data, label = next(it)
# data = data.to(device)
# print(data)
# print(label)
# print(model(data))




model.eval()

tensor = utils.board_square_to_tensor(board)
print(tensor)



data, decoded = training_dataset.create_move_data(board)

print(data[0])
print(data[0][1].reshape((8,8)))
print(decoded[0])

moves = utils.moves_from_rules(board)
print(moves)

mv2 = utils.moves_from_model(device, model, board, False)
print('\n\n')
mv = utils.moves_from_rules(board)
fig, ax = plt.subplots()
gboard.draw_board(ax, board)
gboard.draw_moves(ax, mv2)



plt.show()
