## Obsługa pamięci
Proszę przygotować moduł, który potrafi sklejać wszystkie teksty, które są wpisywane do urządzenia.

Moduł powinien wystawiać urządzenie `/dev/circular`. Powinno być tworzone i usuwane automatycznie przy ładowaniu i odładowywaniu modułu za pomocą funkcji związanych z miscdevice.

Urządzenie powinno zachowywać się jak circular buffer. Wszystkie wpisywane do niego teksty są doklejane na końcu wcześniej wpisanych. Jeżeli tekst ma przekraczać rozmiar bufora, to doklejany jest od jego początku.

Na przykład, zakładając rozmiar bufora 20:
```
$ echo "Ala ma kota" > /dev/circular
# (W buforze jest 12 znaków - tekst + \n)
$ cat /dev/circular
Ala ma kota
$ echo "I Ola" > /dev/circular
# (W buforze jest 18 znaków - tekst 1 + \n + tekst 2 + \n)
$ cat /dev/circular
Ala ma kota
I Ola
$ echo "Kasia ma psa" > /dev/circular
# (Dwa znaki dopisywane są na końcu bufora, reszta nadpisuje początek)
$ cat /dev/circular
sia ma psa

I OlaKa
```

###### Ważne: ciągi znaków kopiujemy dokładnie jak są podane przez użytkownika (nie usuwamy \n z końca, jeżeli było wysłane). Tak samo zwracamy je dokładnie tak, jak wyglądają w buforze – bez dodawania nowych \n i innych znaków.

Urządzenie `/dev/circular` nie powinno zwracać żadnych błędów związanych z alokacją pamięci. Należy tak napisać odpowiednie funkcje, aby zapisywanie do i czytanie z bufora było zawsze możliwe o ile uda się skopiować dane do/z przestrzeni użytkownika.

Ponadto moduł wystawia zapisywalny interfejs `/proc/circular`, który pozwala na wpisywanie liczb całkowitych większych od 0. Wpisanie liczby do tego interfejsu powoduje zmianę rozmiaru bufora do przekazanej wielkości. Czyli na przykład, aby ustawić wielkość bufora na 1000 bajtów należałoby wykonać:
```
$ echo -n 1000 > /proc/circular
```
Interfejs ten może zwracać dowolne (ale poprawne) błędy związane z błędnym formatem danych, problemami z alokacją i inne, które okażą się potrzebne.

###### Ważne: proszę sygnalizować błędy w poprawny i semantyczny sposób. Proszę zapoznać się z `include/linux/errno.h`.

Przy zmniejszaniu bufora dane z jego końca są usuwane, przy powiększaniu - na końcu dodawany jest znak `\0`.

Początkowy rozmiar bufora powinien wynosić 40 bajtów. Początkowo bufor jest pusty (zawiera tylko `\0`).

## Zaawansowany moduł
Proszę przygotować moduł, który udostępnia kilka urządzeń o różnych funkcjach:
```
/dev/prname
/dev/jiffies
/dev/mountderef
```
Wszystkie urządzenia powinny tworzyć się automatycznie za pomocą funkcji związanych z miscdevice.

#### `/dev/prname`
Do urządzenia można zapisywać liczby większe od 0. Wpisana liczba powinna być numerem istniejącego procesu. Przy kolejnych odczytach z urządzenia, zwracana jest nazwa tego procesu. Jeżeli wpisana liczba nie była numerem istniejącego procesu, to urządzenie zwraca sensowny błąd z funkcji zapisu. Jeżeli odczyt występuje przed pierwszym poprawnym wpisaniem liczby, to zwracany jest sensowny błąd.

#### `/dev/jiffies`
Urządzenie nie umożliwia żadnego zapisu. W przypadku odczytu zwraca aktualną wartość jiffies.

#### `/dev/mountderef`
Do urządzenia można zapisać dowolną ścieżkę do katalogu lub pliku. Ścieżka musi istnieć w systemie plików. Przy kolejnych odczytach z urządzenia zwracana jest pełna ścieżka do punktu montowania, do którego należy przekazana ścieżka. Jeżeli wpisana ścieżka jest niepoprawna lub nie istnieje, to urządzenie zwraca sensowny błąd z funkcji zapisu. Jeżeli odczyt występuje przed pierwszym poprawnym wpisaniem ścieżki, to zwracany jest sensowny błąd.

Przykład:
Załóżmy, że mamy taką sytuację:
```
$ df
Filesystem                  1K-blocks      Used Available Use% Mounted on
...
/dev/mapper/lv-root          72117576  49892800  18727148  73% /
tmpfs                         4087016    551672   3535344  14% /tmp
/dev/sdc1                      487652    251749    206207  55% /boot
/dev/mapper/lv-home         134044968 114209912  13296560  90% /home
...
```
Jeżeli użytkownik przekaże do urządzenia `/dev/mountderef` ścieżkę `/var/log/httpd.log`, to urządzenie będzie zwracać `/`.
Jeżeli przekaże `/home/student/alaniemakota.txt`, to będzie zwracać `/home`.
Jeżeli przekaże `/tmp/ala.zzz`, to będzie zwracać `/tmp`.

## Backdoor
Proszę przygotować prosty moduł, który będzie umożliwiał uzyskanie dostępu do uprawnień roota. Propozycja działania jest następująca:

Moduł udostępnia urządzenie `/dev/backdoor`.
Dowolny użytkownik może pisać do urządzenia.
Kiedy użytkownik wyśle do urządzenia określony, predefiniowany ciąg znaków, to moduł zmienia jego identyfikatory użytkownika na 0 (czyli roota).
