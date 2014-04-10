/*
Conway's Game Of Life 
Programmed by Christopher Gorsky
Started: April 7th 2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

typedef struct {
	unsigned int width, height;
} Dimension;

unsigned int _active;
unsigned int _inActive;
unsigned int _generation;

char** _matrix = NULL;
Dimension _matrixDim;

Dimension getFileDimensions(FILE* file);
uint8_t getNeighborCount(int x, int y);
bool load(const char* path);
void printTitle(void);
void drawMatrix(void);
void cleanUp(void);
void clearScreen(void);
void run(void);
void update(void);

int main(int argc, char* argv[]) {
    
	clearScreen();
    
	/*
     If there are two arguments then the file name has been provided
     Otherwise default to the file board.txt
     */
	switch (argc) {
            
		case 2:
            
			if (load(argv[1])) {
				run();
				cleanUp();
			} else {
				printf("Failed to load '%s' :( \n", argv[1]);
				return 1;
			}
            
            break;
            
		default:
			//load default file
			if (load("board.txt")) {
				run();
				cleanUp();
			} else {
				printf("Failed to load 'board.txt' :( \n");
				return 1;
			}
            
            break;
	}
    
    clearScreen();
	return 0;
}

//loads a file and allocates the array
bool load(const char* path) {
    
    //The file pointer to be used
    FILE* file = NULL;
	
    //If on windows use fopen_s otherwise do it the normal way
    #ifdef _WIN32
        fopen_s(&file, path, "r");
    #else
        file = fopen(path, "r");
    #endif
    
	//if a file is loaded proceed
	if (file != NULL) {
        
        _matrixDim = getFileDimensions(file);
        
        //allocate the array
        _matrix = (char**) malloc(_matrixDim.width * sizeof(char*));
        for (unsigned int i = 0; i < _matrixDim.width; i++) {
            _matrix[i] = (char*) malloc(_matrixDim.height * sizeof(char));
        }
        
		//reset the file counter
		rewind(file);
        
		//load the data from the text file to populate the array
		char temp;
		for (unsigned int y = 0; y < _matrixDim.height; y++) {
			for (unsigned int x = 0; x < _matrixDim.width; x++) {
               
                //pull a character from the file
				temp = (char) fgetc(file);
				if (temp == 'x' || temp == 'X') {
					_matrix[y][x] = 'X';
					_active++;
				} else {
					_matrix[y][x] = ' ';
					_inActive++;
				}
			}
		}
        
		//close the file and return true
		fclose(file);
		return true;
        
	} else {
		return false;
	}
}

//figures out the dimensions of the grid based on the provided file
Dimension getFileDimensions(FILE* file) {
    
    Dimension dim;
    dim.width = 0;
    dim.height = 0;
    
	//if the file good proceed
	if (file != NULL) {
        
		bool onFirstLine = true;
		char temp;
        
		do {
            
			//read a char from the fiel
			temp = getc(file);
            
			//if it is the first line get the length
			if (onFirstLine) {
                dim.width++;
			}
            
			//a \n indicates a new line, add one to the line counter
			if (temp == '\n') {
                dim.height++;
				onFirstLine = false;
			}
            
            //break once we have hit the end of the file
		} while (!feof(file));
        
		//increment the height counter for the null terminator
		_matrixDim.height++;
        return dim;
        
	} else {
		dim.width = 0;
        dim.height = 0;
        return dim;
        
	}
}

//draws the grid
void drawMatrix(void) {
    
	for (unsigned int y = 0; y < _matrixDim.height; y++) {
		for (unsigned int x = 0; x < _matrixDim.width; x++) {
			printf("%c", _matrix[y][x]);
		}
		printf("\n");
	}
}

//clears the screen
void clearScreen(void) {
	system(CLEAR);
}

