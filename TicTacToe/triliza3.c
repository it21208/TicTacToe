#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Το βασικό struct για το board
typedef struct {
	int **a;
	int size;
} _board;

//Τ ο board είναι δείκτης στο _board
typedef _board* board;

// Το struct για τη θέση όπου τοποθετείται το ‘Χ’ ή ‘Ο’
typedef struct {
	int row,col;
} position;

// enum για επιτρεπτές τιμές στην τρίλιζα
enum board_values { BOARD_X = 'X', BOARD_O = 'O', BOARD_ = '_' };

// enum για παίκτες στην τρίλιζα
enum board_players { PLAYER_1, PLAYER_2 };

// Το struct που κρατά όλο το μοντέλο του παιχνιδιού
typedef struct {
	board B;	// δείκτης στην υποχρεωτική δομή
	int freeCells; // πλήθος άδειων κελιών
	char *playerName[2]; // όνομα παίκτη
	int playerSymbol[2]; // σύμβολο παίκτη
	int result;	/* -2: παίζει ακόμη, -1 = ισοπαλία; 
					0 = player 1; 1 = player 2 */
	int winRow;
	int winCol;
	int winDiag;
} _triliza;

// δομή που κρατά τα στοιχεία του παιχνιδιού
_triliza triliza;


// Πρότυπα συναρτήσεων που χρειάζεται η main()
int createBoard(int N);
void displayBoard();
char *playerName(int player);
int getPlayersSymbolChoices();
void displayPlayersSymbolChoices();
int getPlayerForSymbol(int symbol);
position getPlayersMove(int Player);
int makePlayersMove(position pos, int player);
int isGameOver();
void displayGameResult();

int main(int argc, char *argv[])
{
	char *progName = argv[0];	// Όνομα προγράμματος
	int N;						// Διάσταση τρίλιζας (N x N)
	int nextPlayer;
	
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
	if (!createBoard(N)) {
		fprintf(stderr, 
			"Λάθος: Δεν μπόρεσε να δημιουργηθεί η τρίλιζα");
		exit(3);		
	}
	
	// Επιλογή συμβόλου από τον Παίκτη 1
	if (!getPlayersSymbolChoices()) {
		fprintf(stderr, 
			"Λάθος: Δεν έγινε επιλογή συμβόλου!");
		exit(4);
	} else 
		displayPlayersSymbolChoices();
	
	// Ξεκινάει το παιχνίδι με πρώτο τον Παίκτη 1
	nextPlayer = PLAYER_1;
	do {
		// Παίζει ο επόμενος Παίκτης - έλεγχος διαθεσιμότητας
		while(!makePlayersMove(getPlayersMove(nextPlayer), 
								nextPlayer))
			printf("Λάθος: το κελί αυτό είναι κατειλημένο!\n");
		// Εμφάνιση Τρίλιζας
		displayBoard();
		// Αλλαγή παίκτη
		nextPlayer = (nextPlayer == PLAYER_1) ? PLAYER_2 : PLAYER_1;
		// επανάληψη μέχρι να τελειώσει το παιχνίδι
	} while (!isGameOver());
					
	// Εκτύπωση αποτελέσματος
	displayGameResult();
	return 0;
}


/* name: createBoard
 * @param: N int
 * @return: int (0 failure, 1 for sucess)
 * @descr: Δημιουργία της Τρίλιζας και αρχικοποίησή της
 */
