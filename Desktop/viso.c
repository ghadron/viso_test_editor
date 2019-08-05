#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#define INIT_LINE_SIZE 64

struct file_lines {
	char *line;
	struct file_lines *nxt_line;
};

struct file_data {
	int cursor;
	int line_count;
};

// init_file(_file_, _file_name) Opens or creates a file with the given file
// name
// requires: _file_ does not point to null
// 			 _file_name does not point to null
// 			 _file_name is not empty
// effects: Modifies _file_ to point to file with given file name
// time: O(1)
void init_file(FILE **_file_, char *_file_name, struct file_data *_file_state_) {
	assert(_file_);
	assert(_file_state_);
	assert(_file_name);
	assert(strlen(_file_name) > 0);
	FILE *test_file_exits;
	test_file_exits = fopen(_file_name, "r");
	if(test_file_exits == NULL) *_file_ = fopen(_file_name, "w+");
	else *_file_ = fopen(_file_name, "r+");
	fclose(test_file_exits);
}

// add_new_lines(_cur_line) Creates a new file_lines
// struct for the new line with a size of INIT_LINE_SIZE, then makes the
// current line point to the new, then sets the current line to the new line
// requires: _cur_line does not point to null
// effects: allocates memory for new file_lines struct
//			sets [_cur_line].nxt_line to the new file_lines struct
// 			changes [_cur_line] to the new file_lines struct
void append_new_line(struct file_lines *_cur_line) {
	assert(_cur_line);
	struct file_lines *cur_line = _cur_line;
	struct file_lines *new_line = malloc(INIT_LINE_SIZE * sizeof(char) +
		sizeof(struct file_lines *));
	new_line->line = malloc(INIT_LINE_SIZE * sizeof(char));
	new_line->nxt_line = NULL;
	while(cur_line != NULL) {
		if(cur_line->nxt_line == NULL)break;
		cur_line = cur_line->nxt_line;
	}
	cur_line->nxt_line = new_line;
}

// fill_file_lines(_file, _cur_line, _file_state_) Reads though the given 
// [_file] and creates file_lines structs for each lines and fills them with
// the characters from the file. As well the meta data about the file is stored
// in [_file_state_]
// requires: _file does not point to null
//			 _cur_line does not point to null
// 			 _file_state_ does not point to null
// effects: reads input from [_file]
// 			may reallocate memory to make _cur_line larger
// 			changes _file_state
// time: O(n) where n is the number of characters in [_file]
void fill_file_lines(FILE *_file, struct file_lines *_lines_head, struct file_data *_file_state_) {
	assert(_file);
	assert(_lines_head);
	assert(_file_state_);
	char ch = fgetc(_file);
	if(ch == EOF) return;
	append_new_line(_lines_head);
	struct file_lines *cur_line = _lines_head->nxt_line;
	int cur_line_size = INIT_LINE_SIZE;
	int line_ch_count = 0;
	while(ch != EOF) {
		if(line_ch_count >= cur_line_size) {
			cur_line = realloc(cur_line, 2 * sizeof(cur_line));
			cur_line_size *= 2;
		}
		if(ch == '\n') {
			append_new_line(cur_line);
			cur_line = cur_line->nxt_line;
			cur_line_size = INIT_LINE_SIZE;
			line_ch_count = 0;
			_file_state_->line_count += 1;
		} else cur_line->line[line_ch_count++] = ch;
		ch = fgetc(_file);
	}
	_file_state_->line_count += 1;
	rewind(_file);
}

// init_file_lines(_file, _file_state_) Returns a pointer to the frist line of
// a linked list of the lines of the given [_file]
// requires: _file does not point to null
//			 _file_state_ does not point to null
// effects: Modifies _file_state_ to the [_file]'s line anc char count
struct file_lines *init_file_lines(FILE *_file, struct file_data *_file_state_) {
	assert(_file);
	assert(_file_state_);
	struct file_lines *lines_head_ = malloc(INIT_LINE_SIZE * sizeof(char) + 
		sizeof(struct file_lines *));
	lines_head_->line = malloc(INIT_LINE_SIZE * sizeof(char));
	lines_head_->nxt_line = NULL;
	fill_file_lines(_file, lines_head_, _file_state_);
	return(lines_head_);
}

