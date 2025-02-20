# Find-Recreation

A basic recreation of the `find` UNIX command in a UNIX file system.

## Description

This was a program made for my cybersecurity class that recrates some functionality of the `find` UNIX command.

The following filters are implemented in the program:

- `-name filename`: finds and print all files with the exact filename
- `-type filetype`: finds all files with the file type. The filetype can be `f` to find a regular file, `d` to find a directory, or `l` to find a symbolic link. (example: `-type f` will find all normal files)
- `-size sizespec`: finds and prints all files that meet the file size requirement of `sizespec`. `sizespec` can be have a `+` or a `-` to find files that are greater or less than the `sizespec`. If neither are inputted, it will find files with the exact `sizespec`. `sizespec` can end in a `c` to find specific character size, `k` to have a size in kibibytes (1024 bytes), or just bytes if neither are put at the end. (example: `-size +100c` will find all files with more than 100 characters)

## Folder Structure

The most of the files are not in files, to make the `MakeFile` easier, but there are a few.

- `testdir.tar.gz`: a compressed folder that contains the environment the tests run in
- `testdir`: a folder that is created when `make test` is the uncompressed version of `testdir.tar.gz`

## Execution

This program will only work in a UNIX environment. After `find.cc` is compiled, you can run the program with a variation of the following format:

```./find directory [filters]```

The `MakeFile` can run any of the following commands:

- `make`: compiles `find.cc` into `find`
- `make test`: runs all tests shell scripts and outputs the correct results
- `make clean`: removes all test results from `make test`
- `make spotless`: removes all test results from `make test`, removes the `testdir` directory, and all files inside it

Each of the individual test can be run by running the shell script with a compiled version of find.cc (example: `./test.e1`)

## Credit

All files that are not `find.cc` or the `README.ME` were made by Shawn Ostermann. They are there to for future use if the `find.cc` needs to be reran.

All of `find.cc` was made by me.