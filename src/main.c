#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[])
{
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n   - create new database file\n");
    printf("\t -f   - (required) path to the database file\n");
    return;
}

int main(int argc, char *argv[])
{
    bool newFile = false;
    char *filePath = NULL;
    int c;

    int dbfd = -1;

    struct dbheader_t *dbheader;

    while ((c = getopt(argc, argv, "nf:")) != -1)
    {
        switch (c)
        {
        case 'n':
            newFile = true;
            break;
        case 'f':
            filePath = optarg;
            break;
        case '?':
            print_usage(argv);
            break;
        default:
            return STATUS_ERROR;
        }
    }
 

    if (filePath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);
        return 0;
    }

    if (newFile) {
        dbfd = create_db_file(filePath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return STATUS_ERROR;
        }

        if(create_db_header(dbfd, &dbheader) == STATUS_ERROR) {
            printf("Failed to create database header\n");
            return STATUS_ERROR;
        }
    } else {
        dbfd = open_db_file(filePath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return STATUS_ERROR;
        }

        if (validate_db_header(dbfd, &dbheader) == STATUS_ERROR) {
            printf("Failed to validate database header\n");
            return STATUS_ERROR;
        }
    }

    output_file(dbfd, dbheader);

    return STATUS_SUCCESS;
}