int createBoard(int N) {
	int **a;
	int row, col;
	
	// δέσμευση της δομής όπου δείχνει το B
	triliza.B = (board) malloc(1*sizeof(_board));
	// αποθήκευση του N στο B->size και στο freeCells
	triliza.B->size = N;
	triliza.freeCells = N*N;
	triliza.result = -2;
	triliza.winCol = -1;
	triliza.winRow = -1;
	triliza.winDiag = -1;
	// αρχικοποίηση ονομάτων παικτών
	triliza.playerName[0] = "ΠΑΙΚΤΗΣ 1";
	triliza.playerName[1] = "ΠΑΙΚΤΗΣ 2";
	// δέσμευση N (int *) που θα δείχνουν στις γραμμές
	triliza.B->a = (int **)malloc(N*sizeof(int *));
	// αν αποτύχει, αποδέσμευση B και επιστροφή με λάθος
	if (triliza.B->a == NULL) {
		free((void *)triliza.B);
		return 0;
	}
	
	a = triliza.B->a;
	for (row = 0; row < N; row++) {
		// δέσμευση Ν (int) που θα είναι μια γραμμή
		a[row] = (int *) malloc(N * sizeof(int));
		// αν αποτύχει, έξοδος από βρόχο
		if (!a[row])
			break;
		else // διαφορετικά
			// αρχικοποίηση τιμών γραμμής
			for (col = 0; col < N; col++)
				a[row][col] = (int)BOARD_;
	}
	
	/* αν μετά το βρόχο ισχύει row < N, τότε σημαίνει ότι απέτυχε
	 * το malloc() στη γραμμή row-1, επομένως πρέπει .. */
	if (row < N) {
		// να αποδεσμευτούν όλες οι προηγούμενες γραμμές
		while (row--)
			free((void *)a[row]);
		// να αποδεσμευτεί το B->a
		free((void *)a);
		// να αποδεσμευτεί το B
		free((void *)triliza.B);
		return 0;
	} else 
		return 1;
}

/* name: displayBoard
 * @param: -
 * @return: -
 * @descr: Εκτυπώνει την Τρίλιζα
 */
void displayBoard() {
	int row, col;

	if (triliza.B == NULL) {
		fprintf(stderr, "Λάθος: Δεν υπάρχει Τρίλιζα!\n");
		return;
	}
	printf("\033[2J\033[H");
	
	if (triliza.winCol > 0) {
	} else if (triliza.winRow > 0) {
		for (row = 0; row < triliza.B->size; row++) {
			/* if (row == triliza.winRow - 1)
				printf("\033[32m"); */
			for (col = 0; col < triliza.B->size; col++)
				printf("%4c", *(*(triliza.B->a + row) + col));
			/* if (row == triliza.winRow - 1)
				printf("\033[0m"); */
			printf("\n");
		}
	} else if (triliza.winDiag > 0) {
	} else {
		for (row = 0; row < triliza.B->size; row++) {
			for (col = 0; col < triliza.B->size; col++)
				printf("%4c", *(*(triliza.B->a + row) + col));
			printf("\n");
		}
	}
}

/* name: getPlayersSymbolChoices
 * @param: -
 * @return: int (0 failure, 1 for sucess)
 * @descr: Διαβάζει την επιλογή συμβόλου για τους παίκτες και
 *         αρχικοποιεί τον πίνακα player_symbol
 */
int getPlayersSymbolChoices() {
	int symbol;

	// προτροπή στον Παίκτη 1 για επιλογή συμβόλου
	do {
		printf("%s, επιλέξτε σύμβολο (X ή O) : ", 
				triliza.playerName[PLAYER_1]);
		if (scanf(" %c", &symbol) == 1)
			symbol = toupper(symbol);
		else
			return 0;
	} while ((symbol != BOARD_X) && (symbol != BOARD_O));
	triliza.playerSymbol[PLAYER_1] = symbol;
	// το σύμβολο που απέμεινε δίνεται στον Παίκτη 2
	triliza.playerSymbol[PLAYER_2] = (symbol == BOARD_X) ? 
											BOARD_O :
											BOARD_X;
	return 1;
}

/* name: displayPlayersSymbolChoices
 * @param: -
 * @return: -
 * @descr: Εκτυπώνει τις επιλογές συμβόλων των παικτών
 */
void displayPlayersSymbolChoices() {
	printf("ΣΥΜΒΟΛΟ(%s) = %c\n", 
		triliza.playerName[PLAYER_1],
		triliza.playerSymbol[PLAYER_1]);
	printf("ΣΥΜΒΟΛΟ(%s) = %c\n", 
		triliza.playerName[PLAYER_2],
		triliza.playerSymbol[PLAYER_2]);
}

