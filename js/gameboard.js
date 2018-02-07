// gameboard.js
// Ryan Stonebraker
// 1/19/2018
// Checkers GUI game board

// ** Global HTML5 Canvas declarations
var ctx;
var canvas;
var width = 500;
var height = 500;
// ** End Global

var fs = require('fs');
const path = require('path');

// Global object literal to store various game state info
var game = {
	"passiveStepMode": false,
	"passiveStepCache": "",
	"tileDim": 8,
	"teamPieceCount": 12,
	"communicationLocation": __dirname,
	"darkTileColor": "black",
	"lightTileColor": "white",
	"kingColor": "gold",
	"redPieces": [],
	"whitePieces": [],

	// Controller States (can add a state to communicate to competition server)
	"redFile": false,
	"redPlayer": true,
	"whiteFile": true,
	"whitePlayer": false,

	"lastMouseClick":
	{
		"x": -1,
		"y": -1
	},
	"pixelsPerTile": 64,
	"selectedSquare":
	{
		"row": -1,
		"col": -1,
		"index": -1
	},
	"prevSelectedSquare":
	{
		"row": -1,
		"col": -1,
		"index": -1
	},
  "currentHighlightSquares": [],
  "futureHighlightSquares": [],
  "jumpHighlightColor": "lightgreen",
  "futureJumpHighlightColor": "darkgreen",
	"highlightColor": "yellow",
	"highlightOpacity": 0.5,
	"redTeamColor": "red",
	"whiteTeamColor": "lightgrey",
	"pause": false,
	"fileBoardState": "",

	"fileBoardPrevState": "",
	"fileServer": "localhost:3000",
	"folderPath": "/comm/currentgame/",
	"playerBoardState": "",
	"controllerConnected": false,

	"fastForward": -1,

  "holdTeamTurn": false,

  "winner": ""
};

// Player vs Player
// game.redPlayer = true;
// game.whitePlayer = true;
// game.redFile = false;
// game.whiteFile = false;

// Global object literal for keeping track of scoring and turns
var scoreboard = {
	"red":
	{
		"moveCount": 0,
		"pieceCount": game.teamPieceCount,
		"kingCount": 0,
		"enemyCaptured": 0,
		"lastMoveStarted": 0,
		"timePerLastMove": 0
	},
	"white":
	{
		"moveCount": 0,
		"pieceCount": game.teamPieceCount,
		"kingCount": 0,
		"enemyCaptured": 0,
		"lastMoveStarted": 0,
		"timePerLastMove": 0
	},
	"totalMoveCount": 0,
	"passiveMoveCountBackUp": 0,
	"startTime": 0,
	"totalTime": 0,
	"passiveTime": 0,
	"startClock": false
};

// Default loaded board. If this string is changed before runtime, the game will start in this state.
var DEFAULT_BOARD = "11111111111100000000222222222222";
// var DEFAULT_BOARD = "11111111101101000220000222222022";

// Main "class"/start function
function GameBoard(div, start_board, start_mode)
{
	if (!div)
		return null;

  if (start_board) {
    DEFAULT_BOARD = start_board;
  }

  // If a value selected for start mode (0 is an option, so compare against null)
  if (start_mode != null) {
    switch (parseInt(start_mode)) {
      // AI vs Player
      case 0:
        game.redPlayer = false;
        game.whitePlayer = true;
        game.redFile = true;
        game.whiteFile = false;
        break;
      // Player vs AI
      case 1:
        game.redPlayer = true;
        game.whitePlayer = false;
        game.redFile = false;
        game.whiteFile = true;
        break;
      // Player vs Player
      case 2:
        game.redPlayer = true;
        game.whitePlayer = true;
        game.redFile = false;
        game.whiteFile = false;
        break;
      // AI vs AI
      case 3:
        game.redPlayer = false;
        game.whitePlayer = false;
        game.redFile = true;
        game.whiteFile = true;
        break;
    }
  }

	canvas = div;

	if (canvas.getContext)
	{
		ctx = canvas.getContext("2d");
	}

	// Set initial location states for pieces
	this.generateTeams();

	this.updateScoreBoard();

	if ((game.redFile || game.whiteFile) && !this.moveWritten(0))
	{
		this.sendHandshake();

		game.playerBoardState = DEFAULT_BOARD;
		game.fileBoardState = DEFAULT_BOARD;
		game.fileBoardPrevState = DEFAULT_BOARD;
		this.writeMove();
	}

	// Check board for update conditions
	this.refreshBoard();

	// Watch mouse click events on HTML5 canvas
	canvas.addEventListener("mousedown", this.mouseEventHandler.bind(this), false);
	window.addEventListener("keydown", this.keyEventHandler.bind(this), true);
}

// If mouse clicked, log location
GameBoard.prototype.mouseEventHandler = function (evt)
{
	if (!game.pause)
	{
		var screen = canvas.getBoundingClientRect();
		game.lastMouseClick.x = evt.clientX - screen.left;
		game.lastMouseClick.y = evt.clientY - screen.top;
	}
}

