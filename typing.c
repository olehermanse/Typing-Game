/*
Terminal Typing Game in C
typing.c
2015-01-18
- olehermanse ( http://www.github.com/olehermanse )

World's most lazy license:
This piece of software is completely free and open source.
It is meant for educational purposes and provided "as is"
with no expressed or implied warranty of any kind. You are
free to use, alter, edit, share, perform or distribute the
source code in any way desired for any purpose, including
but not limited to commercial, personal, entertainment and
educational purposes.

Credit is always appreciated.
- olehermanse ( http://www.github.com/olehermanse )
*/

// Includes from standard library:
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// Standard delay function, 1 second:
void delay(){
	sleep(1);
}

// Deallocate memory used by ~4000 word dictionary
void freeDictionary(char** dict){
	if(dict == NULL)
		return;
	int i = 0;
	while(dict[i] != 0){	// Null terminated char* array
		free(dict[i]);
		dict[i] = 0;
		++i;
	}
}

// Pick a random word from dictionary dict and copy it to result:
void pickWord(char** dict, char* result){
	int len = 0;
	int i = 0;
	for(i = 0; dict[i] != NULL; ++i);
	len = i;
	i = rand()%len;
	sprintf(result, "%s", dict[i]);
}

// Loads a newline separated dictionary of english words:
char** loadDictionary(){
	FILE* fp = fopen("dictionary.txt", "r");
	if(fp == 0){
		printf("Could not find dictionary.\n");
		exit(0);
	}

	char line[256];
	char** dict = malloc(4096 * sizeof(char*));
	memset((void*)dict, 0, 4096 * sizeof(char*));

	int i = 0;
	while(i < 4095 && fgets(line,256,fp)){	// dict[4095] is always left as NULL
		line[strlen(line)-1] = 0;			// Replace newline with 0 terminating byte
		dict[i] = malloc(strlen(line)+1);	// Allocate
		sprintf(dict[i], "%s", line);		// Copy string
		++i;
	}
	fclose(fp);
	return dict;
}

// Append score to highscores.txt file:
void saveScore(char* name, int points){
	FILE* fp = fopen("highscores.txt", "a");
	fprintf(fp, "%d\n%s\n", points, name);
	fclose(fp);
}

// Reads all scores from file and prints the best one as well as no. of entries:
void readScore(){
	FILE* fp = fopen("highscores.txt", "r");
	if(fp == 0){
		printf("There is no high score.\n");
		return;
	}

	char line[256];
	int lines = 0;

	// Current entry:
	char name[256];
	int score = 0;

	// Best entry:
	char bestName[256];
	int bestScore = 0;

	// Read all lines in file:
	while(fgets(line,256,fp)){
		line[strlen(line)-1] = 0;				// Replace newline with 0 terminator
		++lines;
		if(lines%2 == 1){						// Score(points) is on every other line
			score = strtol(line, 0, 10);
		}else{									// Names are on the other lines
			sprintf(name, "%s", line);
			if(score > bestScore){				// Check if current score is better
				bestScore = score;
				sprintf(bestName, "%s", name);
			}
		}
	}
	fclose(fp);

	// Check if high score was found:
	if(bestScore == 0){
		printf("There is no high score.\n");
		return;
	}

	// Print high score:
	printf("The highest score is %s with %d points.\n", bestName, bestScore);
	if(lines>3)
		printf("There are %d entries in 'highscores.txt'\n", lines/2);
	delay();
}

// Starts a new game with a new player name:
void newGame(char** dict){
	if(dict == 0){
		printf("There is no dictionary.\n");
		return;
	}
	srand(time(0));								// New random seed from time

	// Get player name from terminal input
	char* name = 0;
	printf("What is your name?\n");
	size_t len = 0;
	getline(&name, &len, stdin);
	name[strlen(name)-1] = 0;					// Replace newline with 0 terminator

	// Countdown to game start:
	printf("Game Starting in 3..");
	delay();
	printf("2..");
	delay();
	printf("1..\n");
	delay();

	// Game variables:
	char word[64];
	char response[64];
	int points = 0;
	int i = 0;

	// Start game, record start time:
	time_t start = time(0);
	time_t current = 0;

	// Loop while time has not run out:
	while(current < 45){
		pickWord(dict, word);					// Get random word
		printf("Name: %s. Points: %d. Time: %ld.\n", name, points, current);
		printf("'%s'\n", word);
		printf(":>");
		scanf("%s", response);					// Get player answer

		// Compare player answer with word, award points per character:
		for(i = 0; word[i] != 0 && response[i] != 0; ++i){
			if(word[i] == response[i]){
				points += 1 + i/3;
			}
		}
		current = time(0) - start;
	}

	// Print and save score:
	printf("Your score: %d.\n", points);
	saveScore(name, points);
	delay();
}

// Main menu lets user start a new game, display high score or exit:
void mainMenu(char** dict){
	int choice = 0;
	while(choice < 1 || choice > 3){
		printf("\nMAIN MENU:\n");
		printf("[1] New game\n");
		printf("[2] High scores\n");
		printf("[3] Exit\n");
		printf(":>");
		scanf(" %d%*c", &choice);
	}
	switch(choice){
		case 1:{
			newGame(dict);
			break;
		}
		case 2:{
			readScore();
			break;
		}
		case 3:{
			printf("Goodbye.\n");
			freeDictionary(dict);
			exit(0);
			break;
		}
	}
}

// Main function prints a message, loads a dictionary and loops main menu indefinitely:
int main(int argc, char** argv){
	// Disable buffering on stdout:
	setbuf(stdout, NULL);

	// Print welcome message:
	printf("\n\nTyping game in C\n");
	printf("by olehermanse(http://www.github.com/olehermanse)\n");

	// Load dictionary from file:
	char** dict = loadDictionary();

	// Loop main menu indefinitely:
	while(1)
		mainMenu(dict);

	return 0;
}
