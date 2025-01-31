#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wchar.h>
#include <unistd.h>
#include <stdbool.h>
#define NUM_ITEM_MENU 5
#define SIZE_USERNAME 50
#define SIZE_PASSWORD 50
#define SIZE_EMAIL 75
#define REGULAR_ROOM 0
#define ENCHANT_ROOM 1
#define REGULAR_DOOR 0
#define PASSWORD_DOOR 1
#define SECRET_DOOR 2

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

typedef struct {
	int y;
    int x;
} location;

typedef struct {
    int x;
	int y;
	int value; 
} element;

typedef struct {
	int room_type;      //REGULAR_ROOM   ENCHANT_ROOM    TREASURE_ROOM
	int door_type[2];   //REGULAR_DOOR   PASSWORD_DOOR   SECRET_DOOR 
    char cell[20][20]; 
    int height;
    int wide;
	bool lock_door;
	element gold[5];
	element black_gold; 
	location door[2]; //room first and last only one door
	location start_point;
} room_info;

typedef struct {
	int health;
	char name;
	int room;
	location place;
}enemy_info;

typedef struct {
	room_info room[6];
	location corridor[500];
	int number_corridor;
	int open_room;
	int open_corridor;
	enemy_info enemy[16];
	int number_enemy;
} floor_info;

typedef struct {
	int mace_amount;
	int dagger_amount;
	int wand_amount;
	int arrow_amount;
	int sword_amount;
	char current; //a char for save
} weapon_info;

typedef struct {
	int speed_amount;
	int health_amount;
	int damage_amount;
	char current; //a char for save
	bool active;
	int distance;
} enchant_info;

typedef struct {
	weapon_info weapon;
	enchant_info enchant;
	int save_gold;
	int score;
	int health;   //100
	int hunger_bar;  //20
	int movement;
	int movement_unhungry;  
	int food_amount;
}achievement_info;

typedef struct {
	room_info room;
	enemy_info enemy[20];
	int number_enemy;
}treasure_info;


int is_login = 0; //0 means no login    1 means user_login    2 means Guest player
user_info logged_in_user;
int color;   //1 green    2 red    3 golden    5 blue
int difficulty;

void first_page();
void design_initial_menu();
void draw_page_border();
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
void generate_room(room_info* a_room, int number);
bool check_value(room_info* room, int y, int x, char value);
char door_symbol(room_info* room, int n, int side);
void new_game();
void start_location_random(location* start, room_info* room);
void print_map_conditionally(floor_info* floor, location* place);
void print_all_map(floor_info* floor);
void print_one_element(int y, int x, char value);
void generate_a_floor(floor_info* a_floor, int number);
void generate_treasure_room(treasure_info* treasure);
void print_treasure_room(treasure_info* treasure);
bool go_to_treasure_room(floor_info* floor, location* place, int num_floor);
int handle_corridor(floor_info* floor, int first, int second, int index);
bool check_location(floor_info* floor, location* place);
bool check_location_in_treasure_room(treasure_info* treasure, location* place);
bool check_location_as_be_enemy(floor_info* floor, location* place, int j);
int  check_location_as_be_enemy_with_index(floor_info* floor, location* place);
int check_location_as_be_enemy_with_index_in_treasure_room(treasure_info* treasure, location* place);
void control_list_and_inputs(floor_info* floor, location* place, int* num_floor, achievement_info*, int);
void control_movement_for_player(floor_info* floor, location* place, achievement_info*, int);
void control_movement_for_player_in_treasure(treasure_info* treasure, location* place, achievement_info*, int);
void control_list_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int ch);
int current_room(floor_info* floor, location* place);
void pickup_a_thing(room_info* room, location* place, achievement_info*);
void pickup_and_check_trap_and_health(floor_info* floor, location* place, achievement_info* achievement);
void check_trap_and_distance_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement);
void pickup_a_gold_food(room_info* room, location* place, achievement_info*);
void check_for_trap(room_info* room, location* place, achievement_info*);
void list_of_weapon(weapon_info* weapon);
void list_of_enchant(enchant_info* enchant);
void change_weapon(weapon_info* weapon);
void show_health_rate(achievement_info* achievement);
void eat_a_meal(achievement_info* achievement);
void cure_when_is_unhungry(achievement_info* achievement);
void check_hunger_bar(achievement_info* achievement);
void print_enemy_conditionally(floor_info* floor);
void move_alive_enemies(floor_info* floor, location* place, achievement_info*);
void move_permanently_enemy(floor_info* floor, location* place, int i);
void move_temporary_enemy(floor_info* floor, location* place, int i);
void move_alive_enemies_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement);
void check_around_for_enemy(floor_info* floor, location* place, achievement_info* achievement);
void transfer_snake_to_other_floor(floor_info* p_floor, floor_info* n_floor);
void transfer_snake_to_treasure_room(floor_info* p_floor, treasure_info*);
void fight(floor_info* floor, location* place, achievement_info* achievement) ;
void fight_with_active_damage_enchant(floor_info* floor, location* place, achievement_info* achievement);
void shot_with_mace(floor_info* floor, location* place, achievement_info* achievement, int);
void shot_with_sword(floor_info* floor, location* place, achievement_info* achievement, int);
void shot_with_dagger(floor_info* floor, location* place, achievement_info* achievement, int);
void shot_with_wand(floor_info* floor, location* place, achievement_info* achievement, int);
void shot_with_arrow(floor_info* floor, location* place, achievement_info* achievement, int);
int search_path_direction(floor_info* floor, location* place, int direction, int len);
char* tell_name_enemy(char abbreviation);
void check_active_enchant(achievement_info* achievement);
void lose_page(achievement_info* achievement);
void win_page(achievement_info* achievement);
bool exit_the_game();
void show_help_for_game();
int handle_input_key(int ch);
void set_to_zero(achievement_info*);
void open_password_door(room_info* room, int door_number);
char* input_without_initial_and_final_space(int max_size);
void check_around_for_enemy_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement);
void fight_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement);
void fight_with_active_damage_enchant_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement);
void shot_with_mace_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power);
void shot_with_sword_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power);
void shot_with_dagger_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power);
void shot_with_wand_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power);
void shot_with_arrow_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power);
int search_path_direction_in_treasure_room(treasure_info* treasure, location* place, int direction, int len);
void score_kill_enemy(achievement_info* achievement, char name, int n);



