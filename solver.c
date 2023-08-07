#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// max!
static int max(int first, int second) {
	return (first > second) ? first : second;
}

// min!
static int min(int first, int second) {
	return (first < second) ? first : second;
}

const int RPN_WHAT_OPERATOR_BUFFER_SIZE = 8;
const int RPN_WHAT_OPERATOR_PRIORITY[] = {0, 0, 1, 1, 2};
const char RPN_WHAT_OPERATOR_OPERATORS_LENGTH[] = {1, 1, 1, 1, 2};
const char RPN_WHAT_OPERATOR_OPERATORS[][RPN_WHAT_OPERATOR_BUFFER_SIZE] = {"-", "+", "/", "*", "**"};;
const int RPN_WHAT_OPERATOR_LENGTH = sizeof(RPN_WHAT_OPERATOR_OPERATORS) / RPN_WHAT_OPERATOR_BUFFER_SIZE;

// Just like what the name says, it asks what operator it is!
bool rpn_what_operator(int (*output)[2], char *current_characters, int index_limit) {
	
	int j = 0;
	bool match = false;
	//printf("\n\nStarting!\n\n");
	for (int i = RPN_WHAT_OPERATOR_LENGTH-1; i >= 0; i--) { // Looping through the strings!
		
		match = true;
		for (j = 0; j < RPN_WHAT_OPERATOR_OPERATORS_LENGTH[i]; j++) { // Matching each character in string to see if they are the same!
			//printf("%c vs %c\n", RPN_WHAT_OPERATOR_OPERATORS[i][j],  current_characters[j]);
			if (RPN_WHAT_OPERATOR_OPERATORS[i][j] != current_characters[j]) { // Mistmatch handler here!
				match = false;
				break;
			}
		}
		
		if (match) { // If matched then LETS GOOOOO
			(*output)[0] = i;
			(*output)[1] = RPN_WHAT_OPERATOR_PRIORITY[i];
		
			return true;
		}
	
		
	}
	
	// Noo there wasn't any matches!
	
	(*output)[0] = -1;
	(*output)[1] = 0;
	
	return false;
	
}

// Just some funny function to handle all the calculations. It is possible to add custom thingies! :D

double rpn_perform_operator(double a, double b, int operation) {
	
	//printf("%lf, %lf, %d\n", a, b, operation);
	
	// I think going directly to return is fine since it just exits the program
	switch (operation) {
		case 0:
			return a - b;
		case 1:
			return a + b;
		case 2:
			return (b != 0) ? (a / b) : 0.0;
		case 3:
			return a * b;
		case 4:
			return pow(a, b);
	}
	
	return 0.0;
}

// The stuff needed for the rpn_is_idigit(char) function!
const char RPN_CHARACTER_START = '0';
const char RPN_CHARACTER_END = '9';

bool rpn_is_digit(char current_character) {
	return RPN_CHARACTER_START <= current_character && current_character <= RPN_CHARACTER_END;
}

// Just some code to convert the text into integer!
double rpn_convert_to_number(char *current_text, int size) {
	
	int jump = 1;
	double output = 0;
	char current_character = 0;
	
	for (int i = size-1; i >= 0; i--) {
		
		
		// Moves back the entire output when it discovers the dot!
		current_character = current_text[i];
		if(current_character == '.') {
			output = output / jump;
			jump = 1;
			continue;
		}
		
		// Adding the current_value into the output!
		output += (current_character - RPN_CHARACTER_START) * jump;
		jump = jump * 10;
	}
	
	return output;
}

// Some constant variables 
const int RPN_ERROR_PREVIEW = 32;
const int RPN_BUFFER_LIMIT = 256;
const int RPN_PRETEND_STACK_LIMIT = 1024;