// close_file(_file, _lines_head) Closes [_file] and frees all of [_lines_head]
// requires: _file does not point to null
//			 _lines_head does not point to null
// effects: closed [_file]
//			frees all nodes in [_lines_head]
void close_file(FILE *_file, struct file_lines *_lines_head) {
	assert(_file);
	assert(_lines_head);
	struct file_lines *cur_line = _lines_head;
	struct file_lines *nxt_line = cur_line->nxt_line;
	while(cur_line != NULL) {
		free(cur_line->line);
		free(cur_line);
		cur_line = nxt_line;
		if(cur_line != NULL)nxt_line = cur_line->nxt_line;
	}
	fclose(_file);
}

// get_cursor_line(_lines, _cursor_line) Returns the pointer to the file_line
// struct where the cursor is
// requires: _lines does not point to null
//			 _cursor_line > 0
struct file_lines *get_cursor_line(struct file_lines *_lines, int _cursor_line) {
	assert(_cursor_line > 0);
	struct file_lines *cur_line = _lines;
	int line_count = 1;
	while(cur_line != NULL) {
		if(line_count++ == _cursor_line)return cur_line;
		else cur_line = cur_line->nxt_line;
	}
	return NULL;
}

// show_lines_in_range(_lines, _file_struct, _lower_line, _upper_line) Prints
// each line in [_lines] with the range set by [_lower_line] and [_upper_line]
// requires: _lines does not point to null
// 			 _file_state does not point to null
// 			 _lower_line >= 1
// 			 _upper_line <= line count + 1
//			 _lower_line <= _upper_line
// effects: prints out to the screen
// time: O(n) where n is the number of lines in [_lines]
void show_lines_in_range(struct file_lines *_lines, struct file_data *_file_state, int _lower_line, int _upper_line) {
	assert(_lines);
	assert(_file_state);
	assert(_lower_line >= 1);
	assert(_upper_line <= _file_state->line_count + 1);
	assert(_lower_line <= _upper_line);
	int line_count = 1;
	struct file_lines *cur_line = _lines;
	while(cur_line != NULL) {
		if(line_count >= _lower_line) {
			if(line_count != _file_state->cursor)printf("%d. ", line_count);
			else printf("@. ");
			printf("%s\n", cur_line->line);
		}
		if(line_count > _upper_line)break;
		line_count++;
		cur_line = cur_line->nxt_line;
	}
}

// show_file(_lines, _cursor_line) Prints each line of the given [_file_lines]
// with a line number except the line which has the _cursor which will display
// an @
// requires: _lines does not point to null
//			 _file_state does not point to null
// effects: prints out to screen
// time: O(n) where n is the number lines in the [_lines]
void show_file(struct file_lines *_lines, struct file_data *_file_state) {
	assert(_file_state);
	if(_lines) {
		show_lines_in_range(_lines, _file_state, 1, 
			_file_state->line_count + 1);
	} else printf("1. \n");

}

// show_cursor(_lines, _file_state, _args) Prints the lines around the cursor
// with the number lines past the cursor that are shown. If there is no [_args]
// or it is not a number then only the cursor line is shown
// requires: _lines does not point to null
// 			 _file_state does not point to null
// effects: prints out to the screen
void show_cursor(struct file_lines *_lines, struct file_data *_file_state, char *_args) {
	assert(_file_state);
	int line_radius = 0;
	if(_args != NULL)line_radius = atoi(_args);
	if(line_radius < 0)line_radius = 0;
	if(_file_state->line_count == 0){
		printf("1. \n");
		return;
	}
	if(line_radius == 0){
		printf("@. %s\n", get_cursor_line(_lines, _file_state->cursor)->line);
		return;
	}
	int lower_line = _file_state->cursor - line_radius;
	int upper_line = _file_state->cursor + line_radius;
	if(lower_line <= 0)lower_line = 1;
	if(upper_line > _file_state->line_count) {
		upper_line = _file_state->line_count + 1;
	}
	show_lines_in_range(_lines, _file_state, lower_line, upper_line - 1);
}

// cusor_up(_file_state, _args) Moves the cursor position up by the given 
// number of lines, if there is no given number what is given is not a number
// the cursor is not moved. 
// requires: _file_state does not point to null
// effects: may modify _file_state
void cursor_up(struct file_data *_file_state, char *_args) {
	assert(_file_state);
	int lines_up = 0;
	if(_args != NULL)lines_up = atoi(_args);
	if(lines_up < 0)lines_up = 0;
	_file_state->cursor += lines_up;
	if(_file_state->cursor > _file_state->line_count) {
		_file_state->cursor = _file_state->line_count;
	}
}

