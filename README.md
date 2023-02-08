
# ELF loader, wywołania standardowe dla wykonywania pliku występują tylko dla małego programu, który mapuje w pamięci inny plik ELF 
Celem jest wykonanie pliku ELF bez użycia standardowych wywołań, które do tego służą 
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
Jest to dokładnie zachowanie, którego oczekujemy, ponieważ target.c wygląda następująco:
```c
#include <math.h>

int printf ( const char * format, ... );

int absabs(int a) { 
    if (a < 0) { 

## strace bez skryptu uwidaczniający syscalle [```./bin/loader ./bin/target```]  
Jak widać syscalle tożsame z wywoływaniem czegokolwiek występują tylko dla loadera, który z punktu widzenia działania tylko odczytuje bajty oraz alokuje pamięć

```
execve("./bin/loader", ["./bin/loader", "./bin/target"], 0x7ffe3225df38 /* 75 vars */) = 0
brk(NULL)                               = 0x1271000
arch_prctl(0x3001 /* ARCH_??? */, 0x7ffe070c28a0) = -1 EINVAL (Invalid argument)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=121519, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 121519, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f05d8f55000
close(3)                                = 0
openat(AT_FDCWD, "/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\320\226\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0@\0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0"..., 80, 848) = 80
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\202W\356\220vF\351\260W\31u3\321\344\254\216"..., 68, 928) = 68
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2375624, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f05d8f53000
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2104720, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f05d8c00000
mmap(0x7f05d8c28000, 1523712, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7f05d8c28000
mmap(0x7f05d8d9c000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x19c000) = 0x7f05d8d9c000
mmap(0x7f05d8df4000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1f3000) = 0x7f05d8df4000
mmap(0x7f05d8dfa000, 32144, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f05d8dfa000
close(3)                                = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f05d8f51000
arch_prctl(ARCH_SET_FS, 0x7f05d8f54600) = 0
set_tid_address(0x7f05d8f548d0)         = 30298
set_robust_list(0x7f05d8f548e0, 24)     = 0
rseq(0x7f05d8f54fa0, 0x20, 0, 0x53053053) = 0
mprotect(0x7f05d8df4000, 16384, PROT_READ) = 0
mprotect(0x403000, 4096, PROT_READ)     = 0
mprotect(0x7f05d8fa8000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7f05d8f55000, 121519)          = 0
getrandom("\xcd\xd5\x3f\x47\x27\xa4\xd7\x8e", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x1271000
brk(0x1292000)                          = 0x1292000
openat(AT_FDCWD, "./bin/target", O_RDONLY) = 3
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=22072, ...}, AT_EMPTY_PATH) = 0
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=22072, ...}, AT_EMPTY_PATH) = 0
lseek(3, 20480, SEEK_SET)               = 20480
read(3, "\2\0\0\0\0\0\0\0\210\4\0\0\0\0\0\0\210\4\0\0\0\0\0\0\217\0\0\0\0\0\0\0"..., 1592) = 1592
lseek(3, 0, SEEK_SET)                   = 0
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0P\20\0\0\0\0\0\0"..., 20480) = 20480
read(3, "\2\0\0\0\0\0\0\0\210\4\0\0\0\0\0\0\210\4\0\0\0\0\0\0\217\0\0\0\0\0\0\0"..., 4096) = 1592
close(3)                                = 0
mmap(NULL, 22072, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f05d8f6d000
mprotect(0x7f05d8f6d000, 1608, PROT_READ|PROT_EXEC) = 0
mprotect(0x7f05d8f6e000, 461, PROT_READ|PROT_EXEC) = 0
mprotect(0x7f05d8f6f000, 260, PROT_READ|PROT_EXEC) = 0
mprotect(0x7f05d8f70de0, 592, PROT_READ|PROT_EXEC) = -1 EINVAL (Invalid argument)
newfstatat(1, "", {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x4), ...}, AT_EMPTY_PATH) = 0
write(1, "hello from other elf\n", 21hello from other elf
)  = 21
exit_group(0)                           = ?
+++ exited with 0 +++
```
    return a;
}

int main(int argc, char** argv) {
    absabs(-10);
    double k = sin(343.3);
    const char* aa = "hello from other elf\n"
```
## strace bez skryptu uwidaczniający syscalle [```./bin/loader ./bin/target```]  
Jak widać syscalle tożsame z wywoływaniem czegokolwiek występują tylko dla loadera, który z punktu widzenia działania tylko odczytuje bajty oraz alokuje pamięć

