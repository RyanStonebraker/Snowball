#!/usr/bin/env python3
import requests
import json
import os
from sys import argv, stdout
from time import sleep

# Skynet class handles all network communication and abstracts it away so the rest of the code can
# focus on managing the comm folder
class Skynet:
	def __init__(self, address, game_name, color):
		self.address = address
		self.game_name = game_name
		self.color = color
		self.refreshGameInfo()

	def refreshGameInfo(self):
		addr = self.address + '/?info_game=true'
		payload = json.dumps({'name': self.game_name})
		r = requests.post(addr, data=payload)
		result = r.json()
		self.status = result['status']
		self.boards = result['boards']
		

	def submitMove(self, board):
		addr = self.address + '/?play_game=true'
		payload = json.dumps({'name': self.game_name, 'board': self._snowballToSkynetBoard(board)})
		requests.post(addr, data=payload)
		self.refreshGameInfo()

	def currentTurnPlayer(self):
		if self.status == 'red_turn':
			return 'r'
		elif self.status == 'black_turn':
			return 'b'
		else:
			# This should maybe raise an exception, I need to see how Skynet handles games
			return ''

	def isMyTurn(self):
		if self.status == 'red_turn':
			return 'r' == self.color.strip()
		elif self.status == 'black_turn':
			return 'b' == self.color.strip()
		elif self.status == 'red_won':
			print('Red won!')
			exit()
		elif self.status == 'black_won':
			print('Black won!')
			exit()
		else:
			print('Unknown status: ' + self.status)

	def getBoards(self):
		result = []
		for board in self.boards:
			result.append(self._skynetToSnowballBoard(board))
		return result

	def currentBoard(self):
		return self._skynetToSnowballBoard(self.boards[-1])

	def currentTurnNumber(self):
		return len(self.boards)

	def _flipBoard(self, board):
		flippedBoard = []
		for i in range(8):
			row = board[i * 4 : (i + 1) * 4]
			for j in reversed(row):
				flippedBoard.append(j)
		flippedBoard = ''.join(flippedBoard)
		return flippedBoard
	
	def _skynetToSnowballBoard(self, board):
		board = board.replace('_', '0')
		board = board.replace('r', '1')
		board = board.replace('b', '2')
		board = board.replace('R', '3')
		board = board.replace('B', '4')
		board = self._flipBoard(board)
		return board

	def _snowballToSkynetBoard(self, board):
		board = board.replace('0', '_')
		board = board.replace('1', 'r')
		board = board.replace('2', 'b')
		board = board.replace('3', 'R')
		board = board.replace('4', 'B')
		board = self._flipBoard(board)
		return board

def pathForTurn(turn):
	return '../comm/currentgame/turn' + str(turn) + '.txt'

# Main function
if __name__ == '__main__':
	if len(argv) < 4:
		print('Missing argument.')
		print('USAGE: ./skynet-adapter.py <http://skynet-address> <game-name> <r|b>')
		exit()
	
	try:
		skynet = Skynet(argv[1], argv[2], argv[3])
	except KeyError:
		print('Unable to parse response from Skynet -- Does the game exist?')
		exit()
	
	# We're acting like the interface, so we create handshake0
	with open('../comm/handshake0.txt', 'w') as f:
		if skynet.color == 'r':
			f.write('0')
		else:
			f.write('1')

	while not os.path.exists('../comm/handshake1.txt'):
		print('Waiting for handshake from Snowball...')
		sleep(1)
	
	# Write existing turns
	skynet.refreshGameInfo()
	for i, board in enumerate(skynet.getBoards()):
		with open(pathForTurn(i), 'w') as f:
			f.write(board)

	# Main game loop
	while True:
		skynet.refreshGameInfo()
		if skynet.isMyTurn():
			stdout.write('Waiting for turn ' + str(skynet.currentTurnNumber()) + ' from Snowball...')
			while not os.path.exists(pathForTurn(skynet.currentTurnNumber())):
				sleep(0.01)
				stdout.write('.')
				stdout.flush()
			with open(pathForTurn(skynet.currentTurnNumber())) as f:
				board = f.read()
				print('\nSending board to Skynet: ' + board)
				skynet.submitMove(board)
				print('Done.')
		else:
			stdout.write('Waiting for turn ' + str(skynet.currentTurnNumber()) + ' from opponent...')
			while not skynet.isMyTurn():
				skynet.refreshGameInfo()
				stdout.write('.')
				stdout.flush()
			print('\nMove received, sending to Snowball.')
			with open(pathForTurn(skynet.currentTurnNumber() - 1), 'w') as f:
				f.write(skynet.currentBoard())