// cursor_down(_file_state, _args) Moves the cursor position down by the given
// number of lines, if there is no given number what is given is not a number
// the cursor is not moved.
// requires: _file_state does not point to null
// effects: may modify _file_state
void cursor_down(struct file_data *_file_state, char *_args) {
	assert(_file_state);
	int lines_down = 0;
	if(_args != NULL)lines_down = atoi(_args);
	if(lines_down < 0)lines_down = 0;
	_file_state->cursor -= lines_down;
	if(_file_state->cursor < 1)_file_state->cursor = 1;
}

void cursor_to(struct file_data *_file_state, char *_args) {
	assert(_file_state);
	int target_line = 0;
	if(_args != NULL)target_line = atoi(_args);
	if(target_line <= 0 || target_line > _file_state->line_count)return;
	_file_state->cursor = target_line;
}

// new_line(_lines, _file_state, _args) Adds a new line after the cursor and if
// the contents of [_args] into that new line
// requires: _lines does not point to null
// 			 _file_state does not point to null
// effects: may modify _lines_head
// 			may modify _file_state
void new_line(struct file_lines *_lines_head, struct file_data *_file_state, char *_args) {
	assert(_lines_head);
	assert(_file_state);
	struct file_lines *cursor_line = get_cursor_line(_lines_head->nxt_line, 
		_file_state->cursor);
	struct file_lines *new_line = malloc(INIT_LINE_SIZE * sizeof(char) +
		sizeof(struct file_lines *));
	new_line->line = malloc(INIT_LINE_SIZE * sizeof(char));
	if(cursor_line == NULL) {
		_lines_head->nxt_line = new_line;
		new_line->nxt_line = NULL;
	} else {
		struct file_lines *nxt_line = cursor_line->nxt_line;
		cursor_line->nxt_line = new_line;
		new_line->nxt_line = nxt_line;
		_file_state->cursor += 1;
	}
	_file_state->line_count += 1;
	if(_args != NULL)strcat(new_line->line, _args);
}

// delete_line(_lines_head, _file_state) Removes the line were the cursor is at
// requires: _lines_head does not point to null
//			 _file_state does not point to null
// effects: may modify _lines_head
//			may modify _file_state
void delete_line(struct file_lines *_lines_head, struct file_data *_file_state) {
	assert(_lines_head);
	assert(_file_state);
	if(_file_state->line_count <= 0)return;
	if(_file_state->cursor > 1) {
		struct file_lines *prev_cursor_line = 
			get_cursor_line(_lines_head->nxt_line, _file_state->cursor - 1);
		struct file_lines *cursor_line = prev_cursor_line->nxt_line;
		prev_cursor_line->nxt_line = cursor_line->nxt_line;
		free(cursor_line->line);
		free(cursor_line);
	}
	else {
		struct file_lines *delete_line = _lines_head->nxt_line;
		_lines_head->nxt_line = NULL;
		free(delete_line->line);
		free(delete_line);
	}
	_file_state->line_count -= 1;
	if(_file_state->cursor > _file_state->line_count)_file_state->cursor -= 1;
}

// line_count(_file_state) Prints out the number of lines in the file
// requires: _file_state does not point to null
// effects: Prints out to the screen
void line_count(struct file_data *_file_state) {
	assert(_file_state);
	printf("lc. %d\n", _file_state->line_count);
}

bool is_word_char(char _letter) {
	if(isalpha(_letter))return true;
	else if(isdigit(_letter))return true;
	else if(_letter == '_')return true;
	else if(_letter == '-')return true;
	else if(_letter == '\'')return true;
	return false;
}

// word_count(_lines) Prints out the number of words in the file
// requires: _lines does not point to null
// effects Prints out to the screen
void word_count(struct file_lines *_lines) {
	struct file_lines *cur_line = _lines;
	int word_count = 0;
	bool word_char = false;
	bool in_word = false;
	while(cur_line != NULL) {
		for(int i = 0; i < strlen(cur_line->line); i++) {
			word_char = is_word_char(cur_line->line[i]);
			if(!in_word && word_char){
				word_count++;
				in_word = true;
			}
			else if(in_word && !word_char)in_word = false;
		}
		cur_line = cur_line->nxt_line;
	}
	printf("wc. %d\n", word_count);
}

