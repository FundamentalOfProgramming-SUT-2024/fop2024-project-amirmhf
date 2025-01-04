#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#define NUM_ITEM_MENU 5
#define SIZE_USERNAME 50
#define SIZE_PASSWORD 50
#define SIZE_EMAIL 75

typedef struct {
	char username[SIZE_USERNAME];
	char password[SIZE_PASSWORD];
	char email[SIZE_EMAIL];
} user_info;

typedef struct {
	char username[SIZE_USERNAME];
	int total_score;
	int game_score;
	int gold;
	int number_game;
	time_t start_time;
} game_user_info;


int is_login = 0; //0 means no login    1 means user_login    2 means Guest player
user_info logged_in_user;

void first_page();
void design_initial_menu();
void draw_page_border();
void draw_a_box(int height, int weight, int y, int x);
void create_new_user();
void login_user();
void page_score_table();
void pre_game_menu();
void setting_for_game();
int get_and_check_username_and_pass_for_login();
void forgot_password();
int find_password(user_info* finding);
int check_username(char* username);
int check_password(char* password);
int check_email(char* email);
char* input_without_initial_and_final_space(int max_size);

int main() {
	setlocale(LC_ALL, " ");
	initscr();
	noecho();
	keypad(stdscr, 1);
	curs_set(0);
	start_color();

	first_page();
	design_initial_menu();
	


	endwin();
	return 0;
}