GameBoard.prototype.keyEventHandler = function (evt)
{
	switch (evt.keyCode)
	{
		// space or P pauses game
	case " ".charCodeAt():
	case "P".charCodeAt():
		game.pause = (game.pause ? false : true);
		break;

		// right arrow
	case 39:
		if (game.passiveStepMode && scoreboard.totalMoveCount < scoreboard.passiveMoveCountBackUp)
		{
			++scoreboard.totalMoveCount;
			this.readMove(scoreboard.totalMoveCount);
		}
		break;
		// left arrow
	case 37:
		if (game.passiveStepMode && scoreboard.totalMoveCount > 0)
		{
			--scoreboard.totalMoveCount;
			this.readMove(scoreboard.totalMoveCount);
		}
		break;
		// Up arrow
	case 38:
		if (!game.passiveStepMode)
		{
      // ctx.shadowColor = "black";
      ctx.shadowOffsetX = 0;
      ctx.shadowOffsetY = 0;
      ctx.shadowBlur = 0;

			game.passiveStepMode = true;
      game.pause = false;
			scoreboard.passiveMoveCountBackUp = scoreboard.totalMoveCount;
			game.passiveStepCache = game.fileBoardState;
			scoreboard.passiveTime = scoreboard.totalTime;
			alert("Passive Step Mode On");
		}
		break;
		// Down Arrow
	case 40:
		if (game.passiveStepMode)
		{
			game.passiveStepMode = false;
			scoreboard.totalMoveCount = scoreboard.passiveMoveCountBackUp;
			game.fileBoardState = game.passiveStepCache;
			scoreboard.totalTime = scoreboard.passivetime;
			this.updateScoreBoard();
			alert("Passive Step Mode Off");
		}
		break;
	case "R".charCodeAt():
		this.uninformedBoardPopulate();
	}
}

GameBoard.prototype.sendHandshake = function ()
{
	if (fs.existsSync(path.join(game.communicationLocation, "/comm/handshake0.txt")))
	{
		fs.readFile(path.join(game.communicationLocation, "/comm/handshake0.txt"), 'utf-8', (err, data) =>
		{
			if (err)
			{
				alert("Handshake0 Exists, but Read Error:" + err.message);
				game.pause = true;
				return;
			}
			if (parseInt(data[0]) === 1)
			{
				// Player red goes first
				game.redPlayer = true;
				game.redFile = false;
			}
			else if (parseInt(data[0]) === 0)
			{
				// Computer red goes first
				game.redFile = true;
				game.redPlayer = false;
			}
		});
	}
	else
	{
		var startPacket = ((game.redPlayer && !game.redFile) ? 1 : 0).toString();
		// Write out boolean of player or computer going first
		// In case of AI vs AI, 0/1 markers must be irregularly chosen
		fs.writeFile(path.join(game.communicationLocation, "/comm/handshake0.txt"),
			startPacket, (err) =>
			{
				if (err)
				{
					alert("Handshake0 Write Error:" + err.message);
					game.pause = true;
					return;
				}
			});
	}
}

GameBoard.prototype.confirmSend = function ()
{
	if (fs.existsSync(path.join(game.communicationLocation, "/comm/handshake1.txt")))
	{
		fs.readFile(path.join(game.communicationLocation, "/comm/handshake1.txt"), 'utf-8', (err, data) =>
		{
			if (err)
			{
				alert("Handshake1 Exists, but Read Error:" + err.message);
				game.pause = true;
				return;
			}
			if (parseInt(data[0]) === 1)
			{
				console.log("Controller Connected!");
				game.pause = false;
				// Start game
				game.controllerConnected = true;
			}
			else if (parseInt(data[0]) === 0)
			{
				// Error with prev handshake read
				this.sendHandshake();
			}
		});
	}
}

// Write to a new incremented text file
GameBoard.prototype.writeMove = function ()
{
	var fname = "turn" + (scoreboard.totalMoveCount).toString() + ".txt";
	var board = game.playerBoardState.length >= 32 ? game.playerBoardState : game.fileBoardState;
	fs.writeFile(path.join(game.communicationLocation, game.folderPath + fname),
		board, (err) =>
		{
			if (err)
			{
				alert(fname + " Write Error:" + err.message);
				game.pause = true;
				return;
			}
		});
}

GameBoard.prototype.readMove = function (moveCount)
{
	var movename = "turn" + moveCount.toString() + ".txt";
	if (fs.existsSync(path.join(game.communicationLocation, game.folderPath + movename)))
	{
		fs.readFile(path.join(game.communicationLocation, game.folderPath + movename), 'utf-8', (err, data) =>
		{
			if (err)
			{
				alert(movename + " Read Error:" + err.message);
				game.pause = true;
				return;
			}
			if (data.toString().length >= 32)
			{
				game.fileBoardState = data.toString().replace("\n", "").substring(0, 32);
			}
			else
				console.log("Error - " + movename + " BoardState Not Valid: " + data);
		});
	}
}

GameBoard.prototype.moveWritten = function (moveNum)
{
	var fname = "turn" + moveNum.toString() + ".txt";
	return fs.existsSync(path.join(game.communicationLocation, game.folderPath + fname));
}

