#include <iostream>
#include <fstream>
#include <cstring>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <chrono>

constexpr size_t BUFFER_SIZE = 4096; // Размер блока чтения/записи

void print_usage() {
    std::cerr << "Usage: acrypt INPUT_FILE_NAME OUTPUT_FILE_NAME\n";
}

void invert_bits(uint8_t* buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = ~buffer[i];
    }
}

int acrypt(const char* input_file, const char* output_file) {
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd < 0) {
        perror("Failed to open input file");
        return 1;
    }

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) {
        perror("Failed to open output file");
        close(input_fd);
        return 1;
    }

    aiocb read_cb{}, write_cb{};
    uint8_t read_buffer[BUFFER_SIZE], write_buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    read_cb.aio_fildes = input_fd;
    read_cb.aio_buf = read_buffer;
    read_cb.aio_nbytes = BUFFER_SIZE;

    write_cb.aio_fildes = output_fd;
    write_cb.aio_buf = write_buffer;

    while (true) {
        // Инициируем асинхронное чтение
        if (aio_read(&read_cb) < 0) {
            perror("Failed to initiate aio_read");
            break;
        }
        std::cout << "Started aio_read\n";

        // Ждём завершения чтения
        while (aio_error(&read_cb) == EINPROGRESS) {
            usleep(1000); // Ждём, чтобы не нагружать процессор
        }

        // Проверяем статус чтения
        int read_status = aio_error(&read_cb);
        if (read_status != 0) {
            perror("Error during aio_read");
            break;
        }

        // Получаем результат чтения
        bytes_read = aio_return(&read_cb);
        if (bytes_read < 0) {
            perror("Failed to read from file");
            break;
        }
        if (bytes_read == 0) {
            // Конец файла
            std::cout << "Reached end of file\n";
            break;
        }

        std::cout << "Read " << bytes_read << " bytes\n";

        // Шифруем данные
        std::memcpy(write_buffer, read_buffer, bytes_read);
        invert_bits(write_buffer, bytes_read);

        // Инициируем асинхронную запись
        write_cb.aio_nbytes = bytes_read;
        if (aio_write(&write_cb) < 0) {
            perror("Failed to initiate aio_write");
            break;
        }
        std::cout << "Started aio_write\n";

        // Ждём завершения записи
        while (aio_error(&write_cb) == EINPROGRESS) {
            usleep(1000); // Ждём, чтобы не нагружать процессор
        }

        // Проверяем статус записи
        int write_status = aio_error(&write_cb);
        if (write_status != 0) {
            perror("Error during aio_write");
            break;
        }

        if (aio_return(&write_cb) < 0) {
            perror("Failed to write to file");
            break;
        }
        std::cout << "Wrote " << bytes_read << " bytes\n";
    }

    close(input_fd);
    close(output_fd);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage();
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    auto start_time = std::chrono::high_resolution_clock::now();
    int result = acrypt(input_file, output_file);
    auto end_time = std::chrono::high_resolution_clock::now();

    if (result == 0) {
        std::cout << "File encrypted successfully.\n";
        std::chrono::duration<double> elapsed = end_time - start_time;
        std::cout << "Elapsed time: " << elapsed.count() << " seconds.\n";
    } else {
        std::cerr << "Encryption failed.\n";
    }

    return result;
}
