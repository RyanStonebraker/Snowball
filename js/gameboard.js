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

// Global object literal to store various game state info
var game = {
  "tileDim" : 8,
  "teamPieceCount" : 12,
  "darkTileColor" : "black",
  "lightTileColor" : "white",
  "kingColor" : "gold",
  "redPieces" : [],
  "whitePieces" : [],
  "lastMouseClick" : {"x" : -1, "y" : -1},
  "pixelsPerTile" : 64,
  "selectedSquare" : {"row" : -1, "col" : -1, "index" : -1},
  "highlightColor" : "yellow",
  "highlightOpacity" : 0.5,
  "redTeamColor" : "red",
  "whiteTeamColor" : "lightgrey",
  "pause" : false,
  "fileBoardState" : ""
};

// Global object literal for keeping track of scoring and turns
var scoreboard = {
  "red" : {"moveCount" : 0,
           "pieceCount" : game.teamPieceCount,
           "kingCount" : 0,
           "lastMoveStarted" : 0,
           "timePerLastMove" : 0},
  "white" : {"moveCount" : 0,
          "pieceCount" : game.teamPieceCount,
          "kingCount" : 0,
          "lastMoveStarted" : 0,
          "timePerLastMove" : 0},
  "totalMoveCount" : 0,
  "startTime" : 0,
  "totalTime" : 0
};

// Main "class"/start function
function GameBoard(div) {
  if (!div)
    return null;

  canvas = div;

  if (canvas.getContext) {
    ctx = canvas.getContext("2d");
  }

  // Set initial location states for pieces
  this.generateTeams();

  scoreboard.startTime = performance.now();

  // Check board for update conditions
  this.refreshBoard();

  // Watch mouse click events on HTML5 canvas
  canvas.addEventListener("mousedown", this.mouseEventHandler.bind(this), false);
  window.addEventListener("keydown", this.keyEventHandler.bind(this), true);
}

// If mouse clicked, log location
GameBoard.prototype.mouseEventHandler = function(evt) {
  if (!game.pause) {
    var screen = canvas.getBoundingClientRect();
    game.lastMouseClick.x = evt.clientX - screen.left;
    game.lastMouseClick.y = evt.clientY - screen.top;
  }
}

GameBoard.prototype.keyEventHandler = function (evt) {
  switch (evt.keyCode) {
    case " ".charCodeAt():
    case "P".charCodeAt():
      game.pause = (game.pause ? false : true);
      break;
  }
}

// Update function to constantly redraw board
GameBoard.prototype.refreshBoard = function () {
  var self = this;
  requestAnimationFrame(function(){self.refreshBoard();});

  // Draw checkerboard
  this.drawBackground();

  // Draw both team arrays
  this.drawTeam(game.redPieces, game.redTeamColor);
  this.drawTeam(game.whitePieces, game.whiteTeamColor);

  if (!game.pause) {
    this.moveController();

    this.updateScoreBoard();
  }
  else {
    this.drawPause();
  }
}

// draws the pause icon on the screen
GameBoard.prototype.drawPause = function () {
  ctx.fillStyle = "blue";
  var pauseBarWidth = 80;
  var pauseBarHeight = 200;
  ctx.fillRect(width/2 - 1.25 * pauseBarWidth, height/2 - 0.5 * pauseBarHeight, pauseBarWidth, pauseBarHeight);
  ctx.fillRect(width/2 + 0.25 * pauseBarWidth, height/2 - 0.5 * pauseBarHeight, pauseBarWidth, pauseBarHeight);
}

