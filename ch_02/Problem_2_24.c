#include <sys/types.h>
#include <sys/uio.h> // vector I/O operations https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysuio.h.html
#include <unistd.h> // read, write https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html
#include <fcntl.h> // open, O_RDONLY
#include <stdio.h> // perror(), to print detailed error messa
#include <sys/stat.h> // See man 2 chmod.

/* see https://github.com/darbinreyes/OSC9e-v1/blob/master/src/ch2/programming_problems/pp_2_26/prog_prob_2_26.c
*/

int main(void) {

    //You can use int or long int data types, however ssize_t is a system data type 
    // that should be used for cross-platform portability
    ssize_t w_nbytes;
    ssize_t r_nbytes;

    int src_file_fd;
    #define INPUT_BUFFER_SIZE 256
    char src_file_path[INPUT_BUFFER_SIZE];

    int dest_file_fd;
    char dest_file_path[INPUT_BUFFER_SIZE];

    unsigned char cp_buf[INPUT_BUFFER_SIZE];
    mode_t dest_file_mode = S_IRUSR | S_IWUSR;

    /*
    KNR pg. 138.
    "three files are open, with file descriptors 0, 1, and 2, called the standard
    input, the standard output, and the standard error
    */

    #define STD_IN_FD  0
    #define STD_OUT_FD 1
    #define STD_ERR_FD 2
    #define SRC_FILE_PROMPT "\nEnter the source path: "

    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/write.html
    // write to STD OUT
    //  RETURN VALUE
    //      Upon successful completion, write() and pwrite() will return the number of bytes actually 
    //      written to the file associated with fildes. This number will never be greater than nbyte. 
    //      Otherwise, -1 is returned and errno is set to indicate the error.
    //      Upon successful completion, writev() returns the number of bytes actually written. 
    //      Otherwise, it returns a value of -1, the file-pointer remains unchanged, and errno is set 
    //      to indicate an error.
    w_nbytes = write(STD_OUT_FD, SRC_FILE_PROMPT, sizeof(SRC_FILE_PROMPT));

    // https://pubs.opengroup.org/onlinepubs/7908799/xsh/read.html
    // receive from STD IN
    //  RETURN VALUE
    //      Upon successful completion, read(), pread() and readv() return a non-negative integer 
    //      indicating the number of bytes actually read. Otherwise, the functions return -1 and 
    //      set errno to indicate the error.
    r_nbytes = read(STD_IN_FD, src_file_path, INPUT_BUFFER_SIZE - 1); // -1, to save a place for the string's NULL terminator.

    // remove newline character if there is one
    if (r_nbytes > 0 && src_file_path[r_nbytes - 1] == '\n'){
        src_file_path[r_nbytes - 1] = '\0';
    }

    // append to the end?
    src_file_path[r_nbytes] = '\0';


    src_file_fd = open(src_file_path, O_RDONLY);

    if (src_file_fd == -1) {
        // Error
        #define OPEN_ERR_MSG "\nopen() error. Bye.\n"
        w_nbytes = write(STD_OUT_FD, OPEN_ERR_MSG, sizeof(OPEN_ERR_MSG));
        perror("");
        return -1;
    }

    /*
    Acquire output file name.
      Write prompt to screen
      Accept input
    */
    #define DEST_FILE_PROMPT "\nEnter the destination path: "

    // write to STD OUT
    w_nbytes = write(STD_OUT_FD, DEST_FILE_PROMPT, sizeof(DEST_FILE_PROMPT));

    // receive from STD IN
    r_nbytes = read(STD_IN_FD, dest_file_path, INPUT_BUFFER_SIZE - 1);

    // Remove new line from input
    if (r_nbytes > 0 && dest_file_path[r_nbytes - 1] == '\n'){
        dest_file_path[r_nbytes - 1] = '\0';
    }

    dest_file_path[r_nbytes] = '\0';

    dest_file_fd = open(dest_file_path, O_WRONLY|O_CREAT|O_EXCL, dest_file_mode);

    /*
    Values for oflag in open()
    Value           Meaning
    O_RDONLY        Open the file so that it is read only.
    O_WRONLY        Open the file so that it is write only.
    O_RDWR          Open the file so that it can be read from and written to.
    O_APPEND        Append new information to the end of the file.
    O_TRUNC         Initially clear all data from the file.
    O_CREAT         If the file does not exist, create it. If the O_CREAT option is used, then you must include the third parameter.
    O_EXCL          Combined with the O_CREAT option, it ensures that the caller must create the file. If the file already exists, the call will fail.
    */

    if (dest_file_fd == -1) {
    // Error
    w_nbytes = write(STD_OUT_FD, OPEN_ERR_MSG, sizeof(OPEN_ERR_MSG));
    perror("");
    return -1;
    }

    while( (r_nbytes = read(src_file_fd, cp_buf, INPUT_BUFFER_SIZE)) != 0) {
        if (r_nbytes == -1) {
          // Error
          break;
        }

        w_nbytes = write(dest_file_fd, cp_buf, r_nbytes);

        if (w_nbytes == -1) {
          // Error
          break;
        }
    }   

    if (r_nbytes == -1) {
        // Error
        #define READ_ERR_MSG "\nread() error. Bye.\n"
        w_nbytes = write(STD_OUT_FD, READ_ERR_MSG, sizeof(READ_ERR_MSG));
        perror("");
        return -1;
    }

    if (w_nbytes == -1) {
    // Error
    #define WRITE_ERR_MSG "\nwrite() error. Bye.\n"
    w_nbytes = write(STD_OUT_FD, WRITE_ERR_MSG, sizeof(WRITE_ERR_MSG));
    perror("");
    return -1;
    }

    /*
    Close output file
    ******* Close input file
    Write complete message.
    Terminate normally.
    */
    close(src_file_fd);
    close(dest_file_fd);

    // FYI I am not checking for errors from close().

    #define DONE_MSG "\nCopy successful. Bye.\n"
    w_nbytes = write(STD_OUT_FD, DONE_MSG, sizeof(DONE_MSG));
    return 0;
}