int getPlayerForSymbol(int symbol) {
	int i;
	long size = sizeof(triliza.playerSymbol) / sizeof(int);
	
	for (i = 0; i < size; i++)
		if (symbol == triliza.playerSymbol[i])
			return i;
	return -1;
}

/* name: getPlayersMove
 * @param: player int
 * @return: position
 * @descr: Διαβάζει από τον παίκτη τη θέση όπου θέλει
 * 		   να παίξει
 */
position getPlayersMove(int player) {
	position pos;
	int done;
	// Προπτροπή στον player να επιλέξει κελί
	do {
		printf("%s> επιλέξτε (σειρά/στήλη) : ", 
			triliza.playerName[player]);
		done = (scanf("%d/%d", &pos.row, &pos.col) == 2) &&
				(pos.row > 0) && (pos.col > 0) &&
				(pos.row <= triliza.B->size) && 
				(pos.col <= triliza.B->size);
	} while (!done);
	return pos;
}

/* name: makePlayersMove
 * @param: pos position, player int
 * @return: int (0 failure, 1 for success)
 * @descr: Επιστρέφει true εάν η θέση ήταν άδεια και
 *         τοποθετεί το σύμβολο του παίκτη στο κελί
 */
int makePlayersMove(position pos, int player) {
	// αν είναι κενό το κελί
	if (*(*(triliza.B->a + pos.row - 1) + pos.col - 1) == BOARD_) {
		// βάλε την επιλογή του παίκτη και
		*(*(triliza.B->a + pos.row - 1) + pos.col - 1) =
			triliza.playerSymbol[player];
		// μείωσε τα ελεύθερα κελιά
		triliza.freeCells--;
		// επέστρεψε επιτυχώς
		return 1;
	} else // αλλιώς
		// επέστρεψε ανεπιτυχώς
		return 0;
}

int doesRowWin(int row, int *winSymbol);
int doesColWin(int col, int *winSymbol);
int doesDiagWin(int diagNo, int *winSymbol);

/* name: isGameOver
 * @param: -
 * @return: int (0 game continues, 1 gave over)
 * @descr: Επιστρέφει true εάν συμπληρώθηκε και η τελευταία θέση και
 * 		   μόνο τότε συμπληρώνει το isDraw εάν έχουμε ισοπαλία και το
 * 		   *playerWinner με τον νικητή
 */
int isGameOver() {
	int row, col, winSymbol, winnerFound;
	
	// Εύρεση πιθανού νικητή
	// 1.Έλεγχος διαγωνίων 1, 2
	winnerFound = (doesDiagWin(1, &winSymbol) ||
				   doesDiagWin(2, &winSymbol));
	// 2.Αν δε βρέθηκε στις διαγωνίους κάνε έλεγχο γραμμών
	if (!winnerFound) {
		for (row = 0; row < triliza.B->size; row++) {
			winnerFound = winnerFound || doesRowWin(row, &winSymbol);
			if (winnerFound)
				break;
		}
	}
	// 3.Αν δε βρέθηκε στις γραμμές κάνε έλεγχο στηλών
	if (!winnerFound) {
		for (col = 0; col < triliza.B->size; col++) {
			winnerFound = winnerFound || doesColWin(col, &winSymbol);
			if (winnerFound)
				break;
		}
	}
	// Αν βρέθηκε νικητής
	if (winnerFound)
		triliza.result = getPlayerForSymbol(winSymbol);
	else // αλλιώς αν δεν υπάρχουν κενά κελιά
		if (!triliza.freeCells)
			// ισοπαλία
			triliza.result = -1;
	// Τελείωσε ή όχι το παιχνίδι;
	return (triliza.result >= -1);
}
		
		// 0 <= row < N