// Update function to constantly redraw board
GameBoard.prototype.refreshBoard = function ()
{
	var self = this;
	requestAnimationFrame(function ()
	{
		self.refreshBoard();
	});

	if ((game.redFile || game.whiteFile) && !game.controllerConnected)
	{
		this.confirmSend();
		game.pause = true;
		console.log("GUI Waiting for handshake response...");
	}

	if (!game.pause)
	{
		if ((scoreboard.red.lastMoveStarted || scoreboard.white.lastMoveStarted) && !scoreboard.startClock && !game.passiveStepMode)
		{
			scoreboard.startTime = performance.now();
			scoreboard.startClock = true;
		}

    if ((!scoreboard.red.pieceCount || !scoreboard.white.pieceCount) && !game.passiveStepMode) {
      game.pause = true;
      game.winner = (scoreboard.red.pieceCount == 0) ? "white" : "red";
    }

		// Draw checkerboard
		this.drawBackground();

    if (scoreboard.totalMoveCount == 1 && game.redFile && game.whiteFile && fs.existsSync(path.join(game.communicationLocation, "/comm/handshake1.txt"))) {
      // NOTE: Spawning a process WOULD work if the C++ code ran slower (took up 15 secs) Problem is they are trying to write at same times
      // var spawn = require('child_process').spawn;
      fs.unlinkSync(path.join(game.communicationLocation, "/comm/handshake1.txt"));
      fs.writeFileSync(path.join(game.communicationLocation, "/comm/handshake0.txt"),
        "1", (err) =>
        {
          if (err)
          {
            alert("Handshake0 Write Error:" + err.message);
            game.pause = true;
            return;
          }
        });

        // var enemyAI = spawn('make', ['run_ai']);
    }

		if (!game.passiveStepMode)
		{
			// Draw both team arrays
			this.drawTeam(game.redPieces, game.redTeamColor);
			this.drawTeam(game.whitePieces, game.whiteTeamColor);

			this.moveController();
		}
		else
		{
			this.passiveStepModeDrawBoard();
		}

    // Draw on jump highlight squares
    for (var square = 0; square < game.currentHighlightSquares.length; ++square) {
        this.highlightSquare(game.currentHighlightSquares[square].row, game.currentHighlightSquares[square].col, game.jumpHighlightColor, 0.7);
    }

    // Draw on future jump highlight squares
    for (var square = 0; square < game.futureHighlightSquares.length; ++square) {
        this.highlightSquare(game.futureHighlightSquares[square].row, game.futureHighlightSquares[square].col, game.futureJumpHighlightColor, 0.7);
    }

		this.updateScoreBoard();

		// hacky way to force repopulation of the board if out of sync
		if (game.fastForward <= 50 && game.fastForward >= 0)
		{
			this.uninformedBoardPopulate();
			++game.fastForward;
			if (game.fastForward >= 50)
			{
				game.fastForward = -1;
			}
		}
	}
	else
	{
    if (game.winner)
      this.drawVictory();
    else
		  this.drawPause();

	}
}

GameBoard.prototype.drawVictory = function () {
  ctx.shadowColor = "black";
  ctx.shadowOffsetX = 5;
  ctx.shadowOffsetY = 5;
  ctx.shadowBlur = 7;
  ctx.font = "95px Comic Sans";
  if (game.winner == "white")
    ctx.font = "80px Comic Sans";
  ctx.textAlign = "center";
  ctx.fillStyle = game.winner;
  ctx.fillText(game.winner.toUpperCase() + " WON!", width/2, height/2);
}

GameBoard.prototype.boardToString = function ()
{
	var board = []

	for (var i = 0; i < 32; ++i)
	{
		board[i] = 0;
	}

	for (var i = 0; i < 12; ++i)
	{
		var tile = Math.floor((game.redPieces[i].row * game.tileDim + game.redPieces[i].col) / 2);
		board[tile] = (game.redPieces[i].isKing ? 3 : 1);
		board[tile] = (game.redPieces[i].alive ? board[tile] : 0);

		var wtile = Math.floor((game.whitePieces[i].row * game.tileDim + game.whitePieces[i].col) / 2);
		board[wtile] = (game.whitePieces[i].isKing ? 4 : 2);
		board[wtile] = (game.whitePieces[i].alive ? board[wtile] : 0);
	}
	var bState = ""
	for (var i = 0; i < 32; ++i)
	{
		bState += board[i].toString();
	}
	game.playerBoardState = bState;
}

// draws the pause icon on the screen
GameBoard.prototype.drawPause = function ()
{
	ctx.fillStyle = "blue";
	var pauseBarWidth = 80;
	var pauseBarHeight = 200;
	ctx.fillRect(width / 2 - 1.25 * pauseBarWidth, height / 2 - 0.5 * pauseBarHeight, pauseBarWidth, pauseBarHeight);
	ctx.fillRect(width / 2 + 0.25 * pauseBarWidth, height / 2 - 0.5 * pauseBarHeight, pauseBarWidth, pauseBarHeight);
}

