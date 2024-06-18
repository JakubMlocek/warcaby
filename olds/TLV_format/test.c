#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to encode data into TLV format
unsigned char* tlv_encode(const char* data, size_t data_len, size_t* tlv_len) {
    // Calculate total TLV length
    *tlv_len = 2 + data_len;  // 1 byte for type + 1 byte for length + data length

    // Allocate memory for TLV buffer
    unsigned char* tlv_data = (unsigned char*)malloc(*tlv_len);
    if (tlv_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Type (assuming 1 byte, you can change it as per your requirements)
    tlv_data[0] = 0x01;

    // Length (assuming 1 byte, you can change it as per your requirements)
    tlv_data[1] = (unsigned char)data_len;

    // Copy data into TLV buffer
    memcpy(&tlv_data[2], data, data_len);

    return tlv_data;
}

// Function to decode TLV data
char* tlv_decode(const unsigned char* tlv_data, size_t tlv_len) {
    // Validate minimum TLV length
    if (tlv_len < 2) {
        fprintf(stderr, "Invalid TLV data\n");
        return NULL;
    }

    // Type (assuming 1 byte, you can change it as per your requirements)
    unsigned char tlv_type = tlv_data[0];

    // Length (assuming 1 byte, you can change it as per your requirements)
    unsigned char tlv_length = tlv_data[1];

    // Validate total TLV length
    if (tlv_len != 2 + tlv_length) {
        fprintf(stderr, "Invalid TLV length\n");
        return NULL;
    }

    // Allocate memory for data string
    char* data = (char*)malloc(tlv_length + 1);
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy value part into data string
    memcpy(data, &tlv_data[2], tlv_length);
    data[tlv_length] = '\0';  // Null-terminate the string

    return data;
}

int main() {
    const char* original_data = "Hello, TLV!";

    // Encode data into TLV format
    size_t original_len = strlen(original_data);
    size_t tlv_len;
    unsigned char* encoded_tlv = tlv_encode(original_data, original_len, &tlv_len);
    if (encoded_tlv == NULL) {
        fprintf(stderr, "Encoding failed\n");
        return 1;
    }

    // Print encoded TLV data (hex representation)
    printf("Encoded TLV:");
    for (size_t i = 0; i < tlv_len; ++i) {
        printf(" %02x", encoded_tlv[i]);
    }
    printf("\n");

    // Decode TLV data
    char* decoded_data = tlv_decode(encoded_tlv, tlv_len);
    if (decoded_data == NULL) {
        fprintf(stderr, "Decoding failed\n");
        return 1;
    }

    // Print decoded data
    printf("Decoded Value: %s\n", decoded_data);

    // Clean up allocated memory
    free(encoded_tlv);
    free(decoded_data);

    return 0;
}
