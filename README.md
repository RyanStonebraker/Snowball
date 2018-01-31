# Snowball
Snowball: An Artificially Intelligent Checkers player.
____

Developer Dependencies:

  - node.js
  - node package manager
  - electron

Example frontend build setup:

```
$ brew install node
$ npm install electron -g

$ electron index.js
```
Example backend build setup:

```
$ cd c++
$ make
$ bin/ai_run
```
Running tests:

```
$ cd c++
$ make ai_test
$ bin/ai_test
```
____

## File Communication Standards

### Handshake Protocol : handshake0.txt
```js
          Sent from Interface

          First number: Starting Controller
                0 = Computer
                1 = Player

          Second number: Computer Color
                0 = red
                1 = black

          Example: 01 = Computer starts first as black.
                   10 = Player starts first as red.
```

### Handshake Protocol : handshake1.txt
```js
          Sent from Computer

          First number: Response Status
                0 = Resend handshake0.txt
                1 = Start Game
```

### Board State : boardstate*.txt
```js
          Increment name depending on move count

          0 = empty
          1 = red
          2 = black
          3 = red king
          4 = black king

          Example: 11111111111100000000222222222222 - default board
```

### Shadow State Standard : shadowstate.txt
```js
       LineNumber = Board Spot: (1-32)

       First Num = Board State Type: (0,1,2,3,4)

       Second Num:   -1 = No Valid Move
                      0 = Top left
                      1 = Top right
                      2 = Bottom left
                      3 = Bottom right
       Example:
            --shadowstate.txt--
              1 -1   // Red piece at (0,0) can't move
              1 23   // Red piece at (0,2) can move down left/right
              3 0123 // Red king at (0,4) can move to any diagonal spot
              2 1    // Black piece at (0,6) can only move diagonally right
              4 023  // Black king at (1,0) can move everywhere except top right
```
