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
	"passiveStepCacheCreated": false,
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
	"highlightColor": "yellow",
	"highlightOpacity": 0.5,
	"redTeamColor": "red",
	"whiteTeamColor": "lightgrey",
	"pause": false,
	"fileBoardState": "",

	"fileBoardPrevState": "",
	"fileServer": "localhost:3000",
	"playerBoardState": "",
	"controllerConnected": false
};

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
	"startTime": 0,
	"totalTime": 0,
	"startClock": false
};

// Main "class"/start function
function GameBoard(div)
{
	if (!div)
		return null;

	canvas = div;

	if (canvas.getContext)
	{
		ctx = canvas.getContext("2d");
	}

	// Set initial location states for pieces
	this.generateTeams();

	this.updateScoreBoard();

	if (game.redFile || game.whiteFile)
	{
		this.sendHandshake();

		game.playerBoardState = "11111111111100000000222222222222";
		game.fileBoardState = "11111111111100000000222222222222";
		game.fileBoardPrevState = "11111111111100000000222222222222";
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
		if (game.passiveStepMode)
		{
			var teamFutureMove = scoreboard.totalMoveCount % 2 == 0 ? scoreboard.white : scoreboard.red;
			++scoreboard.totalMoveCount;
			++teamFutureMove.totalMoveCount;
		}
		break;
		// left arrow
	case 37:
		if (game.passiveStepMode && scoreboard.totalMoveCount >= 0)
		{
			var teamPrevMove = scoreboard.totalMoveCount % 2 == 1 ? scoreboard.red : scoreboard.white;
			--scoreboard.totalMoveCount;
			--teamPrevMove.moveCount;
		}
		break;
		// Up arrow
	case 38:
		if (!game.passiveStepMode)
		{
			// game.maxTurn = 100;
			game.passiveStepMode = true;
			--scoreboard.totalMoveCount;
			console.log("Passive Step Mode On");
		}
		break;
		// Down Arrow
	case 40:
		if (game.passiveStepMode)
		{
			game.passiveStepMode = false;
			console.log("Passive Step Mode Off");
		}
		break;
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
			if (parseInt(data[0]) == 1)
			{
				// Player red goes first
				game.redPlayer = true;
				game.redFile = false;
			}
			else if (parseInt(data[0]) == 0)
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
			if (parseInt(data[0]) == 1)
			{
				console.log("Controller Connected!");
				game.pause = false;
				// Start game
				game.controllerConnected = true;
			}
			else if (parseInt(data[0]) == 0)
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
	fs.writeFile(path.join(game.communicationLocation, "/comm/currentgame/" + fname),
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
	if (fs.existsSync(path.join(game.communicationLocation, "/comm/currentgame/" + movename)))
	{
		fs.readFile(path.join(game.communicationLocation, "/comm/currentgame/" + movename), 'utf-8', (err, data) =>
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
	return fs.existsSync(path.join(game.communicationLocation, "/comm/currentgame/" + fname));
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

		// Draw checkerboard
		this.drawBackground();

		// Draw both team arrays
		this.drawTeam(game.redPieces, game.redTeamColor);
		this.drawTeam(game.whitePieces, game.whiteTeamColor);

		this.moveController();

		this.updateScoreBoard();
	}
	else
	{
		this.drawPause();
	}
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
	if (scoreboard.red.moveCount + scoreboard.white.moveCount != scoreboard.totalMoveCount || scoreboard.totalTime == 0)
	{
		// update total move count
		if (!game.passiveStepMode)
			scoreboard.totalMoveCount = scoreboard.red.moveCount + scoreboard.white.moveCount;
		var scoreText = "SCOREBOARD:</br></br>";
		scoreText += (scoreboard.red.moveCount == scoreboard.white.moveCount) ? "<h2>Red (ACTIVE TURN):</h2><p>" : "Red:<p>";
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
	// move red if count is equal, else move white
	if (scoreboard.red.moveCount == scoreboard.white.moveCount)
	{
		if (game.redPlayer)
		{

			if (game.redFile)
				game.redFile = false;

			this.playerController("red");

		}
		else if (game.redFile == true)
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

// Passive controller for input from computer created files
GameBoard.prototype.fileInputController = function (color)
{
	var teamScoreBoard = (color == "red" ? scoreboard.red : scoreboard.white);

	// If no file for the current move count exists, create one
	if (!this.moveWritten(scoreboard.totalMoveCount))
		this.writeMove();

	// If a file for the next move exists, read it in
	if (this.moveWritten(scoreboard.totalMoveCount + 1))
	{
		this.readMove(scoreboard.totalMoveCount + 1);
	}

	// **************************************************************REMOVE*************************
	if (game.passiveStepMode && this.moveWritten(scoreboard.totalMoveCount))
	{
		this.readMove(scoreboard.totalMoveCount + 1);
	}

	// TODO: Change location of lastMove started so that AI's time is measured
	teamScoreBoard.lastMoveStarted = performance.now();

	var currRed = 0;
	var currWhite = 0;

	if (game.fileBoardPrevState.toString().substring(0, 32) != game.fileBoardState.toString().substring(0, 32) &&
		game.fileBoardState.toString().length >= 32 && game.fileBoardState.toString().substring(0, 32) !=
		game.playerBoardState.toString().substring(0, 32))
	{

		++teamScoreBoard.moveCount;


		// TODO: **** START MOVE READ REWRITE -- ABSTRACT TO FUNCTION
    this.updateChanged(color);
		// FIND DIFF BEFORE ERASING PREV BOARD STATE
		game.playerBoardState = game.fileBoardState;
		game.fileBoardPrevState = game.fileBoardState;

		// for (var spot = 0; spot < 32; ++spot)
		// {
		// 	var row = Math.floor(2 * spot / game.tileDim);
		// 	var col = (2 * spot) % 8 + row % 2;
		// 	if (currRed >= 12 && (game.fileBoardState[spot] == 1 || game.fileBoardState[spot] == 3))
		// 	{
		// 		console.log("Too many Red pieces in boardstate! Ignoring future moves...");
		// 		continue;
		// 	}
		// 	else if (currWhite >= 12 && (game.fileBoardState[spot] == 2 || game.fileBoardState[spot] == 4))
		// 	{
		// 		console.log("Too many White pieces in boardstate! Ignoring future moves...");
		// 		continue;
		// 	}
		// 	switch (game.fileBoardState[spot])
		// 	{
		// 	case "1":
		// 		game.redPieces[currRed].row = row;
		// 		game.redPieces[currRed].col = col;
		// 		game.redPieces[currRed].isKing = false;
		// 		game.redPieces[currRed].highlight = false;
		// 		game.redPieces[currRed].alive = true;
		// 		++currRed;
		// 		break;
		// 	case "2":
		// 		game.whitePieces[currWhite].row = row;
		// 		game.whitePieces[currWhite].col = col;
		// 		game.whitePieces[currWhite].isKing = false;
		// 		game.whitePieces[currWhite].highlight = false;
		// 		game.whitePieces[currWhite].alive = true;
		// 		++currWhite;
		// 		break;
		// 	case "3":
		// 		game.redPieces[currRed].row = row;
		// 		game.redPieces[currRed].col = col;
		// 		game.redPieces[currRed].isKing = true;
		// 		game.redPieces[currRed].highlight = false;
		// 		game.redPieces[currRed].alive = true;
		// 		++currRed;
		// 		break;
		// 	case "4":
		// 		game.whitePieces[currWhite].row = row;
		// 		game.whitePieces[currWhite].col = col;
		// 		game.whitePieces[currWhite].isKing = true;
		// 		game.whitePieces[currWhite].highlight = false;
		// 		game.whitePieces[currWhite].alive = true;
		// 		++currWhite;
		// 		break;
		// 	default:
		// 		break;
		// 	}
		// }
		// for (currRed; currRed < game.teamPieceCount; ++currRed)
		// {
		// 	game.redPieces[currRed].row = 69;
		// 	game.redPieces[currRed].col = 69;
		// 	game.redPieces[currRed].isKing = false;
		// 	game.redPieces[currRed].highlight = false;
		// 	game.redPieces[currRed].alive = false;
		// }
		// for (currRed; currWhite < game.teamPieceCount; ++currWhite)
		// {
		// 	game.whitePieces[currWhite].row = 69;
		// 	game.whitePieces[currWhite].col = 69;
		// 	game.whitePieces[currWhite].isKing = false;
		// 	game.whitePieces[currWhite].highlight = false;
		// 	game.whitePieces[currWhite].alive = false;
		// }
		// *** END REWRITE ZONE
	}
	if (!game.passiveStepMode)
		teamScoreBoard.timePerLastMove = performance.now() - teamScoreBoard.lastMoveStarted;
}


// TODO: Thoroughly test for correctness and implement NOW
GameBoard.prototype.updateChanged = function (color)
{
	// game.fileBoardState
	// game.playerBoardState

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
				var sameColor = (color == "red") ?
					(game.playerBoardState[piece] == '1' || game.playerBoardState[piece] == '3') :
					(game.playerBoardState[piece] == '2' || game.playerBoardState[piece] == '4');

        if (game.playerBoardState[piece] == '0') {
          var sameColor = (color == "red") ?
            (game.fileBoardState[piece] == '1' || game.fileBoardState[piece] == '3') :
            (game.fileBoardState[piece] == '2' || game.fileBoardState[piece] == '4');
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

		var currentTeam = (color == "red" ? game.redPieces : game.whitePieces);
		var enemyTeam = (color == "red" ? game.whitePieces : game.redPieces);
    console.log(pieceChangePool);
		for (var poolPiece = 0; poolPiece < pieceChangePool.length; ++poolPiece)
		{
			if (pieceChangePool[poolPiece].sameColor && pieceChangePool[poolPiece].pieceType == '0')
			{
        // An error occured if there was more than one same colored move
        if (previousColorPieceLocation.length >= 1) {
          alert("Invalid Move! Too many " + color.toUpperCase() + " pieces moved!\n Before Move: " +
          game.playerBoardState + "\n After Move: " + game.fileBoardState);
        }
				previousColorPieceLocation = pieceChangePool[poolPiece].pieceLocation;
			}
			else if (pieceChangePool[poolPiece].sameColor && pieceChangePool[poolPiece].pieceType != '0')
			{
				currentColorPieceLocation = pieceChangePool[poolPiece].pieceLocation;
			}
			else
			{
				// Case: delete jumped enemies
				indexToMove = this.spaceOccupied(pieceChangePool[poolPiece].pieceLocation, enemyTeam);
        console.log("Index to Delete: " + indexToMove);
				enemyTeam[indexToMove].alive = false;
				enemyTeam[indexToMove].row = -5;
				enemyTeam[indexToMove].col = -5;
			}
		}

    // Case: Color piece moved to new location
		var moveFromHere = this.spaceOccupied(previousColorPieceLocation, currentTeam);
		if (moveFromHere >= 0)
		{
      if (!currentTeam[moveFromHere].alive)
        alert("WARNING: MOVING GHOST PIECE!!!\nPREVIOUS BOARD STATE: " + game.playerBoardState +
        "\nCURRENT BOARD STATE: " + game.fileBoardState);

			currentTeam[moveFromHere].row = currentColorPieceLocation.row;
			currentTeam[moveFromHere].col = currentColorPieceLocation.col;
		}

	}
}

// Update selected teams pieces based on mouse click events
GameBoard.prototype.playerController = function (color)
{
	// Store both team arrays by reference
	var teamArr = (color == "red" ? game.redPieces : game.whitePieces);
	var teamOpp = (color == "red" ? game.whitePieces : game.redPieces);
	var teamScore = (color == "red" ? scoreboard.red : scoreboard.white);

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
		if (selected == -1 && oppselect == -1 && game.selectedSquare.index != -1)
		{
			if (Math.abs(teamArr[game.selectedSquare.index].row - game.selectedSquare.row) > 1 ||
				Math.abs(teamArr[game.selectedSquare.index].col - game.selectedSquare.col) > 1)
			{
				++teamScore.enemyCaptured;
			}

			// TODO: Add jump function here

			teamArr[game.selectedSquare.index].row = game.selectedSquare.row;
			teamArr[game.selectedSquare.index].col = game.selectedSquare.col;
			teamArr[game.selectedSquare.index].highlight = false;

			// King if reached end of board
			if ((game.selectedSquare.row == 7 && color == "red") || (game.selectedSquare.row == 0 && color == "white"))
				teamArr[game.selectedSquare.index].isKing = true;

			// Player made move, update string
			this.boardToString();

			// reset states so unnecessary work isn't done and piece stops being highlighted after placed
			game.lastMouseClick.x = -1;
			game.selectedSquare.index = -1;

			// Update scoreboard
			++teamScore.moveCount;
			teamScore.timePerLastMove = performance.now() / 1000 - teamScore.lastMoveStarted;
			teamScore.lastMoveStarted = 0;

		}

		// If player selected one of their color pieces, highlight the tile and store its array index
		if (selected != -1)
		{
			teamArr[selected].highlight = true;
			// deselect previously selected square if there was one
			if (game.selectedSquare.index != -1 && game.selectedSquare.index != selected)
			{
				teamArr[game.selectedSquare.index].highlight = false;
			}
			// Log start of move if not in middle of move
			if (!teamScore.lastMoveStarted)
				teamScore.lastMoveStarted = performance.now() / 1000;

			game.selectedSquare.index = selected;
		}
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
		if (spot.row == loc.row && spot.col == loc.col && spot.alive)
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
			if ((cols + rows) % 2 == 0)
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
		ctx.globalAlpha = game.highlightOpacity;
		ctx.fillStyle = game.highlightColor;
		ctx.fillRect(width / game.tileDim * col, width / game.tileDim * row, width / game.tileDim, width / game.tileDim);
		ctx.globalAlpha = 1;
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

		if (row % 2 == 0)
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
