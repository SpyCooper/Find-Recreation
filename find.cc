#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>

using namespace std;

// enum to check if the file is a regular file, directory, or link
enum file_type
{
    REGULAR,
    DIRECTORY,
    LINK,
};

// enum to check if the size is in characters, kibibytes, or bytes
enum size_type
{
    CHARACTERS,
    KIBIBYTES,
    BYTES,
};

// enum to check if the size is greater than, less than, or equal to the amount
enum size_sign
{
    PLUS,
    MINUS,
    EQUAL,
};

// function prototypes
void find(const char *name, const char *pattern, bool first_run = false);
bool check_file(const struct dirent *dirp, const struct stat &statbuf);

// find filter variables
bool name_flag = false;
string name_pattern;
bool type_flag = false;
file_type type;
bool size_flag = false;
size_type size_type;
int size_amount;
size_sign size_check;


// main function
int main(int argc, char *argv[])
{
    // check if the number of arguments is less than 2
    if (argc < 2)
    {
        cerr << "Not enough arguments\n";
        exit(-1);
    }

    // check which flags are set
    for (int i = 2; i < argc; i++)
    {
        // check if the name flag is set
        if (strcmp(argv[i], "-name") == 0) 
        {
            // checks to make sure there was a name argument
            if (i + 1 >= argc) 
            {
                cerr << "-name requires argument\n";
                exit(-1);
            }
            else
            {
                name_flag = true;
                name_pattern = argv[i + 1];
                // increment i to skip the name argument
                i++;
            }
        }
        // check if the type flag is set
        else if (strcmp(argv[i], "-type") == 0) 
        {
            // checks to make sure there was a type argument
            if (i + 1 >= argc) 
            {
                cerr << "-type requires argument\n";
                exit(-1);
            }
            else
            {
                type_flag = true;
                string type_arg = argv[i + 1];

                // check if the type argument is valid
                if (type_arg != "f" && type_arg != "d" && type_arg != "l") 
                {
                    cerr << "Unknown type: " << type_arg << endl;
                    exit(-1);
                }
                // set the type based on the valid argument
                else if (type_arg == "f") 
                {
                    type = REGULAR;
                }
                else if (type_arg == "d") 
                {
                    type = DIRECTORY;
                }
                else if (type_arg == "l") 
                {
                    type = LINK;
                }
                // increment i to skip the type argument
                i++;
            }
        } 
        // check if the size flag is set
        else if (strcmp(argv[i], "-size") == 0) 
        {
            // checks to make sure there was a size argument
            if (i + 1 >= argc) 
            {
                cerr << "-size requires argument\n";
                exit(-1);
            }
            else
            {
                size_flag = true;
                string size_arg = argv[i + 1];

                // set the comparison type based on the first character of the size argument
                if (size_arg[0] == '+') 
                {
                    size_check = PLUS;
                } 
                else if (size_arg[0] == '-') 
                {
                    size_check = MINUS;
                } 
                else 
                {
                    // no sign was entered for the number, so it can assumed that it has to be equal
                    size_check = EQUAL;
                }

                // set the size starting index based on the comparison type
                size_t start_index = (size_check == EQUAL) ? 0 : 1;

                // check the last character of the size argument to determine the size type
                if (size_arg[size_arg.size() - 1] == 'c') 
                {
                    size_type = CHARACTERS;
                    size_amount = atoi(size_arg.substr(start_index, size_arg.size() - 1 - start_index).c_str());
                } 
                else if (size_arg[size_arg.size() - 1] == 'k') 
                {
                    size_type = KIBIBYTES;
                    size_amount = atoi(size_arg.substr(start_index, size_arg.size() - 1 - start_index).c_str());
                } 
                else 
                {
                    // no size type was entered, so it can be assumed that it is in bytes
                    size_type = BYTES;
                    size_amount = atoi(size_arg.substr(start_index).c_str());
                }

                // increment i to skip the size argument
                i++;
            }
        } 
        // if there was a filter that was entered that isn't valid, print an error message
        else if (argv[i][0] == '-') 
        {
            cerr << "Unknown filter: " << argv[i] << endl;
            exit(-1);
        }
    }

    // call the find function (set the first_run flag to true)
    find(argv[1], argv[2], true);

    // exit the program
    exit(0);
}

// find function
void find(const char *name, const char *pattern, bool first_run)
{
    DIR *dp;
    struct dirent *dirp;
    struct stat statbuf;

    // check if the directory can be opened
    if ((dp = opendir(name)) == NULL)
    {
        cerr << "Cannot open directory " << name << endl;
        return;
    }

    // I'm not sure but this is the only to get my program to check the root directory
    if (first_run)
    {
        // check if the file is the one we want
        if (lstat(name, &statbuf) == -1)
        {
            cerr << "Cannot stat " << name << endl;
            return;
        }

        // check if the file is the one we want 
        if (check_file(NULL, statbuf))
        {
            cout << name << endl;
        }
    }

    // loop through all the files in the directory
    while ((dirp = readdir(dp)) != NULL)
    {
        // check if the file is the one we want
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
        {
            continue;
        }

        // get the full path of the file
        string full_path = string(name) + "/" + dirp->d_name;

        // check if the file can be stat'd
        if (lstat(full_path.c_str(), &statbuf) == -1)
        {
            cerr << "Cannot stat " << full_path << endl;
            continue;
        }

        // check if the file is the one we want
        if (check_file(dirp, statbuf))
        {
            cout << full_path << endl;
        }

        // check if the file is a directory
        if (S_ISDIR(statbuf.st_mode))
        {
            // recursively call the find function to the next directory (set the first_run flag to false)
            find(full_path.c_str(), pattern);
        }

    }

    // check if the directory can be closed
    if (closedir(dp) == -1)
    {
        perror("closedir");
    }
}

// check_file function
bool check_file(const struct dirent *dirp, const struct stat &statbuf)
{
    // check if the name flag is set and the file name doesn't match the pattern
    if (name_flag && (dirp == NULL || strcmp(dirp->d_name, name_pattern.c_str()) != 0))
    {
        return false;
    }

    // check if the type flag is set and the file type doesn't match the type we want
    if (type_flag)
    {
        if (type == REGULAR && !S_ISREG(statbuf.st_mode))
        {
            return false;
        }
        else if (type == DIRECTORY && !S_ISDIR(statbuf.st_mode))
        {
            return false;
        }
        else if (type == LINK && !S_ISLNK(statbuf.st_mode))
        {
            return false;
        }
    }

    // check if the size flag is set and the file size doesn't match the size we want
    if (size_flag)
    {
        // get the size we want to check
        int size_to_check = 0;
        if (size_type == CHARACTERS)
        {
            size_to_check = statbuf.st_size;
        }
        else if (size_type == KIBIBYTES)
        {
            // 1 kibibyte = 1024 bytes
            size_to_check = statbuf.st_size / 1024;
        }
        else if (size_type == BYTES)
        {
            // 1 block = 512 bytes
            size_to_check = statbuf.st_size / 512;
        }

        // check if the size is the one we want
        if (size_check == PLUS && size_to_check <= size_amount)
        {
            return false;
        }
        else if (size_check == MINUS && size_to_check >= size_amount)
        {
            return false;
        }
        else if (size_check == EQUAL && size_to_check != size_amount)
        {
            return false;
        }
    }

    // if all the checks pass, return true
    return true;
}