int main() {
	setlocale(LC_ALL, "");
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

	for(int i = 0; i < 5; i++) {
		if (is_login == 1 && strcmp(list[i].username, logged_in_user.username) == 0) mvprintw(10 + i, COLS / 6 - 8, "----->");
		if (i == 0 || i == 1 || i == 2) { 
			attron(COLOR_PAIR(1) | A_ITALIC | A_BOLD); mvprintw(10 + i, COLS / 6 + 103, "<Legend> "); printw("\U0001f3c6");}
		time_t now = time(NULL);
		mvprintw(10 + i, COLS / 6, "%-10d%-30s%-17d%-10d%-20d", i+1, list[i].username, list[i].total_score, 
																list[i].gold, list[i].number_game);
		printw("%-10.0fday" ,difftime(now, list[i].start_time) / (24*3600));
		if (i == 0 || i == 1 || i == 2) attroff(COLOR_PAIR(1) | A_ITALIC | A_BOLD);
	}

	fclose(fileptr);
	free(list);
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
					if(is_login == 1 || is_login == 2) {
						mvprintw(LINES/2,     COLS / 2 - 20, "Making the game ready...\n");
						mvprintw(LINES/2 + 1, COLS / 2 - 20, "Please wait for seconds...\n");
						new_game();
					}
					else {
						mvprintw(LINES/2,     COLS / 2 - 20, "You haven't LOGIN the game!\n");
						mvprintw(LINES/2 + 1, COLS / 2 - 20, "Return to previous page and login.\n");
						getch();
					}
					break;
				case 1:     //"Resume Game"
					//
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
	init_color(11, 1000, 843, 0);   // طلایی 
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, 11, COLOR_BLACK); // جفت طلایی
	clear();
	int selection = 0;
	while (1){
		clear();
		draw_page_border();
		attron(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		mvprintw(3, 6, "Setting Game Menu");
		attroff(COLOR_PAIR(2) | A_BOLD | A_UNDERLINE);
		char* initial_menu[] = {"Easy", "Medium", "Hard", "Red", "Green", "Blue", "Golden", "Return to previous page"};
		
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
		
		attron(COLOR_PAIR(3));
		mvprintw(3, 50, "<<Default is Medium and color is White!>>");

		int a = getch();
		if (a == KEY_UP) {
			selection == 0 ? selection = 7 : selection-- ;
		}
		else if (a == KEY_DOWN) {
			selection == 7 ? selection = 0 : selection++ ;
		}
		else if (a == '\n') {
			switch (selection){
				case 0:      //"Easy"
					difficulty = 1;
					mvprintw(5, 50, "You choose Easy mode!\n");
					break;
				case 1:     //"Medium"
					difficulty = 0;
					mvprintw(5, 50, "You choose Medium mode!\n");
					break;
				case 2:     //"Hard"
					difficulty = -1;
					mvprintw(5, 50, "You choose Hard mode!\n");
					break;
				case 3:     //"Red"
					color = 2;
					mvprintw(5, 50, "You choose Red color!\n");
					break;
				case 4:     //"Green"
					color = 1;
					mvprintw(5, 50, "You choose Green color!\n");
					break;
				case 5:     //"Blue"
					color = 5;
					mvprintw(5, 50, "You choose Blue color!\n");
					break;
				case 6:     //"Golden"
					color = 3;
					mvprintw(5, 50, "You choose Golden color!\n");
					break;
				case 7:     //"Return to previous page"
					attron(COLOR_PAIR(3));
					return;
				}
			getch();
		}
		attron(COLOR_PAIR(3));
	}
}
void new_game() {
	init_color(11, 1000, 843, 0);   // رنگ طلایی 
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, 11, COLOR_BLACK); // جفت رنگ طلایی
	init_pair(4, COLOR_YELLOW, COLOR_WHITE);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);


	achievement_info achievement;
	floor_info floor[4];
	set_to_zero(&achievement);

	for(int i = 0; i < 4; i++) {
		generate_a_floor(&floor[i], i);
		floor[i].open_room = 0;
		floor[i].open_corridor = -6;
		sleep(1);
	}
	treasure_info treasure;
	generate_treasure_room(&treasure);

	location position;
	start_location_random(&position, &floor[0].room[0]);
	
	int g = 0;  //num_floor

	while (1) {           //Start New game
		clear();
		print_map_conditionally(floor+g, &position); //map

		attron(A_REVERSE | COLOR_PAIR(color));
		mvprintw(position.y, position.x, "$");    //Hero
		attroff(A_REVERSE | COLOR_PAIR(color));

		print_enemy_conditionally(floor+g);     //enemies
		check_around_for_enemy(floor+g, &position, &achievement);

		if(achievement.health <= 0 || achievement.hunger_bar <= 0) {     //check for being alive 
			lose_page(&achievement);
			return;
		}

		if(go_to_treasure_room(floor+g, &position, g) == true) {
			start_location_random(&position, &treasure.room);
			transfer_snake_to_treasure_room(floor+g, &treasure);
			break;
		}

		int temp = g;
		pickup_and_check_trap_and_health(floor+g, &position, &achievement);

		int ch = getch();
		int a = handle_input_key(ch);
		switch (a) {
			case 0:    //list and other 
				control_list_and_inputs(floor+g, &position, &g, &achievement, ch);
				break;
			case 1:    //movement  (8 direction)
				control_movement_for_player(floor+g, &position, &achievement, ch);
				check_active_enchant(&achievement);
				check_hunger_bar(&achievement);
				move_alive_enemies(floor+g, &position, &achievement);
				break;
			case 2:    //fight    (Space)
				fight(floor+g, &position, &achievement);
				move_alive_enemies(floor+g, &position, &achievement);
				check_active_enchant(&achievement);
				check_hunger_bar(&achievement);
				break;
			case 3:     //exit     (Esc)
				if(exit_the_game() == true)
					return;
		}

		if(g != temp) {       //if floor was changed
			start_location_random(&position, &floor[g].room[0]);
			transfer_snake_to_other_floor(floor+temp, floor+g);
		}

		refresh();
	}

	while (1) {           //Treasure room
		clear();
		print_treasure_room(&treasure);
		attron(A_REVERSE | COLOR_PAIR(color));
		mvprintw(position.y, position.x, "$");
		attroff(A_REVERSE | COLOR_PAIR(color));

		check_trap_and_distance_in_treasure_room(&treasure, &position, &achievement);
		check_around_for_enemy_in_treasure_room(&treasure, &position, &achievement);

		if(achievement.health <= 0 || achievement.hunger_bar <= 0) {     //check for being alive 
			lose_page(&achievement);
			return;
		}

		int ch = getch();
		int a = handle_input_key(ch);
		switch (a) {
			case 0:    //list and other 
				control_list_treasure_room(&treasure, &position, &achievement, ch);
				break;
			case 1:    //movement  (8 direction)
				control_movement_for_player_in_treasure(&treasure, &position, &achievement, ch);
				check_active_enchant(&achievement);
				check_hunger_bar(&achievement);
				move_alive_enemies_in_treasure_room(&treasure, &position, &achievement);
				break;
			case 2:    //fight    (Space)
				fight_in_treasure_room(&treasure, &position, &achievement);
				move_alive_enemies_in_treasure_room(&treasure, &position, &achievement);
				check_active_enchant(&achievement);
				check_hunger_bar(&achievement);
				break;
			case 3:     //exit     (Esc)
				if(exit_the_game() == true)
					return;
		}
	}
	
}
void start_location_random(location* start, room_info* room) {
	while(1) {
		start->x = rand() % (room->wide - 2) + 1;  //x
		start->y = rand() % (room->height - 2) + 1;  //y
		if(check_value(room, start->y, start->x, '.')) break;
	} 
	start->x += room->start_point.x;
	start->y += room->start_point.y;
}
void print_map_conditionally(floor_info* floor, location* place) {
	init_color(11, 1000, 843, 0);   // طلایی 
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, 11, COLOR_BLACK); // جفت طلایی
	init_pair(4, COLOR_YELLOW, COLOR_WHITE);
	init_pair(5, COLOR_BLUE, COLOR_BLACK); 

	clear();
	for(int i = 0; i < 6; i++) {
		if( place->x > floor->room[i].start_point.x  &&  place->x < floor->room[i].start_point.x + floor->room[i].wide - 1 &&
			place->y > floor->room[i].start_point.y  &&  place->y < floor->room[i].start_point.y + floor->room[i].height - 1 ) {
			if(i > floor->open_room)  floor->open_room = i;
			break;				
		}
	}
	if(floor->room[floor->open_room + 1].door[0].y == 0) {  //door is up
		if( place->x == floor->room[floor->open_room + 1].door[0].x + floor->room[floor->open_room + 1].start_point.x &&
			place->y == floor->room[floor->open_room + 1].start_point.y - 2)  floor->open_room += 1;
	}
	else if(floor->room[floor->open_room + 1].door[0].x == floor->room[floor->open_room + 1].wide - 1) {    //door is right
		if( place->x == floor->room[floor->open_room + 1].door[0].x + floor->room[floor->open_room + 1].start_point.x + 2 &&
			place->y == floor->room[floor->open_room + 1].door[0].y +  floor->room[floor->open_room + 1].start_point.y)  floor->open_room += 1;
	}
	else if(floor->room[floor->open_room + 1].door[0].y == floor->room[floor->open_room + 1].height - 1) {   //door is down
		if( place->x == floor->room[floor->open_room + 1].door[0].x + floor->room[floor->open_room + 1].start_point.x &&
			place->y == floor->room[floor->open_room + 1].door[0].y +  floor->room[floor->open_room + 1].start_point.y + 2)  floor->open_room += 1;
	}
	else if(floor->room[floor->open_room + 1].door[0].x == 0) {   //door is left
		if( place->x == floor->room[floor->open_room + 1].start_point.x - 2 &&
			place->y == floor->room[floor->open_room + 1].door[0].y +  floor->room[floor->open_room + 1].start_point.y)  floor->open_room += 1;		
	}
	for(int k = 0; k <= floor->open_room; k++) {   //number of rooms that should be printed
		for (int i = 0; i < floor->room[k].height; i++) {
			for(int j = 0; j < floor->room[k].wide; j++) {
				switch(floor->room[k].room_type) {
					case REGULAR_ROOM:
						attron(COLOR_PAIR(5)); //blue
						break;
					case ENCHANT_ROOM:
						attron(COLOR_PAIR(2));  //red
						break;
				}
				//mvprintw(floor->room[k].start_point.y + i, floor->room[k].start_point.x + j, "%c", floor->room[k].cell[i][j]);
				print_one_element(floor->room[k].start_point.y + i, floor->room[k].start_point.x + j, floor->room[k].cell[i][j]);
				attroff(COLOR_PAIR(5));
			}
		}
	}
	
	for(int i = 0; i < floor->number_corridor; i++) {
		if(place->x == floor->corridor[i].x  &&  place->y == floor->corridor[i].y) {
			if(i > floor->open_corridor) floor->open_corridor = i;
			break;
		}
	}
	for(int i = 0; i <= floor->open_corridor + 5; i++) {
		mvprintw(floor->corridor[i].y, floor->corridor[i].x, "\U00002591");
	}
}
void print_all_map(floor_info* floor) {
		for(int k = 0; k < 6; k++) {
			for (int i = 0; i < floor->room[k].height; i++) {
				for(int j = 0; j < floor->room[k].wide; j++) {
					switch(floor->room[k].room_type) {
						case REGULAR_ROOM:
							attron(COLOR_PAIR(5)); //blue
							break;
						case ENCHANT_ROOM:
							attron(COLOR_PAIR(2));  //red
							break;
					}
					//mvprintw(floor->room[k].start_point.y + i, floor->room[k].start_point.x + j, "%c", floor->room[k].cell[i][j]);
					print_one_element(floor->room[k].start_point.y + i, floor->room[k].start_point.x + j, floor->room[k].cell[i][j]);
					attroff(COLOR_PAIR(5));
				}
			}
		}
		for(int i = 0; i < floor->number_corridor; i++) {
			mvprintw(floor->corridor[i].y, floor->corridor[i].x, "\U00002591");
		}

		for(int i = 0; i < 7; i++) {          //print enemies 
		if(floor->enemy[i].health > 0) {
			mvprintw(floor->enemy[i].place.y, floor->enemy[i].place.x, "%c", floor->enemy[i].name);
		}
	}

}
void print_one_element(int y, int x, char value) {
	init_color(12, 1000, 1000, 0); //yellow
	init_color(11, 1000, 843, 0);   // رنگ طلایی 
	init_pair(1, COLOR_GREEN, COLOR_BLACK);         //-----> doors
	init_pair(2, COLOR_RED, COLOR_BLACK);           
    init_pair(3, 11, COLOR_BLACK); // جفت رنگ طلایی  //-----> Gold
	init_pair(4, 11, COLOR_RED);                    //-----> Black_Gold
	init_pair(5, COLOR_BLUE, COLOR_BLACK);          
	init_pair(6, COLOR_MAGENTA, COLOR_BLACK);       //-----> Food, weapons
	init_pair(7, COLOR_CYAN, COLOR_BLACK);          //-----> staircase
	init_pair(8, 12, COLOR_BLACK);                  //-----> enchants
	init_pair(9, COLOR_RED, COLOR_GREEN);           //-----> enemies

	switch (value) {
		case '<':  //staircase
			attron(COLOR_PAIR(7));
			mvprintw(y, x, "<");
			attroff(COLOR_PAIR(7));
			break;
		case 'T': //trap before  open
			mvprintw(y, x, ".");
			break;
		case 'g':  //GOLD
			attron(COLOR_PAIR(3));
			mvprintw(y, x, "\U000026c0"); 
			attroff(COLOR_PAIR(3));
			break;
		case 'B':   //BLACK_GOLD
			attron(COLOR_PAIR(4));
			mvprintw(y, x, "\U000026c2");
			attroff(COLOR_PAIR(4));
			break;
		case 'X':  //Sign for TREASURE_ROOM
			attron(COLOR_PAIR(3));
			mvprintw(y, x, "\U00002605"); 
			attroff(COLOR_PAIR(3));
			break;
		case 'M':    //FOOD (Meal)
			attron(COLOR_PAIR(6));
			mvprintw(y, x, "\U00002299");
			attroff(COLOR_PAIR(6));
			break;
		case 'd':    //Dagger ***
		case '1':
			attron(COLOR_PAIR(6));
			mvprintw(y, x, "\U0001F5E1");
			attroff(COLOR_PAIR(6));
			break;
		case 'w':    //Magic Wand
		case '2':
			attron(COLOR_PAIR(6));
			mvprintw(y, x, "\U00002020");
			attroff(COLOR_PAIR(6));
			break;
		case 'a':     //Normal Arrow
		case '3':
			attron(COLOR_PAIR(6));
			mvprintw(y, x, "\U000027B3");
			attroff(COLOR_PAIR(6));
			break;
		case 's':      //Sword
			attron(COLOR_PAIR(6));
			mvprintw(y, x, "\U00002694");
			attroff(COLOR_PAIR(6));
			break;
		case 'V':      //Speed enchant (velocity)
			attron(COLOR_PAIR(8));
			mvprintw(y, x, "\U000026f7");
			attroff(COLOR_PAIR(8));
			break;
		case 'i':      //Damage enchant (injury)
			attron(COLOR_PAIR(8));
			mvprintw(y, x, "\U00002620");
			attroff(COLOR_PAIR(8));
			break;
		case 'H':      //Health enchant
			attron(COLOR_PAIR(8));
			mvprintw(y, x, "\U00002695");
			attroff(COLOR_PAIR(8));
			break;
		case '+':      //door
		case '?':      //door
		case '@':      //door
			attron(COLOR_PAIR(1));
			mvprintw(y, x, "%c", value);
			attroff(COLOR_PAIR(1));
			break;
		case 'D':      //enemies
		case 'F':      //enemies
		case 'G':      //enemies
		case 'S':      //enemies
		case 'U':      //enemies
			attron(COLOR_PAIR(9));
			mvprintw(y, x, "%c", value);
			attroff(COLOR_PAIR(9));
			break;
		case '|':      //wall
			mvprintw(y, x, "\U00002503");
			break;
		case '_':      //wall
			mvprintw(y, x, "\U00002501");
			break;
		default :
			mvprintw(y, x, "%c", value);
	}
}
void generate_room(room_info* a_room, int number) {
	srand(time(NULL) + number * 23456);
    int a = (rand() % 4) + 8;
    int b = (rand() % 4) + 8;

    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            if (i == 0 || i == a - 1) a_room->cell[i][j] = '_';
            else if (j == 0 || j == b - 1) a_room->cell[i][j] = '|';
            else a_room->cell[i][j] = '.';
        }
    }
	a_room->height = a;
	a_room->wide = b;

	for(int i = 0; i < 5; i++) { //set to 0
		a_room->gold[i].x = 0;
		a_room->gold[i].y = 0;
		a_room->gold[i].value = 0;
	}

	int c = rand() % 4;  //random for location of first door
    int d;

	switch (c) {  //door
        case 0:    //door 1 is up
            d = rand() % (b - 2);
            a_room->cell[0][d+1] = door_symbol(a_room, 0, 1);
            a_room->door[0].y = 0;
            a_room->door[0].x = d+1;
			if(number == 0 || number == 5) break;
			d = rand() % (b - 2);   //creating second door---> down
            a_room->cell[a-1][d+1] = door_symbol(a_room, 1, 3);
            a_room->door[1].y = a-1;
            a_room->door[1].x = d+1;
            break;
        case 1:    //door 1 is right
            d = rand() % (a - 2);
            a_room->cell[d+1][b-1] = door_symbol(a_room, 0, 2);
            a_room->door[0].y = d+1;
            a_room->door[0].x = b-1;
			if(number == 0 || number == 5) break;
			d = rand() % (b - 2);   //creating second door---> down
            a_room->cell[a-1][d+1] = door_symbol(a_room, 1, 3);
            a_room->door[1].y = a-1;
            a_room->door[1].x = d+1;
            break;
        case 2:    //door 1 is down
            d = rand() % (b - 2);
            a_room->cell[a-1][d+1] = door_symbol(a_room, 0, 3);
            a_room->door[0].y = a-1;
            a_room->door[0].x = d+1;
			if(number == 0 || number == 5) break;
			d = rand() % (b - 2);    //creating second door---> up
            a_room->cell[0][d+1] = door_symbol(a_room, 1, 1);
            a_room->door[1].y = 0;
            a_room->door[1].x = d+1;
            break;
        case 3:    //door 1 is left
            d = rand() % (a - 2);
            a_room->cell[d+1][0] = door_symbol(a_room, 0, 4);
            a_room->door[0].y = d+1;
            a_room->door[0].x = 0;
			if(number == 0 || number == 5) break;
			d = rand() % (a - 2);    //creating second door---> right
            a_room->cell[d+1][b-1] = door_symbol(a_room, 1, 2);
            a_room->door[1].y = d+1;
            a_room->door[1].x = b-1;
            break;
    }

	c = rand() % 3; //0 or 1 or 2           //PILLAR
	for(int i = 0; i < c ; i++) {
		a = rand() % (a_room->wide - 2) + 1;    //x
		b = rand() % (a_room->height - 2) + 1;  //y
		if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
		a_room->cell[b][a] = 'O';
	}
	
	c = rand() % 3 + difficulty;            //FOOD      Easy--->1 to 3    Medium--->0 to 2     Hard--->0 to 1 
	for(int i = 0; i < c ; i++) { 
		a = rand() % (a_room->wide - 2) + 1;    //x
		b = rand() % (a_room->height - 2) + 1;  //y
		if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
		a_room->cell[b][a] = 'M';
	}

	c = rand() % 3; //0 or 1 or 2           //WEAPON 
	for(int i = 0; i < c ; i++) {
		a = rand() % (a_room->wide - 2) + 1;    //x
		b = rand() % (a_room->height - 2) + 1;  //y
		if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
		if(a % 4 == 0) { a_room->cell[b][a] = 'd'; continue;  } //Dagger
		if(a % 4 == 1) { a_room->cell[b][a] = 'w'; continue;  } //Magic Wand
		if(a % 4 == 2) { a_room->cell[b][a] = 'a'; continue;  } //Normal Arrow
		if(a % 4 == 3) { a_room->cell[b][a] = 's'; continue;  } //Sword
	}

	
	switch (a_room->room_type) {
		case REGULAR_ROOM:
			c = rand() % 2; //0 or 1        //GOLD
			for(int i = 0; i < c ; i++) {
				a = rand() % (a_room->wide - 2) + 1;    //x
				b = rand() % (a_room->height - 2) + 1;  //y
				if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
				a_room->gold[i].x = a;
				a_room->gold[i].y = b;
				a_room->gold[i].value = rand() % 10 + 1;
				a_room->cell[b][a] = 'g';
			}
			c = rand() % 3 + -(difficulty);        //TRAP        Easy--->0 to 1    Medium--->0 to 2     Hard--->1 to 3 
			for(int i = 0; i < c ; i++) {
				a = rand() % (a_room->wide - 2) + 1;    //x
				b = rand() % (a_room->height - 2) + 1;  //y
				if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
				a_room->cell[b][a] = 'T';
			}
			c = rand() % 2; //0 or 1         //ENCHANT
			for(int i = 0; i < c ; i++) {
				a = rand() % (a_room->wide - 2) + 1;    //x
				b = rand() % (a_room->height - 2) + 1;  //y
				if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
				if(a % 3 == 0) { a_room->cell[b][a] = 'H'; continue;  }   //Health
				if(a % 3 == 1) { a_room->cell[b][a] = 'V'; continue;  }   //Speed(velocity)
				if(a % 3 == 2) { a_room->cell[b][a] = 'i'; continue;  }   //damage(injury)
			}
			break;
		case ENCHANT_ROOM:
			c = rand() % 5 + 3; // 3 to 7    //ENCHANT
			for(int i = 0; i < c ; i++) {
				a = rand() % (a_room->wide - 2) + 1;    //x
				b = rand() % (a_room->height - 2) + 1;  //y
				if(check_value(a_room, b, a, '.') == false) { i-= 1; continue; }
				if(a % 3 == 0) { a_room->cell[b][a] = 'H'; continue;  }  //Health
				if(a % 3 == 1) { a_room->cell[b][a] = 'V'; continue;  }  //Speed(velocity)
				if(a % 3 == 2) { a_room->cell[b][a] = 'i'; continue;  }  //damage(injury)
			}
			break;
	}

}
bool check_value(room_info* room, int y, int x, char value) {
	if(room->cell[y][x] == value) return true;
	else return false;
}
char door_symbol(room_info* room, int n, int side) {
	if(room->door_type[n] == REGULAR_DOOR)  return '+';
	if(room->door_type[n] == PASSWORD_DOOR) return '@';
	if(room->door_type[n] == SECRET_DOOR) {
		if(side == 1 || side == 3)      return '_';
		else                            return '|';
	}
}
void generate_a_floor(floor_info* a_floor, int number) {
	refresh();
	srand(time(NULL));
	for(int i = 0; i < 6; i++) {
		int a = rand() % 100;
		switch (i) {    //determine type of room
			case 0:
				if(number == 0)     //first floor
					a_floor->room[0].room_type = REGULAR_ROOM;
				else 
					if(a < 50)     //50% probability
						a_floor->room[0].room_type = REGULAR_ROOM;
					else 
						a_floor->room[0].room_type = ENCHANT_ROOM;
				break;
			case 1:
				if(a_floor->room[i-1].door_type[0] == SECRET_DOOR) 
					a_floor->room[i].room_type = ENCHANT_ROOM;
				else
					a_floor->room[i].room_type = REGULAR_ROOM;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(a_floor->room[i-1].door_type[1] == SECRET_DOOR) 
					a_floor->room[i].room_type = ENCHANT_ROOM;
				else
					a_floor->room[i].room_type = REGULAR_ROOM;
				break;
		}

		if(i == 0 || i == 5) {
			if(a < 50)
				a_floor->room[i].door_type[0] = REGULAR_DOOR;
			else
				a_floor->room[i].door_type[0] = PASSWORD_DOOR;
				a_floor->room[i].lock_door = true; /////////////
		}
		else if(a <= 35) {
			a_floor->room[i].door_type[0] = REGULAR_DOOR; 
			a_floor->room[i].door_type[1] = REGULAR_DOOR; 
		}
		else if(a > 35 && a <= 70) {
			a_floor->room[i].door_type[0] = REGULAR_DOOR; 
			a_floor->room[i].door_type[1] = PASSWORD_DOOR;
			a_floor->room[i].lock_door = true;  /////////////
		}
		else if(a > 70 && a < 100) {
			a_floor->room[i].door_type[0] = PASSWORD_DOOR; 
			a_floor->room[i].lock_door = true;  /////////////
			a_floor->room[i].door_type[1] = SECRET_DOOR; 
		}
		// else if(a > 75 && a < 100) {
		// 	a_floor->room[i].door_type[0] = REGULAR_DOOR; 
		// 	a_floor->room[i].door_type[1] = SECRET_DOOR; 
		// }
		
		generate_room(&a_floor->room[i], i);
	}
	
	int a;
	int b;

	for(int k = 0; k < 6; k++) {
		a = (rand() % 25) + 6;
		b = (rand() % 15) + 3 + k * 30;
		a_floor->room[k].start_point.x = b;
		a_floor->room[k].start_point.y = a;
	}
	
	//raahro 
	int index = 0;
	for(int i = 0; i < 5; i++) {
		index = handle_corridor(a_floor, i, i + 1, index);
	}
	a_floor->number_corridor = index;

	while(1) {                     //staircase
		int d = rand() % 5 + 1;
		a = rand() % a_floor->room[d].height;  //this is y
		b = rand() % a_floor->room[d].wide;    //this is x
		if(a_floor->room[d].cell[b][a] == '.') {
			a_floor->room[d].cell[b][a] = '<';
			break;
		}
	}

	int c = rand() % 3 + 3;              //BLACK_GOLD (just in room 3 or 4 or 5)
	while(1) {
		a = rand() % (a_floor->room[c].wide - 2) + 1;    //x
		b = rand() % (a_floor->room[c].height - 2) + 1;  //y
		if(check_value(&a_floor->room[c], b, a, '.') == false) continue;
		a_floor->room[c].black_gold.x = a;
		a_floor->room[c].black_gold.y = b;
		a_floor->room[c].black_gold.value = rand() % 20 + 50;  //This is between 50 to 70
		a_floor->room[c].cell[b][a] = 'B';
		break;
	}

	if(number == 3) { 
		c = rand() % 3 + 3; //3 to 5        // The room with sign for TREASURE_ROOM
		while(1) {
			a = rand() % (a_floor->room[c].wide - 2) + 1;    //x
			b = rand() % (a_floor->room[c].height - 2) + 1;  //y
			if(check_value(&a_floor->room[c], b, a, '.') == false) continue;
			a_floor->room[c].cell[b][a] = 'X';         //sign for TREASURE_ROOM
			break;
		}
	}

	for(int i = 0; i < 16; i++) {            //set all to 0
		a_floor->enemy[i].health = 0;
	}
	a_floor->number_enemy = 0;
	int n[2];
	for(int i = 0; i < 2; i++) {
		n[i] = rand() % 5 + 1;
		if(a_floor->room[n[i]].room_type == ENCHANT_ROOM) { i -= 1; continue;}
		if(i == 1 && n[1] == n[0]) { i -= 1; continue;}
	}
	int d;
	for(int i = 0; i < 2; i++) {  //2 rooms have enemies          //Enemies
		d = rand() % 2 + 1;  //1 or 2 enemies in a room
		for(int j = 0; j < d; j++) {
			while(1) {
				a = rand() % (a_floor->room[n[i]].wide - 2) + 1;    //x
				b = rand() % (a_floor->room[n[i]].height - 2) + 1;  //y
				if(check_value(&a_floor->room[n[i]], b, a, '.') == false) continue;
				if(a % 5 == 0) {      //Deamon
					//a_floor->room[n[i]].cell[b][a] = 'D';
					a_floor->enemy[a_floor->number_enemy].name = 'D';
					a_floor->enemy[a_floor->number_enemy].health = 5;
					a_floor->enemy[a_floor->number_enemy].room = n[i];
					a_floor->enemy[a_floor->number_enemy].place.x = a + a_floor->room[n[i]].start_point.x;
					a_floor->enemy[a_floor->number_enemy].place.y = b + a_floor->room[n[i]].start_point.y;
					a_floor->number_enemy += 1;
					break;  
				} 
				if(a % 5 == 1) {    //Fire Breathing Monster
					//a_floor->room[n[i]].cell[b][a] = 'F';
					a_floor->enemy[a_floor->number_enemy].name = 'F';
					a_floor->enemy[a_floor->number_enemy].health = 10;
					a_floor->enemy[a_floor->number_enemy].room = n[i];
					a_floor->enemy[a_floor->number_enemy].place.x = a + a_floor->room[n[i]].start_point.x;
					a_floor->enemy[a_floor->number_enemy].place.y = b + a_floor->room[n[i]].start_point.y;
					a_floor->number_enemy += 1;
					break;
				} 
				if(a % 5 == 2) {        //Giant
					//a_floor->room[n[i]].cell[b][a] = 'G'; 
					a_floor->enemy[a_floor->number_enemy].name = 'G';
					a_floor->enemy[a_floor->number_enemy].health = 15;
					a_floor->enemy[a_floor->number_enemy].room = n[i];
					a_floor->enemy[a_floor->number_enemy].place.x = a + a_floor->room[n[i]].start_point.x;
					a_floor->enemy[a_floor->number_enemy].place.y = b + a_floor->room[n[i]].start_point.y;
					a_floor->number_enemy += 1;
					break;  
				} 
				if(a % 5 == 3) {         //Snake
					//a_floor->room[n[i]].cell[b][a] = 'S'; 
					a_floor->enemy[a_floor->number_enemy].name = 'S';
					a_floor->enemy[a_floor->number_enemy].health = 20;
					a_floor->enemy[a_floor->number_enemy].room = n[i];
					a_floor->enemy[a_floor->number_enemy].place.x = a + a_floor->room[n[i]].start_point.x;
					a_floor->enemy[a_floor->number_enemy].place.y = b + a_floor->room[n[i]].start_point.y;
					a_floor->number_enemy += 1;
					break;  
				} 
				if(a % 5 == 4) {        //Undeed
					//a_floor->room[n[i]].cell[b][a] = 'U'; 
					a_floor->enemy[a_floor->number_enemy].name = 'U';
					a_floor->enemy[a_floor->number_enemy].health = 30;
					a_floor->enemy[a_floor->number_enemy].room = n[i];
					a_floor->enemy[a_floor->number_enemy].place.x = a + a_floor->room[n[i]].start_point.x;
					a_floor->enemy[a_floor->number_enemy].place.y = b + a_floor->room[n[i]].start_point.y;
					a_floor->number_enemy += 1;
					break;  
				} 
			}
		}
	}
	
	refresh();
}
void generate_treasure_room(treasure_info* treasure) {
	srand(time(NULL));
    int a = (rand() % 8) + 12; 
    int b = (rand() % 8) + 12;
	int c;

    for (int i = 0; i < a; i++) {
        for (int j = 0; j < b; j++) {
            if (i == 0 || i == a - 1) treasure->room.cell[i][j] = '_';
            else if (j == 0 || j == b - 1) treasure->room.cell[i][j] = '|';
            else treasure->room.cell[i][j] = '.';
        }
    }
	treasure->room.height = a;
	treasure->room.wide = b;

	treasure->room.start_point.x = 60;
	treasure->room.start_point.y = 15;

	c = rand() % 10 + 15; //15 to 24           //TRAP
	for(int i = 0; i < c ; i++) {
		a = rand() % (treasure->room.wide - 2) + 1;    //x
		b = rand() % (treasure->room.height - 2) + 1;  //y
		if(check_value(&treasure->room, b, a, '.') == false) { i-= 1; continue; }
		treasure->room.cell[b][a] = 'T';
	}

	c = rand() % 3; //0 or 1 or 2           //PILLAR
	for(int i = 0; i < c ; i++) {
		a = rand() % (treasure->room.wide - 2) + 1;    //x
		b = rand() % (treasure->room.height - 2) + 1;  //y
		if(check_value(&treasure->room, b, a, '.') == false) { i-= 1; continue; }
		treasure->room.cell[b][a] = 'O';
	}


	for(int i = 0; i < 1 ; i++) {             //TREASURE
		a = rand() % (treasure->room.wide - 2) + 1;    //x
		b = rand() % (treasure->room.height - 2) + 1;  //y
		if(check_value(&treasure->room, b, a, '.') == false) { i-= 1; continue; }
		treasure->room.cell[b][a] = 'B';
	}


	for(int i = 0; i < 20; i++) {            //set all to 0
		treasure->enemy[i].health = 0;
	}
	treasure->number_enemy = 0;
	
	c = rand() % 3 + 7 + -(difficulty)*2;         //enemies    Easy--->5 to 7    Medium--->7 to 9     Hard--->9 to 11
	for(int i = 0; i < c ; i++) {
		a = rand() % (treasure->room.wide - 2) + 1;    //x
		b = rand() % (treasure->room.height - 2) + 1;  //y
		if(check_value(&treasure->room, b, a, '.') == false) { i-= 1; continue; }
		if(a % 5 == 0) {      //Deamon                                   //random type for enemy
			treasure->enemy[treasure->number_enemy].name = 'D';
			treasure->enemy[treasure->number_enemy].health = 5;
			treasure->enemy[treasure->number_enemy].place.x = a + treasure->room.start_point.x;
			treasure->enemy[treasure->number_enemy].place.y = b + treasure->room.start_point.y;
			treasure->number_enemy += 1;
			continue;  
		} 
		if(a % 5 == 1) {    //Fire Breathing Monster
			treasure->enemy[treasure->number_enemy].name = 'F';
			treasure->enemy[treasure->number_enemy].health = 10;
			treasure->enemy[treasure->number_enemy].place.x = a + treasure->room.start_point.x;
			treasure->enemy[treasure->number_enemy].place.y = b + treasure->room.start_point.y;
			treasure->number_enemy += 1;
			continue;
		} 
		if(a % 5 == 2) {        //Giant
			treasure->enemy[treasure->number_enemy].name = 'G';
			treasure->enemy[treasure->number_enemy].health = 15;
			treasure->enemy[treasure->number_enemy].place.x = a + treasure->room.start_point.x;
			treasure->enemy[treasure->number_enemy].place.y = b + treasure->room.start_point.y;
			treasure->number_enemy += 1;
			continue;  
		} 
		if(a % 5 == 3) {         //Snake
			treasure->enemy[treasure->number_enemy].name = 'S';
			treasure->enemy[treasure->number_enemy].health = 20;
			treasure->enemy[treasure->number_enemy].place.x = a + treasure->room.start_point.x;
			treasure->enemy[treasure->number_enemy].place.y = b + treasure->room.start_point.y;
			treasure->number_enemy += 1;
			continue;  
		} 
		if(a % 5 == 4) {        //Undeed
			treasure->enemy[treasure->number_enemy].name = 'U';
			treasure->enemy[treasure->number_enemy].health = 30;
			treasure->enemy[treasure->number_enemy].place.x = a + treasure->room.start_point.x;
			treasure->enemy[treasure->number_enemy].place.y = b + treasure->room.start_point.y;
			treasure->number_enemy += 1;
			continue;  
		} 
	}

}
void print_treasure_room(treasure_info* treasure) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	mvprintw(10, 50, "This is TREASURE_ROOM and you should defeat all the enemies!\n");
	for (int i = 0; i < treasure->room.height; i++) {
		for(int j = 0; j < treasure->room.wide; j++) {  //This will be printed from   X: 60  Y: 15
			//mvprintw(treasure->room.start_point.y + i, treasure->room.start_point.x + j, "%c", treasure->room.cell[i][j]);
			attron(COLOR_PAIR(1)); //Green
			print_one_element(treasure->room.start_point.y + i, treasure->room.start_point.x + j, treasure->room.cell[i][j]);
			attroff(COLOR_PAIR(1));
		}
	}
	for(int i = 0; i < 20; i++) {
		if(treasure->enemy[i].health > 0) {
			mvprintw(treasure->enemy[i].place.y, treasure->enemy[i].place.x, "%c", treasure->enemy[i].name);
		}
	}
}
int handle_corridor(floor_info* floor, int first, int second, int index) {
	int a = 1;
	if(first == 0) a = 0;

	floor->corridor[index].x = (floor->room[first].door[a].x) + floor->room[first].start_point.x;
	floor->corridor[index].y = (floor->room[first].door[a].y) + floor->room[first].start_point.y;

	if(floor->room[first].door[a].y == 0) {   //door is up
		floor->corridor[index].y -= 1;
		index += 1;
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].y -= 1;
		index += 1;
	}
	else if (floor->room[first].door[a].x == floor->room[first].wide - 1) {  //door is right
		floor->corridor[index].x += 1;
		index += 1;
	}
	else if (floor->room[first].door[a].y == floor->room[first].height - 1) {  //door is down
		floor->corridor[index].y += 1;
		index += 1;
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].y += 1;
		index += 1;
	}
	else if (floor->room[first].door[a].x == 0) {  //door is left
		floor->corridor[index].x -= 1;
		index += 1;
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].x -= 1;
		index += 1;
		while(floor->corridor[index-1].y > floor->room[first].start_point.y - 3) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].y -= 1;
			index += 1;
		}
	}
	
	while(floor->corridor[index-1].x < (first + 1) * 30) {
		floor->corridor[index] = floor->corridor[index - 1];
		floor->corridor[index].x += 1;
		index += 1;
	}

	while(floor->corridor[index-1].y > floor->room[second].door[0].y + floor->room[second].start_point.y) {
		floor->corridor[index] = floor->corridor[index - 1];
		floor->corridor[index].y -= 1;
		index += 1;
	}
	while(floor->corridor[index-1].y < floor->room[second].door[0].y + floor->room[second].start_point.y) {
		floor->corridor[index] = floor->corridor[index - 1];
		floor->corridor[index].y += 1;
		index++;
	}

	if(floor->room[second].door[0].y == 0) {   //door is up
		while(floor->corridor[index-1].x < floor->room[second].start_point.x - 3) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
		floor->corridor[index] = floor->corridor[index - 1];
		floor->corridor[index].y -= 1;
		index += 1;
		floor->corridor[index] = floor->corridor[index - 1];
		floor->corridor[index].y -= 1;
		index += 1;
		while(floor->corridor[index-1].x < floor->room[second].door[0].x + floor->room[second].start_point.x) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
		floor->corridor[index] = floor->corridor[index - 1];
		floor->corridor[index].y += 1;
		index += 1;
	}
	else if (floor->room[second].door[0].x == floor->room[second].wide - 1) {  //door is right
		while(floor->corridor[index-1].x < floor->room[second].start_point.x - 3) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
		while (floor->corridor[index-1].y > floor->room[second].start_point.y - 3) {
			floor->corridor[index] = floor->corridor[index - 1];
			floor->corridor[index].y -= 1;
			index += 1;
		}
		while(floor->corridor[index-1].x < floor->room[second].door[0].x + floor->room[second].start_point.x + 3) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
		while (floor->corridor[index-1].y < floor->room[second].door[0].y + floor->room[second].start_point.y) {
			floor->corridor[index] = floor->corridor[index - 1];
			floor->corridor[index].y += 1;
			index += 1;
		}
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].x -= 1;
		index += 1;
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].x -= 1;
		index += 1;
	}
	else if (floor->room[second].door[0].y == floor->room[second].height - 1) {  //door is down
		while(floor->corridor[index-1].x < floor->room[second].start_point.x - 3) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].y += 1;
		index += 1;
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].y += 1;
		index += 1;
		while(floor->corridor[index-1].x < floor->room[second].door[0].x + floor->room[second].start_point.x) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
		floor->corridor[index] = floor->corridor[index-1];
		floor->corridor[index].y -= 1;
		index += 1;
	}
	else if (floor->room[second].door[0].x == 0) {  //door is left
		while(floor->corridor[index-1].x < floor->room[second].start_point.x - 1) {
			floor->corridor[index] = floor->corridor[index-1];
			floor->corridor[index].x += 1;
			index += 1;
		}
	}
	for(int i = 0; i < 5; i++) {    //repeat 5 last indexes
		floor->corridor[index] = floor->corridor[index-1];
		index += 1;
	}
	return index;
}
void control_list_and_inputs(floor_info* floor, location* place, int* num_floor, achievement_info* achievement, int ch) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	int current;
	current = current_room(floor, place);

	switch (ch)
	{
	case '>':     //Next floor
		if( check_value(&floor->room[current], place->y - floor->room[current].start_point.y, place->x - floor->room[current].start_point.x, '<') && 
			*num_floor >= 0 && *num_floor <= 2)
			*num_floor += 1;
		break;
	case '<':     //Previous floor
		if(check_value(&floor->room[current], place->y - floor->room[current].start_point.y, place->x - floor->room[current].start_point.x, '<') && 
			*num_floor >= 1 && *num_floor <= 3)
			*num_floor -= 1;
		break;
	case 'g':     //get_a_thing
		if(current >= 0) {
			pickup_a_thing(&floor->room[current], place, achievement);
		}
		break;
	case 'i':     //list for weapons
		list_of_weapon(&achievement->weapon);
		break;
	case 'e':     //list for enchants
		list_of_enchant(&achievement->enchant);
		break;
	case 'H':     //show hunger_bar and Health_rate
		show_health_rate(achievement);
		break;
	case 'G':     //represent Gold saved
		clear();
		attron(COLOR_PAIR(1));
		mvprintw(1, 10, "The amount of your collected Gold ---> %d", achievement->save_gold);
		attroff(COLOR_PAIR(1));
		getch();
		break;
	case 'M':   //represent all the map
		print_all_map(floor);
		attron(A_REVERSE | COLOR_PAIR(3));
		mvprintw(place->y, place->x, "$");
		attroff(A_REVERSE | COLOR_PAIR(3));
		ch = getch();
		break;
	case KEY_F(2):   //SHOW HELP
		show_help_for_game();
		break;
	}
}
void control_movement_for_player(floor_info* floor, location* place, achievement_info* achievement, int ch) {
	switch (ch)
	{
	case KEY_UP:
		place->y -= 1;
		if(check_location(floor, place) == false) place->y += 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_DOWN:
		place->y += 1;
		if(check_location(floor, place) == false) place->y -= 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_RIGHT:
		place->x += 1;
		if(check_location(floor, place) == false) place->x -= 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_LEFT:
		place->x -= 1;
		if(check_location(floor, place) == false) place->x += 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_NPAGE:
		place->x += 1;
		place->y += 1;
		if(check_location(floor, place) == false) { place->x -= 1; place->y -= 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_HOME:
		place->x -= 1;
		place->y -= 1;
		if(check_location(floor, place) == false) { place->x += 1; place->y += 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_PPAGE:
		place->x += 1;
		place->y -= 1;
		if(check_location(floor, place) == false) { place->x -= 1; place->y += 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	case KEY_END:
		place->x -= 1;
		place->y += 1;
		if(check_location(floor, place) == false) { place->x += 1; place->y -= 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement);}
		break;
	}
}
void control_movement_for_player_in_treasure(treasure_info* treasure, location* place, achievement_info* achievement, int ch) {
	switch (ch)
	{
	case KEY_UP:
		place->y -= 1;
		if(check_location_in_treasure_room(treasure, place) == false) place->y += 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_DOWN:
		place->y += 1;
		if(check_location_in_treasure_room(treasure, place) == false) place->y -= 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_RIGHT:
		place->x += 1;
		if(check_location_in_treasure_room(treasure, place) == false) place->x -= 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_LEFT:
		place->x -= 1;
		if(check_location_in_treasure_room(treasure, place) == false) place->x += 1;
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_NPAGE:
		place->x += 1;
		place->y += 1;
		if(check_location_in_treasure_room(treasure, place) == false) { place->x -= 1; place->y -= 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_HOME:
		place->x -= 1;
		place->y -= 1;
		if(check_location_in_treasure_room(treasure, place) == false) { place->x += 1; place->y += 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_PPAGE:
		place->x += 1;
		place->y -= 1;
		if(check_location_in_treasure_room(treasure, place) == false) { place->x -= 1; place->y += 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	case KEY_END:
		place->x -= 1;
		place->y += 1;
		if(check_location_in_treasure_room(treasure, place) == false) { place->x += 1; place->y -= 1; }
		else { achievement->movement += 1; cure_when_is_unhungry(achievement); }
		break;
	}
}
void control_list_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int ch) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);

	switch (ch)
	{
	case 'i':     //list for weapons
		list_of_weapon(&achievement->weapon);
		break;
	case 'e':     //list for enchants
		list_of_enchant(&achievement->enchant);
		break;
	case 'G':     //represent Gold saved
		clear();
		attron(COLOR_PAIR(1));
		mvprintw(1, 10, "The amount of your collected Gold ---> %d", achievement->save_gold);
		attroff(COLOR_PAIR(1));
		getch();
		break;
	case 'H':     //show hunger_bar and Health_rate
		show_health_rate(achievement);
		break;
	case 'g':
		pickup_a_thing(&treasure->room, place, achievement);
		break;
	case KEY_F(2):   //SHOW HELP
		show_help_for_game();
		break;
	}
}
bool check_location(floor_info* floor, location* place) {
	for(int i = 0; i < 6; i++) {
		if( place->x > floor->room[i].start_point.x  &&  place->x < floor->room[i].start_point.x + floor->room[i].wide - 1 &&
			place->y > floor->room[i].start_point.y  &&  place->y < floor->room[i].start_point.y + floor->room[i].height - 1 ) {
				if(floor->room[i].cell[place->y - floor->room[i].start_point.y][place->x - floor->room[i].start_point.x] == 'O' ) //||
					//check_location_as_be_enemy(floor, place) == true) 
					return false;
				else 
					return true;
		}
	}
	//if didn't return
	for(int i = 0; i < floor->number_corridor; i++) {
		if(place->x == floor->corridor[i].x && place->y == floor->corridor[i].y) 
			return true;
	}

	for(int i = 0; i < 6; i++) {  //نوع در مهم است
		for(int j = 0; j < 2; j++) {
			if( place->x == floor->room[i].door[j].x + floor->room[i].start_point.x && 
				place->y == floor->room[i].door[j].y + floor->room[i].start_point.y ) {
				switch(floor->room[i].door_type[j]) {
					case PASSWORD_DOOR:
						//open_password_door(&floor->room[i], j);
						return floor->room[i].lock_door;
					case REGULAR_DOOR:
						return true;
					case SECRET_DOOR:
						floor->room[i].cell[floor->room[i].door[j].y][floor->room[i].door[j].x] = '?';
						return true;				
				}
			}
			if(i == 0 || i == 5) j++;
		}
	}

	return false;
}
bool check_location_in_treasure_room(treasure_info* treasure, location* place) {
	if( place->x > treasure->room.start_point.x  &&  place->x < treasure->room.start_point.x + treasure->room.wide - 1 &&
		place->y > treasure->room.start_point.y  &&  place->y < treasure->room.start_point.y + treasure->room.height - 1 ) {
			if(treasure->room.cell[place->y - treasure->room.start_point.y][place->x - treasure->room.start_point.x] == 'O') 
				return false;
			else {
				return true;
				// for(int i = 0; i < 20; i++) {
				// 	if( treasure->enemy[i].health > 0 && 
				// 		place->x == treasure->enemy[i].place.x && 
				// 		place->y == treasure->enemy[i].place.y ) {
				// 			return false;
				// 	}
				// }
				// return true;
			}
	}
	return false;
}
bool check_location_as_be_enemy(floor_info* floor, location* place, int j) {
	for(int i = 0; i < 16; i++) {
		if(i == j) continue;
		if(floor->enemy[i].health > 0 && floor->enemy[i].room <= floor->open_room) {
			if(floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y)
				return true;
		}
	}
	return false;
}
bool check_location_as_be_enemy_in_treasure_room(treasure_info* treasure, location* place, int j) {
	for(int i = 0; i < 20; i++) {
		if(i == j) continue;
		if(treasure->enemy[i].health > 0) {
			if(treasure->enemy[i].place.x == place->x && treasure->enemy[i].place.y == place->y)
				return true;
		}
	}
	return false;
}
int check_location_as_be_enemy_with_index(floor_info* floor, location* place) {
	for(int i = 0; i < 16; i++) {
		if(floor->enemy[i].health > 0 && floor->enemy[i].room <= floor->open_room) {
			if(floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y)
				return i - 20;                  //it decrease 20 unit for a sign
		}
	}
	return 0;
}
int check_location_as_be_enemy_with_index_in_treasure_room(treasure_info* treasure, location* place) {
	for(int i = 0; i < 20; i++) {
		if(treasure->enemy[i].health > 0) {
			if(treasure->enemy[i].place.x == place->x && treasure->enemy[i].place.y == place->y)
				return i - 20;                  //it decrease 20 unit for a sign
		}
	}
	return 0;
}
int current_room(floor_info* floor, location* place) {
	for(int i = 0; i < 6; i++) {
		if( place->x > floor->room[i].start_point.x  &&  place->x < floor->room[i].start_point.x + floor->room[i].wide - 1 &&
			place->y > floor->room[i].start_point.y  &&  place->y < floor->room[i].start_point.y + floor->room[i].height - 1 ) {
				return i;				
		}
	}
	return -1;
}
void pickup_a_thing(room_info* room, location* place, achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	switch (room->cell[place->y - room->start_point.y][place->x - room->start_point.x])
	{
	case 'd':      //Dagger
			mvprintw(1, 5, "Weapon was gotten :  Dagger \U0001F5E1  10");
			achievement->weapon.dagger_amount += 10;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case '1':      //Dagger             Thrown
			mvprintw(1, 5, "Weapon was gotten :  Dagger \U0001F5E1  1");
			achievement->weapon.dagger_amount += 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case 'w':      //Magic Wand
			mvprintw(1, 5, "Weapon was gotten :  Magic Wand \U00002020  8");
			achievement->weapon.wand_amount += 8;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case '2':      //Magic Wand           Thrown
			mvprintw(1, 5, "Weapon was gotten :  Magic Wand \U00002020  1");
			achievement->weapon.wand_amount += 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case 'a':      //Normal Arrow
			mvprintw(1, 5, "Weapon was gotten :  Normal Arrow \U000027B3  20");
			achievement->weapon.arrow_amount += 20;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case '3':      //Normal Arrow        Thrown
			mvprintw(1, 5, "Weapon was gotten :  Normal Arrow \U000027B3  1");
			achievement->weapon.arrow_amount += 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case 's':      //Sword
			mvprintw(1, 5, "Weapon was gotten :  Sword \U00002694");
			achievement->weapon.sword_amount = 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case 'V':      //Speed enchant (velocity)
			mvprintw(1, 5, "Weapon was gotten :  Speed enchant \U000026f7");
			achievement->enchant.speed_amount += 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case 'i':      //Damage enchant (injury)
			mvprintw(1, 5, "enchant was gotten :  Damage enchant \U00002620");
			achievement->enchant.damage_amount += 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	case 'H':      //Health enchant
			mvprintw(1, 5, "enchant was gotten :  Health enchant \U00002695");
			achievement->enchant.health_amount += 1;
			room->cell[place->y - room->start_point.y][place->x - room->start_point.x] = '.';
			getch();
		break;
	}
	attroff(COLOR_PAIR(1));
}
void pickup_and_check_trap_and_health(floor_info* floor, location* place, achievement_info* achievement) {
	int current;
	current = current_room(floor, place);
	if(current >= 0) {
		pickup_a_gold_food(&floor->room[current], place, achievement);
		check_for_trap(&floor->room[current], place, achievement);
	}
	if(achievement->movement > 100 + difficulty * 50) {   //It will be hungry after a distance
		achievement->hunger_bar -= 1;
		achievement->movement = 0;
	}
}
void check_trap_and_distance_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement) {
	check_for_trap(&treasure->room, place, achievement);
	
	if(achievement->movement > 100 + difficulty * 50) {   //It will be hungry after a distance
		achievement->hunger_bar -= 2;
		achievement->movement = 0;
	}
}
void pickup_a_gold_food(room_info* room, location* place, achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	int a = place->x - room->start_point.x;
	int b = place->y - room->start_point.y;
	if(room->cell[b][a] == 'g') {
		for(int i = 0; i < 5; i++) {
			if(	room->gold[i].x == a || room->gold[i].y == b ) {
				achievement->save_gold += room->gold[i].value;
				mvprintw(1, 5, "Some Gold \U0001F4B0 was gotten with a value %d", room->gold[i].value);
				room->cell[b][a] = '.';
			}
		}
	}
	else if (room->cell[b][a] == 'B') {
		achievement->save_gold += room->black_gold.value;
		mvprintw(1, 5, "Some Black_Gold \U0001FA99 was gotten with a value %d", room->black_gold.value);
		room->cell[b][a] = '.';
	}
	else if (room->cell[b][a] == 'M') {
		if(achievement->food_amount == 5) {
			mvprintw(1, 5, "You can't pick_up more than 5 meals!");
		}
		else {
			mvprintw(1, 5, "You put some food \U0001F354 in your bag!");
			achievement->food_amount += 1;
			room->cell[b][a] = '.';
		}
	}
	attroff(COLOR_PAIR(1));
}
void check_for_trap(room_info* room, location* place, achievement_info* achievement) {
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(2));
	int a = place->x - room->start_point.x;
	int b = place->y - room->start_point.y;
	if(room->cell[b][a] == 'T') {
				mvprintw(1, 5, "You have fallen in Trap!");
				mvprintw(2, 5, "Your health_rate was decreased 5 units!");
				achievement->health -= 5;
				room->cell[b][a] = '^';
	}
	attroff(COLOR_PAIR(2));
}
void list_of_weapon(weapon_info* weapon) {
	clear();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_color(11, 1000, 843, 0);   // رنگ طلایی 
	init_pair(3, 11, COLOR_BLACK);
	attron(COLOR_PAIR(3));
	mvprintw(1, 50, "Press p to put your weapon in your bag and Change your current_weapon\n");
	switch (weapon->current) {
		case 'm': mvprintw(1, 24, "(Mace)"); break;
		case 's': mvprintw(1, 24, "(Sword)"); break;
		case 'd': mvprintw(1, 24, "(Dagger)"); break;
		case 'w': mvprintw(1, 24, "(Magic Wand)"); break;
		case 'a': mvprintw(1, 24, "(Normal Arrow)"); break;
	}
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(1));
	mvprintw(1, 10, "weapon list:");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	mvprintw(5, 10, "Short_range weapons:");
	mvprintw(7, 10,     "Name        Key     Power");
	mvprintw(12, 10, "Thrown weapons:");
	mvprintw(14, 10, "Name             Key      Number    Power   ");
	attroff(COLOR_PAIR(2));

	attron(COLOR_PAIR(1));
	mvprintw(8, 10,     "Mace \u2692      'm'       5  ");
	if(weapon->sword_amount > 0)
		mvprintw(9, 10, "Sword \u2694     's'      10");
	mvprintw(15, 10, "Dagger \U0001F5E1         'd'        %-3d      12 ", weapon->dagger_amount);
	mvprintw(16, 10, "Magic Wand \u2020     'w'        %-3d      15  ", weapon->wand_amount);
	mvprintw(17, 10, "Normal Arrow \u27B3   'a'        %-3d       5 ", weapon->arrow_amount);
	attroff(COLOR_PAIR(1));
	int ch = getch();
	if(ch == 'p') {
		change_weapon(weapon);
	}
	else if(ch == 'm' || ch == 's' || ch == 'd' || ch == 'w' || ch == 'a') {
		attron(COLOR_PAIR(2));
		mvprintw(2, 50, "First Put your weapon in your bag!\n");
		attroff(COLOR_PAIR(2));
		getch();
	}
}
void list_of_enchant(enchant_info* enchant) {
	clear();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_color(11, 1000, 843, 0);   // رنگ طلایی 
	init_pair(3, 11, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	mvprintw(1, 10, "enchant list:");
	attroff(COLOR_PAIR(1));
	attron(COLOR_PAIR(2));
	mvprintw(5, 10, "Name              Key        Number");
	attroff(COLOR_PAIR(2));

	attron(COLOR_PAIR(3));
	mvprintw(1, 60, "Enter a Enchant_Key to activate one!");
	if(enchant->active == true) {
		switch (enchant->current) {
			case 'd': mvprintw(1, 25, "(Damage)"); break;
			case 's': mvprintw(1, 25, "(Speed)"); break;
			case 'h': mvprintw(1, 25, "(Health)"); break;
		}
	}
	else mvprintw(1, 25, "(No)");
	attroff(COLOR_PAIR(3));

	attron(COLOR_PAIR(1));
	mvprintw(6, 10, "Health \u2695          'h'          %-3d", enchant->health_amount);
	mvprintw(7, 10, "Speed \u26f7           's'          %-3d", enchant->speed_amount);
	mvprintw(8, 10, "Damage \u2620          'd'          %-3d", enchant->damage_amount);
	int ch = getch();

	switch (ch) {
		case 'h':
			if(enchant->health_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(2, 60, "You don't have enough number of this enchant!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(2, 60, "Health \u2695 enchant was activated!\n");
				enchant->current = 'h';
				enchant->health_amount -= 1;
				enchant->active = true;
				attroff(COLOR_PAIR(1));
			}
			break;
		case 's':
			if(enchant->speed_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(2, 60, "You don't have enough number of this enchant!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(2, 60, "Speed \u26f7 enchant was activated!\n");
				enchant->current = 's';
				enchant->speed_amount -= 1;
				enchant->active = true;
				attroff(COLOR_PAIR(1));
			}
			break;
		case 'd':
			if(enchant->damage_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(2, 60, "You don't have enough number of this enchant!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(2, 60, "Damage \u2620 enchant was activated!\n");
				enchant->current = 'd';
				enchant->damage_amount -= 1;
				enchant->active = true;
				attroff(COLOR_PAIR(1));
			}
			break;
	}
	getch();
}
void change_weapon(weapon_info* weapon) {
	init_color(11, 1000, 843, 0);   // رنگ طلایی 
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, 11, COLOR_BLACK);
	attron(COLOR_PAIR(3));
	mvprintw(2, 50, "Your weapon was placed in your bag!\n");
	mvprintw(3, 50, "Now Please enter a Gun_Key to change your weapon!\n");
	attroff(COLOR_PAIR(3));
	int ch = getch();
	switch (ch) {
		case 'm':
			attron(COLOR_PAIR(1));
			mvprintw(4, 50, "Your New weapon is Mace \u2692\n");
			weapon->current = 'm';
			attroff(COLOR_PAIR(1));
			break;
		case 's':
			if(weapon->sword_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(4, 50, "You don't have enough number of this weapon!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(4, 50, "Your New weapon is Sword \u2694\n");
				weapon->current = 's';
				attroff(COLOR_PAIR(1));
			}
			break;
		case 'd':
			if(weapon->dagger_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(4, 50, "You don't have enough number of this weapon!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(4, 50, "Your New weapon is Dagger \U0001F5E1 \n");
				weapon->current = 'd';
				attroff(COLOR_PAIR(1));
			}
			break;
		case 'w':
			if(weapon->wand_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(4, 50, "You don't have enough number of this weapon!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(4, 50, "Your New weapon is Magic Wand \u2020 \n");
				weapon->current = 'w';
				attroff(COLOR_PAIR(1));
			}
			break;
		case 'a':
			if(weapon->arrow_amount == 0) {
				attron(COLOR_PAIR(2));
				mvprintw(4, 50, "You don't have enough number of this weapon!\n");
				attroff(COLOR_PAIR(2));
			}
			else {
				attron(COLOR_PAIR(1));
				mvprintw(4, 50, "Your New weapon is Normal Arrow \u27B3 \n");
				weapon->current = 'a';
				attroff(COLOR_PAIR(1));
			}
			break;
		default: 
				attron(COLOR_PAIR(2));
				mvprintw(4, 50, "Input weapon isn't in the list!\n");
				attroff(COLOR_PAIR(2));
	}
	getch();
}
void show_health_rate(achievement_info* achievement) {
	clear();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	mvprintw(2, 10, "Health_bar:");
	mvprintw(4, 10, "Hunger_bar:");
	mvprintw(8, 10, "You can eat a meal by pressing F");
	attroff(COLOR_PAIR(1));
	
	attron(COLOR_PAIR(2));
	for(int i = 0; i < 20; i++) {          //Hunger_bar
		mvprintw(4, 23 + i , "\u2591 ");
	}
	printw("  (%d/20)", achievement->hunger_bar);
	for(int i = 0; i < achievement->hunger_bar; i++) {
		mvprintw(4, 23 + i , "\u258a");
	}
	/////////////////////////////////////////////////
	for(int i = 0; i < 100; i++) {          //Health_bar
		mvprintw(2, 23 + i , "\u2591");
	}
	printw("  (%d/100)", achievement->health);
	for(int i = 0; i < achievement->health; i++) {
		mvprintw(2, 23 + i , "\u258a");
	}

	mvprintw(9, 10, "%d meal you have in your bag!", achievement->food_amount);
	for(int i = 0; i < achievement->food_amount; i++) {
		mvprintw(10, 10 + i*2, "\U0001F354");
	}
	attroff(COLOR_PAIR(2));
	int ch = getch();
	if(ch == 'F' || ch == 'f') eat_a_meal(achievement);
}
void eat_a_meal(achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->food_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(11, 10, "You don't have any meal in your bag!");
		attroff(COLOR_PAIR(2));
		getch();
		return;
	}
	if(achievement->hunger_bar < 20 || achievement->health < 100) {       //eat a meal if was hungry
		attron(COLOR_PAIR(1));
		achievement->hunger_bar += 3 + difficulty;     //increasing hunger_bar
		achievement->food_amount -= 1;
		mvprintw(11, 10, "Good! You ate a meal!");
		if(achievement->health < 100) {
			achievement->health += 10 + difficulty*5;   //increasing Health
			if(achievement->health > 100) achievement->health = 100;
		}
		if(achievement->hunger_bar > 20) achievement->hunger_bar = 20;  
		if(achievement->hunger_bar == 20) achievement->movement = 0;       //when it becomes full its movement set to 0
		attroff(COLOR_PAIR(1));
	}
	else {
		attron(COLOR_PAIR(2));
		mvprintw(11, 10, "Your hunger_bar is Full!");
		attroff(COLOR_PAIR(2));

	}
	getch();
	show_health_rate(achievement);
}
void cure_when_is_unhungry(achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	if(achievement->hunger_bar == 20 || achievement->hunger_bar == 19) {
		achievement->movement_unhungry += 1;
	}
	else {
		achievement->movement_unhungry = 0;
		return;
	}
	attron(COLOR_PAIR(1));
	if(achievement->movement_unhungry == 350) {
		achievement->health += 12 + (difficulty)*3;     //15 for easy    12 for medium     9 for hard
		if(achievement->health > 100) achievement->health = 100;
		mvprintw(1, 5, "You walked 350 meters with full hunger_bar!");
		mvprintw(2, 5, "Your health_bar was increased %d units", 4 + (difficulty));
		achievement->movement_unhungry = 0;
		getch();
	}
	else if(achievement->movement_unhungry == 150) {
		achievement->health += 8 + (difficulty)*2;     //10 for easy    8 for medium     6 for hard
		if(achievement->health > 100) achievement->health = 100;
		mvprintw(1, 5, "You walked 150 meters with full hunger_bar!");
		mvprintw(2, 5, "Your health_bar was increased %d units", 2 + (difficulty));
		getch();
	}
	else if(achievement->movement_unhungry == 50) {
		achievement->health += 5;                     //5 for all mode
		if(achievement->health > 100) achievement->health = 100;
		mvprintw(1, 5, "You walked 50 meters with full hunger_bar!");
		mvprintw(2, 5, "Your health_bar was increased %d units", 5);
		getch();
	}
	attroff(COLOR_PAIR(1));
}
void check_hunger_bar(achievement_info* achievement) {
	if(achievement->hunger_bar >= 8 && achievement->hunger_bar <= 10) {    //in range 8 to 10 -----> 5 units decrease
		achievement->health -= 5;
	}
	if(achievement->hunger_bar < 8) {                //lower than 8  ------> 8 unit decrease
		achievement->health -= 8;
	}
}
void set_to_zero(achievement_info* achievement) {
	achievement->save_gold = 0;
	achievement->score = 0;
	achievement->health = 100;
	achievement->hunger_bar = 20;
	achievement->weapon.mace_amount = 1;
	achievement->weapon.dagger_amount = 0;
	achievement->weapon.arrow_amount = 0;
	achievement->weapon.wand_amount = 0;
	achievement->weapon.sword_amount = 0;
	achievement->weapon.current = 'm';
	achievement->enchant.active = false;
	achievement->enchant.damage_amount = 0;
	achievement->enchant.health_amount = 0;
	achievement->enchant.speed_amount = 0;
	achievement->enchant.distance = 0;
	achievement->movement = 0;
	achievement->food_amount = 0;
	achievement->movement_unhungry = 0;
}
void print_enemy_conditionally(floor_info* floor) {
	for(int i = 0; i < 16; i++) {
		if(floor->enemy[i].health > 0 && floor->enemy[i].room <= floor->open_room) {
			mvprintw(floor->enemy[i].place.y, floor->enemy[i].place.x, "%c", floor->enemy[i].name);
		}
	}
}
void move_alive_enemies(floor_info* floor, location* place, achievement_info* achievement) {
	if(achievement->enchant.active == true && achievement->enchant.current == 's') {   //if speed enchant was active
		if(achievement->enchant.distance % 2) return;
	}
	for(int i = 0; i < 16; i++) {
		if(floor->enemy[i].health > 0 && floor->enemy[i].room <= floor->open_room) {
			switch (floor->enemy[i].name) {
				case 'S':         //Snake
					move_permanently_enemy(floor, place, i);
					break;
				default:          //Other
					move_temporary_enemy(floor, place, i);
					break;
			}
		}
	}
}
void move_permanently_enemy(floor_info* floor, location* place, int i) {
	if(floor->enemy[i].place.x > place->x) {        //both x and y will change
		floor->enemy[i].place.x -= 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true) 
			floor->enemy[i].place.x += 1;
	}
	else if(floor->enemy[i].place.x < place->x) {
		floor->enemy[i].place.x += 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true) 
			floor->enemy[i].place.x -= 1;
	}
	if(floor->enemy[i].place.y > place->y) {
		floor->enemy[i].place.y -= 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true) 
			floor->enemy[i].place.y += 1;
	}
	else if(floor->enemy[i].place.y < place->y) {
		floor->enemy[i].place.y += 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true)
			floor->enemy[i].place.y -= 1;
	}
}
void move_temporary_enemy(floor_info* floor, location* place, int i) {
	if(floor->enemy[i].place.x > place->x) {       //just follow till door
		floor->enemy[i].place.x -= 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true)
			floor->enemy[i].place.x += 1;
	}
	else if(floor->enemy[i].place.x < place->x) {
		floor->enemy[i].place.x += 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true) 
			floor->enemy[i].place.x -= 1;
	}
	else if(floor->enemy[i].place.y > place->y) {
		floor->enemy[i].place.y -= 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true) 
			floor->enemy[i].place.y += 1;
	}
	else if(floor->enemy[i].place.y < place->y) {
		floor->enemy[i].place.y += 1;
		if( check_location(floor, &floor->enemy[i].place) == false || 
			floor->enemy[i].place.x == place->x && floor->enemy[i].place.y == place->y ||
			check_location_as_be_enemy(floor, &floor->enemy[i].place, i) == true) 
			floor->enemy[i].place.y -= 1;
	}
}
void move_alive_enemies_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement) {
	if(achievement->enchant.active == true && achievement->enchant.current == 's') {   //if speed enchant was active
		if(achievement->enchant.distance % 2) return;
	}
	for(int i = 0; i < 20; i++) {
		if(treasure->enemy[i].place.x > place->x) {       //just follow till door
			treasure->enemy[i].place.x -= 1;
			if( check_location_in_treasure_room(treasure, &treasure->enemy[i].place) == false || 
				treasure->enemy[i].place.x == place->x && treasure->enemy[i].place.y == place->y ||
				check_location_as_be_enemy_in_treasure_room(treasure, &treasure->enemy[i].place, i) == true)
				treasure->enemy[i].place.x += 1;
		}
		else if(treasure->enemy[i].place.x < place->x) {
			treasure->enemy[i].place.x += 1;
			if( check_location_in_treasure_room(treasure, &treasure->enemy[i].place) == false || 
				treasure->enemy[i].place.x == place->x && treasure->enemy[i].place.y == place->y ||
				check_location_as_be_enemy_in_treasure_room(treasure, &treasure->enemy[i].place, i) == true)
				treasure->enemy[i].place.x -= 1;
		}
		if(treasure->enemy[i].place.y > place->y) {
			treasure->enemy[i].place.y -= 1;
			if( check_location_in_treasure_room(treasure, &treasure->enemy[i].place) == false || 
				treasure->enemy[i].place.x == place->x && treasure->enemy[i].place.y == place->y ||
				check_location_as_be_enemy_in_treasure_room(treasure, &treasure->enemy[i].place, i) == true)
				treasure->enemy[i].place.y += 1;
		}
		else if(treasure->enemy[i].place.y < place->y) {
			treasure->enemy[i].place.y += 1;
			if( check_location_in_treasure_room(treasure, &treasure->enemy[i].place) == false || 
				treasure->enemy[i].place.x == place->x && treasure->enemy[i].place.y == place->y ||
				check_location_as_be_enemy_in_treasure_room(treasure, &treasure->enemy[i].place, i) == true)
				treasure->enemy[i].place.y -= 1;
		}
	}
}
void transfer_snake_to_other_floor(floor_info* p_floor, floor_info* n_floor) {
	for(int i = 0; i < 16; i++) {
		if(p_floor->enemy[i].health > 0 && p_floor->enemy[i].name == 'S') {
			n_floor->enemy[n_floor->number_enemy].name = 'S';
			n_floor->enemy[n_floor->number_enemy].room = 0;
			n_floor->enemy[n_floor->number_enemy].health = p_floor->enemy[i].health;
			start_location_random(&n_floor->enemy[n_floor->number_enemy].place, &n_floor->room[0]);
			n_floor->number_enemy += 1;
			p_floor->enemy[i].health = 0;
		}
	}
}
void transfer_snake_to_treasure_room(floor_info* p_floor, treasure_info* treasure) {
	for(int i = 0; i < 16; i++) {
		if(p_floor->enemy[i].health > 0 && p_floor->enemy[i].name == 'S') {
			treasure->enemy[treasure->number_enemy].name = 'S';
			treasure->enemy[treasure->number_enemy].room = 0;
			treasure->enemy[treasure->number_enemy].health = p_floor->enemy[i].health;
			start_location_random(&treasure->enemy[treasure->number_enemy].place, &treasure->room);
			treasure->number_enemy += 1;
			p_floor->enemy[i].health = 0;
		}
	}
}
void check_around_for_enemy(floor_info* floor, location* place, achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(2));
	int delta_x[8] = {0,  +1, +1, +1, 0, -1, -1, -1};
	int delta_y[8] = {-1, -1,  0, +1, +1, +1, 0, -1};
	for(int i = 0; i < 8; i++) {
		location temp;
		temp.x = place->x + delta_x[i];
		temp.y = place->y + delta_y[i];
		for(int j = 0; j < 16; j++) {
			if( floor->enemy[j].health > 0 && floor->enemy[j].room <= floor->open_room &&
				floor->enemy[j].place.x == temp.x && floor->enemy[j].place.y == temp.y) {
					switch (floor->enemy[j].name) {
						case 'D':             //Deamon   5
							achievement->health -= 2;
							mvprintw(1, 5, "Deamon hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 2 units!");
							getch();
							mvprintw(1, 5, "               ");
							mvprintw(2, 5, "                                       ");
							break;
						case 'F':             //Fire Breathing Monster    10
							achievement->health -= 4;
							mvprintw(1, 5, "Fire Breathing Monster hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 4 units!");
							getch();
							mvprintw(1, 5, "                               ");
							mvprintw(2, 5, "                                       ");
							break;
						case 'G':             //Giant        15
							achievement->health -= 7;
							mvprintw(1, 5, "Giant hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 7 units!");
							getch();
							mvprintw(1, 5, "              ");
							mvprintw(2, 5, "                                       ");
							break;
						case 'S':             //Snake         20
							achievement->health -= 12;
							mvprintw(1, 5, "Snake hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 12 units!");
							getch();
							mvprintw(1, 5, "              ");
							mvprintw(2, 5, "                                        ");
							break;
						case 'U':             //Undeed        30
							achievement->health -= 20;
							mvprintw(1, 5, "Undeed hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 20 units!");
							getch();
							mvprintw(1, 5, "               ");
							mvprintw(2, 5, "                                        ");
							break;
					}
				break;
			}
		}
	}
	attroff(COLOR_PAIR(2));
}
void fight(floor_info* floor, location* place, achievement_info* achievement) { 
	if(achievement->enchant.active == true && achievement->enchant.current == 'd') {   //Damage Enchant is active
		fight_with_active_damage_enchant(floor, place, achievement);                   //weapon power is 2 times than usual
		return;
	}
	switch (achievement->weapon.current) {
		case 'm':                 //Mace  5              //short_range
			shot_with_mace(floor, place, achievement, 5);
			break;
		case 's':                 //Sword  10
			shot_with_sword(floor, place, achievement, 10);
			break;
		case 'd':             //Dagger    12          //Thrown
			shot_with_dagger(floor, place, achievement, 12);
			break;
		case 'w':             //Magic Wand    15
			shot_with_wand(floor, place, achievement, 15);
			break;
		case 'a':             //Normal Arrow   5
			shot_with_arrow(floor, place, achievement, 5);
			break;
	}
}
void fight_with_active_damage_enchant(floor_info* floor, location* place, achievement_info* achievement) {
	switch (achievement->weapon.current) {
		case 'm':                 //Mace  5              //short_range
			shot_with_mace(floor, place, achievement, 10);
			break;
		case 's':                 //Sword  10
			shot_with_sword(floor, place, achievement, 20);
			break;
		case 'd':             //Dagger    12          //Thrown
			shot_with_dagger(floor, place, achievement, 24);
			break;
		case 'w':             //Magic Wand    15
			shot_with_wand(floor, place, achievement, 30);
			break;
		case 'a':             //Normal Arrow   5
			shot_with_arrow(floor, place, achievement, 10);
			break;
	}
}
void shot_with_mace(floor_info* floor, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	int delta_x[8] = {0,  +1, +1, +1, 0, -1, -1, -1};
	int delta_y[8] = {-1, -1,  0, +1, +1, +1, 0, -1};
	for(int i = 0; i < 8; i++) {
		location temp;
		char* name;
		temp.x = place->x + delta_x[i];
		temp.y = place->y + delta_y[i];
		for(int j = 0; j < 16; j++) {
			if( floor->enemy[j].health > 0 && floor->enemy[j].room <= floor->open_room &&
				floor->enemy[j].place.x == temp.x && floor->enemy[j].place.y == temp.y) {
					name = tell_name_enemy(floor->enemy[j].name);    //full name
					floor->enemy[j].health -= power;   //its damage
					mvprintw(1, 50, "You hit %s with Mace \u2692 !", name);
					mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[j].health > 0 ? floor->enemy[j].health : 0);
					getch();
					mvprintw(1, 50, "                                           ");
					mvprintw(2, 50, "                                           ");
					if(floor->enemy[j].health <= 0) score_kill_enemy(achievement, floor->enemy[j].name, 1);
					break;
			}
		}
	}
	attroff(COLOR_PAIR(1));
}
void shot_with_sword(floor_info* floor, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	int delta_x[8] = {0,  +1, +1, +1, 0, -1, -1, -1};
	int delta_y[8] = {-1, -1,  0, +1, +1, +1, 0, -1};
	for(int i = 0; i < 8; i++) {
		location temp;
		char* name;
		temp.x = place->x + delta_x[i];
		temp.y = place->y + delta_y[i];
		for(int j = 0; j < 16; j++) {
			if( floor->enemy[j].health > 0 && floor->enemy[j].room <= floor->open_room &&
				floor->enemy[j].place.x == temp.x && floor->enemy[j].place.y == temp.y) {
					name = tell_name_enemy(floor->enemy[j].name);          //full name
					floor->enemy[j].health -= power;     //its damage
					mvprintw(1, 50, "You hit %s with Sword \u2694!", name);
					mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[j].health > 0 ? floor->enemy[j].health : 0);
					getch();
					mvprintw(1, 50, "                                           ");
					mvprintw(2, 50, "                                           ");
					if(floor->enemy[j].health <= 0) score_kill_enemy(achievement, floor->enemy[j].name, 1);
					break;
			}
		}
	}
	attroff(COLOR_PAIR(1));
}
void shot_with_dagger(floor_info* floor, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->weapon.dagger_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 50, "You don't have any of this weapon!");
		getch();
		attroff(COLOR_PAIR(2));
		return;
	}
	attron(COLOR_PAIR(1));
	mvprintw(1, 50, "Enter a direction!");
	int direction = getch();
	attroff(COLOR_PAIR(1));
	mvprintw(1, 50, "                  ");
	int output;
	int final = current_room(floor, place);
	char* name;
	switch (direction) {
		case KEY_UP:
			output = search_path_direction(floor, place, 'U', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y - output, place->x};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '1';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.dagger_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_RIGHT:
			output = search_path_direction(floor, place, 'R', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x + output};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '1';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.dagger_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_DOWN:
			output = search_path_direction(floor, place, 'D', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y + output, place->x};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '1';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.dagger_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_LEFT:
			output = search_path_direction(floor, place, 'L', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x - output};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '1';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.dagger_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;		
		default:
			attron(COLOR_PAIR(2));
			mvprintw(1, 50, "Wrong direction!");
			getch();
			attroff(COLOR_PAIR(2));
			return;
	}
}
void shot_with_wand(floor_info* floor, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->weapon.wand_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 50, "You don't have any of this weapon!");
		getch();
		attroff(COLOR_PAIR(2));
		return;
	}
	attron(COLOR_PAIR(1));
	mvprintw(1, 50, "Enter a direction!");
	int direction = getch();
	attroff(COLOR_PAIR(1));
	mvprintw(1, 50, "                  ");
	int output;
	int final = current_room(floor, place);
	char* name;
	switch (direction) {
		case KEY_UP:
			output = search_path_direction(floor, place, 'U', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Magic Wand \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y - output, place->x};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '2';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.wand_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_RIGHT:
			output = search_path_direction(floor, place, 'R', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x + output};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '2';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.wand_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_DOWN:
			output = search_path_direction(floor, place, 'D', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y + output, place->x};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '2';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.wand_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_LEFT:
			output = search_path_direction(floor, place, 'L', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x - output};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '2';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.wand_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;		
		default:
			attron(COLOR_PAIR(2));
			mvprintw(1, 50, "Wrong direction!");
			getch();
			attroff(COLOR_PAIR(2));
			return;
	}
}
void shot_with_arrow(floor_info* floor, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->weapon.arrow_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 50, "You don't have any of this weapon!");
		getch();
		attroff(COLOR_PAIR(2));
		return;
	}
	attron(COLOR_PAIR(1));
	mvprintw(1, 50, "Enter a direction!");
	int direction = getch();
	attroff(COLOR_PAIR(1));
	mvprintw(1, 50, "                  ");
	int output;
	int final = current_room(floor, place);
	char* name;
	switch (direction) {
		case KEY_UP:
			output = search_path_direction(floor, place, 'U', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y - output, place->x};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '3';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.arrow_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_RIGHT:
			output = search_path_direction(floor, place, 'R', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x + output};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '3';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.arrow_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_DOWN:
			output = search_path_direction(floor, place, 'D', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y + output, place->x};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '3';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.arrow_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;
		case KEY_LEFT:
			output = search_path_direction(floor, place, 'L', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				floor->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(floor->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, floor->enemy[output].health > 0 ? floor->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				getch();
				mvprintw(1, 50, "                                                    ");
				mvprintw(2, 50, "                                                    ");
				if(floor->enemy[output].health <= 0) score_kill_enemy(achievement, floor->enemy[output].name, 1);
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x - output};
				final = current_room(floor, &temp);   //final location is in which room?
				if(final != -1) {  //This is in a room   //we don't check in corridor
					floor->room[final].cell[temp.y - floor->room[final].start_point.y][temp.x - floor->room[final].start_point.x] = '3';
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
					achievement->weapon.arrow_amount -= 1;
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					attroff(COLOR_PAIR(2));
				}
				else {
					attron(COLOR_PAIR(2));
					mvprintw(1, 50, "You didn't hit anyone!");
					mvprintw(2, 50, "Weapons can't fall down in corridors! Your weapon amount didn't decreased!");
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                                          ");
					attroff(COLOR_PAIR(2));
				}
			}
			break;		
		default:
			attron(COLOR_PAIR(2));
			mvprintw(1, 50, "Wrong direction!");
			getch();
			attroff(COLOR_PAIR(2));
			return;
	}
}
int search_path_direction(floor_info* floor, location* place, int direction, int len) {
	// output -n --->  hit to enemy(+20)        output n --->  fell down after n
	location temp;
	int a;
	switch (direction) {
		case 'U':                   //UP
			for(int i = 1; i <= len; i++) {
				temp.x = place->x;
				temp.y = place->y - i;
				if((a = check_location_as_be_enemy_with_index(floor, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location(floor, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
		case 'R':                   //RIGHT
			for(int i = 1; i <= len; i++) {
				temp.x = place->x + i;
				temp.y = place->y;
				if((a = check_location_as_be_enemy_with_index(floor, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location(floor, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
		case 'D':                   //DOWN
			for(int i = 1; i <= len; i++) {
				temp.x = place->x;
				temp.y = place->y + i;
				if((a = check_location_as_be_enemy_with_index(floor, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location(floor, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
		case 'L':                   //LEFT
			for(int i = 1; i <= len; i++) {
				temp.x = place->x - i;
				temp.y = place->y;
				if((a = check_location_as_be_enemy_with_index(floor, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location(floor, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
	}
}
char* tell_name_enemy(char abbreviation) {
	switch (abbreviation) {
		case 'D':             //Deamon   5
			return "Deamon";
		case 'F':             //Fire Breathing Monster    10
			return "Fire Breathing Monster";
		case 'G':             //Giant       15
			return "Giant";
		case 'S':             //Snake         20
			return "Snake";
		case 'U':             //Undeed        30
			return "Undeed";
	}
}
void check_active_enchant(achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->enchant.distance == 10) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 5, "Enchant became inactive!");
		achievement->enchant.active = false;
		achievement->enchant.distance = 0;
		attroff(COLOR_PAIR(2));
		getch();
		return;
	}

	if(achievement->enchant.active == true) {
		achievement->enchant.distance += 1;

		if(achievement->enchant.current == 'h') {     //Health enchant
			achievement->health += 2;    //2 units increase to healthy
			if(achievement->health > 100) achievement->health = 100;
		}
		return;
	}
}
int handle_input_key(int ch) {
	switch (ch) {
		case ' ':     //fight
			return 2;
			break;
		case KEY_UP:    case KEY_DOWN: case KEY_RIGHT: case KEY_LEFT:  //movement
		case KEY_NPAGE: case KEY_HOME: case KEY_PPAGE: case KEY_END:
			return 1;
			break;
		case 27:     //exit 
			return 3;
		default:
			return 0;
			break;
	}
}
void lose_page(achievement_info* achievement) {
	init_pair(2, COLOR_RED, COLOR_BLACK);
	clear();
	attron(COLOR_PAIR(2));
	mvprintw(LINES/2    , COLS / 2 - 20, "You LOST the game \U000026a0 \U0000274c  \n");
	mvprintw(LINES/2 + 1, COLS / 2 - 20, "Score: %d    Gold: %d \n", achievement->score, achievement->save_gold);
	mvprintw(LINES/2 + 2, COLS / 2 - 20, "Press q to return to previous page.\n");
	attroff(COLOR_PAIR(2));
	while(1) {
		int ch = getch();
		if(ch == 'q') return;
	}
}
void win_page(achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	clear();
	attron(COLOR_PAIR(1));
	mvprintw(LINES/2    , COLS / 2 - 20, "You WON the game \U0001f973 \U0001f3c6  \n");
	mvprintw(LINES/2 + 1, COLS / 2 - 20, "Score: %d    Gold: %d \n", achievement->score, achievement->save_gold);
	mvprintw(LINES/2 + 2, COLS / 2 - 20, "Press q to return to previous page.\n");
	attroff(COLOR_PAIR(1));
	while(1) {
		int ch = getch();
		if(ch == 'q') return;
	}
}
void show_help_for_game() {
	clear();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	mvprintw(1, 10, "g      pick_up a weapon ro enchant");
	mvprintw(2, 10, "i      list of weapons in your bag and can change your current");
	mvprintw(3, 10, "e      list of enchants in your bag and can activate");
	mvprintw(4, 10, "G      show how many gold you have");
	mvprintw(5, 10, "H      show your health_rate and hunger_bar and meals");
	mvprintw(6, 10, ">      go to next floor (only in < cell)");
	mvprintw(7, 10, "<      go to previous floor (only in < cell)");
	getch();
	attroff(COLOR_PAIR(1));
}
bool go_to_treasure_room(floor_info* floor, location* place, int num_floor) {
	int current = current_room(floor, place);
	if(current >= 0 && num_floor == 3) {   // a room in last floor
		int a = place->x - floor->room[current].start_point.x;
		int b = place->y - floor->room[current].start_point.y;
		if(floor->room[current].cell[b][a] == 'X') {
			return true;
		}
		return false;
	}
	else return false;
}
void open_password_door(room_info* room, int door_number) {
	
}
void check_around_for_enemy_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(2));
	int delta_x[8] = {0,  +1, +1, +1, 0, -1, -1, -1};
	int delta_y[8] = {-1, -1,  0, +1, +1, +1, 0, -1};
	for(int i = 0; i < 8; i++) {
		location temp;
		temp.x = place->x + delta_x[i];
		temp.y = place->y + delta_y[i];
		for(int j = 0; j < 16; j++) {
			if( treasure->enemy[j].health > 0 &&
				treasure->enemy[j].place.x == temp.x && treasure->enemy[j].place.y == temp.y) {
					switch (treasure->enemy[j].name) {
						case 'D':             //Deamon   5
							achievement->health -= 2;
							mvprintw(1, 5, "Deamon hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 2 units!");
							getch();
							mvprintw(1, 5, "               ");
							mvprintw(2, 5, "                                       ");
							break;
						case 'F':             //Fire Breathing Monster    10
							achievement->health -= 4;
							mvprintw(1, 5, "Fire Breathing Monster hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 4 units!");
							getch();
							mvprintw(1, 5, "                               ");
							mvprintw(2, 5, "                                       ");
							break;
						case 'G':             //Giant        15
							achievement->health -= 7;
							mvprintw(1, 5, "Giant hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 7 units!");
							getch();
							mvprintw(1, 5, "              ");
							mvprintw(2, 5, "                                       ");
							break;
						case 'S':             //Snake         20
							achievement->health -= 12;
							mvprintw(1, 5, "Snake hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 12 units!");
							getch();
							mvprintw(1, 5, "              ");
							mvprintw(2, 5, "                                        ");
							break;
						case 'U':             //Undeed        30
							achievement->health -= 20;
							mvprintw(1, 5, "Undeed hit you!");
							mvprintw(2, 5, "Your health_rate was decreased 20 units!");
							getch();
							mvprintw(1, 5, "               ");
							mvprintw(2, 5, "                                        ");
							break;
					}
				break;
			}
		}
	}
	attroff(COLOR_PAIR(2));
}
void fight_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement) { 
	if(achievement->enchant.active == true && achievement->enchant.current == 'd') {   //Damage Enchant is active
		fight_with_active_damage_enchant_in_treasure_room(treasure, place, achievement);  //weapon power is 2 times than usual
		return;
	}
	switch (achievement->weapon.current) {
		case 'm':                 //Mace  5              //short_range
			shot_with_mace_in_treasure_room(treasure, place, achievement, 5);
			break;
		case 's':                 //Sword  10
			shot_with_sword_in_treasure_room(treasure, place, achievement, 10);
			break;
		case 'd':             //Dagger    12          //Thrown
			shot_with_dagger_in_treasure_room(treasure, place, achievement, 12);
			break;
		case 'w':             //Magic Wand    15
			shot_with_wand_in_treasure_room(treasure, place, achievement, 15);
			break;
		case 'a':             //Normal Arrow   5
			shot_with_arrow_in_treasure_room(treasure, place, achievement, 5);
			break;
	}
}
void fight_with_active_damage_enchant_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement) {
	switch (achievement->weapon.current) {
		case 'm':                 //Mace  5              //short_range
			shot_with_mace_in_treasure_room(treasure, place, achievement, 10);
			break;
		case 's':                 //Sword  10
			shot_with_sword_in_treasure_room(treasure, place, achievement, 20);
			break;
		case 'd':             //Dagger    12          //Thrown
			shot_with_dagger_in_treasure_room(treasure, place, achievement, 24);
			break;
		case 'w':             //Magic Wand    15
			shot_with_wand_in_treasure_room(treasure, place, achievement, 30);
			break;
		case 'a':             //Normal Arrow   5
			shot_with_arrow_in_treasure_room(treasure, place, achievement, 10);
			break;
	}
}
void shot_with_mace_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	int delta_x[8] = {0,  +1, +1, +1, 0, -1, -1, -1};
	int delta_y[8] = {-1, -1,  0, +1, +1, +1, 0, -1};
	for(int i = 0; i < 8; i++) {
		location temp;
		char* name;
		temp.x = place->x + delta_x[i];
		temp.y = place->y + delta_y[i];
		for(int j = 0; j < 20; j++) {
			if( treasure->enemy[j].health > 0 &&
				treasure->enemy[j].place.x == temp.x && treasure->enemy[j].place.y == temp.y) {
					name = tell_name_enemy(treasure->enemy[j].name);    //full name
					treasure->enemy[j].health -= power;   //its damage
					mvprintw(1, 50, "You hit %s with Mace \u2692 !", name);
					mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[j].health > 0 ? treasure->enemy[j].health : 0);
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					if(treasure->enemy[j].health <= 0) score_kill_enemy(achievement, treasure->enemy[j].name, 2);
					break;
			}
		}
	}
	attroff(COLOR_PAIR(1));
}
void shot_with_sword_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	int delta_x[8] = {0,  +1, +1, +1, 0, -1, -1, -1};
	int delta_y[8] = {-1, -1,  0, +1, +1, +1, 0, -1};
	for(int i = 0; i < 8; i++) {
		location temp;
		char* name;
		temp.x = place->x + delta_x[i];
		temp.y = place->y + delta_y[i];
		for(int j = 0; j < 20; j++) {
			if( treasure->enemy[j].health > 0 &&
				treasure->enemy[j].place.x == temp.x && treasure->enemy[j].place.y == temp.y) {
					name = tell_name_enemy(treasure->enemy[j].name);          //full name
					treasure->enemy[j].health -= power;     //its damage
					mvprintw(1, 50, "You hit %s with Sword \u2694!", name);
					mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[j].health > 0 ? treasure->enemy[j].health : 0);
					getch();
					mvprintw(1, 50, "                                                    ");
					mvprintw(2, 50, "                                                    ");
					if(treasure->enemy[j].health <= 0) score_kill_enemy(achievement, treasure->enemy[j].name, 2);
					break;
			}
		}
	}
	attroff(COLOR_PAIR(1));
}
void shot_with_dagger_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->weapon.dagger_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 50, "You don't have any of this weapon!");
		getch();
		attroff(COLOR_PAIR(2));
		return;
	}
	attron(COLOR_PAIR(1));
	mvprintw(1, 50, "Enter a direction!");
	int direction = getch();
	attroff(COLOR_PAIR(1));
	mvprintw(1, 50, "                  ");
	int output;
	char* name;
	switch (direction) {
		case KEY_UP:
			output = search_path_direction_in_treasure_room(treasure, place, 'U', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y - output, place->x};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '1';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.dagger_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_RIGHT:
			output = search_path_direction_in_treasure_room(treasure, place, 'R', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x + output};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '1';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.dagger_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_DOWN:
			output = search_path_direction_in_treasure_room(treasure, place, 'D', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y + output, place->x};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '1';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.dagger_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_LEFT:
			output = search_path_direction_in_treasure_room(treasure, place, 'L', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \U0001F5E1 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.dagger_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x - output};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '1';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.dagger_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;		
		default:
			attron(COLOR_PAIR(2));
			mvprintw(1, 50, "Wrong direction!");
			getch();
			attroff(COLOR_PAIR(2));
			return;
	}
}
void shot_with_wand_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->weapon.wand_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 50, "You don't have any of this weapon!");
		getch();
		attroff(COLOR_PAIR(2));
		return;
	}
	attron(COLOR_PAIR(1));
	mvprintw(1, 50, "Enter a direction!");
	int direction = getch();
	attroff(COLOR_PAIR(1));
	mvprintw(1, 50, "                  ");
	int output;
	char* name;
	switch (direction) {
		case KEY_UP:
			output = search_path_direction_in_treasure_room(treasure, place, 'U', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Magic Wand \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y - output, place->x};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '2';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.wand_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_RIGHT:
			output = search_path_direction_in_treasure_room(treasure, place, 'R', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x + output};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '2';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.wand_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_DOWN:
			output = search_path_direction_in_treasure_room(treasure, place, 'D', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y + output, place->x};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '2';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.wand_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_LEFT:
			output = search_path_direction_in_treasure_room(treasure, place, 'L', 10);   // 10 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Dagger \u2020 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.wand_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x - output};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '2';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.wand_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;		
		default:
			attron(COLOR_PAIR(2));
			mvprintw(1, 50, "Wrong direction!");
			getch();
			attroff(COLOR_PAIR(2));
			return;
	}
}
void shot_with_arrow_in_treasure_room(treasure_info* treasure, location* place, achievement_info* achievement, int power) {
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	if(achievement->weapon.arrow_amount == 0) {
		attron(COLOR_PAIR(2));
		mvprintw(1, 50, "You don't have any of this weapon!");
		getch();
		attroff(COLOR_PAIR(2));
		return;
	}
	attron(COLOR_PAIR(1));
	mvprintw(1, 50, "Enter a direction!");
	int direction = getch();
	attroff(COLOR_PAIR(1));
	mvprintw(1, 50, "                  ");
	int output;
	char* name;
	switch (direction) {
		case KEY_UP:
			output = search_path_direction_in_treasure_room(treasure, place, 'U', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y - output, place->x};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '3';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.arrow_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_RIGHT:
			output = search_path_direction_in_treasure_room(treasure, place, 'R', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x + output};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '3';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.arrow_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_DOWN:
			output = search_path_direction_in_treasure_room(treasure, place, 'D', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y + output, place->x};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '3';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.arrow_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;
		case KEY_LEFT:
			output = search_path_direction_in_treasure_room(treasure, place, 'L', 5);   // 5 is distance that this weapon can go
			if(output < 0) {         //Hit to an enemy
				output += 20;  //index
				attron(COLOR_PAIR(1));
				treasure->enemy[output].health -= power;   //its damage
				name = tell_name_enemy(treasure->enemy[output].name);
				mvprintw(1, 50, "You hit %s with Normal Arrow \u27B3 ", name);
				mvprintw(2, 50, "%s life_remain is  %d", name, treasure->enemy[output].health > 0 ? treasure->enemy[output].health : 0);
				achievement->weapon.arrow_amount -= 1;
				if(treasure->enemy[output].health <= 0) score_kill_enemy(achievement, treasure->enemy[output].name, 2);
				getch();
				attroff(COLOR_PAIR(1));				
			}
			else {
				location temp = {place->y, place->x - output};
				treasure->room.cell[temp.y - treasure->room.start_point.y][temp.x - treasure->room.start_point.x] = '3';
				attron(COLOR_PAIR(2));
				mvprintw(1, 50, "You didn't hit anyone!");
				mvprintw(2, 50, "You can pick_up your thrown weapon from the floor!");
				achievement->weapon.arrow_amount -= 1;
				getch();
				attroff(COLOR_PAIR(2));
			}
			break;		
		default:
			attron(COLOR_PAIR(2));
			mvprintw(1, 50, "Wrong direction!");
			getch();
			attroff(COLOR_PAIR(2));
			return;
	}
}
int search_path_direction_in_treasure_room(treasure_info* treasure, location* place, int direction, int len) {
	// output -n --->  hit to enemy(+20)        output n --->  fell down after n
	location temp;
	int a;
	switch (direction) {
		case 'U':                   //UP
			for(int i = 1; i <= len; i++) {
				temp.x = place->x;
				temp.y = place->y - i;
				if((a = check_location_as_be_enemy_with_index_in_treasure_room(treasure, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location_in_treasure_room(treasure, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
		case 'R':                   //RIGHT
			for(int i = 1; i <= len; i++) {
				temp.x = place->x + i;
				temp.y = place->y;
				if((a = check_location_as_be_enemy_with_index_in_treasure_room(treasure, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location_in_treasure_room(treasure, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
		case 'D':                   //DOWN
			for(int i = 1; i <= len; i++) {
				temp.x = place->x;
				temp.y = place->y + i;
				if((a = check_location_as_be_enemy_with_index_in_treasure_room(treasure, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location_in_treasure_room(treasure, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
		case 'L':                   //LEFT
			for(int i = 1; i <= len; i++) {
				temp.x = place->x - i;
				temp.y = place->y;
				if((a = check_location_as_be_enemy_with_index_in_treasure_room(treasure, &temp)) != 0) return a;   //Hit to an enemy
				if(check_location_in_treasure_room(treasure, &temp) == false ) return i-1;                //Go out 
			}
			return len;
			break;
	}
}

bool exit_the_game() {
	init_pair(2, COLOR_RED, COLOR_BLACK);
	clear();
	attron(COLOR_PAIR(2));
	mvprintw(LINES/2,     COLS / 2 - 20, "Do you want to exit the game?\n");
	mvprintw(LINES/2 + 1, COLS / 2 - 20, "Press y/n to end/continue the game.\n");
	attroff(COLOR_PAIR(2));
	while(1) {
		int ch = getch();
		if(ch == 'y' || ch == 'Y') 
			return true;
		if(ch == 'n' || ch == 'N')
			return false;
	}
}
void score_kill_enemy(achievement_info* achievement, char name, int n) {
	switch (name) {
		case 'D':        //Deamon
			achievement->score += 20*n;
			break;
		case 'F':        //Fire Breathing Monster
			achievement->score += 40*n;
			break;
		case 'G':        //Giant
			achievement->score += 60*n;
			break;
		case 'S':        //Snake 
			achievement->score += 80*n;
			break;
		case 'U':        //Undeed
			achievement->score += 120*n;
			break;
	}
}