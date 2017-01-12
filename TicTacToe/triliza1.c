#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//Το βασικό struct για το board
typedef struct {
	int **a;
	int size;
} _board;

//Το board είναι δείκτης στο _board
typedef _board* board;

//Το struct για τη θέση όπου τοποθετείται το ‘Χ’ ή ‘Ο’
typedef struct {
	int row,col;
} position;

// enum για επιτρεπτές τιμές στην τρίλιζα
enum board_values { BOARD_X = 'X', BOARD_O = 'O', BOARD_ = '_' };

// enum για παίκτες στην τρίλιζα
enum board_players { PLAYER_1 = 0, PLAYER_2 = 1 };

// Πρότυπα συναρτήσεων που χρειάζεται η main()
int createBoard(int N, board B);
void displayBoard(board B);
char *playerName(int player);
int getPlayersSymbolChoices(int player_symbol[]);
void displayPlayersSymbolChoices(int player_symbol[]);
int getPlayerForSymbol(int symbol, int player_symbol[]);
position getPlayersMove(int Player, int N);
int makePlayersMove(position pos, int player, int player_symbol[],
					board B);
int isGameOver(int player_symbol[], board B, int *isDraw, int *playerWinner);
void displayGameResult(int isDraw, int playerWinner);

int main(int argc, char *argv[])
{
	char *progName = argv[0];	// Όνομα προγράμματος
	int N;						// Διάσταση τρίλιζας (N x N)
	_board _triliza;			// η Τρίλιζα
	board triliza = &_triliza;	// δείκτης στην Τρίλιζα
	int nextPlayer;
	int player_symbol[2];	// πίνακας με το σύμβολο κάθε παίκτη
	int isDraw;
	int playerWinner;


	
	/* αν δεν υπάρχει ακριβώς 1 παράμετρος, βγάλε μήνυμα
	 * λάθους και κάνε έξοδο */
	if (argc != 2) {
		fprintf(stderr, "Σύνταξη: %s N\n", progName);
		exit(1);
	}
	
	/* Έλεγχος της παραμέτρου εάν είναι ακέραιος >= 3.
	 * Για τιμές 1 και 2 δεν είναι παιχνίδι γιατί ο πρώτος
	 * κερδίζει υποχρεωτικά!!! */
	 if ((N = atoi(argv[1])) < 3) {
		fprintf(stderr, "Σύνταξη: %s N\nΝ >= 3\n", progName);
		exit(2);
	 }

	// Δημιουργία του board
	if (!createBoard(N, triliza)) {
		fprintf(stderr, 
			"Λάθος: Δεν μπόρεσε να δημιουργηθεί η τρίλιζα");
		exit(3);		
	}
	
	// Επιλογή συμβόλου από τον Παίκτη 1
	if (!getPlayersSymbolChoices(player_symbol)) {
		fprintf(stderr, 
			"Λάθος: Δεν έγινε επιλογή συμβόλου!");
		exit(4);
	} else 
		displayPlayersSymbolChoices(player_symbol);
	
	// Ξεκινάει το παιχνίδι με πρώτο τον Παίκτη 1
	nextPlayer = PLAYER_1;
	do {
		// Παίζει ο επόμενος Παίκτης - έλεγχος διαθεσιμότητας
		while(!makePlayersMove(getPlayersMove(nextPlayer, N), 
					nextPlayer, player_symbol, triliza))
			printf("Λάθος: το κελί αυτό είναι κατειλημένο!\n");
		// Εμφάνιση Τρίλιζας
		displayBoard(triliza);
		// Αλλαγή παίκτη
		nextPlayer = (nextPlayer == PLAYER_1) ? PLAYER_2 : PLAYER_1;
		// επανάληψη μέχρι να τελειώσει το παιχνίδι
	} while (!isGameOver(player_symbol, triliza,
					&isDraw, &playerWinner));
					
	// Εκτύπωση αποτελέσματος
	displayGameResult(isDraw, playerWinner);
	return 0;
}


/* name: createBoard
 * @param: N int, B board
 * @return: int (0 failure, 1 for sucess)
 * @descr: Δημιουργία της Τρίλιζας και αρχικοποίησή της
 */
