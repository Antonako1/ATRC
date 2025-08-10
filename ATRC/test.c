#define ATRC_IMPLEMENTATION
#define ATRC_DEBUG
#define ATRC_LOGGING
#define ATRC_DEVELOPMENT
#include <ATRC/ATRC.h>
#include <stdio.h>

int main() {
    ATRC_FD fd = create_atrc_fd();
    init_atrc_fd(&fd);
    int status = read_file(&fd, "test.atrc");
    if(status != ATRC_ERROR_NONE) {
        fprintf(stderr, "Error reading file: %d\n", status);
        return status;
    }
    if(check_status(&fd)) {
        printf("File read successfully and is safe to use.\n");
    } else {
        printf("File is not safe to use.\n");
    }
    // init_atrc_fd
    // read_file_ex
    // read_file
    // read_file_again
    // read_file_again_ex
    // check_status
    free_atrc_fd(&fd);
    return 0;
}