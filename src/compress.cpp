#include <cstdio>
#include <cstdlib>
#include <iosfwd>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h> 
#include <zconf.h>
#include <sys/shm.h>
#include <zlib.h>
#include <cstring>
#include <iomanip>
#include <cassert>

void add_buffer_to_vector(std::vector<char> &vector, const char *buffer, uLongf length) {
    for (int character_index = 0; character_index < length; character_index++) {
        char current_character = buffer[character_index];
        vector.push_back(current_character);
    }
}

int compress_vector(std::vector<char> source, std::vector<char> &destination) {
    unsigned long source_length = source.size();
    uLongf destination_length = compressBound(source_length);

    char *destination_data = (char *) malloc(destination_length);
    if (destination_data == nullptr) {
        return Z_MEM_ERROR;
    }

    Bytef *source_data = (Bytef *) source.data();
    int return_value = compress2((Bytef *) destination_data, &destination_length, source_data, source_length,
                                 Z_BEST_COMPRESSION);
    add_buffer_to_vector(destination, destination_data, destination_length);
    free(destination_data);
    return return_value;
}

int decompress_vector(std::vector<char> source, std::vector<char> &destination) {
    unsigned long source_length = source.size();
    uLongf destination_length = 1 << 23;

    char *destination_data = (char *) malloc(1 << 23);
    if (destination_data == nullptr) {
        //puts("qwq");
        return Z_MEM_ERROR;
    }

    Bytef *source_data = (Bytef *) source.data();
    int return_value = uncompress((Bytef *) destination_data, &destination_length, source_data, source.size());
    add_buffer_to_vector(destination, destination_data, destination_length);
    free(destination_data);
    return return_value;
}

void add_string_to_vector(std::vector<char> &uncompressed_data,
                          const char *my_string) {
    int character_index = 0;
    while (true) {
        char current_character = my_string[character_index];
        uncompressed_data.push_back(current_character);

        if (current_character == '\00') {
            break;
        }

        character_index++;
    }
}

// https://stackoverflow.com/a/27173017/3764804
void print_bytes(std::ostream &stream, const unsigned char *data, size_t data_length, bool format = true) {
    stream << std::setfill('0');
    for (size_t data_index = 0; data_index < data_length; ++data_index) {
        stream << std::hex << std::setw(2) << (int) data[data_index];
        if (format) {
            stream << (((data_index + 1) % 16 == 0) ? "\n" : " ");
        }
    }
    stream << std::endl;
}

int main(int argc, char *argv[]) {
    if(argv[0][0] != 'c' && argv[0][0] != 'd') {
        puts("unrecognized command-line option.");
        return EXIT_FAILURE;
    }
    auto shmID = shmget((key_t)998244353, 1 << 23, IPC_CREAT | 0666);
    auto buf = (char*)shmat(shmID, (void*)0, 0);
    if(argv[0][0] == 'c') {
        std::vector<char> uncompressed(0);
        add_buffer_to_vector(uncompressed, buf, 1 << 23);
        std::vector<char> compressed(0);
        int compression_result = compress_vector(uncompressed, compressed);
        if(compression_result != F_OK) {
            shmctl(shmID, IPC_RMID, 0);
            return EXIT_FAILURE;
        }
        size_t p = 0;
        for(auto &c : compressed)
            buf[p++] = c; 
        FILE *fp = fopen("../config/terminal.dat", "wb+");
        fwrite(buf, p, 1, fp);
        fclose(fp);
        shmctl(shmID, IPC_RMID, 0);
        return EXIT_SUCCESS;
    } else if(argv[0][0] == 'd' ) {
        if(argc < 2) return EXIT_FAILURE;
        std::vector<char> decompressed(0);
        std::vector<char> compressed(0);
        add_buffer_to_vector(compressed, buf, atoi(argv[1]));
        int decompression_result = decompress_vector(compressed, decompressed);
        //std::cout << decompression_result << std::endl;
        if(decompression_result != F_OK) return EXIT_FAILURE;
        memset(buf, 0, 1 << 23);
        int p = 0;
        for(auto &c : decompressed)
            buf[p++] = c;
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}