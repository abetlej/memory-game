# memory-game
A memory game for Arduino Uno. It displays a sequence of arrows based on settings chosen in menu and checks whether a user's input is correct.



USER MANUAL:
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



TEST PLAN:

I) SETTING INITIAL EEPROM VALUES
	1) first, press LEFT to go into STORY mode
	2) press LEFT again to see ranking and use UP and DOWN to scroll between different "results"
	3) notice what the "results" are like (depending on what is saved in your Arduino's EEPROM they may be more
	or less meaningful)
	4) press RESET to go back to menu
	5) press button UP to change EEPROM settings
	6) follow instructions displayed on Arduino until you get back to menu
	7) repeat steps I.1 to I.3 (this time all three results should be "XXX: 0")
	8) repeat step I.4
	
II) PRACTICE MODE TEST 
	1) press RIGHT to get into practice mode
	II.a) default settings test
		1) press SELECT to begin 
		2) observe the screen and memorise the sequence
		Displayed sequence should consist of 3 arrows from the set {LEFT, UP}
		3) enter the sequence correctly
		(If you enter the sequence correctly you get a choice to move on to the sequence of length N = 4 or change settings.)
		4) press RIGHT to continue game 
		5) enter incorrect sequence to get an error message
		6) choose to "play again" (button RIGHT)
		7) do not enter anything until you get an error message
		8) press LEFT to go back to menu
		9) repeat step II.1
		10) press RIGHT again to go to settings changing
		11) set values of M, N, T, D to your liking (however, unless you want to test yourself, when testing for long sequences, 
		I would change D to a really low value and would not try to memorise the sequence)
		12) repeat steps II.a.8 to II.a.11 a few times for different values
		13) when you are ready to move to next stage of testing, press LEFT to go back to menu
	
III) STORY MODE TEST
	1) open Serial monitor (it will be used later)
	2) press LEFT when in menu
	III.a) ranking display
	1) press LEFT to see ranking
	2) use UP and DOWN to scroll between results
	3) use RIGHT to go back to story menu
	III.b) story mode game 
	1) press RIGHT to go to level choice
	2) choose a level and observe screen to check if sequence is implemented correctly 
	(use "level specification" provided in Instructions)
	3) do not enter any answer and observe if you are automatically taken to menu
	III.c) saving your result in ranking
	(To be able to save your result in ranking you need to beat the lowest result in current ranking.)
	1) repeat steps III.1, III.2, III.b.1 (in this order) and choose EASY mode (press LEFT)
	2) observe screen and memorise the sequence
	3) repeat the sequence correctly
	4) when backlight is set to teal, press RIGHT to save your initials 
	5) follow instructions displayed on Arduino
	6) enter 3 initials, press ENTER and RESET your Arduino
	7) repeat steps III.2, III.a.1, III.a.2 (in this order) and see if your initials were saved