// char_count(_lines) Prints out the number of characters in the file
// effects: Prints out to the screen
// time: O(n) where n is the number of character in the files
void char_count(struct file_lines *_lines) {
	struct file_lines *cur_line = _lines;
	int char_count_ = 0;
	while(cur_line != NULL) {
		char_count_ += strlen(cur_line->line);
		cur_line = cur_line->nxt_line;
	}
	printf("cc. %d\n",char_count_ );
}

// save(_file, _lines) Writes all of [_lines] to the given [_file]
// requires: _file does not point to null
// 			 _lines does not point to null
void save(FILE *_file, struct file_lines *_lines) {
	assert(_file);
	assert(_lines);
	freopen(NULL, "w+", _file);
	struct file_lines *cur_line = _lines;
	char *new_line = "\n";
	while(cur_line != NULL) {
		fwrite(cur_line->line, sizeof(char), strlen(cur_line->line), _file);
		cur_line = cur_line->nxt_line;
		if(cur_line != NULL) {
			fwrite(new_line, sizeof(char), strlen(new_line), _file);
		}
	}
}

// cmd_processor(_file, _lines_head, _file_state, _input) Takes the given input
// and passes it to the function that will handle that given command
// requries: _file does not point to null
// 			 _lines_head does not point to null
// 			 _file_state does not point to null
// effects: may modify _file
//   		may modify _lines_head
//			may modify _file_state
void cmd_processor(FILE *_file, struct file_lines *_lines_head, struct file_data *_file_state, char _input[1024]) {
	assert(_file);
	assert(_lines_head);
	assert(_file_state);

	char *show_file_cmd = "sf";
	char *show_cursor_cmd = "sc";
	char *cursor_up_cmd = "cu";
	char *cursor_down_cmd = "cd";
	char *cursor_to_cmd = "ct";
	char *new_line_cmd = "nl";
	char *delete_line_cmd = "dl";
	// char *append_cursor_line_cmd = "ul";
	// char *prepend_cursor_line_cmd = "pl";
	char *line_count_cmd = "lc";
	char *word_count_cmd = "wc";
	char *char_count_cmd = "cc";
	char *save_cmd = "s";
	char *quit_cmd = "q";

	struct file_lines *lines = _lines_head->nxt_line;
	char *cmd = strtok(_input, " ");
	char *args = strtok(NULL, "\0");

	if(strcmp(cmd, show_file_cmd) == 0)show_file(lines, _file_state);
	else if(strcmp(cmd, show_cursor_cmd) == 0)show_cursor(lines, _file_state, args);
	else if(strcmp(cmd, cursor_up_cmd) == 0)cursor_up(_file_state, args);
	else if(strcmp(cmd, cursor_down_cmd) == 0)cursor_down(_file_state, args);
	else if(strcmp(cmd, cursor_to_cmd) == 0)cursor_to(_file_state, args);
	else if(strcmp(cmd, new_line_cmd) == 0)new_line(_lines_head, _file_state, args);
	else if(strcmp(cmd, delete_line_cmd) == 0)delete_line(_lines_head, _file_state);
	else if(strcmp(cmd, line_count_cmd) == 0)line_count(_file_state);
	else if(strcmp(cmd, word_count_cmd) == 0)word_count(lines);
	else if(strcmp(cmd, char_count_cmd) == 0)char_count(lines);
	else if(strcmp(cmd, save_cmd) == 0)save(_file, lines);
	else if(strcmp(cmd, quit_cmd) != 0) printf("Could not find: %s\n", cmd);
}

// input_loop(_file, _lines_head, _file_state) Reads input from the user for
// the commands for viso to excute
// requires: _file does not point to null
// 			 _lines_head does not point to null
// 			 _file_state does not point to null
// effects: prints to the screen
// 			takes user input
void input_loop(FILE *_file, struct file_lines *_lines_head, struct file_data *_file_state) {
	assert(_file);
	assert(_lines_head );
	assert(_file_state);
	char input[1024] = "";
	char *quit_cmd = "q";
	while(strcmp(input, quit_cmd) != 0) {
		printf("> ");
		scanf ("%[^\n]%*c", input);
		cmd_processor(_file, _lines_head, _file_state, input);
	}
}

int main(int argc, char **argv) {
	assert(argc > 1);
	FILE *file;
	struct file_data file_state = {1, 0};
	init_file(&file, argv[1], &file_state);
	struct file_lines *lines_head = init_file_lines(file, &file_state);
	input_loop(file, lines_head, &file_state);
	close_file(file, lines_head);
	return 0;
}