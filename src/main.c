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
    printf("\t -a   - the employee record in format \"Name,Address,Hours\"\n");
    return;
}

int main(int argc, char *argv[])
{
    bool newFile = false;
    bool listEmployees = false;
    char *filePath = NULL;
    char *addstring = NULL;
    int c;

    int dbfd = -1;

    struct dbheader_t *dbheader;
    struct employee_t *employees;

    while ((c = getopt(argc, argv, "nlf:a:")) != -1)
    {
        switch (c)
        {
        case 'n':
            newFile = true;
            break;
        case 'f':
            filePath = optarg;
            break;
        case 'a':
            addstring = optarg;
            break;
        case 'l':
            listEmployees = true;
            break;
        case '?':
            print_usage(argv);
            break;
        default:
            return STATUS_ERROR;
        }
    }

    if (filePath == NULL)
    {
        printf("Filepath is a required argument\n");
        print_usage(argv);
        return 0;
    }

    if (newFile)
    {
        dbfd = create_db_file(filePath);
        if (dbfd == STATUS_ERROR)
        {
            printf("Unable to create database file\n");
            return STATUS_ERROR;
        }

        if (create_db_header(&dbheader) == STATUS_ERROR)
        {
            printf("Failed to create database header\n");
            return STATUS_ERROR;
        }
    }
    else
    {
        dbfd = open_db_file(filePath);
        if (dbfd == STATUS_ERROR)
        {
            printf("Unable to open database file\n");
            return STATUS_ERROR;
        }

        if (validate_db_header(dbfd, &dbheader) == STATUS_ERROR)
        {
            printf("Failed to validate database header\n");
            return STATUS_ERROR;
        }
    }

    if (read_employees(dbfd, dbheader, &employees) == STATUS_ERROR)
    {
        printf("Failed to read employees from database\n");
        return STATUS_ERROR;
    }

    if (addstring != NULL)
    {
        add_employee(dbheader, &employees, addstring);
    }

    if (listEmployees)
    {
        list_employees(dbheader, employees);
    }

    output_file(dbfd, dbheader, employees);

    return STATUS_SUCCESS;
}
