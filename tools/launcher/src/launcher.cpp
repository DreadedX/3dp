#include <ncurses.h>

int main() {

	WINDOW *w;
	char list[4][20] = { "Build Linux Debug", "Build Linux Release", "Build Windows Debug", "Build Linux Release" };
	char item[20];
	int ch, i = 0, width = 20;

	initscr(); // initialize Ncurses
	w = newwin( 10, 42, 1, 1 ); // create a new window
	box( w, 0, 0 ); // sets default borders for the window

	// now print all the menu items and highlight the first one
	for( i=0; i<4; i++ ) {
		if( i == 0 ) 
			wattron( w, A_BOLD ); // highlights the first item.
		else
			wattroff( w, A_BOLD );
		sprintf(item, "%-20s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item );
	}

	wrefresh( w ); // update the terminal screen

	i = 0;
	noecho(); // disable echoing of characters on the screen
	keypad( w, TRUE ); // enable keyboard input for the window.
	curs_set( 0 ); // hide the default screen cursor.

	// get the input
	while(( ch = wgetch(w)) != 'q'){ 

		// right pad with spaces to make the items appear with even width.
		sprintf(item, "%-20s",  list[i]); 
		mvwprintw( w, i+1, 2, "%s", item ); 
		// use a variable to increment or decrement the value based on the input.
		switch( ch ) {
			case 107:
				i--;
				i = ( i<0 ) ? 3 : i;
				break;
			case 106:
				i++;
				i = ( i>3 ) ? 3 : i;
				break;
		}
		// now highlight the next item in the list.
		wattron( w, A_BOLD );

		sprintf(item, "%-20s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item);
		wattroff( w, A_BOLD );
	}

	delwin( w );
	endwin();
}
