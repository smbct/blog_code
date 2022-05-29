import numpy
from matplotlib import pyplot as plt
from matplotlib.widgets import Slider, Button
import matplotlib.patches as mpatches

import chess

import utils

class gui:

    #-------------------------------------------------------------------------------
    def __init__(self, moves):

        self.board = chess.Board()
        self.moves = moves
        self.move_index = 0

        self.ax = None
        self.slider = None
        self.bnext = None
        self.bprev = None

        self.build_window()

        return

    #-------------------------------------------------------------------------------
    def build_window(self):

        p1 = 0.08
        r1_w = 1.-2.*p1
        r1_h = r1_w
        r1_x = p1
        r1_y = p1

        b_w = r1_w
        b_h = r1_h*0.8
        b_x = r1_x
        b_y = 1.-p1-b_h

        r2_w = b_w
        r2_h = r1_h-b_h
        r2_x = r1_x
        r2_y = r1_y

        p2 = 0.3*r2_h

        s_w = r2_w*0.5
        bt_w = 0.1

        p3 = (r2_w-s_w-bt_w-bt_w)/4.

        s_x = r2_x+p3
        s_y = p1#r2_y+p2
        s_h = r2_h-p2*2.

        bt_h = s_h

        bt1_x = s_x+s_w+p3
        bt1_y = s_y

        bt2_x = s_x+s_w+p3+bt_w+p3
        bt2_y = s_y

        # print('slider: ', s_x, s_y, s_w, s_h)
        # print('b1: ', bt1_x, bt1_y, bt_w, bt_h)

        fig = plt.figure()
        self.ax = plt.axes([b_x,b_y,b_w,b_h])
        ax2 = plt.axes([s_x,s_y,s_w,s_h])
        ax3 = plt.axes([bt1_x,bt1_y,bt_w,bt_h])
        ax4 = plt.axes([bt2_x,bt2_y,bt_w,bt_h])

        global moves

        self.slider = Slider(ax2, '', 0, len(self.moves), 0, valstep=1)
        self.bprev = Button(ax3, '<-')
        self.bnext = Button(ax4, '->')

        self.bnext.on_clicked(self.next_move_gui)
        self.bprev.on_clicked(self.prev_move_gui)

        self.slider.on_changed(self.slider_update_gui)

        return

    #-------------------------------------------------------------------------------
    def update_board(self):

        gboard.draw_board(self.ax, self.board)

        moves = utils.moves_from_rules(self.board)
        gboard.draw_moves(self.ax, moves)

        return

    #-------------------------------------------------------------------------------
    def next_move_gui(self, event):

        # board.push_san(moves[move_index])

        if self.move_index < len(self.moves):
            self.move_index += 1
            self.slider.set_val(self.move_index)
            utils.go_to_move(self.board, self.moves, self.move_index)

        plt.ion()
        self.ax.cla()
        self.update_board()

        return

    #-------------------------------------------------------------------------------
    def prev_move_gui(self, event):

        # board.push_san(moves[move_index])
        if self.move_index > 0:
            self.move_index -= 1
            self.slider.set_val(self.move_index)
            utils.go_to_move(self.board, self.moves, self.move_index)

        plt.ion()
        self.ax.cla()
        self.update_board()

        return

    #-------------------------------------------------------------------------------
    def go_to_move(self, move_index):
        self.board.reset()
        for i in range(move_index):
            self.board.push_san(self.moves[i])
        return

    #-------------------------------------------------------------------------------
    def slider_update_gui(self, value):

        self.move_index = int(value)
        utils.go_to_move(self.board, self.moves, self.move_index)
        plt.ion()
        self.ax.cla()
        self.update_board()

        return




#----------------------------------------------
class gboard:


    #-------------------------------------------------------------------------------
    def draw_board(ax, board):

        ax.set_xlim((0,8))
        ax.set_ylim((0,8))

        ax.xaxis.set_ticks([i+0.5 for i in range(8)], ['a','b', 'c', 'd', 'e', 'f', 'g', 'h'])
        ax.yaxis.set_ticks([i+0.5 for i in range(8)], list(range(1,9)))

        for i in range(8):
            for j in range(8):
                if i%2 != j %2:
                    ax.fill([i,i+1, i+1, i], [j, j, j+1, j+1], 'mediumseagreen')

        for i in range(8):
            for j in range(8):
                piece = board.piece_at(i*8+j)
                if not piece is None:
                    symbol = piece.symbol()
                    if piece.color == chess.WHITE:
                        ax.imshow(gboard.sprites[symbol.lower()+'l'], extent=(j, j+1, i, i+1), zorder=2)
                    else:
                        ax.imshow(gboard.sprites[symbol.lower()+'d'], extent=(j, j+1, i, i+1), zorder=2)

    #-------------------------------------------------------------------------------
    def draw_moves(ax, moves):

        for orig in moves:
            for dest in moves[orig]:
                arrow = mpatches.FancyArrowPatch((orig[0]+0.5, orig[1]+0.5), (dest[0]+0.5, dest[1]+0.5), mutation_scale=20, zorder=3, alpha=0.35)
                ax.add_patch(arrow)

        return

    #-------------------------------------------------------------------------------
    def load_sprites():
        sprites = {}
        for a in ['b', 'k', 'n', 'p', 'q', 'r']:
            for b in ['d','l']:
                sprites[a+b] = plt.imread('sprites/' + a + b + '.png')
        return sprites

    #-------------------------------------------------------------------------------
    # class variable to store the sprites
    sprites = load_sprites()