double rpn_calculate(char *current_text, int current_size) {
	
	// Just for detecting new scopes
	int current_parenthesis_immunities = 0;
	
	// Pretend stacks to have them on the stack and it would be quite silly to actually create stack structures inside here and that would take a bunch of more code!
	bool current_dotted = false;
	bool previous_was_operator = true;
	bool negative = false;
	
	double current_number_stack[RPN_PRETEND_STACK_LIMIT] = {0};
	int current_operator_stack[RPN_PRETEND_STACK_LIMIT][2] = {0};
	
	// Counters to keep track of the tails of each stack!
	int current_number_stack_counter = 0;
	int current_operator_stack_counter = 0;
	
	// Holds the characters!
	char current_character = 0;
	char other_character   = 0;
	
	// The character buffer that is going to be used for storing the numbers in text form!
	char current_character_buffer[RPN_BUFFER_LIMIT] = {0};
	int current_character_buffer_counter = 0;
	
	int current_operator[2] = {0};
	
	int i = 0; int j = current_size; int k = 0; int l = 0;
	while (i < j) {
		
		current_character = current_text[i];
		if (current_character == ')') { // Check if there is a stray closing parenthesis!
			previous_was_operator = false; // This can sit here for a reason :D // Just some negative logic for like -10
			
			printf("\n[!] stray ')' found at index%d\n\n'", i+current_character_buffer_counter);
				
			// This just prints the text before the incident!
			for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
				printf("%c", current_text[k]); 
			}
			printf("<- \n\n[<] exiting...\n");
				
			return 0.0;
			
		} else if (current_character == '(') { // Checks if there is a possible new scope!
		
			// Just preparation!
			current_character_buffer_counter = 0;
			current_parenthesis_immunities = 1;
			
			// It tries to loop until it finishes the scope or it hits the end!
			while (current_parenthesis_immunities > 0 && (i+current_character_buffer_counter+1) < j) {
				other_character = current_text[i+current_character_buffer_counter+1];
				
				if (other_character == '(') { // Checks if it has to ignore inner scopes!
					current_parenthesis_immunities += 1;
					
				} else if (other_character == ')') { // Check if the parenthesis has ended!
					current_parenthesis_immunities -= 1;
					
					// Just to break out early when there is finally none!
					if (current_parenthesis_immunities == 0) { break; }
				}
				
				// It just checks if the buffer is being overflowed!
				if (current_character_buffer_counter > RPN_BUFFER_LIMIT) {
					printf("\n[!] character buffer overflowed at index%d\n\n'", i+current_character_buffer_counter);
					
					// This just prints the text before the incident!
					for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
						printf("%c", current_text[k]); 
					}
					printf("<- \n\n[<] exiting...\n");
					
					return 0.0;
				}
				
				// Adding it to the buffer and stuff!
				current_character_buffer[current_character_buffer_counter] = other_character;
				current_character_buffer_counter += 1;
				
			}
			
			// Checks if it actually completed the scope or not!
			if (current_parenthesis_immunities != 0) {
				printf("\n[!] missing ')' of scope at index%d\n\n'", i+current_character_buffer_counter);
				
				// This just prints the text before the incident!
				for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
					printf("%c", current_text[k]); 
				}
				printf("<- \n\n[<] exiting...\n");
				
				return 0.0;
			}
			
			// Checking and handling if the amount of numbers were too much!
			if (current_number_stack_counter > RPN_PRETEND_STACK_LIMIT) { 
				printf("\n[!] number stack overflowed at index%d\n\n'", i+current_character_buffer_counter);
				
				// This just prints the text before the incident!
				for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
					printf("%c", current_text[k]); 
				}
				printf("<- \n\n[<] exiting...\n");
				
				return 0.0;
			}
			
			// Now to call itself to handle this smaller scope!
			current_number_stack[current_number_stack_counter] = rpn_calculate(current_character_buffer, current_character_buffer_counter);
			current_number_stack_counter += 1;
			
			// Jumping the index!
			i += current_character_buffer_counter+1;
			
			// Just some negative logic for like -10
			negative = false;
			previous_was_operator = false;
			
		} else if (rpn_what_operator(&current_operator, &(current_text[i]), j-i) == true) { // If the current characters might be an operator or not!
			//printf("I SAID THERE WAS A MATCH\n");
			//printf("There was a match! :D %s\n", RPN_WHAT_OPERATOR_OPERATORS[current_operator[0]]);
			
			// Just a little something to handle the negative thing :D
			if (previous_was_operator == true && current_operator[0] == 0) {
				negative = true;
				previous_was_operator = false;
				
			} else if (previous_was_operator == true) { // So it doesn't get an error calculating stuff!
				printf("\n[!] operator '{%c}' placed wrongly found  at index %d\n\n'", current_character, i);
			
				 // This just prints the text before the incident!
				for (k = max(0, i-RPN_ERROR_PREVIEW-1); k <= i; k++) {
					printf("%c", current_text[k]); 
				}
				printf("<- \n\n[<] exiting...\n");
				
				return 0.0;
			} else {
				
				// This allow chain reactions on whether or not to add numbers based on the priority of each operation!
			
				// Checking if the current operator allows the previous operator to execute without affecting the following calculations!
				while (current_operator_stack[current_operator_stack_counter-1][1] >= current_operator[1] && current_operator_stack_counter > 0 && current_number_stack_counter > 0) {
					current_number_stack[current_number_stack_counter-2] = rpn_perform_operator(current_number_stack[current_number_stack_counter-2], current_number_stack[current_number_stack_counter-1], current_operator_stack[current_operator_stack_counter-1][0]);
					
					current_number_stack_counter -= 1;
					current_operator_stack_counter -= 1;
				}
			
				// Adding the current operator now!
				
				// Checks if there are too much operators!
				if (current_operator_stack_counter > RPN_PRETEND_STACK_LIMIT) { 
					printf("\n[!] operator stack overflowed at index%d\n\n'", i+current_character_buffer_counter);
					
					// This just prints the text before the incident!
					for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
						printf("%c", current_text[k]); 
					}
					printf("<- \n\n[<] exiting...\n");
					
					return 0.0;
				}
				
				// Finally adding the current oeprator!
				current_operator_stack[current_operator_stack_counter][0] = current_operator[0];
				current_operator_stack[current_operator_stack_counter][1] = current_operator[1];
				
				current_operator_stack_counter += 1;
				
				// Just some closing logic and negative logic for like -10
				
				negative = false;
				previous_was_operator = true;
			
				i += RPN_WHAT_OPERATOR_OPERATORS_LENGTH[current_operator[0]]-1;
			
			}
		} else if (rpn_is_digit(current_character)) { // Looking if numbers are possibly ahead!
			
			current_character_buffer[0] = current_character;
			current_character_buffer_counter = 1;	
			
			// Extracting the characters that make up the current possible number!!
			while (true) {
				
				other_character = current_text[i+current_character_buffer_counter];
				if (other_character == '.') { // Checking and handling if there were two dots in a single number!
					if (current_dotted == true) {
						printf("\n[!] invalid syntax of dot at index %d\n\n'", i+current_character_buffer_counter);
					
						// This just prints the text before the incident!
						for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
							printf("%c", current_text[k]); 
						}
						printf("<- \n\n[<] exiting...\n");
						
						return 0.0;
					}
					current_dotted = true;
				}
				
				// Checking if the current character is still in a number!
				if ((rpn_is_digit(other_character) || other_character == '.') == false) { break; } 
				
				// Checking and handling if the character buffer responsible for holding the number in text form is okay!
				if (current_character_buffer_counter > RPN_BUFFER_LIMIT) {
					printf("\n[!] character buffer overflowed at index %d\n\n'", i+current_character_buffer_counter);
					
					 // This just prints the text before the incident!
					for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) {
						printf("%c", current_text[k]); 
					}
					printf("<- \n\n[<] exiting...\n");
					
					return 0.0;
				} 
				
				// Adding the number to the buffer stack!
				current_character_buffer[current_character_buffer_counter] = other_character;
				current_character_buffer_counter++;
				
				
			}
			
			// Checking and handling if the amount of numbers were too much!
			if (current_number_stack_counter > RPN_PRETEND_STACK_LIMIT) { 
				printf("\n[!] number stack overflowed at index%d\n\n'", i+current_character_buffer_counter);
				
				// This just prints the text before the incident!
				for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter ; k++) { 
					printf("%c", current_text[k]); 
				}
				printf("<- \n\n[<] exiting...\n");
				
				return 0.0;
			}
			
			// Converting the number and then storing it!
			
			// Remember the negative logic I added? This goes here!
			current_number_stack[current_number_stack_counter] = rpn_convert_to_number(current_character_buffer, current_character_buffer_counter) * ((negative == true) ? -1 : 1);
			current_number_stack_counter++;
			
			// Preparation for continuing on!
			current_dotted = false;
			i += current_character_buffer_counter-1;
			
			//printf("%lf\n", current_number_stack[current_number_stack_counter-1]);
			
			// Just some negative logic for like -10
			negative = false;
			previous_was_operator = false;
			
		} else if (current_character != ' ' && current_character != 0) { // Just checking if the current character is valid or not!
			previous_was_operator = false; // Leave this be! // Just some negative logic for like -10
			printf("\n[!] invalid character '{%c}' found in current text\n\n'", current_character);
			
			 // This just prints the text before the incident!
			for (k = max(0, i+current_character_buffer_counter-RPN_ERROR_PREVIEW - 1); k <= i+current_character_buffer_counter; k++) {
				printf("%c", current_text[k]); 
			}
			printf("<- \n\n[<] exiting...\n");
			
			return 0.0;
		}
		
		i++; // Moving on!
	}
	
	// Performing the rest of the operations that didn't get included. :(
	
	int n = current_number_stack_counter;
	int m = current_operator_stack_counter;
	
	while (m > 0) {
		current_number_stack[n-2] = rpn_perform_operator(current_number_stack[n-2], current_number_stack[n-1], current_operator_stack[m-1][0]);
		n--;
		m--;
	}
	
	// Now the results of all the calculations should have ended up in the very first section of the current number stack!
	return current_number_stack[0];
}

