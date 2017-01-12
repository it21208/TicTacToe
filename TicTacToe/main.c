#include <stdio.h>
#include <stdlib.h>
// �������� ��������
typedef struct {   //�� ������ struct ��� �� board.
        int **a;
        int size;
} _board;

typedef _board *board; //To board ����� ������� ��� _board.

typedef struct{ // �� struct ��� �� ���� ���� ������������ �� 'X' � 'O'. 
        int row,col;
} position;

typedef struct {
	char firstPlayer,secondPlayer ;
} _playerSymbols;

_board _triliza;
board triliza = &_triliza;
int freeCells;

#define BOARD_X 'X'
#define BOARD_O 'O'
#define BOARD_ '_'



// ��������� �����������
void printCurrentBoardStatus(void); 
char askFirstPlayer(void);
int createBoard(int N);
void displayBoard();
int main(int argc, char *argv[]) //������ � main ��������� 
{
	_playerSymbols symbol ;
	char firstPlayerSymbol;
	int N;
    if (argc != 2) {
        fprintf(stderr,"Wrong number of argument count!");         
        exit(1);
    }
          
    if((N=atoi(argv[1])) < 3) {
        fprintf(stderr,"Wrong argv[1]!");
        exit(2);
    }
	if (!createBoard(N)) {
		fprintf(stderr, " ");
		exit(3);		
	}
	
	displayBoard();
	/*
	firstPlayerSymbol = askFirstPlayer();                               
	if (firstPlayerSymbol == 'X')
	{
		symbol.firstPlayer = 'X';
		symbol.secondPlayer = 'O';
	}
	if	(firstPlayerSymbol == 'O')
	{
		symbol.firstPlayer = 'O';
		symbol.secondPlayer = 'X';
	}                                                                                                                                                                                                 
  system("PAUSE"); */	
  return 0;
}// ����� ���������� main

char askFirstPlayer() {
	char y;
	do {
	// ���� ��� 1� ������ �� �������� �� ������ �� �� "�" � �� "�".
	printf("Player 1 : Enter the symbol you would like to play with, in "
			"capital letters\"X\" or \"O\".");
	scanf("%c",&y);
	} while ((y != 'X') && (y != 'O'));
	return y;	
}

int createBoard(int N) {
	int row, col, **a;
	
	freeCells = N*N;
	triliza->a = (int**) malloc(N*sizeof(int*));
	a = triliza->a;
	for(row = 0; row < N; row++) {
		a[row] = (int*) malloc(N*sizeof(int));
		for(col = 0; col < N; col++)
			a[row][col] = (int)BOARD_;
	}
	return 1;
}

void displayBoard() {
		int row, col;
	
	for(row = 0; row < triliza->size; row++) {
		for(col = 0; col < triliza->size; col++)
			printf("%c  ", triliza->a[row][col]);
		printf("\n");
	}
}