```
execve("./bin/loader", ["./bin/loader", "./bin/target"], 0x7ffe3225df38 /* 75 vars */) = 0
brk(NULL)                               = 0x1271000
arch_prctl(0x3001 /* ARCH_??? */, 0x7ffe070c28a0) = -1 EINVAL (Invalid argument)
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
newfstatat(3, "", {st_mode=S_IFREG|0644, st_size=121519, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 121519, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f05d8f55000
close(3)                                = 0
openat(AT_FDCWD, "/lib64/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\320\226\2\0\0\0\0\0"..., 832) = 832
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
pread64(3, "\4\0\0\0@\0\0\0\5\0\0\0GNU\0\2\0\0\300\4\0\0\0\3\0\0\0\0\0\0\0"..., 80, 848) = 80
pread64(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0\202W\356\220vF\351\260W\31u3\321\344\254\216"..., 68, 928) = 68
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=2375624, ...}, AT_EMPTY_PATH) = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f05d8f53000
pread64(3, "\6\0\0\0\4\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0@\0\0\0\0\0\0\0"..., 784, 64) = 784
mmap(NULL, 2104720, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7f05d8c00000
mmap(0x7f05d8c28000, 1523712, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7f05d8c28000
mmap(0x7f05d8d9c000, 360448, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x19c000) = 0x7f05d8d9c000
mmap(0x7f05d8df4000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1f3000) = 0x7f05d8df4000
mmap(0x7f05d8dfa000, 32144, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7f05d8dfa000
close(3)                                = 0
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f05d8f51000
arch_prctl(ARCH_SET_FS, 0x7f05d8f54600) = 0
set_tid_address(0x7f05d8f548d0)         = 30298
set_robust_list(0x7f05d8f548e0, 24)     = 0
rseq(0x7f05d8f54fa0, 0x20, 0, 0x53053053) = 0
mprotect(0x7f05d8df4000, 16384, PROT_READ) = 0
mprotect(0x403000, 4096, PROT_READ)     = 0
mprotect(0x7f05d8fa8000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7f05d8f55000, 121519)          = 0
getrandom("\xcd\xd5\x3f\x47\x27\xa4\xd7\x8e", 8, GRND_NONBLOCK) = 8
brk(NULL)                               = 0x1271000
brk(0x1292000)                          = 0x1292000
openat(AT_FDCWD, "./bin/target", O_RDONLY) = 3
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=22072, ...}, AT_EMPTY_PATH) = 0
newfstatat(3, "", {st_mode=S_IFREG|0755, st_size=22072, ...}, AT_EMPTY_PATH) = 0
lseek(3, 20480, SEEK_SET)               = 20480
read(3, "\2\0\0\0\0\0\0\0\210\4\0\0\0\0\0\0\210\4\0\0\0\0\0\0\217\0\0\0\0\0\0\0"..., 1592) = 1592
lseek(3, 0, SEEK_SET)                   = 0
read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0P\20\0\0\0\0\0\0"..., 20480) = 20480
read(3, "\2\0\0\0\0\0\0\0\210\4\0\0\0\0\0\0\210\4\0\0\0\0\0\0\217\0\0\0\0\0\0\0"..., 4096) = 1592
close(3)                                = 0
mmap(NULL, 22072, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f05d8f6d000
mprotect(0x7f05d8f6d000, 1608, PROT_READ|PROT_EXEC) = 0
mprotect(0x7f05d8f6e000, 461, PROT_READ|PROT_EXEC) = 0
mprotect(0x7f05d8f6f000, 260, PROT_READ|PROT_EXEC) = 0
mprotect(0x7f05d8f70de0, 592, PROT_READ|PROT_EXEC) = -1 EINVAL (Invalid argument)
newfstatat(1, "", {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0x4), ...}, AT_EMPTY_PATH) = 0
write(1, "hello from other elf\n", 21hello from other elf
)  = 21
exit_group(0)                           = ?
+++ exited with 0 +++
```