// Check game state and write stats
GameBoard.prototype.updateScoreBoard = function ()
{
	// temp vars to hold count
	var redCount = 0;
	var redKings = 0;
	var whiteCount = 0;
	var whiteKings = 0;

	// iterate through all pieces and check if alive and if king
	for (var i = 0; i < game.teamPieceCount; ++i)
	{
		if (game.redPieces[i].alive)
			++redCount;
		if (game.redPieces[i].isKing)
			++redKings;
		if (game.whitePieces[i].alive)
			++whiteCount;
		if (game.whitePieces[i].isKing)
			++whiteKings;
	}

	// update scoreboard object based on team counts
	scoreboard.red.pieceCount = redCount;
	scoreboard.white.pieceCount = whiteCount;
	scoreboard.red.kingCount = 0;
	scoreboard.white.kingCount = 0;

	// Get precision timing
	scoreboard.totalTime = scoreboard.startClock ? Math.round((performance.now() - scoreboard.startTime) / 10) / 100 : 0;

	// replace text for scoreboard section
	if (scoreboard.red.moveCount + scoreboard.white.moveCount != scoreboard.totalMoveCount || scoreboard.totalTime === 0)
	{
		// update total move count
		if (!game.passiveStepMode)
			scoreboard.totalMoveCount = scoreboard.red.moveCount + scoreboard.white.moveCount;
		var scoreText = "SCOREBOARD:</br></br>";
		scoreText += (scoreboard.red.moveCount === scoreboard.white.moveCount) ? "<h2>Red (ACTIVE TURN):</h2><p>" : "Red:<p>";
		scoreText += "Piece Count: " + scoreboard.red.pieceCount;
		scoreText += "</br>King Count: " + scoreboard.red.kingCount;
		scoreText += "</br>Enemies Captured: " + scoreboard.red.enemyCaptured;
		scoreText += "</br>Last Move Time: " + Math.round(scoreboard.red.timePerLastMove * 100, 0.01) / 100 + "s";
		scoreText += (scoreboard.red.moveCount != scoreboard.white.moveCount) ?
			"</p></br><h2>White (ACTIVE TURN):</h2>" : "</p></br>White:";
		scoreText += "<p>Piece Count: " + scoreboard.white.pieceCount;
		scoreText += "</br>King Count: " + scoreboard.white.kingCount;
		scoreText += "</br>Enemies Captured: " + scoreboard.white.enemyCaptured;
		scoreText += "</br>Last Move Time: " + Math.round(scoreboard.white.timePerLastMove * 100, 0.01) / 100 + "s</p>";

		scoreText += "</br>Total Move Count: " + scoreboard.totalMoveCount;
		$('.scoreboard').html(scoreText);
	}
	if (scoreboard.red.timePerLastMove || scoreboard.white.timePerLastMove ||
		scoreboard.red.lastMoveStarted || scoreboard.white.lastMoveStarted)
	{
		$('.scoreboard_wrapper span').html((scoreboard.totalTime).toFixed(2) + "s");
	}
}

// Start team based gameplay
GameBoard.prototype.moveController = function ()
{
	// If not changing total move count and on the first move.
	if (!game.passiveStepMode && !scoreboard.totalMoveCount && this.moveWritten(2))
	{
		// Fast forward to current file
		while (this.moveWritten(scoreboard.totalMoveCount))
		{
			++scoreboard.totalMoveCount;
			if (scoreboard.totalMoveCount % 2 === 1)
			{
				++scoreboard.red.moveCount;
			}
			else
			{
				++scoreboard.white.moveCount;
			}
		}

		this.readMove(parseInt(scoreboard.totalMoveCount));

		game.fileBoardPrevState = game.fileBoardState;
		game.playerBoardState = game.fileBoardState;
		DEFAULT_BOARD = game.fileBoardState;

		alert("Unsynchronized board state! Attempting to Fast Forward...\n If board doesn't synchronize, Press 'R' to force synchronization.");

		this.fastForward = 0;
		return;
	}

	// move red if count is equal, else move white
	if (scoreboard.red.moveCount === scoreboard.white.moveCount)
	{
		if (game.redPlayer)
		{

			if (game.redFile)
				game.redFile = false;

			this.playerController("red");

		}
		else if (game.redFile === true)
		{

			if (game.redPlayer)
				game.redPlayer = false;

			this.fileInputController("red");
		}
		else
		{
			console.log("NO RED PIECE CONTROLLER SET!");
		}

	}
	else
	{

		if (game.whitePlayer)
		{

			if (game.whiteFile)
				game.whiteFile = false;

			this.playerController("white");
		}
		else if (game.whiteFile)
		{

			if (game.whitePlayer)
				game.whitePlayer = false;

			this.fileInputController("white");

		}
		else
		{
			console.log("NO WHITE PIECE CONTROLLER SET!");
		}
	}
}

