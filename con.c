#include <stdio.h>

const char* testString = "{ 'string':'hallo iedereen', 'number':123453.32, 'array':[12, 'a string', ['nested array'], {'nested':'map'}) }";

/** 
    Characters 
*/
#define MAP_START 		"{"
#define MAP_END 		"}"
#define ARRAY_START 		"]"
#define ARRAY_END 		"["
#define STRING_START 		"'\""
#define STRING_END   		"'\""
#define WHITE_SPACE 		" \t\n"

#define VALUE_SEPERATOR 	','
#define KEY_VALUE_SEPERATOR 	':'

#define CONTROL_COUNT 		3 // {, [ and "

const char* STARTS[] = {
        MAP_START, ARRAY_START, STRING_START
};

const char* ENDS[] = {
        MAP_END, ARRAY_END, STRING_END
};

/**
   Errors
 */
typedef enum Error {
        ERROR_WRONG_START_CHAR	= -1,
        ERROR_UNCLOSED_OBJECT	= -2,
        ERROR_ILLEGAL_NUMBER	= -3,
        ERROR_COMMA_EXPECTED	= -4,
        ERROR_WRONG_TYPE	= -5
} Error;

/**
   Object
*/
typedef struct Object {
	const char* totalString;
        int start, end;
} Object;

typedef enum ObjectType {
        TYPE_MAP,
        TYPE_ARRAY,
        TYPE_NUMBER,
        TYPE_STRING,
        TYPE_UNKNOWN
} ObjectType;

/**
   Returns the position of the next non-white-character. Or -1 if there isn't one left.
*/
int find_next_start_and_count(int start_position, const char* string, int* commas, int* colons) {
        *commas = 0;
        *colons = 0;
        int i = start_position - 1;
        while(string[++i]) {
                int isWhiteSpace = 0;
                int j=-1;
                while(WHITE_SPACE[++j]) {
                        if (WHITE_SPACE[j] == string[i]) {
                                isWhiteSpace = 1;
                                break;
                        }
                }
                if (!isWhiteSpace) return i;
                else if (string[i] == ',') (*commas)++;
                else if (string[i] == ':') (*colons)++;
        }
        return -1;
}

int find_next_start(int start_position, const char* string) {
        int a = 0;
        return find_next_start_and_count(start_position, string, &a, &a);
}

/**
   Returns the closing varient of the starting character at start_position.
*/
char find_char_end(int start_position, const char* string) {
        for (int i=0; i<CONTROL_COUNT; i++) {
                int count = -1;
                while (STARTS[i][++count]) {
                        if (string[start_position] == STARTS[i][count]) {
                                return ENDS[i][count];
                        }
                }
        }
        return 0;
}

/**
   Returns the type of the object starting at start_position.
*/
ObjectType get_object_type(int start_position, const char* string) {
        char c = string[start_position];
        if (c == '-' || (c >=48 && c <= 57)) return TYPE_NUMBER;
        if (c == MAP_START[0]) {
                return TYPE_MAP;
        } else if (c == ARRAY_START[0]) {
                return TYPE_ARRAY;
        } else if (c == STRING_START[0] || c == STRING_START[1]) {
                return TYPE_STRING;
        }
        return TYPE_UNKNOWN;
}

/**
   Returns the position of the character that closed the object starting
   at start_position.
*/
int find_matching_position(int start_position, const char* string) {

        // Find match of a possible number ...
        if (get_object_type(start_position, string) == TYPE_NUMBER) {
                int i=start_position;
                int foundDot = 0;
                while(string[++i]) {
                        char c = string[i];
                        if (c == '.') {
                                foundDot++;
                                if (foundDot > 1) return ERROR_ILLEGAL_NUMBER;
                        } else if (c == '-') {
                                return ERROR_ILLEGAL_NUMBER;
                        } else if (c < 48 || c > 57) {
                                return i-1;
                        }
                }
                return ERROR_ILLEGAL_NUMBER;
        }

        // Find match of any other object type ...
        int char_start = string[start_position];
        char char_end = find_char_end(start_position, string);
        if (!char_end) return ERROR_WRONG_START_CHAR;
       
	int i = start_position;
        int count = 0;
        while (string[++i]) {
                char c = string[i];
                if (c == char_start) count++;
                else if (c == char_end) {
                        if (count-- == 0) return i;
                }
        }
        return ERROR_UNCLOSED_OBJECT;
}

//////

int read_string(int start_position, const char* string, char* result) {
        start_position = find_next_start(start_position, string);
        if (get_object_type(start_position, string) != TYPE_STRING) {
                return ERROR_WRONG_TYPE;
        }
        int end_position = find_char_end(start_position, string);
        for (int i=0; i<end_position-start_position-1; i++) {
                result[i] = string[start_position+i];
        }
        result[end_position-start_position] = '\0';
        return end_position-start_position;
}

int main() {
        int p1 = find_matching_position(38, testString);
	int p2 = get_object_type(38, testString);
	int p3 = find_char_end(2, testString);

        char str[256];
        if (read_string(2, testString, str) < 0) {
                printf("Error\n");
        }
        printf("%d\n", p1);
        printf("%d\n", p2);
        printf("%d\n", p3);
        printf("%s\n", str);
        return 0;
}