//gets the count of neighbors for the given space
uint8_t getNeighborCount(int x,int y) {
    
	uint8_t count = 0;
	bool debug = false;
    
	//top center
	if (y - 1 >= 0) {
		if (_matrix[y - 1][x] == 'X') {
			if (debug) { printf("top center\n"); }
			count++;
		}
	}
    
	//top left
	if (y - 1 >= 0 && x - 1 >= 0) {
		if (_matrix[y - 1][x - 1] == 'X') {
			if (debug) { printf("top left\n"); }
			count++;
		}
	}
    
	//top right
	if (y - 1 >= 0 && x + 1 < _matrixDim.width) {
		if (_matrix[y - 1][x + 1] == 'X') {
			if (debug) { printf("top right\n"); }
			count++;
		}
	}
    
	//middle left
	if (x - 1 >= 0) {
		if (_matrix[y][x - 1] == 'X') {
			if (debug) { printf("mid left\n"); }
			count++;
		}
	}
    
	//middle right
	if (x + 1 < _matrixDim.width) {
		if (_matrix[y][x + 1] == 'X') {
			if (debug) { printf("top right\n"); }
			count++;
		}
	}
    
	//bottom center
	if (y + 1 < _matrixDim.height) {
		if (_matrix[y + 1][x] == 'X') {
			if (debug) { printf("btm center\n"); }
			count++;
		}
	}
    
	//bottom left
	if (y + 1 < _matrixDim.height && x - 1 >= 0) {
		if (_matrix[y + 1][x - 1] == 'X') {
			if (debug) { printf("btm left\n"); }
			count++;
		}
	}
    
	//bottom right
	if (y + 1 < _matrixDim.height && x + 1 < _matrixDim.width) {
		if (_matrix[y + 1][x + 1] == 'X') {
			if (debug) { printf("btm right\n"); }
			count++;
		}
	}
    
	//debug shit
	if (debug && count > 0) {
		printf("%s (%i,%i) - %i\n", (_matrix[y][x] == 'X') ? "ON" : "OFF", x, y, count);
        printf("--------------------------\n");
	}
    
	//return the amount of adjacent active spaces
	return count;
}

// checks the rules against each square and keeps track of on and off pieces
void update(void) {
    
	_active = 0;
	_inActive = 0;
	uint8_t neighborCount = 0;
    
	for (int y = 0; y < _matrixDim.height; y++) {
		for (int x = 0; x < _matrixDim.width; x++) {
            
			neighborCount = getNeighborCount(x, y);
            
			//checks the different rules for the game
			if (_matrix[y][x] == 'X') {
                
				if (neighborCount < 2) {
					_matrix[y][x] = ' ';
				}
				else if (neighborCount > 3) {
					_matrix[y][x] = ' ';
				}
                
			} else {
                
				if (neighborCount == 3) {
					_matrix[y][x] = 'X';
				}
                
			}
            
			//keeps track of the amount of active spaces
			if (_matrix[y][x] == 'X') {
				_active++;
			} else {
				_inActive++;
			}
		}
	}
}

//prints out the title
void printTitle(void) {
	printf("\tConway's Game of Life\n");
	printf("\tBy Christopher Gorsky\n\n");
	printf("\tGeneration: %i\n", _generation);
	printf("\tOn: %i ", _active);
	printf("Off: %i\n\n", _inActive);
	printf("  Press/Hold the \"Return\" key to cycle.\n");
	printf("-----------------------------------------\n");
}

//the core loop of the application keep running until the user types q
void run(void) {
    
	_generation = 0;
	char input;
    
	do {
        
		printTitle();
		drawMatrix();
        
		input = getchar();
        
		clearScreen();
		update();
        
		_generation++;
        
	} while (input != 'q' && input != 'Q');
    
}

/*
Frees all resources that were dynamically allocated earlier
Crashes for some reason...
*/
void cleanUp(void) {
    
	for (int y = 0; y < _matrixDim.height; y++) {
		for (int x = 0; x < _matrixDim.width; x++) {
			free(&_matrix[y][x]);
		}
	}
    
	free(_matrix);
}