/*
(base) davideynon@Davids-MacBook-Pro silberschatz_solutions % sudo dtruss ./a.out
dtrace: system integrity protection is on, some features will not be available

SYSCALL(args)        = return

Enter the source path: open("/dev/dtracehelper\0", 0x2, 0x0)         = 3 0
ioctl(0x3, 0x80086804, 0x7FFEE96E79A0)       = 0 0
close(0x3)       = 0 0
mprotect(0x10651B000, 0x4000, 0x1)       = 0 0
access("/AppleInternal/XBS/.isChrooted\0", 0x0, 0x0)         = -1 2
bsdthread_register(0x7FFF20342434, 0x7FFF20342420, 0x2000)       = 1073742047 0
shm_open(0x7FFF2021AF66, 0x0, 0x20219CBB)        = 3 0
fstat64(0x3, 0x7FFEE96E6930, 0x0)        = 0 0
mmap(0x0, 0x1000, 0x1, 0x40001, 0x3, 0x0)        = 0x106529000 0
close(0x3)       = 0 0
ioctl(0x2, 0x4004667A, 0x7FFEE96E69E4)       = 0 0
mprotect(0x10652F000, 0x1000, 0x0)       = 0 0
mprotect(0x106536000, 0x1000, 0x0)       = 0 0
mprotect(0x106537000, 0x1000, 0x0)       = 0 0
mprotect(0x10653E000, 0x1000, 0x0)       = 0 0
mprotect(0x10652A000, 0x90, 0x1)         = 0 0
mprotect(0x10653F000, 0x1000, 0x1)       = 0 0
mprotect(0x10652A000, 0x90, 0x3)         = 0 0
mprotect(0x10652A000, 0x90, 0x1)         = 0 0
issetugid(0x0, 0x0, 0x0)         = 0 0
getentropy(0x7FFEE96E6180, 0x20, 0x0)        = 0 0
getentropy(0x7FFEE96E61D0, 0x40, 0x0)        = 0 0
getpid(0x0, 0x0, 0x0)        = 49430 0
stat64("/AppleInternal\0", 0x7FFEE96E6F10, 0x0)      = -1 2
csops_audittoken(0xC116, 0x7, 0x7FFEE96E6A40)        = -1 22
proc_info(0x2, 0xC116, 0xD)      = 64 0
csops_audittoken(0xC116, 0x7, 0x7FFEE96E6B30)        = -1 22
sysctlbyname(kern.osvariant_status, 0x15, 0x7FFEE96E6F60, 0x7FFEE96E6F58, 0x0)       = 0 0
csops(0xC116, 0x0, 0x7FFEE96E6F94)       = 0 0
dtrace: error on enabled probe ID 1681 (ID 170: syscall::write:return): invalid kernel access in action #12 at DIF offset 68
./codes.txt

Enter the destination path: dtrace: error on enabled probe ID 1683 (ID 168: syscall::read:return): invalid kernel access in action #12 at DIF offset 68
open("./codes.txt\0", 0x0, 0x0)      = 3 0
dtrace: error on enabled probe ID 1681 (ID 170: syscall::write:return): invalid kernel access in action #12 at DIF offset 68
./cracker10.txt

Copy successful. Bye.
dtrace: error on enabled probe ID 1683 (ID 168: syscall::read:return): invalid kernel access in action #12 at DIF offset 68
open("./cracker10.txt\0", 0xA01, 0x180)      = 4 0
dtrace: error on enabled probe ID 1683 (ID 168: syscall::read:return): invalid kernel access in action #12 at DIF offset 68
dtrace: error on enabled probe ID 1681 (ID 170: syscall::write:return): invalid kernel access in action #12 at DIF offset 68
dtrace: error on enabled probe ID 1683 (ID 168: syscall::read:return): invalid kernel access in action #12 at DIF offset 68
close(0x3)       = 0 0
close(0x4)       = 0 0
dtrace: error on enabled probe ID 1681 (ID 170: syscall::write:return): invalid kernel access in action #12 at DIF offset 68

(base) davideynon@Davids-MacBook-Pro silberschatz_solutions % 
*/


