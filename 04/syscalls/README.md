## Zadanie 1 (`topuser`)
Zaimplementować wywołanie systemowe o numerze 341. Zwraca ono (przez parametr) ID użytkownika, który ma w danej chwili uruchomionych najwięcej procesów w systemie.
```
long sys_topuser(uid_t *);
```
## Zadanie 2 (`timeuser`)
Zaimplementować wywołanie systemowe o numerze 342. Zwraca ono (przez parametr) UID użytkownika, którego uruchomione obecnie procesy zajęły najwięcej czasu procesora do tej pory.
```
long sys_timeuser(uid_t *);
```
## Zadanie 3 (`kernelps`)
Zaimplementować wywołanie systemowe o numerze 343. Zwraca ono (przez drugi parametr) listę wszystkich procesów w systemie (w formie tablicy stringów o stałej długości), a przez pierwszy – ilość procesów wpisanych w tablicę. Użytkownik jest odpowiedzialny za alokację pamięci. Jeżeli drugi parametr jest równy NULL, to wywołanie powinno zwracać w pierwszym parametrze rozmiar tablicy (ilości wpisów) do alokacji. Jeżeli drugi parametr nie jest równy NULL to pierwszy musi odpowiadać wielkości tablicy (ilości możliwych wpisów). Jeżeli przekazany bufor jest za mały na wszystkie wpisy, to zwracany jest błąd.

Implementacja najprostszej tablicy stringów to tablica wskaźników do stringów, w której ostatni element wskazuje na NULL.
```
long sys_kernelps(size_t *, char *);
```
## Zadanie 4 (`freeblocks`)
Zaimplementować wywołanie systemowe o numerze 344. Zwraca ono (przez parametr) ilość wolnych bloków na danym zamontowanym systemie plików (po podaniu ścieżki do pliku znajdującego się w tym systemie).
```
long sys_freeblocks(char *, u64 *);
```
## Zadanie 5 (`pidtoname`)
Zaimplementować wywołanie systemowe o numerze 345. Zwraca ono (przez parametr) nazwę procesu o podanym PID.
```
long sys_pidtoname(pid_t, char *);
```