int createBoard(int N, board B) {
	int **a, *b, len;
	
	// αποθήκευση του N στο B->size
	B->size = N;
	// δέσμευση N (int *) που θα δείχνουν στις γραμμές
	B->a = (int **)malloc(B->size*sizeof(int *));
	// αν αποτύχει, επιστροφή με λάθος
	if (B->a == NULL) 
		return 0;
	a = B->a;
	while (N) {
		// δέσμευση Ν (int) που θα είναι μια γραμμή
		*a = (int *)malloc(B->size*sizeof(int));
		if (*a == NULL)
			break;
		else {
			// αρχικοποίηση τιμών γραμμής
			b = *a;
			len = B->size;
			while (len--)
				*b++ = (int)BOARD_;
			// επόμενη γραμμή
			N--;
			a++;
		}
	}
	/* αν μετά το βρόχο ισχύει N > 0, τότε σημαίνει ότι κάποιο
	 * malloc() απέτυχε, επομένως πρέπει .. */
	if (N > 0) {
		// να αποδεσμευτούν όλες οι προηγούμενες γραμμές
		while (N++ < B->size) {
			free((void *)*(--a));
		}
		// να αποδεσμευτεί το B->a
		free((void *)B->a);
		return 0;
	} else 
		return 1;
}

/* name: displayBoard
 * @param: B board
 * @return: -
 * @descr: Εκτυπώνει την Τρίλιζα
 */
void displayBoard(board B) {
	int **a, *b, noRows, noCols;

	if (B == NULL) {
		fprintf(stderr, "Λάθος: Δεν υπάρχει Τρίλιζα!\n");
		return;
	}
	noRows = B->size;
	a = B->a;
	while (noRows--) {
		b = *a;
        noCols = B->size;
		while (noCols--) {
			printf("%4c", *b);
			b++;
		}
		printf("\n");
		a++;
	}
}

/* name: playerName
 * @param: player int
 * @return: char *
 * @descr: Επιστρέφει το όνομα του παίκτη
 */
char *playerName(int player) {
	return (player == PLAYER_1) ? 
				"ΠΑΙΚΤΗΣ 1" :
				((player == PLAYER_2) ? 
					"ΠΑΙΚΤΗΣ 2" : 
					(char *)NULL);
}

/* name: getPlayersSymbolChoices
 * @param: -
 * @return: int (0 failure, 1 for sucess)
 * @descr: Διαβάζει την επιλογή συμβόλου για τους παίκτες και
 *         αρχικοποιεί τον πίνακα player_symbol
 */
int getPlayersSymbolChoices(int player_symbol[]) {
	int symbol;

	// προτροπή στον Παίκτη 1 για επιλογή συμβόλου
	do {
		printf("%s, επιλέξτε σύμβολο (X ή O) : ", playerName(PLAYER_1));
		if (scanf(" %c", &symbol) == 1)
			symbol = toupper(symbol);
		else
			return 0;
	} while ((symbol != BOARD_X) && (symbol != BOARD_O));
	player_symbol[PLAYER_1] = symbol;
	// το σύμβολο που απέμεινε δίνεται στον Παίκτη 2
	player_symbol[PLAYER_2] = (symbol == BOARD_X) ? BOARD_O : BOARD_X;
	return 1;
}

/* name: displayPlayersSymbolChoices
 * @param: -
 * @return: -
 * @descr: Εκτυπώνει τις επιλογές συμβόλων των παικτών
 */
void displayPlayersSymbolChoices(int player_symbol[]) {
	printf("ΣΥΜΒΟΛΟ(%s) = %c\n", 
		playerName(PLAYER_1),
		player_symbol[PLAYER_1]);
	printf("ΣΥΜΒΟΛΟ(%s) = %c\n", 
		playerName(PLAYER_2),
		player_symbol[PLAYER_2]);
}

int getPlayerForSymbol(int symbol, int player_symbol[]) {
	int i;
	long size = sizeof(player_symbol) / sizeof(int);
	
	for (i = 0; i < size; i++)
		if (symbol == player_symbol[i])
			return i;
	return -1;
}


