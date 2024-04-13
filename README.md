# VSCodium Updater

## Overview
The goal of this project, as the name suggests, is to create an updater for VSCodium in C. What it doesn't tell is that it only targets Linux.

It may be unnecessary, but I'm doing it for learning purposes, and as you might expect, the program's
quality is beginner level.

## Features
This program will fetch the latest version and download it to a predetermined directory, or one of your choosing. 

It will create a bash script that you can add to your other local scripts to more easily open VSCodium.

## Contributing & Others
Feel free to suggest changes or improvements, naturally, respectfully.


## Building & Usage
I used libarchive, curls and will call some of your system's (very basic) utils.
To build:
```
gcc -o FOO vscu.c -larchve -lcurl
```
To use it, I hope my simple built-in help page will be enough. To see it, run the program without any arguments, or use ```-h``` or ```--help```