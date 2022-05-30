import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import Dataset
from torchvision.transforms import ToTensor, Lambda

from model import ChessModel

import chess

#---------------------------------------------------------------------------
def train_loop(device, dataloader, model, loss_fn, optimizer):

    size = len(dataloader.dataset)
    for batch, (X, y) in enumerate(dataloader):

        if device == 'cuda':
            X, y = X.to(device), y.to(device)

        pred = model(X)
        loss = loss_fn(pred, y)
        # Backpropagation
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        if batch % 100 == 0:
        # if batch % 1000 == 0:
            loss, current = loss.item(), batch * len(X)
            print(f"loss: {loss:>7f}  [{current:>5d}/{size:>5d}]")

#---------------------------------------------------------------------------
def test_loop(device, dataloader, model, loss_fn, epoch):

    size = len(dataloader.dataset)
    num_batches = len(dataloader)
    test_loss, correct = 0, 0
    with torch.no_grad():

        accuracy = 100.

        for X, y in dataloader:

            if device == 'cuda':
                X, y = X.to(device), y.to(device)

            pred = model(X)
            test_loss += loss_fn(pred, y).item()

            pred_norm = pred.cpu().numpy()

            # computation of the prediction accuracy
            error = 0.
            if (epoch+1) % 2 == 0:
                for ind_sample in range(len(pred_norm)):
                    sub_error = 0.
                    for ind in range(len(pred_norm[ind_sample])):
                        pred_val = 0. if pred_norm[ind_sample][ind] <= 0.5 else 1.
                        if abs(pred_val - y[ind_sample][ind]) >= 0.5:
                            sub_error += 1.
                    sub_error /= len(pred_norm[ind_sample])
                    error += sub_error
                error /= len(pred_norm)
                accuracy -= error


        test_loss /= num_batches

        if (epoch+1) % 10 == 0:
            print(f"Test Error: \n Accuracy: {(accuracy):>8f}%, Avg loss: {test_loss:>8f} \n")
        else:
            print(f"Test Error: \n Avg loss: {test_loss:>8f} \n")

#---------------------------------------------------------------------------
def train_model(device, model, train_dataloader, test_dataloader):

    learning_rate = 1e-1

    epochs = 50

    # Initialize the loss function
    loss_fn = nn.CrossEntropyLoss()

    optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)

    scheduler = torch.optim.lr_scheduler.MultiStepLR(optimizer, milestones=[16, 20], gamma=0.1)

    for t in range(epochs):
        print(f"Epoch {t+1}\n-------------------------------")
        train_loop(device, train_dataloader, model, loss_fn, optimizer)
        test_loop(device, test_dataloader, model, loss_fn, t)

        scheduler.step()

    print("Done!")

    return
