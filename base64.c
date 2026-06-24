#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "base64.h"

// Function to encode a binary buffer into Base64
char *base64_encode_malloc(const unsigned char *data, size_t input_length) {
    size_t output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = (char *)malloc(output_length + 1);
    if (encoded_data == NULL) {
        return NULL; // Memory allocation failed
    }

    int res = base64_encode( data, input_length, output_length, encoded_data );

    if (B64R_SUCCESS != res) {
        free(encoded_data);
        return NULL; // Got an error
    }

    return encoded_data;
}

// Function to decode a Base64 encoded string into binary data
unsigned char *base64_decode_malloc(const char *input, size_t *output_length) {
    size_t input_length = strlen(input);
    if (input_length % 4 != 0) {
        return NULL; // Invalid Base64 input length
    }

    // Calculate the expected output length
    *output_length = (3 * input_length) / 4;
    if (input[input_length - 1] == '=') {
        (*output_length)--;
    }
    if (input[input_length - 2] == '=') {
        (*output_length)--;
    }
    size_t output_size = *output_length;

    // Allocate memory for the decoded data
    unsigned char *decoded_data = (unsigned char *)malloc(*output_length);
    if (decoded_data == NULL) {
        return NULL; // Memory allocation failed
    }

    int res = base64_decode( input, output_size, decoded_data, output_length );

    if (B64R_SUCCESS != res) {
        free(decoded_data);
        return NULL; //Got an error
    }

    return decoded_data;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    size_t input_length = strlen(input);

    // Check if the input is already Base64 encoded
    int is_base64 = 1;
    for (size_t i = 0; i < input_length; i++) {
        if (strchr(base64_chars, input[i]) == NULL && input[i] != '=') {
            is_base64 = 0;
            break;
        }
    }

    if (is_base64) {
        size_t decoded_length;
        unsigned char *decoded_data = base64_decode_malloc(input, &decoded_length);
        if (decoded_data != NULL) {
            decoded_data[decoded_length] = '\0';
            printf("Decoded Value: %s\n", decoded_data);
            free(decoded_data);
        } else {
            printf("Decoding failed.\n");
        }
    } else {
        // Encode the input as Base64
        char *encoded_data = base64_encode_malloc((unsigned char *)input, input_length);
        if (encoded_data != NULL) {
            printf("Encoded Base64: %s\n", encoded_data);
            free(encoded_data);
        } else {
            printf("Encoding failed.\n");
        }
    }

    return 0;
}
