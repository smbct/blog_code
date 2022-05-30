import torch
from torch import nn

class ChessModel(nn.Module):

    def __init__(self):
        super(ChessModel, self).__init__()

        n_conv_layers = 64

        self.conv0 =  nn.Conv3d(1, n_conv_layers, (13,3,3), 1) # 8*8 -> 6*6
        self.bn0 = nn.BatchNorm2d(n_conv_layers)

        self.conv1 =  nn.Conv2d(n_conv_layers, n_conv_layers, (3,3), 1) # 6*6 -> 4*4
        self.bn1 = nn.BatchNorm2d(n_conv_layers)
        self.dp0 = nn.Dropout2d(p=0.2)
        self.pad1 = torch.nn.ZeroPad2d(1) # 4*4 -> 6*6

        self.conv2 =  nn.Conv2d(n_conv_layers, n_conv_layers, (3,3), 1) # 6*6 -> 4*4 + res1
        self.bn2 = nn.BatchNorm2d(n_conv_layers)
        self.dp1 = nn.Dropout2d(p=0.2)
        self.pad2 = torch.nn.ZeroPad2d(1) # 4*4 -> 6*6

        self.conv3 =  nn.Conv2d(n_conv_layers, n_conv_layers, (3,3), 1) # 6*6 -> 4*4 + res2
        self.bn3 = nn.BatchNorm2d(n_conv_layers)
        self.dp2 = nn.Dropout2d(p=0.2)
        self.pad3 = torch.nn.ZeroPad2d(1) # 4*4 -> 6*6

        self.conv4 =  nn.Conv2d(n_conv_layers, n_conv_layers, (3,3), 1) # 6*6 -> 4*4 + res3
        self.bn4 = nn.BatchNorm2d(n_conv_layers)
        self.dp3 = nn.Dropout2d(p=0.2)

        self.deconv0 = nn.ConvTranspose2d(n_conv_layers, n_conv_layers, (3,3), 1) # 4*4 -> 6*6
        self.bn5 = nn.BatchNorm2d(n_conv_layers)

        self.deconv1 = nn.ConvTranspose2d(n_conv_layers, 1, (3,3), 1) # 6*6 -> 8*8


    def forward(self, x):

        out = self.conv0(x)
        out = torch.squeeze(out, 2)
        out = self.bn0(out).relu()

        # print(out.shape)

        res1 = out

        out = self.conv1(out)
        out = self.bn1(out).relu()
        out = self.dp0(out)
        out = self.pad1(out)

        res2 = out

        out = self.conv2(out+res1)
        out = self.bn2(out).relu()
        out = self.dp1(out)
        out = self.pad2(out)

        res3 = out

        out = self.conv3(out+res2)
        out = self.bn3(out).relu()
        out = self.dp2(out)
        out = self.pad3(out)

        out = self.conv4(out+res3)
        out = self.bn4(out).relu()
        out = self.dp3(out)


        out = self.deconv0(out)
        out = self.bn5(out).relu()
        out = self.deconv1(out)

        out = torch.flatten(out, start_dim=1)

        return out
