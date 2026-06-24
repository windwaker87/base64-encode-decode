#ifndef __BASE64_H_
#define __BASE64_H_

// Function return values
#define B64R_SUCCESS  0
#define B64R_ERR_INPUT_PARAM_NULL  -1
#define B64R_ERR_INPUT_BAD_LENGTH  -2
#define B64R_ERR_OUTPUT_PARAM_NULL  -3
#define B64R_ERR_OUTPUT_BAD_LENGTH  -4
#define B64R_ERR_DECODE_NON_BASE64_CHAR  -5
//------------------------------------------------------------------------------


// Base64 character set
const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//------------------------------------------------------------------------------


// Forward function declarations
int base64_encode(const unsigned char*, const size_t, const size_t, char*);
int base64_decode(const char*, const size_t, unsigned char*, size_t*);
//------------------------------------------------------------------------------


// Function to encode a binary buffer into Base64
int base64_encode(
    const unsigned char *input,
    const size_t input_length,
    const size_t output_size,
    char *output)
{
    if (input == NULL || input_length < 1)
        return B64R_ERR_INPUT_PARAM_NULL;

    if (output == NULL || output_size < 1)
        return B64R_ERR_OUTPUT_PARAM_NULL;

    size_t output_length = 4 * ((input_length + 2) / 3);
    if (output_size < output_length)
    {
        // Output buffer not big-enough for encoding
        return B64R_ERR_OUTPUT_BAD_LENGTH;
    }

    // Ready for work - zero output buffer
    memset(output, 0, output_size);

    // Loop through the input data and encode it to Base64
    for (size_t i = 0, j = 0; i < input_length;) {
        // Extract three octets from input data (or use zero padding)
        uint32_t octet_a = i < input_length ? input[i++] : 0;
        uint32_t octet_b = i < input_length ? input[i++] : 0;
        uint32_t octet_c = i < input_length ? input[i++] : 0;

        // Combine three octets into a 24-bit triple
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        // Encode triple as Base64 characters
        output[j++] = base64_chars[(triple >> 18) & 0x3F];
        output[j++] = base64_chars[(triple >> 12) & 0x3F];
        output[j++] = base64_chars[(triple >> 6) & 0x3F];
        output[j++] = base64_chars[triple & 0x3F];
    }

    // Handle padding and null-terminate the encoded string
    while (output_length > 0 && output[output_length - 1] == '=') {
        output_length--;
    }
    output[output_length] = '\0';

    return B64R_SUCCESS;
}
//------------------------------------------------------------------------------


// Function to decode a Base64 encoded string into binary data
int base64_decode(
    const char *input,
    const size_t output_size,
    unsigned char *output,
    size_t *output_length)
{
    if (input == NULL) {
        return B64R_ERR_INPUT_PARAM_NULL;
    }

    size_t input_length = strlen(input);
    if (input_length % 4 != 0) {
        // Invalid Base64 input length
        return B64R_ERR_INPUT_BAD_LENGTH;
    }

    // Calculate the expected output length
    *output_length = (3 * input_length) / 4;
    if (input[input_length - 1] == '=') {
        (*output_length)--;
    }
    if (input[input_length - 2] == '=') {
        (*output_length)--;
    }

    // Check output pointer/length
    if (output == NULL || output_size < *output_length) {
        // NULL pointer or not enough allocated memory
        return B64R_ERR_OUTPUT_PARAM_NULL;
    }

    // Ready for work - zero output buffer
    memset(output, 0, output_size);

    // Initialize variables for decoding process
    size_t j = 0;
    uint32_t sextet_bits = 0;
    int sextet_count = 0;

    // Loop through the Base64 input and decode it
    for (size_t i = 0; i < input_length; i++)
    {
        // Convert Base64 character to a 6-bit value
        uint32_t base64_value = 0;
        if (input[i] == '=')
        {
            base64_value = 0;
        }
        else
        {
            const char *char_pointer = strchr(base64_chars, input[i]);
            if (char_pointer == NULL)
            {
                // Invalid Base64 character
                return B64R_ERR_DECODE_NON_BASE64_CHAR;
            }
            base64_value = char_pointer - base64_chars;
        }

        // Combine 6-bit values into a 24-bit sextet
        sextet_bits = (sextet_bits << 6) | base64_value;
        sextet_count++;

        // When a sextet is complete, decode it into three bytes
        if (sextet_count == 4) {
            output[j++] = (sextet_bits >> 16) & 0xFF;
            output[j++] = (sextet_bits >> 8) & 0xFF;
            output[j++] = sextet_bits & 0xFF;
            sextet_bits = 0;
            sextet_count = 0;
        }
    }

    return B64R_SUCCESS;
}
//------------------------------------------------------------------------------

#endif //__BASE64_H_