/* name: getPlayersMove
 * @param: player int, N int
 * @return: position
 * @descr: Διαβάζει από τον παίκτη τη θέση όπου θέλει
 * 		   να παίξει
 */
position getPlayersMove(int player, int N) {
	position pos;
	int done;
	// Προπτροπή στον player να επιλέξει κελί
	do {
		printf("%s> επιλέξτε (σειρά/στήλη) : ", playerName(player));
		done = (scanf("%d/%d", &pos.row, &pos.col) == 2) &&
				(pos.row > 0) && (pos.col > 0) &&
				(pos.row <= N) && (pos.col <= N);
	} while (!done);
	return pos;
}

/* name: makePlayersMove
 * @param: pos position, player int, player_symbol[] int, board B
 * @return: int (0 failure, 1 for sucess)
 * @descr: Επιστρέφει true εάν η θέση ήταν άδεια και
 *         τοποθετεί το σύμβολο του παίκτη στο κελί
 */
int makePlayersMove(position pos, int player, int player_symbol[],
					board B) {
	int **a, *b;
	
	a = B->a;
	// Μετακίνηση στην επιθυμητή γραμμή
	a = a + pos.row - 1;
	b = *a;
	// Μετακίνηση στην επιθυμητή στήλη
	b = b + pos.col - 1;
	// αν είναι κενό το κελί
	if (*b == BOARD_) {
		// βάλε την επιλογή του παίκτη και
		*b = player_symbol[player];
		// επέστρεψε επιτυχώς
		return 1;
	} else {	// αλλιώς
		// επέστρεψε ανεπιτυχώς
		return 0;
	}
}

int doesRowWin(int row, board B, int *winSymbol);
int doesColWin(int col, board B, int *winSymbol);
int doesDiagWin(int diagNo, board B, int *winSymbol);

/* name: isGameOver
 * @param: player_symbol[] int, B board, isDraw int, 
 * 		   playerWinner *int
 * @return: int (0 game continues, 1 gave over)
 * @descr: Επιστρέφει true εάν συμπληρώθηκε και η τελευταία θέση και
 * 		   μόνο τότε συμπληρώνει το isDraw εάν έχουμε ισοπαλία και το
 * 		   *playerWinner με τον νικητή
 */
int isGameOver(int player_symbol[], board B, 
	int *isDraw, int *playerWinner) {
	int emptyCells, emptyCellsLeft, row, col,
		winSymbol, winnerFound;
	
	// Εύρεση πιθανού νικητή
	// 1.Έλεγχος διαγωνίων 1, 2
	winnerFound = (doesDiagWin(1, B, &winSymbol) ||
				   doesDiagWin(2, B, &winSymbol));
	// 2.Αν δε βρέθηκε στις διαγωνίους κάνε έλεγχο γραμμών
	if (!winnerFound) {
		for (row = 0; row < B->size; row++) {
			winnerFound = winnerFound || doesRowWin(row, B, &winSymbol);
			if (winnerFound)
				break;
		}
	}
	// 3.Αν δε βρέθηκε στις γραμμές κάνε έλεγχο στηλών
	if (!winnerFound) {
		for (col = 0; col < B->size; col++) {
			winnerFound = winnerFound || doesColWin(col, B, &winSymbol);
			if (winnerFound)
				break;
		}
	}
	// Έλεγχος αν έχει μείνει άδειο κελί
	emptyCells = 0;
	for (row = 0; row < B->size; row++)
		for (col = 0; col < B->size; col++)
			if (*(*(B->a + row) + col) == BOARD_)
				emptyCells++;
	emptyCellsLeft = (emptyCells > 0);	
	// Ισοπαλία = Δεν βρέθηκε νικητής ΚΑΙ Δεν έμειναν κενά κελιά
	*isDraw = (!winnerFound) && (!emptyCellsLeft);
	// Αν βρέθηκε νικητής βρίσκει τον αριθμό του
	if (winnerFound)
		*playerWinner = getPlayerForSymbol(winSymbol, player_symbol);
	// Τελείωσε ή όχι το παιχνίδι;
	return ((!emptyCellsLeft) || (winnerFound));
}
		
		// 0 <= row < N
