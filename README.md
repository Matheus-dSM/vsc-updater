# VSCodium Updater

## Overview
The goal of this project, as the name suggests, is to create an updater for VSCodium in C. What it doesn't tell is that it only targets Linux.

It may be unnecessary, but I'm doing it for learning purposes, and as you might expect, the program's quality is beginner level.

## Features
This program will fetch the latest version and download it to a predetermined directory, or one of your choosing. 

It will create a bash script that you can add to where you keep your other local scripts to more easily open VSCodium.

## Contributing & Others
Feel free to suggest changes or improvements, naturally, respectfully. If possible, I'd prefer to receive suggestions rather than big pull requests.

## Building & Usage
I used libarchive, curls and will call some of your system's (very basic) utils.
To build:
```
gcc -o FOO vscu.c -larchve -lcurl
```
To use it, I hope my simple built-in help page will be enough. To see it, run the program without any arguments, or use ```-h``` or ```--help```

To delete what was downloaded, you'll only need to either delete the folder "VSC-Updater" or if you chose to have it be hidden, ".VSC-Updater". If you picked a custom folder, deleting it or the VSCodium-*version* folder will do. Don't forget to remove the bash script if you chose to move it somewhere else.

I believe it will be unlikely that you'll need to delete it, but a folder named vscu will also be created on /tmp
