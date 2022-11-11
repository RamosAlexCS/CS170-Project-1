#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
using namespace std;

struct Node {
	vector<vector<int>> puzzle;
	int id;
	int g_cost;
	int h_cost;
	int f_cost;
	Node* up;
	Node* down;
	Node* left;
	Node* right;
	Node* prev;
	Node(vector<vector<int>> puzzle, int id, int g_cost, int h_cost, int f_cost) :puzzle(puzzle), id(id), g_cost(g_cost), h_cost(h_cost), f_cost(f_cost), up(0), down(0), left(0), right(0), prev(0) {}
};

void PrintMenu();
void Correct_Puzzle();
void Print_Puzzle(vector<vector<int>>);
bool Goal_Puzzle(vector<vector<int>>);
void Find_Int(vector<vector<int>>, int);
bool Move_Up();
bool Move_Down();
bool Move_Left();
bool Move_Right();
void Tile_Up(Node*);
void Tile_Down(Node*);
void Tile_Left(Node*);
void Tile_Right(Node*);
void Remove_First_Priority();
void Sort_Priority(int, int);
Node* Find_Id();
Node* Fill_Puzzle();
Node* Branch_Parent(Node*);
void Puzzle_Mangement(Node*);
bool Duplicate_Check(vector<vector<int>>);
void Algorithm_Method();
int Misplaced_Tile(vector<vector<int>>);
int Manhattan_Distance(vector<vector<int>>);
int Calc_h(vector<vector<int>>);
Node* Preset_Puzzle();
void Traceback(Node*);

vector<vector<int>> puzzle;
vector<vector<int>> correct_puzzle;
vector<Node*> List;
vector<pair<int, int>> order; //id, f_cost
pair<int, int> coordinate; //row, column
int puzzle_size;
int priority = 1;
string Method;
string choice;

int main() {

	PrintMenu();

	return 0;
}

//Menu
void PrintMenu() {
	cout << "===============================================================================================\n";
	cout << " This program assumes an n x n puzzle.\n";
	cout << " This program assumes that all numbers from 0 to ( (n*n) - 1 ) is included once in the puzzle.\n";
	cout << " This program assumes that users inputs are correct.\n";
	cout << " This program assumes that the first puzzle starts at depth 0.\n";
	cout << "===============================================================================================\n\n";
	string menuOp = "\0";

	//Output menu option, prompt users for valid selection
	while (menuOp != "Q") {
		Node* curr = NULL;
		Method = "\0";
		choice = "\0";
		cout << "\n PUZZLE MENU" << endl;
		cout << "1 - Solve preset puzzle" << endl;
		cout << "2 - Custom puzzle" << endl;
		cout << "Q - Quit" << endl << endl;
		cout << "Choose an option:" << endl;
		cin >> menuOp;

		while (menuOp != "1" && menuOp != "2" && menuOp != "Q") {
			cout << "Invalid Input, Try again.\n";
			cin >> menuOp;
		}

		char Op = menuOp.at(0);

		//Call corresponding menu action
		switch (Op) {
		case '1':
			while (choice != "Q") {
				curr = Preset_Puzzle();
				if (choice != "Q") {
					Correct_Puzzle();
					while (Method != "Q") {
						Algorithm_Method();
						if (Method != "Q") {
							Puzzle_Mangement(curr);
						}
					}
					Method = "\0";
					List.clear();
					order.clear();
				}
			}
			Method = "\0";
			List.clear();
			order.clear();

			break;
		case '2':
			curr = Fill_Puzzle();
			Correct_Puzzle();
			while (Method != "Q") {
				Algorithm_Method();
				if (Method != "Q") {
					Puzzle_Mangement(curr);
				}
			}
			List.clear();
			order.clear();

			break;
		}
	}
}

//Fill the correct_puzzle once puzzle_size is determined //for goal state
void Correct_Puzzle() {
	correct_puzzle.clear();
	vector<int> correct_row;
	int value = 1;

	for (int i = 0; i < puzzle_size; i++) {
		for (int j = 0; j < puzzle_size; j++) {
			if (i == puzzle_size - 1 && j == puzzle_size - 1) {
				correct_row.push_back(0);
			}
			else {
				correct_row.push_back(value);
				value++;
			}
		}
		correct_puzzle.push_back(correct_row);
		correct_row.clear();
	}
}

//Print the puzzle
void Print_Puzzle(vector<vector<int>> print) {
	for (int i = 0; i < print.size(); i++) {
		cout << "[ ";
		for (int j = 0; j < print[i].size(); j++) {
			cout << print[i][j];
			if (j < print[i].size() - 1) {
				cout << ", ";
			}
		}
		cout << "]\n";
	}
}

