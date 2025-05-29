#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

#define FIRSTNAME "Thomas"
#define SURNAME "Meier"
#define BRANCH_OF_STUDY "TIS"
#define EMAIL "meier.thomas-it24@it.dhbw-ravensburg.de"

const char* morse[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", // A-J
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",  // K-T
    "..-", "...-", ".--", "-..-", "-.--", "--..",                         // U-Z
    "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", // 0-9
    ".-.-.-", "--..--", "---...", "-.-.-.", "..--..", "-...-", "-....-", ".-.-.",  // .,:;?=-+
    "..--.-", "-.--.", "-.--.-", "-..-.", ".--.-." //_()/@
};

const char supported_characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,:;?=-+_()/@";

void print_help() {
    printf("Morse Code Encoder/Decoder\n");
    printf("Usage: morse [OPTIONS] [FILE]\n");
    printf("If you want to type the text directly in the console do:");
    printf("echo exampleText | morse [OPTIONS] [FILE]\n");
    printf("Options:\n");
    printf("  -h, --help             Display this help message.\n");
    printf("  --programmer-info      Display information about the programmer.\n");
    printf("  -e, --encode           Encode text to Morse code (default behavior).\n");
    printf("  -d, --decode           Decode Morse code to text.\n");
    printf("  -o, --out [FILE]       Write output to the specified file.\n");
}

void print_programmer_info() {
    printf("{\n");
    printf("  \"firstname\": \"%s\",\n", FIRSTNAME);
    printf("  \"surname\": \"%s\",\n", SURNAME);
    printf("  \"branch_of_study\": \"%s\",\n", BRANCH_OF_STUDY);
    printf("  \"contact\": \"%s\"\n", EMAIL);
    printf("}\n");
}

void encode_to_morse(const char *text, char *morse_result) {
    char c;
    int index;
    for (; *text; ++text) {
        c = *text;
        
        // Ignore \n and \r
        if (c == '\n' || c == '\r') {
            continue;
        }
        
        if (isalpha(c)) {
            c = toupper(c);
            strcat(morse_result, morse[c - 'A']);
        } else if (c >= '0' && c <= '9') {
            strcat(morse_result, morse[c - '0' + 26]);
        } else {
            const char* ptr = strchr(supported_characters, c);
            if (ptr) {
                index = (int)(ptr - supported_characters);
                strcat(morse_result, morse[index]);
            } else if (c == ' ') {
                strcat(morse_result, "   ");
                continue;
            } else {
                strcat(morse_result, "*");
            }
        }
        strcat(morse_result, " ");
    }
}

char decode_morse(const char *morse_letter) {
    for (int i = 0; i < sizeof(morse) / sizeof(morse[0]); ++i) {
        if (strcmp(morse[i], morse_letter) == 0) {
            return supported_characters[i];
        }
    }
    return '*'; // if no match 
}

void decode_from_morse(const char *morse_code, char *text_result) {
    char buffer[10];
    int buffer_index = 0;
    
    for (; *morse_code; ++morse_code) {
        // Ignore \n and \r
        if (*morse_code == '\n' || *morse_code == '\r') {
            continue;
        }
        
        if (*morse_code != ' ') {
            buffer[buffer_index++] = *morse_code;
        } else {
            if (buffer_index != 0) {
                buffer[buffer_index] = '\0';
                char letter = decode_morse(buffer);
                strncat(text_result, &letter, 1);
                buffer_index = 0;
            }
            if (*(morse_code+1) == ' ' && *(morse_code+2) == ' ') {
                strcat(text_result, " ");
                morse_code += 2; 
            }
        }
    }
    
    // use last morse code if existing
    if (buffer_index != 0) {
        buffer[buffer_index] = '\0';
        char letter = decode_morse(buffer);
        strncat(text_result, &letter, 1);
    }
}

// read input into string
char* read_entire_input(FILE *input) {
    char *content = NULL;
    size_t capacity = 1024;
    size_t length = 0;
    
    content = malloc(capacity);
    if (!content) {
        return NULL;
    }
    
    int c;
    while ((c = fgetc(input)) != EOF) {
        if (length >= capacity - 1) {
            capacity *= 2;
            char *new_content = realloc(content, capacity);
            if (!new_content) {
                free(content);
                return NULL;
            }
            content = new_content;
        }
        content[length++] = c;
    }
    
    content[length] = '\0';
    return content;
}

void process_input(FILE *input, char *text_result, int encode) {
    // read entire input
    char *input_content = read_entire_input(input);
    if (!input_content) {
        fprintf(stderr, "Error: Couldn't read input.\n");
        return;
    }
    
    //process entire string
    if (encode) {
        encode_to_morse(input_content, text_result);
    } else {
        decode_from_morse(input_content, text_result);
    }
    
    free(input_content);
}

int main(int argc, char *argv[]) {
    int option_index = 0, encode = 0, decode = 0;
    char *output_file = NULL;
    char *input_file = NULL;
    
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"programmer-info", no_argument, 0, 0},
        {"encode", no_argument, 0, 'e'},
        {"decode", no_argument, 0, 'd'},
        {"out", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };
    
    int c;
    while ((c = getopt_long(argc, argv, "hedo:", long_options, &option_index)) != -1) {
        switch (c) {
            case 0:
                if (strcmp(long_options[option_index].name, "programmer-info") == 0) {
                    print_programmer_info();
                    return 0;
                }
                break;
            case 'h':
                print_help();
                return 0;
            case 'e':
                encode = 1;
                break;
            case 'd':
                decode = 1;
                break;
            case 'o':
                output_file = optarg;
                break;
            default:
                print_help();
                return 1;
        }
    }
    
    if (encode && decode) {
        fprintf(stderr, "Error: Optionen --encode and --decode can't be used simultaneously.\n");
        return 1;
    }
    
    if (!encode && !decode) {
        encode = 1;
    }
    
    FILE *input = stdin;  
    if (optind < argc) {
        input_file = argv[optind];
        input = fopen(input_file, "r");
        if (!input) {
            fprintf(stderr, "Error: File %s could not be opened.\n", input_file);
            return 1;
        }
    }
    
    char result[8192] = "";
    process_input(input, result, encode);
    
    if (input != stdin) {
        fclose(input);
    }
    
    FILE *output = stdout;
    if (output_file) {
        output = fopen(output_file, "w");
        if (!output) {
            fprintf(stderr, "Error: output into file %s has failed.\n", output_file);
            return 1;
        }
    }
    
    fprintf(output, "%s", result);
    
    if (output != stdout) {
        fclose(output);
    }
    
    return 0;
}