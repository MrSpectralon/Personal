#include "../header_files/helper_methods.h"



void remove_new_line (char *str)
{
    size_t len = strlen (str);
    if (len > 0 && str[len - 1] == '\n') 
    {
        str[len - 1] = '\0';
    }
}



int string_compare_64b (const char *str1, const char *str2)
{
    size_t len = strlen(str1);
    size_t len2 = strlen(str2);

    if (len != len2)
    {
        return 0;
    }
    size_t i;

    // Compare 8 bytes (64 bits) at a time as long as there are 64 or more bits left of the string.
    for (i = 0; i + sizeof (uint64_t) <= len; i += sizeof (uint64_t))
    {
        uint64_t *ptr1 = (uint64_t *) (str1 + i);
        uint64_t *ptr2 = (uint64_t *) (str2 + i);
        if (*ptr1 != *ptr2)
        {
            return 0;
        }
    }
    // Handle remaining bytes (if the string length is not a multiple of 8)
    for (; i < len; ++i)
    {
        if (str1[i] != str2[i])
        {
            return 0;
        }
    }
    return 1;
}


char* base64url_encode(const char* string, const size_t len)
{
    //Base 64 encoded string.
    char* encoded = NULL;
    //Base 64 URL safe table. Base 64 decimal value = index value 
    char b64urls_table[] = "ABCDEFGHIJKLMNOPQRSTUZWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    
    //Decimal value for lookup table.
    int8_t val = 0;
    //Number of 6 bit chunks
    uint64_t num_o_chunks = (len * 8) / 6;
    //Counter for which base64 character is being worked on
    uint64_t chunk_pos = 0;
    //Counter for position in the base64 bit index.
    uint8_t bit_chunk_pos = 5;

    encoded = calloc(num_o_chunks + 1, sizeof(char));
    if(encoded == NULL)
    {
        fprintf(stderr, "Error allocating memory for b64 encoded string.\n");
        goto cleanup;        
    }
    
    //Iterating through all characters in string
    for (size_t c = 0; c < len; c++)
    {

        //Iterating through all bits in current character.
        for (int8_t b = 7; b >= 0; b--)
        {
            //Checking if current bit has value 1.    
            if(string[c] >> b & 1)
            {
                //Adding 2^bit_chunk_pos to the base64 value
                val += (uint8_t)1 << bit_chunk_pos;
            }

            //Check if we need to go to the next 6 bit chunk.
            if (bit_chunk_pos == 0)
            {
                //Appends current character and resets.
                encoded[chunk_pos] = b64urls_table[val];
                chunk_pos++;
                bit_chunk_pos = 5;
                val = 0;
            }
            else
            {
                //Checking if on last character iteration
                if (c == (len - 1) && b == 0)
                {
                    //Adding next encoded character if character iterations ended before finishing next complete base64 character.
                    encoded[chunk_pos] = b64urls_table[val];
                }
                bit_chunk_pos--;
            }
        }
    }
    encoded[len + 1] = '\0';
    return encoded;

    cleanup:
    free(encoded);
    return NULL;
}

/**
 * Rotates the bits in an unsigned integer to the right.
 * I.E; Bits rightshifted out of memory 'scope' will be added to the left of memory scope.
 * Returns this shifted value - does not tamper with the original variable.
 */
uint32_t bit_rotate_right(const uint32_t data, const int rotations)
{
    uint32_t temp = data;
    unsigned char flip = 0b10000000;
    uint8_t* byte_ptr = (uint8_t*)&temp;
    for (size_t i = 0; i < rotations; i++)
    {
        if(temp & 1)
        {
            temp >>= 1;
            byte_ptr[3] |= flip;
        }
        else
        {
            temp >>= 1;
        }    
    }
    
    return temp;
}

void sha256_chunk_processing(const b512_t padded_string_chunk, uint32_t* hash_values, const uint32_t* k)
{
    uint32_t w[64] = {0}; //Message schedule - 64x 32bit words.

    uint32_t* word_ptr = (uint32_t*)padded_string_chunk;
    
    //Aparently interpreting strings as int will make the string a big endian.
    //So here i need to reverse the orders so that the strings are interpreted correctly. 
    for (size_t i = 0; i < 16; i++)
    {
        unsigned char* bptr = (unsigned char*)&word_ptr[i];
        unsigned char* bptr2 = (unsigned char*)&w[i];
        bptr2[0] = bptr[3];
        bptr2[1] = bptr[2];
        bptr2[2] = bptr[1];
        bptr2[3] = bptr[0];
    }
    

    //Actually have no idea of what is happening here, but i'm following the algorithm specified here:
    // https://blog.boot.dev/cryptography/how-sha-2-works-step-by-step-sha-256/#step-5---create-message-schedule-w
    for (size_t i = 16; i < 64; i++) //w for words.
    {        
        uint32_t s0 = (bit_rotate_right(w[i-15], 7)) ^ (bit_rotate_right(w[i-15], 18)) ^ (w[i-15] >> 3);
        uint32_t s1 = (bit_rotate_right(w[i-2], 17)) ^ (bit_rotate_right(w[i-2], 19)) ^ (w[i-2] >> 10);
        uint64_t tmp = w[i-16] + s0 + w[i-7] + s1;
        w[i] = (uint32_t)tmp;
    }

    uint32_t a = hash_values[0];
    uint32_t b = hash_values[1];    
    uint32_t c = hash_values[2];
    uint32_t d = hash_values[3];
    uint32_t e = hash_values[4];
    uint32_t f = hash_values[5];
    uint32_t g = hash_values[6];
    uint32_t h = hash_values[7];

    for (size_t i = 0; i < 64; i++)
    {
        uint32_t S1 = bit_rotate_right(e, 6) ^ bit_rotate_right(e, 11) ^ bit_rotate_right(e, 25);  
        uint32_t ch = (e & f) ^ ((~e) & g);        
        uint32_t SO = bit_rotate_right(a, 2) ^ bit_rotate_right(a, 13) ^ bit_rotate_right(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);

        uint64_t temp1 = h + S1 + ch + k[i] + w[i];
        uint64_t temp2 = SO + maj;
        
        uint64_t rtmp1 = temp1 + d;
        uint64_t rtmp2 = temp1 + temp2;

        h = g;  
        g = f;
        f = e;

        e = (uint32_t)rtmp1; 
        d = c;
        c = b;
        b = a;
        a = (uint32_t)rtmp2; 
   }
    
    hash_values[0] =  (uint32_t)((uint64_t)(hash_values[0] + a) % UINT32_MAX);
    hash_values[1] =  (uint32_t)((uint64_t)(hash_values[1] + b) % UINT32_MAX);
    hash_values[2] =  (uint32_t)((uint64_t)(hash_values[2] + c) % UINT32_MAX);
    hash_values[3] =  (uint32_t)((uint64_t)(hash_values[3] + d) % UINT32_MAX);
    hash_values[4] =  (uint32_t)((uint64_t)(hash_values[4] + e) % UINT32_MAX);
    hash_values[5] =  (uint32_t)((uint64_t)(hash_values[5] + f) % UINT32_MAX);
    hash_values[6] =  (uint32_t)((uint64_t)(hash_values[6] + g) % UINT32_MAX);
    hash_values[7] =  (uint32_t)((uint64_t)(hash_values[7] + h) % UINT32_MAX);

}


