#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_ROTATIONS 0x2000
#define MAX_LINE_LENGTH 0x10

#define STARTING_POSITION 50
#define DIAL_SIZE 100

size_t read_door_codes_file(const char *filename, size_t rotations_size,
                            int8_t rotations[restrict]) {
        FILE *file = fopen(filename, "r");

        if (file == NULL) {
                fprintf(stderr, "Error: Could not open file '%s'\n", filename);
                return 0;
        }

        char line[MAX_LINE_LENGTH];
        size_t line_number = 0;

        while (fgets(line, sizeof(line), file) != NULL) {
                // Skip empty lines
                if (line[0] == '\n' || line[0] == '\0')
                        continue;

                if (line_number >= rotations_size) {
                        fprintf(stderr, "Error: Too many rotations (max %zu)\n",
                                rotations_size);
                        fclose(file);
                        return 0;
                }

                char direction = line[0];

                char *endptr;
                long value = strtol(&line[1], &endptr, 10);
                if (endptr == &line[1] || (*endptr != '\n' && *endptr != '\0')) {
                        fprintf(stderr, "Error: Invalid number on line %zu\n",
                                line_number + 1);
                        fclose(file);
                        return 0;
                }

                // Normalize the rotation value
                value = value % DIAL_SIZE;

                if (direction == 'L') {
                        rotations[line_number] = (int8_t) -value;
                } else if (direction == 'R') {
                        rotations[line_number] = (int8_t) value;
                } else {
                        fprintf(stderr,
                                "Error: Malformed line %zu (expected 'L' or 'R', got '%c')\n",
                                line_number + 1, direction);
                        fclose(file);
                        return 0;
                }

                line_number++;
        }

        fclose(file);
        return line_number;
}

__attribute__((hot))
size_t count_zero_positions(size_t num_rotations,
                            const int8_t rotations[restrict]) {
        int position = STARTING_POSITION;
        size_t count = 0;

        // Assumes that rotation is now guaranteed to be in the range
        // [-DIAL_SIZE, DIAL_SIZE] from parsing step, so we only need to add or
        // subtract once
        for (size_t i = 0; i < num_rotations; i++) {
                position += rotations[i];

                if (position < 0)
                        position += DIAL_SIZE;
                else if (position >= DIAL_SIZE)
                        position -= DIAL_SIZE;

                // Branchless Counting micro-optimization
                count += (position == 0);
        }

        return count;
}

int main(int argc, char *argv[]) {
        // Using int8_t to save memory, as *currently* rotations are in range:
        // [-100, 100]
        // This reduces the array footprint from 32KB to 16KB, improving the
        // likelihood that the working set remains in L1 or L2 cache.
        // Note: If DIAL_SIZE changes, this may need to be updated.
        int8_t rotations[MAX_NUM_ROTATIONS] = {0};

        if (argc < 2) {
                fprintf(stderr, "Usage:\n\t%s <input_file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        const size_t num_rotations_read = read_door_codes_file(argv[1],
                                                               MAX_NUM_ROTATIONS,
                                                               rotations);

        if (!num_rotations_read) {
                fprintf(stderr, "Error: Failed to read file '%s'\n", argv[1]);
                return EXIT_FAILURE;
        }

        const size_t password = count_zero_positions(num_rotations_read,
                                                     rotations);

        printf("Password: %zu\n", password);

        return EXIT_SUCCESS;
}
