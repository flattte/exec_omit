
# ELF loader, pomija wszystkie syscalle typu execv 
Celem jest wykonanie pliku ELF (64bit) bez użycia standardowych wywołań, które do tego służą  
Temat zadany w poleceniu projektu okazał dla mnie niespójny, więc zdecydowałem się na taka implementację  
W trakcie realizacji dogłebnie musiałem zrozumieć jak działają pliki wykonywalne więc było absolutnie warto  
## Zasada działania
Loader (na ten moment, można go zastąpić lepszą metodą i wykonywać arbitralny kod bez zapisu na dysku) odczytuje target ELF z pliku, waliduje go,nastepnie intepretuje go zgodnie z headerem foramtu ELF otrzymując w pamięcie prawidłowy obraz tego ELF'a (emulacja execv).  
Ta czynność składa się z kroków opisanych w kodzie źródłowym, i widocznych jako sam kod. Kilka ciekowaych rzeczy, które implementuje loader, to między innymi parsowanie symboli z target, odnajdywanie i relokowanie prawidłowych odniesień symboli do libc, lokalizowanie *main*, i wywołanie go jako funkcji.
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
[INFO] :: mmap done, resulting address of exec is: 0x7f8abb731000
[INFO] :: resolved sym not in libc, is a function
[INFO] :: resolved sym not in libc, is a function
[INFO] :: resolved sym not in libc, is a function
[INFO] :: entry of mapped progrma localised at 0x7f8abb732152
[INFO] :: Run the loaded program:
----------------

hello from other elf

----------------
[INFO] :: End of the loaded program: program returned status: 3048
```
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
    const char* aa = "hello from other elf\n";[INFO] :: File has been read into the buffer
[INFO] :: ELF validated.
[INFO] :: mmap done, resulting address of exec is: 0x7f8abb731000
[INFO] :: resolved sym not in libc, is a function
[INFO] :: resolved sym not in libc, is a function
[INFO] :: resolved sym not in libc, is a function
[INFO] :: entry of mapped progrma localised at 0x7f8abb732152
[INFO] :: Run the loaded program:
----------------

hello from other elf

----------------
[INFO] :: End of the loaded program: program returned status: 3048
```

