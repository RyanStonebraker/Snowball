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
  "whiteTeamColor" : "lightgrey"
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

  // Check board for update conditions
  this.refreshBoard();

  // Watch mouse click events on HTML5 canvas
  canvas.addEventListener("mousedown", this.mouseEventHandler.bind(this), false);
}

// If mouse clicked, log location
GameBoard.prototype.mouseEventHandler = function(evt) {
  var screen = canvas.getBoundingClientRect();
  game.lastMouseClick.x = evt.clientX - screen.left;
  game.lastMouseClick.y = evt.clientY - screen.top;
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

  // Make red team controllable by human
  this.playerController("red");
}

// Update selected teams pieces based on mouse click events
GameBoard.prototype.playerController = function (color) {
  // Store both team arrays by reference
  var teamArr = (color == "red" ? game.redPieces : game.whitePieces);
  var teamOpp = (color == "red" ? game.whitePieces : game.redPieces);

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
    }

    // If player selected one of their color pieces, highlight the tile and store its array index
    if (selected != -1) {
      teamArr[selected].highlight = true;
      if (game.selectedSquare.index != -1 && game.selectedSquare.index != selected) {
        teamArr[game.selectedSquare.index].highlight = false;
      }
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
    game.redPieces.push({"col" : col, "row" : row, "isKing" : false, "highlight" : false});
    if (row % 2 == 0)
      ++col;
    else {
      --col;
    }
    game.whitePieces.push({"col" : col, "row" : row + game.tileDim - game.teamPieceCount/(Math.floor(game.tileDim/2)), "isKing" : false, "highlight" : false});
  }
}

// Draws every checker piece for specified team
GameBoard.prototype.drawTeam = function (pieceArr, color) {
  var self = this;
  pieceArr.forEach(function (piece) {
    self.drawPiece(piece.col, piece.row, color, piece.isKing, piece.highlight);
  });
}