GameBoard.prototype.uninformedBoardPopulate = function (board)
{
	if (!board)
		board = game.fileBoardState;
	var currRed = 0;
	var currWhite = 0;
	for (var spot = 0; spot < board.length; ++spot)
	{
		var row = Math.floor(2 * spot / game.tileDim);
		var col = (2 * spot) % 8 + row % 2;
		if (currRed >= 12 && (board[spot] === 1 || board[spot] === 3))
		{
			console.log("Too many Red pieces in boardstate! Ignoring future moves...");
			continue;
		}
		else if (currWhite >= 12 && (board[spot] === 2 || board[spot] === 4))
		{
			console.log("Too many White pieces in boardstate! Ignoring future moves...");
			continue;
		}
		switch (board[spot])
		{
		case "1":
			game.redPieces[currRed].row = row;
			game.redPieces[currRed].col = col;
			game.redPieces[currRed].isKing = false;
			game.redPieces[currRed].highlight = false;
			game.redPieces[currRed].alive = true;
			++currRed;
			break;
		case "2":
			game.whitePieces[currWhite].row = row;
			game.whitePieces[currWhite].col = col;
			game.whitePieces[currWhite].isKing = false;
			game.whitePieces[currWhite].highlight = false;
			game.whitePieces[currWhite].alive = true;
			++currWhite;
			break;
		case "3":
			game.redPieces[currRed].row = row;
			game.redPieces[currRed].col = col;
			game.redPieces[currRed].isKing = true;
			game.redPieces[currRed].highlight = false;
			game.redPieces[currRed].alive = true;
			++currRed;
			break;
		case "4":
			game.whitePieces[currWhite].row = row;
			game.whitePieces[currWhite].col = col;
			game.whitePieces[currWhite].isKing = true;
			game.whitePieces[currWhite].highlight = false;
			game.whitePieces[currWhite].alive = true;
			++currWhite;
			break;
		default:
			break;
		}
	}
	for (currRed; currRed < game.teamPieceCount; ++currRed)
	{
		game.redPieces[currRed].row = -1;
		game.redPieces[currRed].col = -1;
		game.redPieces[currRed].isKing = false;
		game.redPieces[currRed].highlight = false;
		game.redPieces[currRed].alive = false;
	}
	for (currRed; currWhite < game.teamPieceCount; ++currWhite)
	{
		game.whitePieces[currWhite].row = -1;
		game.whitePieces[currWhite].col = -1;
		game.whitePieces[currWhite].isKing = false;
		game.whitePieces[currWhite].highlight = false;
		game.whitePieces[currWhite].alive = false;
	}
}

// Passive controller for input from computer created files
GameBoard.prototype.fileInputController = function (color)
{
	var teamScoreBoard = (color === "red" ? scoreboard.red : scoreboard.white);

	// If no file for the current move count exists, create one
	if (!this.moveWritten(scoreboard.totalMoveCount))
		this.writeMove();

	// If a file for the next move exists, read it in
	if (this.moveWritten(scoreboard.totalMoveCount + 1))
	{
		this.readMove(scoreboard.totalMoveCount + 1);
	}

	teamScoreBoard.lastMoveStarted = performance.now();

	if (game.fileBoardPrevState.toString().substring(0, 32) != game.fileBoardState.toString().substring(0, 32) &&
		game.fileBoardState.toString().length >= 32 && game.fileBoardState.toString().substring(0, 32) !=
		game.playerBoardState.toString().substring(0, 32))
	{

		++teamScoreBoard.moveCount;

		// smartly updates internal board by finding changes in boardstates
		this.updateChanged(color);

		game.playerBoardState = game.fileBoardState;
		game.fileBoardPrevState = game.fileBoardState;
	}
	if (!game.passiveStepMode)
		teamScoreBoard.timePerLastMove = performance.now() - teamScoreBoard.lastMoveStarted;
}

// short circuits game logic and just draws whatever is in files
GameBoard.prototype.passiveStepModeDrawBoard = function ()
{
	for (var spot = 0; spot < game.fileBoardState.length; ++spot)
	{
		var row = Math.floor(2 * spot / game.tileDim);
		var col = (2 * spot) % 8 + row % 2;

		if (game.fileBoardState[spot] != "0")
		{
			// returns true for 1 and 3 (red)
			var pieceColor = (parseInt(game.fileBoardState[spot]) % 2 === 1) ? game.redTeamColor : game.whiteTeamColor;
			// 3, 4 are kings
			var pieceKing = (parseInt(game.fileBoardState[spot]) >= 3) ? true : false;
			// draw piece on board
			this.drawPiece(col, row, pieceColor, pieceKing, false);
		}

	}
}