int doesRowWin(int row, int *winSymbol) {
	int col, firstSymbol, curSymbol;

	// το πρώτο σύμβολο είναι στο κελί (row, 0)
	firstSymbol = *(*(triliza.B->a + row) + 0);
	/* έλεγχος αν όλα τα υπόλοιπα κελιά της γραμμής row
	 * έχουν την ίδια τιμή με το firstSymbol */
	for (col = 1; col < triliza.B->size; col++) {
		// διάβασε το τρέχων κελί στη θέση (row, col)
		curSymbol = *(*(triliza.B->a + row) + col);
		/* αν το τρέχων κελί είναι κενό ή διαφορετικό από
		 * το πρώτο κελί firstSymbol, έξοδος με false */
		if ((curSymbol == BOARD_) || 
		    (curSymbol != firstSymbol))
			return 0;
	}
	// ενημέρωση του συμβόλου που κερδίζει και έξοδος με true
	*winSymbol = firstSymbol;
	triliza.winRow = row+1;
	return 1;
}

		// 0 <= col < N
int doesColWin(int col, int *winSymbol) {
	int row, firstSymbol, curSymbol;

	// το πρώτο σύμβολο είναι στο κελί (0, col)
	firstSymbol = *(*(triliza.B->a + 0) + col);
	/* έλεγχος αν όλα τα υπόλοιπα κελιά της στήλης col
	 * έχουν την ίδια τιμή με το firstSymbol */
	for (row = 1; row < triliza.B->size; row++) {
		// διάβασε το τρέχων κελί στη θέση (row, col)
		curSymbol = *(*(triliza.B->a + row) + col);
		/* αν το τρέχων κελί είναι κενό ή διαφορετικό από
		 * το πρώτο κελί firstSymbol, έξοδος με false */
		if ((curSymbol == BOARD_) || 
		    (curSymbol != firstSymbol))
			return 0;
	}
	// ενημέρωση του συμβόλου που κερδίζει και έξοδος με true
	*winSymbol = firstSymbol;
	triliza.winCol = col+1;
	return 1;
}

		
int doesDiagWin(int diagNo, int *winSymbol) {
	int row, col, firstSymbol, curSymbol;

	// έλεγχος έγκυρων τιμών διαγωνίου (1,2)
	if ((diagNo < 1) && (diagNo > 2))
		return 0;
	/* το πρώτο σύμβολο είναι:
	 * για την 1η Διαγώνιο στο κελί (0, 0) και
	 * για τη  2η Διαγώνιο στο κελί (0, N-1) */ 
	firstSymbol = (diagNo == 1) ? 
					*(*(triliza.B->a + 0) + 0) :
					*(*(triliza.B->a + 0) + triliza.B->size - 1);
	/* έλεγχος αν όλα τα υπόλοιπα κελιά (row == col)
	 * έχουν την ίδια τιμή με το firstSymbol */
	for (row = 1; row < triliza.B->size; row++) {
		/* διάβασε το τρέχων κελί στη θέση (row, col)
		 * όπου το col είναι
		 * για την 1η Διαγώνιο col = row και
		 * για τη  2η Διαγώνιο col = (N-1) - row */ 
		col = (diagNo == 1) ? row : (triliza.B->size - 1 - row);
		curSymbol = *(*(triliza.B->a + row) + col);
		/* αν το τρέχων κελί είναι κενό ή διαφορετικό από
		 * το πρώτο κελί firstSymbol, έξοδος με false */
		if ((curSymbol == BOARD_) || 
		    (curSymbol != firstSymbol))
			return 0;
	}
	// ενημέρωση του συμβόλου που κερδίζει και έξοδος με true
	*winSymbol = firstSymbol;
	triliza.winDiag = diagNo;
	return 1;
}

void displayGameResult() {
	printf("\nΑΠΟΤΕΛΕΣΜΑ ΠΑΙΧΝΙΔΙΟΥ\n");
	if (triliza.result == -1)
		printf("Ισοπαλία\n");
	else
		printf("Νικητής ο %s\n", triliza.playerName[triliza.result]);
}