//Checking if the current puzzle is the correct puzzle
bool Goal_Puzzle(vector<vector<int>> curr_puzzle) {
	if (curr_puzzle == correct_puzzle) {
		return true;
	}
	return false;
}

//Find integer target in puzzle //meant for 0 but left as parameter for flexibility
void Find_Int(vector<vector<int>> curr_puzzle, int target) {
	bool found = false;

	for (int i = 0; i < curr_puzzle.size(); i++) {
		for (int j = 0; j < curr_puzzle[i].size(); j++) {
			if (curr_puzzle[i][j] == target) {
				coordinate.first = i;
				coordinate.second = j;
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
		if (i == curr_puzzle.size() - 1) {
			coordinate.first = -1;
			coordinate.second = -1;
			break;
		}
	}
}

//Check if 0 can move in the direction
bool Move_Up() {
	if (coordinate.first == 0) {
		return false;
	}
	return true;
}

bool Move_Down() {
	if (coordinate.first == puzzle_size - 1) {
		return false;
	}
	return true;
}

bool Move_Left() {
	if (coordinate.second == 0) {
		return false;
	}
	return true;
}

bool Move_Right() {
	if (coordinate.second == puzzle_size - 1) {
		return false;
	}
	return true;
}

//Move 0 in any 4 direction
void Tile_Up(Node* parent) {
	Find_Int(parent->puzzle, 0);

	if (Move_Up()) {
		Node* child = Branch_Parent(parent);

		vector<int> t1 = child->puzzle.at(coordinate.first - 1);
		vector<int> t2 = child->puzzle.at(coordinate.first);

		t2.at(coordinate.second) = t1.at(coordinate.second);
		t1.at(coordinate.second) = 0;

		child->puzzle.at(coordinate.first - 1) = t1;
		child->puzzle.at(coordinate.first) = t2;

		if (!Duplicate_Check(child->puzzle)) {
			parent->up = child;
			child->prev = parent;
			child->g_cost++;
			child->h_cost = Calc_h(child->puzzle);
			child->f_cost = child->g_cost + child->h_cost;
			List.push_back(child);
			Sort_Priority(child->id, child->f_cost);
		}
		else {
			parent->up = NULL;
			priority--;
		}
	}
	else {
		parent->up = NULL;
	}
}

void Tile_Down(Node* parent) {
	Find_Int(parent->puzzle, 0);

	if (Move_Down()) {
		Node* child = Branch_Parent(parent);

		vector<int> t1 = child->puzzle.at(coordinate.first + 1);
		vector<int> t2 = child->puzzle.at(coordinate.first);

		t2.at(coordinate.second) = t1.at(coordinate.second);
		t1.at(coordinate.second) = 0;

		child->puzzle.at(coordinate.first + 1) = t1;
		child->puzzle.at(coordinate.first) = t2;

		if (!Duplicate_Check(child->puzzle)) {
			parent->down = child;
			child->prev = parent;
			child->g_cost++;
			child->h_cost = Calc_h(child->puzzle);
			child->f_cost = child->g_cost + child->h_cost;
			List.push_back(child);
			Sort_Priority(child->id, child->f_cost);
		}
		else {
			parent->down = NULL;
			priority--;
		}
	}
	else {
		parent->down = NULL;
	}
}

void Tile_Left(Node* parent) {
	Find_Int(parent->puzzle, 0);

	if (Move_Left()) {
		Node* child = Branch_Parent(parent);

		vector<int> t1 = child->puzzle.at(coordinate.first);

		t1.at(coordinate.second) = t1.at(coordinate.second - 1);
		t1.at(coordinate.second - 1) = 0;

		child->puzzle.at(coordinate.first) = t1;

		if (!Duplicate_Check(child->puzzle)) {
			parent->left = child;
			child->prev = parent;
			child->g_cost++;
			child->h_cost = Calc_h(child->puzzle);
			child->f_cost = child->g_cost + child->h_cost;
			List.push_back(child);
			Sort_Priority(child->id, child->f_cost);
		}
		else {
			parent->left = NULL;
			priority--;
		}
	}
	else {
		parent->left = NULL;
	}
}

void Tile_Right(Node* parent) {
	Find_Int(parent->puzzle, 0);

	if (Move_Right()) {
		Node* child = Branch_Parent(parent);

		vector<int> t1 = child->puzzle.at(coordinate.first);

		t1.at(coordinate.second) = t1.at(coordinate.second + 1);
		t1.at(coordinate.second + 1) = 0;

		child->puzzle.at(coordinate.first) = t1;

		if (!Duplicate_Check(child->puzzle)) {
			parent->right = child;
			child->prev = parent;
			child->g_cost++;
			child->h_cost = Calc_h(child->puzzle);
			child->f_cost = child->g_cost + child->h_cost;
			List.push_back(child);
			Sort_Priority(child->id, child->f_cost);
		}
		else {
			parent->right = NULL;
			priority--;
		}
	}
	else {
		parent->right = NULL;
	}
}

//Removing high priority node if not goal
void Remove_First_Priority() {
	for (int i = 0; i < order.size() - 1; i++) {
		order.at(i) = order.at(i + 1);
	}
	order.pop_back();
}

//Sort the priority of new children nodes with previously created nodes
void Sort_Priority(int identification, int f) {
	int temp1;
	int temp2;
	int i;
	bool flag = false;

	for (i = 0; i < order.size(); i++) {
		if (f <= order.at(i).second) {
			temp1 = order.at(i).first;
			temp2 = order.at(i).second;
			order.at(i).first = identification;
			order.at(i).second = f;
			identification = temp1;
			f = temp2;
			flag = true;
			i++;
			break;
		}
	}
	if (flag) {
		for (i = i; i < order.size(); i++) {
			temp1 = order.at(i).first;
			temp2 = order.at(i).second;
			order.at(i).first = identification;
			order.at(i).second = f;
			identification = temp1;
			f = temp2;
		}
	}
	pair<int, int> temp;
	temp.first = identification;
	temp.second = f;
	order.push_back(temp);
}

//Return the node that currently has the smallest f_cost place with id
Node* Find_Id() {
	int identification = order.at(0).first;
	for (int i = 0; i < List.size(); i++) {
		if (identification == List.at(i)->id) {
			return List.at(i);
		}
	}

	return NULL;
}

//Inital user input puzzle
Node* Fill_Puzzle() {
	vector<vector<int>> User_Puzzle;
	vector<int> User_Row;
	int input;

	cout << "\nWhat is the size of the puzzle you want to insert: ";
	cin >> puzzle_size;

	for (int i = 0; i < puzzle_size; i++) {
		cout << "\nEnter " << puzzle_size << " digits seperated by a space for row " << i + 1 << " from left to right:\n";
		for (int j = 0; j < puzzle_size; j++) {
			cin >> input;
			User_Row.push_back(input);
		}
		User_Puzzle.push_back(User_Row);
		User_Row.clear();
	}

	Node* t = new Node(User_Puzzle, 0, 0, Calc_h(User_Puzzle), Calc_h(User_Puzzle));
	t->prev = NULL;
	List.push_back(t);
	Sort_Priority(0, t->f_cost);

	return t;
}

//Create children from branching parent
Node* Branch_Parent(Node* parent) {
	Node* child = new Node(parent->puzzle, priority, parent->g_cost, parent->h_cost, parent->f_cost);
	priority++;
	return child;
}

//Solve puzzle //Main function that solves the puzzle
void Puzzle_Mangement(Node* Parent) {
	time_t start;
	time_t current_time;
	int timer = 0;
	int time_check_min = 0;
	int check_in = 0;
	bool searching = true;
	start = time(NULL);

	while (!Goal_Puzzle(Parent->puzzle)) {
		current_time = time(NULL);
		timer = current_time - start;

		if (timer >= 900) { //15 min timer in sec //Just change 900 to the desired time in sec
			searching = false;
			break;
		}

		time_check_min = timer / 60;
		if (time_check_min != check_in) {
			check_in = time_check_min;
			cout << check_in << " minute(s) have passed" << endl;
		}

		Remove_First_Priority();

		Tile_Up(Parent);
		Tile_Down(Parent);
		Tile_Left(Parent);
		Tile_Right(Parent);

		for (int i = 0; i < List.size(); i++) {
			if (order.at(0).first == List.at(i)->id) {
				Parent = List.at(i);
				break;
			}
		}
	}

	if (searching) {
		Traceback(Parent);
		cout << "Goal State!\n";
		cout << "Solution depth was " << Parent->g_cost << endl;
		cout << "Number of nodes expanded: " << Parent->id << endl;

		int min = timer / 60;
		int sec = timer % 60;
		cout << "It took " << min << " minute(s) and " << sec << " second(s) to solve this puzzle" << endl;
	}
	else {
		cout << "Could not find solution within " << timer / 60 << " minute(s)" << endl;
	}

	priority = 1;
	Node* save = List.at(0);
	pair<int, int> original;
	original.first = save->id;
	original.second = save->f_cost;

	List.clear();
	order.clear();

	List.push_back(save);
	order.push_back(original);
}

//Check for any duplicate of previous puzzle
bool Duplicate_Check(vector<vector<int>> check) {
	for (int i = 0; i < List.size(); i++) {
		if (check == List.at(i)->puzzle) {
			return true;
		}
	}
	return false;
}

//Picking which algorithm to use to solve the puzzle
void Algorithm_Method() {
	Method = '\0';
	cout << "\nWhich algorithm would you like to use to solve the current puzzle:\n";
	cout << "U  - Uniform Cost Search\n";
	cout << "MT - Misplaced Tile Heuristic\n";
	cout << "MD - Manhattan Distance Heuristic\n";
	cout << "Q  - go back to menu\n";
	cin >> Method;
	while (Method != "U" && Method != "MT" && Method != "MD" && Method != "Q") {
		cout << "Invalid Input, Try again.\n";
		cin >> Method;
	}
}

//Calculate h for MT
int Misplaced_Tile(vector<vector<int>> MT_status) {
	int cnt = 0;

	for (int i = 0; i < puzzle_size; i++) {
		for (int j = 0; j < puzzle_size; j++) {
			if (MT_status[i][j] != correct_puzzle[i][j] && MT_status[i][j] != 0) {
				cnt++;
			}
		}
	}

	return cnt;
}

//Calculate h for MD
int Manhattan_Distance(vector<vector<int>> MD_status) {
	int cnt = 0;

	for (int i = 0; i < puzzle_size; i++) {
		for (int j = 0; j < puzzle_size; j++) {
			if (MD_status[i][j] != correct_puzzle[i][j] && MD_status[i][j] != 0) {
				int row = (MD_status[i][j] - 1) / puzzle_size;
				int column = (MD_status[i][j] - 1) % puzzle_size;

				if (row < i) {
					cnt = (i - row) + cnt;
				}
				else {
					cnt = (row - i) + cnt;
				}
				if (column < j) {
					cnt = (j - column) + cnt;
				}
				else {
					cnt = (column - j) + cnt;
				}
			}
		}
	}
	return cnt;
}

//Determines how to calculate h based on user preference solution algorithm
int Calc_h(vector<vector<int>> parent) {
	int cost;
	if (Method == "MT") {
		cost = Misplaced_Tile(parent);
	}
	else if (Method == "MD") {
		cost = Manhattan_Distance(parent);
	}
	else {
		cost = 0;
	}

	return cost;
}

//A list of puzzles already made to solve
Node* Preset_Puzzle() {
	vector<vector<int>> Preset;
	vector<vector<int>> Preset_A = { { 4, 1, 2 },
									 { 5, 3, 0 },
									 { 7, 8, 6 } };
	vector<vector<int>> Preset_B = { { 7, 5, 2 },
									 { 4, 3, 6 },
									 { 8, 1, 0 } };
	vector<vector<int>> Preset_C = { {  1,  2, 12,  4 },
									 {  5,  6, 11,  3 },
									 {  9, 10,  0,  8 },
									 { 13, 14, 15,  7 } };
	vector<vector<int>> Preset_D = { { 0, 7, 2 },
									 { 4, 6, 1 },
									 { 3, 5, 8 } };

	cout << "\nChose one of the following lists of puzzles:\n\n";
	Print_Puzzle(Preset_A);
	cout << "A)\n\n";
	Print_Puzzle(Preset_B);
	cout << "B)\n\n";
	Print_Puzzle(Preset_C);
	cout << "C)\n\n";
	Print_Puzzle(Preset_D);
	cout << "D)\n\n";

	cout << "Enter either A, B, C, D, or Q to go back to menu\n";
	cin >> choice;

	while (choice != "A" && choice != "B" && choice != "C" && choice != "D" && choice != "Q") {
		cout << "Invalid Input, Try again.\n";
		cin >> choice;
	}

	if (choice == "A") {
		Preset = Preset_A;
	}
	else if (choice == "B") {
		Preset = Preset_B;
	}
	else if (choice == "C") {
		Preset = Preset_C;
	}
	else if (choice == "D") {
		Preset = Preset_D;
	}
	else if (choice == "Q") {
		Preset.clear();
	}

	puzzle_size = Preset.size();
	Node* t = new Node(Preset, 0, 0, Calc_h(Preset), Calc_h(Preset));
	t->prev = NULL;
	List.push_back(t);
	Sort_Priority(0, t->f_cost);

	return t;
}

//Print backtracking to print moves made from original puzzle to solved
void Traceback(Node* trace) {
	vector<Node*> tracing;

	while (trace != NULL) {
		tracing.push_back(trace);
		trace = trace->prev;
	}

	for (int i = tracing.size() - 1; i >= 0; i--) {
		cout << "The best state to expand with a g(n) = " << tracing.at(i)->g_cost << " and h(n) = " << tracing.at(i)->h_cost << " is?\n";
		Print_Puzzle(tracing.at(i)->puzzle);
	}
}