// A smart moving function that finds differences in boardstates and only moves a single piece
GameBoard.prototype.updateChanged = function (color)
{
	console.log("PlayerBoardState: " + game.playerBoardState + "\nFileBoardState:   " + game.fileBoardState + "\nColor: " + color);
	if (game.fileBoardState != game.playerBoardState)
	{
		// Holds all piece changes
		// Structure: {"pieceLocation":{"row":[0-7], "col":[0-7]}, "pieceType": [0|1|2|3|4], "sameColor":[true-false]}
		var pieceChangePool = []

		// Iterate through all pieces until find change
		for (var piece = 0; piece < game.fileBoardState.length; ++piece)
		{
			// If this piece was previously here, but not anymore
			if (game.fileBoardState[piece] != game.playerBoardState[piece])
			{
				var row = Math.floor(2 * piece / game.tileDim);
				var col = (2 * piece) % 8 + row % 2;

				// Check if previous piece was the moving color's piece
				var sameColor = (color === "red") ?
					(game.playerBoardState[piece] === '1' || game.playerBoardState[piece] === '3') :
					(game.playerBoardState[piece] === '2' || game.playerBoardState[piece] === '4');

				if (game.playerBoardState[piece] === '0')
				{
					var sameColor = (color === "red") ?
						(game.fileBoardState[piece] === '1' || game.fileBoardState[piece] === '3') :
						(game.fileBoardState[piece] === '2' || game.fileBoardState[piece] === '4');
				}

				// Add current spot of fileBoardState to the pieceChangePool
				pieceChangePool.push(
				{
					"pieceLocation":
					{
						"row": row,
						"col": col
					},
					"pieceType": game.fileBoardState[piece],
					"sameColor": sameColor
				});
			}
		}

		// stores the previous location of the piece being updating
		var previousColorPieceLocation = {};

		// stores the current location of the piece being updated
		var currentColorPieceLocation = {};

		var currentTeam = (color === "red" ? game.redPieces : game.whitePieces);
		var enemyTeam = (color === "red" ? game.whitePieces : game.redPieces);
    var teamScore = (color === "red" ? scoreboard.red : scoreboard.white);
		console.log(pieceChangePool);
		for (var poolPiece = 0; poolPiece < pieceChangePool.length; ++poolPiece)
		{
			if (pieceChangePool[poolPiece].sameColor && pieceChangePool[poolPiece].pieceType === '0')
			{
				// An error occured if there was more than one same colored move
				// Still updates to new location though to attempt and visually show bug
				if (!!previousColorPieceLocation.pieceLocation)
				{
					alert("Invalid Move! Too many " + color.toUpperCase() + " pieces moved!\n Before Move: " +
						game.playerBoardState + "\n After Move: " + game.fileBoardState);
				}
				previousColorPieceLocation = pieceChangePool[poolPiece];
			}
			else if (pieceChangePool[poolPiece].sameColor && pieceChangePool[poolPiece].pieceType != '0')
			{
				currentColorPieceLocation = pieceChangePool[poolPiece];
			}
			else
			{
        ++teamScore.enemyCaptured;

				// Case: delete jumped enemies
				indexToMove = this.spaceOccupied(pieceChangePool[poolPiece].pieceLocation, enemyTeam);
				console.log("Index to Delete: " + indexToMove);
				enemyTeam[indexToMove].alive = false;
				enemyTeam[indexToMove].row = -5;
				enemyTeam[indexToMove].col = -5;
			}
		}

		// Case: Color piece moved to new location
		var moveFromHere = this.spaceOccupied(previousColorPieceLocation.pieceLocation, currentTeam);
		if (moveFromHere >= 0)
		{
			if (!currentTeam[moveFromHere].alive)
				alert("WARNING: MOVING GHOST PIECE!!!\nPREVIOUS BOARD STATE: " + game.playerBoardState +
					"\nCURRENT BOARD STATE: " + game.fileBoardState);

			currentTeam[moveFromHere].row = currentColorPieceLocation.pieceLocation.row;
			currentTeam[moveFromHere].col = currentColorPieceLocation.pieceLocation.col;

			kingForColor = (color === "red") ? 3 : 4;
			if (parseInt(currentColorPieceLocation.pieceType) === kingForColor)
			{
				currentTeam[moveFromHere].isKing = true;
			}
		}

	}
}

