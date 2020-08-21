# Memory game for Arduino
<p>
	A memory game for Arduino Uno. It displays a sequence of arrows based on settings chosen in menu and checks whether a user's input is correct.
</p>

## Table of contents
* [Technologies](#technologies)
* [Software](#software)
* [Hardware](#hardware)
* [User manual](#user-manual)

## Technologies:
* C/C++

## Software:
* [Arduino IDE](https://www.arduino.cc/en/Guide)
	
## Hardware:
* Arduino Uno
* I2C LCD 16x2
	
## USER MANUAL:

	Use of characters in menu:
	* ">" - press button RIGHT if you wish to change to a given option
	* "<" - press button LEFT if you wish to change to a given option
	* "^" - press button UP if you wish to change to a given option
	* special character consisting of up and down arrow ("^" and a similar one pointing down) - use buttons 
			UP and DOWN to change displayed value
	* M - number of different elements in the set from which sequence is created
	* N - length of seguence that you need to remember
	* T - time for your input of a next element during the game
	* D - time for which element will be displayed on a screen for you to remember it

	Before the first game in story mode you need to use EEPROM change (press button UP) to set the initial values in
	ranking to 0 points. In the future you can use this function whenever you would like to restart RANKING.

	In the menu you can choose from two modes: STORY and PRACTICE, or you can choose to set/change initial values 
	in ranking by choosing EEPROM.

	In PRACTICE MODE you can press SELECT to begin with default settings (M = 2, N =3, T = 2.0s, D = 1.0s)
	or RIGHT to change settings. If you wish to change values of N, M, T or D use buttons UP and DOWN to 
	change values and LEFT and RIGHT to navigate between changed varaibles. Then press SELECT to start
	the game.

	During the game, first, the sequence will be displayed, then you will be asked for input 
	(suitable message - "ENTER ANSWER" - will be displayed on the screen). After N inputs your
	sequence will be checked and if it is correct you will either be able to continue with the 
	game and a sequence of length N + 1 or go back to menu.
	If it is incorrect you will be able to try once again with the sequence of length N or go back to menu.

	In STORY MODE you can either see ranking (best three results; use UP and DOWN to scroll between 
	them) or choose a level you wish to begin from. Levels are:
	1) EASY:
		* M = 2
		* N = 5
		* D = 2.8s
		* T = 3.0s
		And your points will be calculated using 1*M*N (for the highest M and N you reached)
	2) MEDIUM:
		* M = 3
		* N = 6
		* D = 1.8s
		* T = 2.0s
		And your points will be calculated using 2*M*N (for the highest M and N you reached)
	3) HARD:
		* M = 4
		* N = 7
		* D = 1.2s
		* T = 1.5s
		And your points will be calculated using 3*M*N (for the highest M and N you reached)

	After correct input you will move on to a sequence of length N+1. When you reach N = 12, 
	you will move to M = M+1 and N = 6.
	If you beat one of the current highest results in ranking you can choose to either continue with the game
	to get an even better result or save your current result (however, you will need to start the game again).
