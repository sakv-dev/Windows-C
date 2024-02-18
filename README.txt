# plist.exe
## A home made pslist.exe program
### How to run
1. First, download and install _[msbuild](https://github.com/dotnet/msbuild/)_
2. In _Developer Powershel_ for VS 2022 run `cl` :
````cl .\main.c````
3. Run it :
```.\main.exe``` 

### --Help
````
Usage: C:\path\to\main.exe [options] [name]

Options:
 -h                     Display this help message and exit.
 -d                     Show thread detail.
 name                   Show information about processes that begin with the name specified.

Abbreviation key:
 PID                    Process Identifier
 Pri                    Priority
 Thd                    Number of Threads
 Hnd                    Number of Handles
 CPU Time               CPU Time in seconds
 Elapsed Time           Elapsed Time in seconds
````