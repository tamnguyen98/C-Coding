# About
This is assignment one for WSU-V CS 360. The purpose of this program is to
> write a program (in C) targeted at the Linux platform which reads words from one or more files, and prints out a list of the most frequently occurring sequential pairs of words and the number of times they occurred, in decreasing order of occurrence.

Words are defined by the getNextWord() function in getWord.c file, or
> Words are defined to be separated by whitespace and start 
 with an alphabetic character.  All alphabetic characters  
translated to lower case and punctuation is removed.

The following files were provide prior to starting the assignment:
- getWord.*
- crc64.*
- /Dataset/*
- /lib/

### Features

- Read external files passed in through arguments
- Dynamic hashtable (with a grow factor of >.75)
- Compute most occuring word pairs in a group text (or single text) and display it in decending order.

### How to use
Navigate to the the main directory /Word-Pairs-Hashing/ and run the following commands
> make

> ./wordpairs <-count> fileName1   fileName1 fileName3 ...

Where: count is the integer number of word pairs to print out and fileNameN arepathnames from which to read words. If no count argument is specified, ALL wordspairs are printed to stdout.
#### Example
> make

> ./wordpairs -3 Dataset/mobydick.txt
