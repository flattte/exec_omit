
# ELF loader, pomija wszystkie syscalle typu execv 
Celem jest wykonanie pliku ELF bez użycia standardowych wywołań, które do tego służą
## Get Started 🚀  
Plik ELF, który jest "targetem" to  
*./target/target.c*  
Loader, który odpowiada za pełne zmapowanie + dostęp do libc (na razie tylko libc)  
*./src/loader.c*  
#### Budowanie wszystkich plików, wywołanie targetu loaderem z feedbackiem co się dzieje
```sh
./run.sh verbose
```
#### Bez feedbacku (no interaction with stdout, stderr)
```sh
./run
```

## Output, który powinien się pojawić (verbose)
```
[INFO] :: File has been read into the buffer
[INFO] :: ELF validated.
[INFO] :: mmap done, resulting address of exec is: 0x7f7e1ce90000
[INFO]: resolved sym not in libc, is a function
[INFO]: resolved sym not in libc, is a function
[INFO]: resolved sym not in libc, is a function
[INFO] entry of mapped program localised at 0x7f7e1ce91152
[INFO] :: Run the loaded program:
----------------

hello from elf

----------------
[INFO] :: End of the loaded program: program returned status: 3048
```
Co jest pożądanym wynikiem, ponieważ target.c robi dokładnie to co zostało zaprezentowane:
```c
#include <math.h>

int printf ( const char * format, ... );

int absabs(int a) { 
    if (a < 0) { 
        return -a;
    }
    return a;
}

int main(int argc, char** argv) {
    absabs(-10);
    double k = sin(343.3);
    const char* aa = "hello from other elf\n";

    printf(aa);
    return absabs((int)(0.4  * k * 10000));
}
```