// Check game state and write stats
GameBoard.prototype.updateScoreBoard = function () {
  // temp vars to hold count
  var redCount = 0;
  var redKings = 0;
  var whiteCount = 0;
  var whiteKings = 0;

  // iterate through all pieces and check if alive and if king
  for (var i = 0; i < game.teamPieceCount; ++i) {
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
  scoreboard.totalTime = Math.round((performance.now() - scoreboard.startTime)/10)/100;

  // replace text for scoreboard section
  if (scoreboard.red.moveCount + scoreboard.white.moveCount != scoreboard.totalMoveCount || scoreboard.totalTime < 0.01) {
    // update total move count
    scoreboard.totalMoveCount = scoreboard.red.moveCount + scoreboard.white.moveCount;
    var scoreText = "SCOREBOARD:</br></br>";
    scoreText += (scoreboard.red.moveCount == scoreboard.white.moveCount) ? "<h2>Red (ACTIVE TURN):</h2><p>" : "Red:<p>";
    scoreText += "Piece Count: " + scoreboard.red.pieceCount;
    scoreText += "</br>King Count: " + scoreboard.red.kingCount;
    scoreText += "</br>Last Move Time: " + Math.round(scoreboard.red.timePerLastMove*100, 0.01)/100 + "s";
    scoreText += (scoreboard.red.moveCount != scoreboard.white.moveCount) ? "</p></br><h2>White (ACTIVE TURN):</h2>" : "</p></br>White:";
    scoreText += "<p>Piece Count: " + scoreboard.white.pieceCount;
    scoreText += "</br>King Count: " + scoreboard.white.kingCount;
    scoreText += "</br>Last Move Time: " + Math.round(scoreboard.white.timePerLastMove*100, 0.01)/100 + "s</p>";

    scoreText += "</br>Total Move Count: " + scoreboard.totalMoveCount;
    $('.scoreboard').html(scoreText);
  }
  if (scoreboard.red.timePerLastMove || scoreboard.white.timePerLastMove || scoreboard.red.lastMoveStarted || scoreboard.white.lastMoveStarted) {
    $('.scoreboard_wrapper span').html((scoreboard.totalTime).toFixed(2) + "s");
  }
}

// Start team based gameplay
GameBoard.prototype.moveController = function () {
  // red's turn if red and white have same number of total moves
  if (scoreboard.red.moveCount == scoreboard.white.moveCount) {
    this.playerController("red");
  }
  else {
    this.playerController("white");
    // this.fileInputController("white");
  }
}

GameBoard.prototype.fileInputController = function (color) {
  // STANDARD: 0 = empty
  //           1 = red
  //           2 = black
  //           3 = red king
  //           4 = black king

  // Shadow State Standard:
  //        Line Number = checker piece number (1-32)
  //        First Number = STANDARD type (0,1,2,3,4)
  //        Second Number: 0 = Top left
  //                       1 = Top right
  //                       2 = Bottom left
  //                       3 = Bottom right
  //        Example: (Line 0) -> 1 01

  var teamScoreBoard = (color == "red" ? scoreboard.red : scoreboard.white);
  $.get("../comm/boardstate.txt", function(input) {
    var currRed = 0;
    var currWhite = 0;

    if (input != game.fileBoardState) {
      // game.fileBoardState = input;
      ++teamScoreBoard.moveCount;

      for (var spot = 0; spot < 32; ++spot) {
        var row = Math.floor(2 * spot / game.tileDim);
        var col = (2 * spot) % 8;

        switch (input[spot]) {
          case 1:
            game.redPieces[currRed].row = row;
            game.redPieces[currRed].col = col;
            game.redPieces[currRed].isKing = false;
            game.redPieces[currRed].highlight = false;
            ++currRed;
            break;
          case 2:
            game.whitePieces[currWhite].row = row;
            game.whitePieces[currWhite].col = col;
            game.whitePieces[currWhite].isKing = false;
            game.whitePieces[currWhite].highlight = false;
            ++currWhite;
            break;
          case 3:
            game.redPieces[currRed].row = row;
            game.redPieces[currRed].col = col;
            game.redPieces[currRed].isKing = true;
            game.redPieces[currRed].highlight = false;
            ++currRed;
            break;
          case 4:
            game.whitePieces[currWhite].row = row;
            game.whitePieces[currWhite].col = col;
            game.whitePieces[currWhite].isKing = true;
            game.whitePieces[currWhite].highlight = false;
            ++currWhite;
            break;
        }
        // for (;currRed < game.teamPieceCount; ++currRed) {
        //   game.redPieces[currRed].alive = false;
        // }
        // for (;currWhite < game.teamPieceCount; ++currWhite) {
        //   game.whitePieces[currWhite].alive = false;
        // }
      }
    }
  });
}

// Update selected teams pieces based on mouse click events
GameBoard.prototype.playerController = function (color) {
  // Store both team arrays by reference
  var teamArr = (color == "red" ? game.redPieces : game.whitePieces);
  var teamOpp = (color == "red" ? game.whitePieces : game.redPieces);
  var teamScore = (color == "red" ? scoreboard.red : scoreboard.white);

  // Use x position of last mouse click as a way to check if playerController has already been called
  if (game.lastMouseClick.x != -1) {
    // Find row/column of clicked spot
    game.selectedSquare.col = Math.floor(game.lastMouseClick.x/game.pixelsPerTile);
    game.selectedSquare.row = Math.floor(game.lastMouseClick.y/game.pixelsPerTile);

    // Check whether the player selected a game piece (-1 means no game piece from specified array)
    var selected = this.spaceOccupied(game.selectedSquare, teamArr);
    var oppselect = this.spaceOccupied(game.selectedSquare, teamOpp);

    // If player selected an empty tile and they previously selected their color piece, move piece here
    if (selected == -1 && oppselect == -1 && game.selectedSquare.index != -1) {
      teamArr[game.selectedSquare.index].row = game.selectedSquare.row;
      teamArr[game.selectedSquare.index].col = game.selectedSquare.col;
      teamArr[game.selectedSquare.index].highlight = false;

      // reset states so unnecessary work isn't done and piece stops being highlighted after placed
      game.lastMouseClick.x = -1;
      game.selectedSquare.index = -1;

      // Update scoreboard
      ++teamScore.moveCount;
      teamScore.timePerLastMove = performance.now() / 1000 - teamScore.lastMoveStarted;
      teamScore.lastMoveStarted = 0;

    }

    // If player selected one of their color pieces, highlight the tile and store its array index
    if (selected != -1) {
      teamArr[selected].highlight = true;
      // deselect previously selected square if there was one
      if (game.selectedSquare.index != -1 && game.selectedSquare.index != selected) {
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
GameBoard.prototype.spaceOccupied = function (loc, arr) {
  for (var i = 0; i < arr.length; ++i) {
    var spot = arr[i];
    if (spot.row == loc.row && spot.col == loc.col) {
      return i;
    }
  }
  return -1;
}

// draws the checker board background onto canvas
GameBoard.prototype.drawBackground = function () {
  ctx.fillStyle = game.lightTileColor;
  ctx.fillRect(0, 0, width, height);
  ctx.fillStyle = game.darkTileColor;
  for (var cols = 0; cols < game.tileDim; ++cols) {
    for (var rows = 0; rows < game.tileDim; ++rows) {
      if ((cols + rows) % 2 == 0)
        ctx.fillRect((width/game.tileDim * rows) % width, height/game.tileDim * cols, width/game.tileDim, height/game.tileDim);
    }
  }
}

// draws a checker piece based on information stored in object
GameBoard.prototype.drawPiece = function (col, row, color, king, highlight) {
  // Prevent out of bounds errors
  if (col < 0 || row < 0 || col >= game.tileDim || row >= game.tileDim)
    return;

  var radius = width/(2*game.tileDim + 2);
  ctx.beginPath();
  ctx.arc(width/(2 * game.tileDim) + width/game.tileDim * col, width/(2 * game.tileDim) + width/game.tileDim * row, radius, 0, 2 * Math.PI, false);
  ctx.fillStyle = color;
  ctx.fill();

  if (king) {
    ctx.beginPath();
    ctx.arc(width/(2 * game.tileDim) + width/game.tileDim * col, width/(2 * game.tileDim) + width/game.tileDim * row, radius/2, 0, 2 * Math.PI, false);
    ctx.lineWidth = 10;
    ctx.strokeStyle = game.kingColor;
    ctx.stroke();
  }

  if (highlight) {
    ctx.globalAlpha = game.highlightOpacity;
    ctx.fillStyle = game.highlightColor;
    ctx.fillRect(width/game.tileDim * col, width/game.tileDim*row, width/game.tileDim, width/game.tileDim);
    ctx.globalAlpha = 1;
  }
}

// resets game pieces into starting configuration
GameBoard.prototype.generateTeams = function () {
  game.redPieces = []
  game.whitePieces = []

  for (var i = 0; i < game.teamPieceCount; ++i) {
    var col = (i * 2) % game.tileDim;
    var row = Math.floor((i*2)/game.tileDim);
    if (row % 2 == 1)
      ++col;
    game.redPieces.push({"col" : col, "row" : row, "isKing" : false, "highlight" : false, "alive" : true});
    if (row % 2 == 0)
      ++col;
    else {
      --col;
    }
    game.whitePieces.push({"col" : col, "row" : row + game.tileDim - game.teamPieceCount/(Math.ceil(game.tileDim/2)), "isKing" : false, "highlight" : false, "alive" : true});
  }
}

// Draws every checker piece for specified team
GameBoard.prototype.drawTeam = function (pieceArr, color) {
  var self = this;
  pieceArr.forEach(function (piece) {
    self.drawPiece(piece.col, piece.row, color, piece.isKing, piece.highlight);
  });
}
