if [ "$1" = "verbose" ] 
then
    gcc -Wall -fpie -D DEBUG -g -o ./bin/loader ./src/loader.c 
else 
    gcc -Wall -fpie -g -o ./bin/loader ./src/loader.c 
fi