const int RPN_INPUT_LIMIT = 1024;
void rpn_arguement_error() {
	
	// Just some advice on what to do to properly use the program!

	printf("\n\n[?] Please follow the %d character limit and use only the following operators\n[", RPN_INPUT_LIMIT);
	for (int i = 0; i < RPN_WHAT_OPERATOR_LENGTH-1; i++) {
		printf("%s, ", RPN_WHAT_OPERATOR_OPERATORS[i]);
	}
	
	printf("%s", RPN_WHAT_OPERATOR_OPERATORS[RPN_WHAT_OPERATOR_LENGTH-1]);
	printf("]\n[<] exiting\n");

}

// Where it all begins!
int main(int argc, char *argv[]) {
	
	if (argc < 2) { // Checks if there is too ittle arguements!
		printf("\n[!] too few arguements!");
		rpn_arguement_error();
		
		return 1;
	}
	
	int input_counter    = 0;
	int temporary_length = 0;
	char rpn_input[RPN_INPUT_LIMIT] = {0};
	
	for (int i = 1; i < argc; i++) {	
	
		if (input_counter > RPN_INPUT_LIMIT) {
			printf("\n[!] too much arguements!");
			rpn_arguement_error();
		
			return 1;
		}
	
		int temporary_length = 0;
		while (argv[i][temporary_length] != 0) { 
			rpn_input[input_counter] = argv[i][temporary_length];
			
			temporary_length += 1;
			input_counter += 1;
		}
		rpn_input[input_counter] = ' ';
		input_counter += 1;
	}
	
	printf("\n[>] got it!\n [*] calculating...\n\n");
	printf(" [-] calculation result : %lf\n", rpn_calculate(rpn_input, input_counter));
	printf("[<] exiting...\n");
	return 0;