// Checks whether a jump needs to be forced and if one does, returns all possible jumps, the
// enemy pieces that would be killed, and highlights jump spaces if user tries to move elsewhere.
GameBoard.prototype.jumpRecurse = function (color)
{
  // Structure: {"startRow": START, "startCol": START, "endRow": END, "endCol": END, "killSpots": [ENEMIES]}
	var jumpMoves = []

	var colorPieces = (color === "red") ? game.redPieces : game.whitePieces;
  var colorScore = (color === "red") ? scoreboard.red.moveCount : scoreboard.white.moveCount;
	var oppColorPieces = (color === "red") ? game.whitePieces : game.redPieces;
	var relativeCheckSpots = (color === "red") ? [
	{
		"addRow": 1,
		"addCol": 1
	},
	{
		"addRow": 1,
		"addCol": -1
	}] : [
	{
		"addRow": -1,
		"addCol": -1
	},
	{
		"addRow": -1,
		"addCol": 1
	}];
	var kingCheckSpots = [
	{
		"addRow": 1,
		"addCol": 1
	},
	{
		"addRow": 1,
		"addCol": -1
	},
	{
		"addRow": -1,
		"addCol": -1
	},
	{
		"addRow": -1,
		"addCol": 1
	}];

  game.currentHighlightSquares = [];
  game.futureHighlightSquares = [];

  var keepHoldingTurn = false;

	for (var i = 0; i < colorPieces.length; ++i)
	{
		if (colorPieces[i].alive)
		{
			var localCheck = colorPieces[i].isKing ? kingCheckSpots : relativeCheckSpots;

			for (var rel = 0; rel < localCheck.length; ++rel)
			{
				var coordCheck = {
					"row": localCheck[rel].addRow + colorPieces[i].row,
					"col": localCheck[rel].addCol + colorPieces[i].col
				};
        var emptyCoordCheck = {
					"row": localCheck[rel].addRow * 2 + colorPieces[i].row,
					"col": localCheck[rel].addCol * 2 + colorPieces[i].col
				};
        if (emptyCoordCheck.row < 0 || emptyCoordCheck.col < 0 ||
            emptyCoordCheck.row >= game.tileDim || emptyCoordCheck.col >= game.tileDim) {
              continue;
            }

        var killSpot = this.spaceOccupied(coordCheck, oppColorPieces);
				if (killSpot >= 0 && this.spaceOccupied(emptyCoordCheck, oppColorPieces) === -1 &&
            this.spaceOccupied(emptyCoordCheck, colorPieces) === -1)
				{
          game.currentHighlightSquares.push(emptyCoordCheck);
          jumpMoves.push(
          {
            "startRow":colorPieces[i].row,
            "startCol":colorPieces[i].col,
            "endRow": emptyCoordCheck.row,
            "endCol": emptyCoordCheck.col,
            "killSpots": [killSpot]
          });

          var futureLocalCheck = localCheck;

          var kingRow = (color === "red") ? 7 : 0;

          if (emptyCoordCheck.row === kingRow) {
            futureLocalCheck = kingCheckSpots;
          }
          for (var futureRel = 0; futureRel < futureLocalCheck.length; ++futureRel) {
            var futureCoordCheck = {
    					"row": futureLocalCheck[futureRel].addRow + emptyCoordCheck.row,
    					"col": futureLocalCheck[futureRel].addCol + emptyCoordCheck.col
    				};
            var futureEmptyCoordCheck = {
    					"row": futureLocalCheck[futureRel].addRow * 2 + emptyCoordCheck.row,
    					"col": futureLocalCheck[futureRel].addCol * 2 + emptyCoordCheck.col
    				};
            if (futureEmptyCoordCheck.row < 0 || futureEmptyCoordCheck.col < 0 ||
                futureEmptyCoordCheck.row >= game.tileDim || futureEmptyCoordCheck.col >= game.tileDim) {
                  continue;
                }
    				// jumpMoves.push({"startRow":3, "startCol":3, "endRow": 5, "endCol": 1, "killSpots":[0]});
            var futureKillSpot = this.spaceOccupied(futureCoordCheck, oppColorPieces);
    				if (futureKillSpot >= 0 && futureKillSpot != killSpot && this.spaceOccupied(futureEmptyCoordCheck, oppColorPieces) === -1)
    				{

              game.futureHighlightSquares.push(futureEmptyCoordCheck);
              game.holdTeamTurn = true;
              keepHoldingTurn = true;
            }
          }
				}
			}
		}
	}

  if (!keepHoldingTurn)
    game.holdTeamTurn = false;

	return jumpMoves;
}

GameBoard.prototype.highlightSquare = function (row, col, color = game.highlightColor, opacity = game.highlightOpacity)
{
	ctx.globalAlpha = opacity;
	ctx.fillStyle = color;
	ctx.fillRect(width / game.tileDim * col, width / game.tileDim * row, width / game.tileDim, width / game.tileDim);
	ctx.globalAlpha = 1;
}

GameBoard.prototype.distance = function (coord1, coord2) {
  return Math.max(Math.abs(coord2.row - coord1.row), Math.abs(coord2.col - coord2.col));
}

// Update selected teams pieces based on mouse click events
GameBoard.prototype.playerController = function (color)
{
	// Store both team arrays by reference
	var teamArr = (color === "red" ? game.redPieces : game.whitePieces);
	var teamOpp = (color === "red" ? game.whitePieces : game.redPieces);
	var teamScore = (color === "red" ? scoreboard.red : scoreboard.white);

	// Use x position of last mouse click as a way to check if playerController has already been called
	if (game.lastMouseClick.x != -1)
	{
		// Find row/column of clicked spot
		game.selectedSquare.col = Math.floor(game.lastMouseClick.x / game.pixelsPerTile);
		game.selectedSquare.row = Math.floor(game.lastMouseClick.y / game.pixelsPerTile);

		// Check whether the player selected a game piece (-1 means no game piece from specified array)
		var selected = this.spaceOccupied(game.selectedSquare, teamArr);
		var oppselect = this.spaceOccupied(game.selectedSquare, teamOpp);

		// If player selected an empty tile and they previously selected their color piece, move piece here
		if (selected === -1 && oppselect === -1 && game.selectedSquare.index != -1)
		{
			var possibleJumps = this.jumpRecurse(color);

			var chosenJumpPath = false;

			if (possibleJumps.length > 0)
			{
				for (var posMove = 0; posMove < possibleJumps.length; ++posMove)
				{
					if (game.selectedSquare.row === possibleJumps[posMove].endRow &&
              game.selectedSquare.col === possibleJumps[posMove].endCol &&
              possibleJumps[posMove].startRow === game.prevSelectedSquare.row &&
              possibleJumps[posMove].startCol === game.prevSelectedSquare.col)
					{
            ++teamScore.enemyCaptured;
						for (var removeEnemy = 0; removeEnemy < possibleJumps[posMove].killSpots.length; ++removeEnemy)
						{
							teamOpp[possibleJumps[posMove].killSpots[removeEnemy]].alive = false;
							teamOpp[possibleJumps[posMove].killSpots[removeEnemy]].row = -1;
							teamOpp[possibleJumps[posMove].killSpots[removeEnemy]].col = -1;
						}
						game.selectedSquare.row = possibleJumps[posMove].endRow;
						game.selectedSquare.col = possibleJumps[posMove].endCol;
						chosenJumpPath = true;
						break;
					}
				}
			}

			if (!possibleJumps.length || chosenJumpPath)
			{
				teamArr[game.selectedSquare.index].row = game.selectedSquare.row;
				teamArr[game.selectedSquare.index].col = game.selectedSquare.col;
				teamArr[game.selectedSquare.index].highlight = false;
        game.currentHighlightSquares = [];

				// King if reached end of board
				if ((game.selectedSquare.row === 7 && color === "red") || (game.selectedSquare.row === 0 && color === "white"))
					teamArr[game.selectedSquare.index].isKing = true;

        // reset states so unnecessary work isn't done and piece stops being highlighted after placed
        game.lastMouseClick.x = -1;
        game.selectedSquare.index = -1;

        if (!game.holdTeamTurn) {
  				// Player made move, update string
  				this.boardToString();

  				// Update scoreboard
  				++teamScore.moveCount;
  				teamScore.timePerLastMove = performance.now() / 1000 - teamScore.lastMoveStarted;
  				teamScore.lastMoveStarted = 0;
        }
			}

		}

		// If player selected one of their color pieces, highlight the tile and store its array index
		if (selected != -1)
		{
			teamArr[selected].highlight = true;

      // Shift current to previous
      game.prevSelectedSquare.index = game.selectedSquare.index;
      game.prevSelectedSquare.row = game.selectedSquare.row;
      game.prevSelectedSquare.col = game.selectedSquare.col;

			// deselect previously selected square if player changes mind on piece selection
			if (game.selectedSquare.index != -1 && game.selectedSquare.index != selected)
			{
				teamArr[game.selectedSquare.index].highlight = false;
			}

			// Log start of move if not in middle of move
			if (!teamScore.lastMoveStarted)
				teamScore.lastMoveStarted = performance.now() / 1000;

			game.selectedSquare.index = selected;
		}
    game.lastMouseClick.x = -1;
	}
}