char* hmac256sha_encode(const char* string, const size_t len)
{
    char* hash = NULL;

    // Round constants
    const uint32_t k[] = {
        0x428a2f98 ,0x71374491 ,0xb5c0fbcf ,0xe9b5dba5 ,0x3956c25b ,0x59f111f1 ,0x923f82a4 ,0xab1c5ed5
        ,0xd807aa98 ,0x12835b01 ,0x243185be ,0x550c7dc3 ,0x72be5d74 ,0x80deb1fe ,0x9bdc06a7 ,0xc19bf174
        ,0xe49b69c1 ,0xefbe4786 ,0x0fc19dc6 ,0x240ca1cc ,0x2de92c6f ,0x4a7484aa ,0x5cb0a9dc ,0x76f988da
        ,0x983e5152 ,0xa831c66d ,0xb00327c8 ,0xbf597fc7 ,0xc6e00bf3 ,0xd5a79147 ,0x06ca6351 ,0x14292967
        ,0x27b70a85 ,0x2e1b2138 ,0x4d2c6dfc ,0x53380d13 ,0x650a7354 ,0x766a0abb ,0x81c2c92e ,0x92722c85
        ,0xa2bfe8a1 ,0xa81a664b ,0xc24b8b70 ,0xc76c51a3 ,0xd192e819 ,0xd6990624 ,0xf40e3585 ,0x106aa070
        ,0x19a4c116 ,0x1e376c08 ,0x2748774c ,0x34b0bcb5 ,0x391c0cb3 ,0x4ed8aa4a ,0x5b9cca4f ,0x682e6ff3
        ,0x748f82ee ,0x78a5636f ,0x84c87814 ,0x8cc70208 ,0x90befffa ,0xa4506ceb ,0xbef9a3f7 ,0xc67178f2
    };

    //Initialized hash values.
    uint32_t h[] = {
                0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a
                ,0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
               };
    
    uint64_t padding_fill_size = 0;

    const uint64_t orig_size = len * 8;
    uint64_t size = orig_size;
    // + 1 because of the first bit insertion.
    int remainder = (size % 512);
    if (remainder > 448)
    {
        padding_fill_size = 512 - remainder;
        padding_fill_size += 512;       
    }
    else
    {
        padding_fill_size = 512 - remainder;
    }

    size +=  padding_fill_size;
    if (!size % 512)
    {
        fprintf(stderr, "Size of data is not a multiple of 512 termenating hashing function.\n");
        return NULL;
    }
    
    size_t byte_size = size/8;
    
    printf("Original size: %ld, S = %ld, P = %ld, Remainder: %d\n", len*8, size, padding_fill_size, remainder);
    
    uint64_t data_chunks = size/512;

    //Using calloc to sanitize data and not have to manually do padding with zeroes.
    //This will be a lot slower for large files, but since this function is intended to be used only with O-auth2, total bit size will likely never exceed 1500 bits.
    b512_t* padded_data = calloc(data_chunks, sizeof(b512_t));
    if (!padded_data)
    {
        fprintf(stderr, "Error allocating memory for padded data.\n");
    }

    memcpy(padded_data, string, len);
    unsigned char* byte_ptr = (unsigned char*)padded_data;

    //Adds a 1 to the first bit after the input data.
    byte_ptr[len] |= 0b10000000; 

    //Copies the length of original data as a big-endian at the end of my data blob.
    uint8_t* big_endian_ptr = (uint8_t*)&orig_size;
    for (size_t i = 0; i < 8; i++)
    {
        byte_ptr[byte_size-i-1] = big_endian_ptr[i];
    }

    for (size_t i = 0; i < data_chunks; i++)
    {
        sha256_chunk_processing(padded_data[i], h, k);
    }

    hash = malloc(65);
    hash[0] = '\0';
    char hex_str[9];
    for (size_t i = 0; i < 8; i++)
    {
        sprintf(hex_str, "%08x", h[i]);
        strcat(hash, hex_str);
    }
    free(padded_data);
    return hash; 
}