int doesRowWin(int row, board B, int *winSymbol) {
	int col, firstSymbol, curSymbol;

	// το πρώτο σύμβολο είναι στο κελί (row, 0)
	firstSymbol = *(*(B->a + row) + 0);
	/* έλεγχος αν όλα τα υπόλοιπα κελιά της γραμμής row
	 * έχουν την ίδια τιμή με το firstSymbol */
	for (col = 1; col < B->size; col++) {
		// διάβασε το τρέχων κελί στη θέση (row, col)
		curSymbol = *(*(B->a + row) + col);
		/* αν το τρέχων κελί είναι κενό ή διαφορετικό από
		 * το πρώτο κελί firstSymbol, έξοδος με false */
		if ((curSymbol == BOARD_) || 
		    (curSymbol != firstSymbol))
			return 0;
	}
	// ενημέρωση του συμβόλου που κερδίζει και έξοδος με true
	*winSymbol = firstSymbol;
	return 1;
}

		// 0 <= col < N
int doesColWin(int col, board B, int *winSymbol) {
	int row, firstSymbol, curSymbol;

	// το πρώτο σύμβολο είναι στο κελί (0, col)
	firstSymbol = *(*(B->a + 0) + col);
	/* έλεγχος αν όλα τα υπόλοιπα κελιά της στήλης col
	 * έχουν την ίδια τιμή με το firstSymbol */
	for (row = 1; row < B->size; row++) {
		// διάβασε το τρέχων κελί στη θέση (row, col)
		curSymbol = *(*(B->a + row) + col);
		/* αν το τρέχων κελί είναι κενό ή διαφορετικό από
		 * το πρώτο κελί firstSymbol, έξοδος με false */
		if ((curSymbol == BOARD_) || 
		    (curSymbol != firstSymbol))
			return 0;
	}
	// ενημέρωση του συμβόλου που κερδίζει και έξοδος με true
	*winSymbol = firstSymbol;
	return 1;
}

		
int doesDiagWin(int diagNo, board B, int *winSymbol) {
	int row, col, firstSymbol, curSymbol;

	// έλεγχος έγκυρων τιμών διαγωνίου (1,2)
	if ((diagNo < 1) && (diagNo > 2))
		return 0;
	/* το πρώτο σύμβολο είναι:
	 * για την 1η Διαγώνιο στο κελί (0, 0) και
	 * για τη  2η Διαγώνιο στο κελί (0, N-1) */ 
	firstSymbol = (diagNo == 1) ? 
					*(*(B->a + 0) + 0) :
					*(*(B->a + 0) + B->size - 1);
	/* έλεγχος αν όλα τα υπόλοιπα κελιά (row == col)
	 * έχουν την ίδια τιμή με το firstSymbol */
	for (row = 1; row < B->size; row++) {
		/* διάβασε το τρέχων κελί στη θέση (row, col)
		 * όπου το col είναι
		 * για την 1η Διαγώνιο col = row και
		 * για τη  2η Διαγώνιο col = (N-1) - row */ 
		col = (diagNo == 1) ? row : (B->size - 1 - row);
		curSymbol = *(*(B->a + row) + col);
		/* αν το τρέχων κελί είναι κενό ή διαφορετικό από
		 * το πρώτο κελί firstSymbol, έξοδος με false */
		if ((curSymbol == BOARD_) || 
		    (curSymbol != firstSymbol))
			return 0;
	}
	// ενημέρωση του συμβόλου που κερδίζει και έξοδος με true
	*winSymbol = firstSymbol;
	return 1;
}

void displayGameResult(int isDraw, int playerWinner) {
	printf("\nΑΠΟΤΕΛΕΣΜΑ ΠΑΙΧΝΙΔΙΟΥ\n");
	if (isDraw)
		printf("Ισοπαλία\n");
	else
		printf("Νικητής ο %s\n", playerName(playerWinner));
}