// Simple function that checks whether a row/col is in an array of row/cols (used to check if occupied)
// returns -1 if no match found
GameBoard.prototype.spaceOccupied = function (loc, arr)
{
	if (loc.row >= game.tileDim || loc.col >= game.tileDim || loc.row < 0 || loc.col < 0)
	{
		return -2;
	}
	for (var i = 0; i < arr.length; ++i)
	{
		var spot = arr[i];
		if (spot.row === loc.row && spot.col === loc.col && !!spot.alive)
		{
			return i;
		}
	}
	return -1;
}

// draws the checker board background onto canvas
GameBoard.prototype.drawBackground = function ()
{
	ctx.fillStyle = game.lightTileColor;
	ctx.fillRect(0, 0, width, height);
	ctx.fillStyle = game.darkTileColor;
	for (var cols = 0; cols < game.tileDim; ++cols)
	{
		for (var rows = 0; rows < game.tileDim; ++rows)
		{
			if ((cols + rows) % 2 === 0)
				ctx.fillRect((width / game.tileDim * rows) % width, height / game.tileDim * cols, width / game.tileDim, height / game.tileDim);
		}
	}
}

// draws a checker piece based on information stored in object
GameBoard.prototype.drawPiece = function (col, row, color, king, highlight)
{
	// Prevent out of bounds errors
	if (col < 0 || row < 0 || col >= game.tileDim || row >= game.tileDim)
		return;

	var radius = width / (2 * game.tileDim + 2);
	ctx.beginPath();
	ctx.arc(width / (2 * game.tileDim) + width / game.tileDim * col,
		width / (2 * game.tileDim) + width / game.tileDim * row,
		radius, 0, 2 * Math.PI, false);
	ctx.fillStyle = color;
	ctx.fill();

	if (king)
	{
		ctx.beginPath();
		ctx.arc(width / (2 * game.tileDim) + width / game.tileDim * col,
			width / (2 * game.tileDim) + width / game.tileDim * row,
			radius / 2, 0, 2 * Math.PI,
			false);
		ctx.lineWidth = 10;
		ctx.strokeStyle = game.kingColor;
		ctx.stroke();
	}

	if (highlight)
	{
		this.highlightSquare(row, col);
	}
}

// resets game pieces into starting configuration
GameBoard.prototype.generateTeams = function ()
{
	game.redPieces = []
	game.whitePieces = []

	for (var i = 0; i < game.teamPieceCount; ++i)
	{
		var row = Math.floor((i * 2) / game.tileDim);
		var col = (i * 2) % game.tileDim + row % 2;

		game.redPieces.push(
		{
			"col": col,
			"row": row,
			"isKing": false,
			"highlight": false,
			"alive": true
		});

		if (row % 2 === 0)
			++col;
		else
		{
			--col;
		}
		game.whitePieces.push(
		{
			"col": col,
			"row": row + game.tileDim - game.teamPieceCount / (Math.ceil(game.tileDim / 2)),
			"isKing": false,
			"highlight": false,
			"alive": true
		});
	}
	this.uninformedBoardPopulate(DEFAULT_BOARD);
}

// Draws every checker piece for specified team
GameBoard.prototype.drawTeam = function (pieceArr, color)
{
	var self = this;
	pieceArr.forEach(function (piece)
	{
		if (piece.alive)
			self.drawPiece(piece.col, piece.row, color, piece.isKing, piece.highlight);
	});
}