void first_page() {
	draw_page_border();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(1) | A_BOLD);
	mvprintw(LINES / 2, COLS / 3, "Wellcome to this game");
	mvprintw(LINES / 2 + 1, COLS / 3, "Please press any button to continue...");
	attroff(COLOR_PAIR(1) | A_BOLD);
	getch();
}
void design_initial_menu() {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	clear();
	int selection = 0;
	while (1){
		clear();
		draw_page_border();
		attron(COLOR_PAIR(1));
		if (is_login == 0) mvprintw(3, COLS - 50, "USER: No User");
		else if (is_login == 1) mvprintw(3, COLS - 50, "USER: %-50s", logged_in_user.username);
		else if (is_login == 2) mvprintw(3, COLS - 50, "USER: Guest player");
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		mvprintw(3, 6, "Menu Game");
		attroff(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		char* initial_menu[] = {"Create a new user", "Login user", "Pre_Game Menu", "Score table", "Exit"};
		
		for (int i = 0; i < NUM_ITEM_MENU; i++) {
			if (i == selection) attron(A_REVERSE);
			attron(COLOR_PAIR(1));
			mvprintw(6 + i, 10, "%s", initial_menu[i]);
			attroff(COLOR_PAIR(1));
			if (i == selection) attroff(A_REVERSE);
		}
		
		int a = getch();
		if (a == KEY_UP) {
			selection == 0 ? selection = NUM_ITEM_MENU - 1 : selection-- ;
		}
		else if (a == KEY_DOWN) {
			selection == NUM_ITEM_MENU - 1 ? selection = 0 : selection++ ;
		}
		else if (a == '\n') 
			switch (selection)
			{
			case 0:
				create_new_user();
				break;
			case 1:
				login_user();
				break;
			case 2:
				pre_game_menu();
				break;
			case 3:
				page_score_table();
				break;
			case 4:
				return;
			}
	}
}
void draw_page_border() {
    clear();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
    for (int x = 0; x < COLS; x++)
    {
        mvprintw(0, x, "-");
        mvprintw(LINES - 1, x, "-");
    }
    for (int y = 1; y < LINES; y++)
    {
        mvprintw(y, 0, "|");
        mvprintw(y, COLS - 1, "|");
    }
	attroff(COLOR_PAIR(1));
}
void draw_a_box(int height, int weight, int y, int x) {
	for (int i = 0; i < weight; i++) {
		mvprintw(y, x + i, "_");
		mvprintw(y + height, x + i, "_");
	}
	for (int i = 1; i <= height; i++) {
		mvprintw(y + i, x, "|");
		mvprintw(y + i, x + weight, "|");
	}

}
void create_new_user() {
	
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);	

	int selection = 2;
	int flag = 0; //This shows how much information has been entered.
					//bit 1 for username, bit 2 for password, bit 3 for email.
	user_info new_user;
	char* creating_menu[] = {"Return to previous menu", "Save and exit", "Your UserName: ", "Your password: ", "Your email: "};
	while (1) {
		clear();
		draw_page_border();
		//draw_a_box(7, 90, 9, COLS / 3 - 4);
		attron(COLOR_PAIR(1) |  A_BOLD);
		mvprintw(3, 6, "Creating a new user");
		attroff(COLOR_PAIR(1) | A_BOLD);

		for (int i = 0; i < 5; i++) {
			if (i == selection) attron(A_REVERSE);
			if (i == 0) {
				attron(COLOR_PAIR(2) | A_UNDERLINE);
				mvprintw(6, (COLS - 30) / 2, "%s", creating_menu[i]);
				attroff(COLOR_PAIR(2) | A_UNDERLINE);
			}
			else if (i == 1) {
				attron(COLOR_PAIR(1) | A_UNDERLINE);
				mvprintw(7, (COLS - 30) / 2, "%s", creating_menu[i]);
				attroff(COLOR_PAIR(1) | A_UNDERLINE);
			}
			else {
				mvprintw(7 + i*2, COLS / 3, "%s", creating_menu[i]);
			}
			if (i == selection) attroff(A_REVERSE);
		}

		attron(COLOR_PAIR(3));
		int a = getch();
		refresh();
		mvprintw(24, (COLS - 70) / 4, "***Move up or down to continue and press enter to write.\n");
		switch (a) {
			case KEY_UP:
				selection == 0 ? selection = 4 : selection--;
				break;
			case KEY_DOWN:
				selection == 4 ? selection = 0 : selection++;
				break;
			case '\n':
				switch (selection) {
					case 0:  //return to the initial menu
						return;
					case 1: {
							if ((flag & 7) != 7) {
								attron(COLOR_PAIR(2));
								mvprintw(26, (COLS - 70) / 4, "***You haven't entered your informatoin completely.\n");
								attroff(COLOR_PAIR(2));
								getch();
								break;
							}
							else {
								FILE* fileptr = NULL;
								fileptr = fopen("users.dat", "ab+");
								if (fileptr == NULL) {
									attron(COLOR_PAIR(2));
									mvprintw(26, (COLS - 70) / 4, "***Problem in saving data. Please Try again.\n");
									attroff(COLOR_PAIR(2));
									getch();
									break;
								}
								else {
									fwrite(&new_user, sizeof(user_info), 1, fileptr);
									fclose(fileptr);
									return;
								}
							}
					}
					case 2: {
							echo();
							curs_set(1);
							move(11, COLS / 3 + 16);
							char* temper = input_without_initial_and_final_space(SIZE_USERNAME);
							strcpy(new_user.username, temper);
							free(temper);
							attron(COLOR_PAIR(2));
							if ((flag & 1) == 1) {
								mvprintw(18, COLS / 3, "You have enter your username.\n");
								mvprintw(19, COLS / 3, "Please Enter Next one.\n");
								mvprintw(20, COLS / 3, "If you wants to enter again thid field, you can return menu and come again.\n");
							}
							else {
								int res = check_username(new_user.username);
								if (res == 1) {
									flag |= 1;
									attron(COLOR_PAIR(1));
									mvprintw(18, COLS / 3, "Your Username saved.\n");
									mvprintw(19, COLS / 3, "Please Enter Next one.\n");
									attroff(COLOR_PAIR(1));
								}
							}
							noecho();
							curs_set(0);
							attroff(COLOR_PAIR(2));
							getch();
							break; }
					case 3: {
							echo();
							curs_set(1);
							mvprintw(22, (COLS - 70) / 4, "***Be careful! Your password must has at least 7 characters, one digit and a capital and a small letter");
							move(13, COLS / 3 + 16);
							char* temper = input_without_initial_and_final_space(SIZE_PASSWORD);
							strcpy(new_user.password, temper);
							free(temper);
							attron(COLOR_PAIR(2));
							if ((flag & 2) == 2) {
								mvprintw(18, COLS / 3, "You have enter your password.\n");
								mvprintw(19, COLS / 3, "Please Enter Next one\n");
								mvprintw(20, COLS / 3, "If you wants to enter again thid field, you can return menu and come again.\n");
							}
							else {
								int res = check_password(new_user.password);
								if (res == 1) {
									flag |= 2;
									attron(COLOR_PAIR(1));
									mvprintw(18, COLS / 3, "Your password saved.\n");
									mvprintw(19, COLS / 3, "Please Enter Next one.\n");
									attroff(COLOR_PAIR(1));
								}
							}
							noecho();
							curs_set(0);
							attroff(COLOR_PAIR(2));
							getch();
							break; }
					case 4: {
							echo();
							curs_set(1);
							mvprintw(22, (COLS - 70) / 4, "***Be careful! Your email must be like this pattern: xxx@yy.zz\n");
							move(15, COLS / 3 + 13);
							char* temper = input_without_initial_and_final_space(SIZE_EMAIL+1);
							strcpy(new_user.email, temper);
							free(temper);
							attron(COLOR_PAIR(2));
							if ((flag & 4) == 4) {
								mvprintw(18, COLS / 3, "You have enter your email.\n");
								mvprintw(19, COLS / 3, "Please Enter Next one.\n");
								mvprintw(20, COLS / 3, "If you wants to enter again thid field, you can return menu and come again.\n");
							}
							else {
								int res = check_email(new_user.email);
								if (res == 1) {
									flag |= 4;
									attron(COLOR_PAIR(1));
									mvprintw(18, COLS / 3, "Your email saved.\n");
									mvprintw(19, COLS / 3, "If you have enter all information, you can Save and exit.\n");
									attroff(COLOR_PAIR(1));
								}
								else {
									mvprintw(18, COLS / 3, "This email is invalid! Pay attention to the pattern.\n");
									mvprintw(19, COLS / 3, "Please Try again.\n");
								}
							}
							noecho();
							curs_set(0);
							attroff(COLOR_PAIR(2));
							getch();
							break; }
				}
				break;
		}
		refresh();
		attroff(COLOR_PAIR(3));
	}
	

}
int check_username(char* username) {
	if (strlen(username) == 0) {
		mvprintw(18, COLS / 3, "This username is invalid !!!!!!!\n");
		mvprintw(19, COLS / 3, "Please Try again.\n");
		return 0;
	}
	FILE* fileptr = NULL;
	fileptr = fopen("users.dat", "rb");
	if (fileptr == NULL) {
		return 1;  //it means there wasn't any user before and the file isn't available
	}
	while (!feof(fileptr)) {
		user_info check;
		fread(&check, sizeof(user_info), 1, fileptr);
		if (strcmp(check.username, username) == 0) {
			mvprintw(19, COLS / 3, "Please Enter a new username");
			mvprintw(20, COLS / 3, "This username has been used previously !!!!!!\n");
			return 0;
		}
	}
	fclose(fileptr);
	return 1;
}
int check_password(char* password) {
	if (strlen(password) < 7) {
		mvprintw(18, COLS / 3, "The length of password must be at least 7 characters !!!!!!!\n");
		mvprintw(19, COLS / 3, "Please Try again.\n");
		return 0;
	}
	int flag = 0;
	for (int i = 0; i < strlen(password); i++) {
		if (password[i] >= '0' && password[i] <= '9')  { flag = 1; break; }
	}
	if(flag == 0) {
		mvprintw(18, COLS / 3, "The password must have at least 1 digit !!!!!!!\n");
		mvprintw(19, COLS / 3, "Please Try again.\n");
		return 0;
	}
	flag = 0;
	for (int i = 0; i < strlen(password); i++) {
		if (password[i] >= 'a' && password[i] <= 'z')  { flag = 1; break; }
	}
	if(flag == 0) {
		mvprintw(18, COLS / 3, "The password must have at least 1 small character !!!!!!!\n");
		mvprintw(19, COLS / 3, "Please Try again.\n");
		return 0;
	}
	flag = 0;
	for (int i = 0; i < strlen(password); i++) {
		if (password[i] >= 'A' && password[i] <= 'Z')  { flag = 1; break; }
	}
	if(flag == 0) {
		mvprintw(18, COLS / 3, "The password must have at least 1 capital character !!!!!!!\n");
		mvprintw(19, COLS / 3, "Please Try again.\n");
	 	return 0;
	}
	return 1;
}
int check_email(char* email) {
	int flag = 0;
	int i = 0;
		while(i < strlen(email) && isalnum(email[i])) {   flag = 1;  i++;  }
		if(flag == 0) return 0;
		flag = 0;
		if(!(i < strlen(email) && email[i++] == '@')) return 0;
		while(i < strlen(email) && isalnum(email[i])) {   flag = 1;  i++;  }
		if(flag == 0) return 0;
		flag = 0;
		if(!(i < strlen(email) && email[i++] == '.')) return 0;
		while(i < strlen(email) && isalnum(email[i])) {   flag = 1;  i++;  }
		if(flag == 0) return 0;
		return 1;
}
char* input_without_initial_and_final_space(int max_size) {
    char* output = malloc(max_size * sizeof(char));
    char temp;
    int i = 0;
    while ((temp = getch()) == ' ');
    while (temp != '\n') {
        output[i++] = temp;
        temp = getch();
    }
    while(output[--i] == ' ');
    output[i+1] = '\0';
    return output;
}
void login_user() {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);	

	int selection = 1;
	int flag = 0;
	char* login_menu[] = {"Return to previous menu", "Your UserName: ", "Your password: ", "Start Game as a Guest", "Forgot password"};

	while(1) {
		draw_page_border();

		attron(COLOR_PAIR(1) |  A_BOLD);
		mvprintw(3, 6, "login the game");
		attroff(COLOR_PAIR(1) | A_BOLD);

		for (int i = 0; i < 5; i++) {  //writing items in the page
			if (i == selection && i != 2) attron(A_REVERSE);
			if (i == 0) {
				attron(COLOR_PAIR(2) | A_UNDERLINE);
				mvprintw(8, (COLS - 30) / 2, "%s", login_menu[i]);
				attroff(COLOR_PAIR(2) | A_UNDERLINE);
			}
			else if (i == 3) {
				attron(COLOR_PAIR(1) | A_UNDERLINE | A_BOLD);
				mvprintw(15, (COLS - 30) / 2, "%s", login_menu[i]);
				attroff(COLOR_PAIR(1) | A_UNDERLINE | A_BOLD);
			}
			else if (i == 4) {
				attron(COLOR_PAIR(1) | A_UNDERLINE | A_BOLD);
				mvprintw(17, (COLS - 30) / 2, "%s", login_menu[i]);
				attroff(COLOR_PAIR(1) | A_UNDERLINE | A_BOLD);
			}
			else {
				mvprintw(9 + i*2, COLS / 3, "%s", login_menu[i]);
			}
			if (i == selection && i != 2) attroff(A_REVERSE);
		}
		attron(COLOR_PAIR(2));

		mvprintw(20, (COLS - 70) / 4, "***Start entering data from your username !!!!!!!\n");

		int a = getch();
		if (a == KEY_UP) {
			selection == 0 ? selection = 4 : selection-- ;
			if(selection == 2) selection--;
		}
		else if (a == KEY_DOWN) {
			selection == 4 ? selection = 0 : selection++ ;
			if(selection == 2) selection++;
		}
		else if (a == '\n') {
			switch (selection)
			{
			case 0:     //Return to previous menu
				return;
			case 1:     //"Your UserName: ", "Your password: "
			case 2:
				if (get_and_check_username_and_pass_for_login()) {
					attron(COLOR_PAIR(1));
					mvprintw(25, (COLS - 70) / 2, "You have successfully logged in.\n");
					mvprintw(26, (COLS - 70) / 2, "Press any button to return to previous menu and Start the game.\n");
					attroff(COLOR_PAIR(1));
					is_login = 1;
					getch();
				}
				return;
			case 3:        //"Start Game as a Guest"
					attron(COLOR_PAIR(1));
					mvprintw(25, (COLS - 70) / 2, "Now you can play the game as a Guest player.\n");
					mvprintw(26, (COLS - 70) / 2, "Press any button to return to previous menu and Start the game.\n");
					attroff(COLOR_PAIR(1));
					is_login = 2;
					getch();
				return;
			case 4:        //"Forgot password"
				forgot_password(); 
				attroff(COLOR_PAIR(2));
				mvprintw(24, COLS / 3, "Press any button to return the previous page....\n");
				getch();
				break;
			}
		}
		attroff(COLOR_PAIR(2));
		refresh();
	}
}
int get_and_check_username_and_pass_for_login() {
	user_info user;
	echo();
	curs_set(1);
	move(11, COLS / 3 + 16);
	char* temper = input_without_initial_and_final_space(SIZE_USERNAME+1);
	strcpy(user.username, temper);
	free(temper);
	move(13, COLS / 3 + 16);
	temper = input_without_initial_and_final_space(SIZE_USERNAME+1);
	strcpy(user.password, temper);
	free(temper);
	noecho();
	curs_set(0);

	FILE* fileptr = NULL;
	fileptr = fopen("users.dat", "rb");

	if (fileptr == NULL) {
		mvprintw(22, (COLS - 70) / 4, "***No user has been registered !!!!!!!\n");
		getch();
		fclose(fileptr);
		return 0;
	}

	while (!feof(fileptr)) {
		user_info check;
		fread(&check, sizeof(user_info), 1, fileptr);

		if(strcmp(check.username, user.username) == 0) {
			if(strcmp(check.password, user.password) == 0) {
				logged_in_user = check;
				fclose(fileptr);
				return 1;
			}
			else {
				mvprintw(22, (COLS - 70) / 4, "***The input password is not for this user !!!!!!!\n");
				getch();
				fclose(fileptr);
				return 0;
			}
		}
	}
	mvprintw(22, (COLS - 70) / 4, "***No user was found !!!!!!!\n");
	mvprintw(23, (COLS - 70) / 4, "***You can return to previous menu and create a new user OR you can play as a guest !!!!!!!\n");
	refresh();
	getch();
	fclose(fileptr);
	return 0;
}
void forgot_password() {
	clear();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);	

	draw_page_border();

	attron(COLOR_PAIR(1) |  A_BOLD);
	mvprintw(3, 6, "Forgot Password:");
	attroff(COLOR_PAIR(1) | A_BOLD);

	user_info find;
	attron(COLOR_PAIR(3));
	mvprintw(15, COLS / 3, "please enter your username ---->  ");
	attroff(COLOR_PAIR(3));
	echo();
	curs_set(1);
	attron(COLOR_PAIR(2));
	char* temp = input_without_initial_and_final_space(SIZE_USERNAME + 1);
	strcpy(find.username, temp);
	free(temp);
	attroff(COLOR_PAIR(2));
	attron(COLOR_PAIR(3));
	mvprintw(17, COLS / 3, "please enter your email ---->  ");
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(2));
	temp = input_without_initial_and_final_space(SIZE_EMAIL + 1);
	strcpy(find.email, temp);
	free(temp);
	attroff(COLOR_PAIR(2));
	noecho();
	curs_set(0);
	if (find_password(&find)) {
		attron(COLOR_PAIR(1));
		mvprintw(22, COLS / 3, "Your password ----> ");
		attroff(COLOR_PAIR(1));
		attron(A_REVERSE);
		printw("%s", find.password);
		attroff(A_REVERSE);
		refresh();
	}
}
int find_password(user_info* finding) {
	FILE* fileptr = NULL;
	fileptr = fopen("users.dat", "rb");

	attron(COLOR_PAIR(2));
	if(fileptr == NULL) {
		mvprintw(22, (COLS - 70) / 4, "There isn't any registered users !!!!!!!\n");
		fclose(fileptr);
		return 0;
	}

	while (!feof(fileptr)) {
		user_info check;
		fread(&check, sizeof(user_info), 1, fileptr);
		if (strcmp(check.username, finding->username) == 0) {
			if (strcmp(check.email, finding->email) == 0) {
				strcpy(finding->password, check.password);
				fclose(fileptr);
				return 1;
			}
			else {
				mvprintw(22, (COLS - 70) / 4, "Not compatible username and email !!!!!!!\n");
				fclose(fileptr);
				return 0;
			}
			refresh();
		}
	}

	mvprintw(22, (COLS - 70) / 4, "This user wasn't found !!!!!!!\n");
	attroff(COLOR_PAIR(2));
	fclose(fileptr);
	return 0;

}
void page_score_table() {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);

	clear();
	draw_page_border();
	refresh();
	
	FILE* fileptr = NULL;
	fileptr = fopen("score.dat", "rb");

	if (fileptr == NULL) {
		mvprintw(5, 15, "Can not open the file!!\n");
		getch();
		return;
	}

	int num_read = 0;
	int p = 1;
	game_user_info* list = malloc(p * 100 * sizeof(game_user_info));

	while (!feof(fileptr)) {
		num_read += fread(&list[num_read], sizeof(game_user_info), 1, fileptr);
		if (num_read >= p * 100) {
			p++;
			list = realloc(list, p * 100 * sizeof(game_user_info));
		}
	}
	
	int is_sorted = 0;
	for(int i = 1; i < num_read && is_sorted == 0; i++) {
		is_sorted = 1;
		for (int j = 0; j < num_read - i; j++) {
			if(list[j].total_score < list[j+1].total_score) {
				game_user_info temp = list[j];
				list[j] = list[j+1];
				list[j+1] = temp;
				is_sorted = 0;
			}
		}
	}

	attron(COLOR_PAIR(3));
	mvprintw(4, 10, "%s", "You can press any key to return to the main menu...\n");
	attroff(COLOR_PAIR(3));

	attron(COLOR_PAIR(2));
	mvprintw(8, COLS / 6, "%-10s%-30s%-17s%-10s%-20s%-15s", "Rank", "Username", "Total score", "Gold", "Number of game", "Experience");
	attroff(COLOR_PAIR(2));

	for(int i = 0; i < num_read; i++) {
		if (is_login == 1 && strcmp(list[i].username, logged_in_user.username) == 0) mvprintw(10 + i, COLS / 6 - 8, "----->");
		if (i == 0 || i == 1 || i == 2) { attron(COLOR_PAIR(1) | A_ITALIC | A_BOLD); mvprintw(10 + i, COLS / 6 + 103, "<Legend>");}
		time_t now = time(NULL);
		mvprintw(10 + i, COLS / 6, "%-10d%-30s%-17d%-10d%-20d", i+1, list[i].username, list[i].total_score, 
																list[i].gold, list[i].number_game);
		printw("%-10.0fday" ,difftime(now, list[i].start_time) / (24*3600));
		if (i == 0 || i == 1 || i == 2) attroff(COLOR_PAIR(1) | A_ITALIC | A_BOLD);
	}

	refresh();
	getch();
}
void pre_game_menu() {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	clear();
	int selection = 0;
	while (1){
		clear();
		draw_page_border();
		attron(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		mvprintw(3, 6, "Pre_Game Menu");
		attroff(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		char* initial_menu[] = {"New game", "Resume Game", "Setting", "Scoreboard", "Return to previous menu"};
		
		for (int i = 0; i < 5; i++) {
			if (i == selection) attron(A_REVERSE);
			attron(COLOR_PAIR(1));
			mvprintw(6 + i, 10, "%s", initial_menu[i]);
			attroff(COLOR_PAIR(1));
			if (i == selection) attroff(A_REVERSE);
		}
		
		int a = getch();
		if (a == KEY_UP) {
			selection == 0 ? selection = 4 : selection-- ;
		}
		else if (a == KEY_DOWN) {
			selection == 4 ? selection = 0 : selection++ ;
		}
		else if (a == '\n') 
			switch (selection)
				{
				case 0:      //"New game"
					//create_new_user();
					break;
				case 1:     //"Resume Game"
					//login_user();
					break;
				case 2:     //"Setting"
					setting_for_game();
					break;
				case 3:    //"Scoreboard"
					page_score_table();
					break;
				case 4:     //"Return to previous menu"
					return;
				}
	}
}
void setting_for_game() {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	clear();
	int selection = 0;
	while (1){
		clear();
		draw_page_border();
		attron(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		mvprintw(3, 6, "Setting Game Menu");
		attroff(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		char* initial_menu[] = {"Easy", "Medium", "Hard", "Red", "Green", "Blue", "White", "Return to previous page"};
		
		mvprintw(10, 15, "choose Level of difficulty");
		for (int i = 0; i < 3; i++) {
			if (i == selection) attron(A_REVERSE);
			attron(COLOR_PAIR(1));
			mvprintw(11 + i, 20, "%s", initial_menu[i]);
			attroff(COLOR_PAIR(1));
			if (i == selection) attroff(A_REVERSE);
		}
		mvprintw(17, 15, "Select Color for Hero");
		for (int i = 3; i < 8; i++) {
			if (i == selection) attron(A_REVERSE);
			attron(COLOR_PAIR(1));
			if (i == 7) {
				attron(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
				mvprintw(25, 25, "Return to previous page");
				attroff(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
			}
			else mvprintw(15 + i, 20, "%s", initial_menu[i]);
			attroff(COLOR_PAIR(1));
			if (i == selection) attroff(A_REVERSE);
		}
		
		int a = getch();
		if (a == KEY_UP) {
			selection == 0 ? selection = 7 : selection-- ;
		}
		else if (a == KEY_DOWN) {
			selection == 7 ? selection = 0 : selection++ ;
		}
		else if (a == '\n') 
			switch (selection)
				{
				case 0:      //"Easy"
					//
					break;
				case 1:     //"Medium"
					//
					break;
				case 2:     //"Hard"
					//
					break;
				case 3:    //""Red"
					//
					break;
				case 4:     //"Green"
					//
					break;
				case 5:     //"Blue"
					//
					break;
				case 6:     //"White"
					//
					break;
				case 7:     //"Return to previous page"
					return;
				}
	}
}