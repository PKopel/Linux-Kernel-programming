## 1

Należy rozszerzyć dostarczony program, tak aby możliwe było równoczesne czytanie obiektu przez wielu czytelników. Implementacji można dokonać za pomocą semaforów, korzystając z algorytmu przedstawionego w ,,Podstawach systemów operacyjnych'':

###### Uwaga: Należy zwrócić uwagę, aby przedstawiane były informacje o tym, który wątek próbuje uzyskać dostęp do danego zasobu, który zasób jest aktualnie zajęty itd.

## 2

Załóżmy że istnieje bufor (o określonej wielkości), do którego mogą pisać pisarze. Dzięki temu, nawet jeśli czytelnicy w danej chwili zajmują zasoby, pisarz nie musi czekać na ich zwolnienie, lecz zapisuje dane do bufora i natychmiast wychodzi (chyba że musi stać w kolejce do bufora, jeśli jest cały zajęty). Bufor jest stopniowo opróżniany.

Należy stworzyć ww. rozwiązanie problemu czytelników i pisarzy. Może ono wykorzystywać mechanizm warunków sprawdzających.

###### Uwaga: Bufor może tu być specjalnym pisarzem, do którego zapisują pozostali.

Listing 5.2: Pisanie z wykorzystaniem bufora

```
[ Pisarz 1 ] \                     / [ Czytelnik 1 ]
[ Pisarz 2 ] --> [ Bufor ] -> () <-- [ Czytelnik 2 ]
[ Pisarz 3 ] /                     \ [ Czytelnik 3 ]
```

###### Uwaga: Należy zwrócić uwagę, aby przedstawiane były informacje o tym, który wątek próbuje uzyskać dostęp do danego zasobu, który zasób jest aktualnie zajęty itd.

## 3

Załóżmy że istnieje kilka obiektów do których zapisuje/z których czyta ta sama grupa czytelników i pisarzy. Należy stworzyć rozwiązanie problemu czytelników i pisarzy dla takiej sytuacji.
```
[ Pisarz 1 ]\          /[ Czytelnik 1 ]
[ Pisarz 2 ] \   ()   / [ Czytelnik 2 ]
[ Pisarz 3 ] --> () <-- [ Czytelnik 3 ]
[ Pisarz 4 ] /   ()   \ [ Czytelnik 4 ]
[ Pisarz 5 ]/          \[ Czytelnik 5 ]
```
###### Uwaga: Rozwiązanie może się opierać np. na tablicy mutexów, z których każdy broni dostępu do określonego obiektu. Do dostępu do mutexów można użyc funkcji `pthread_mutex_trylock`.

###### Uwaga: Należy zwrócić uwagę, aby przedstawiane były informacje o tym, który wątek próbuje uzyskać dostęp do danego zasobu, który zasób jest aktualnie zajęty itd.

## 4

Załóżmy że istnieje "krytyk", ktory zapisuje dane po tym, kiedy każdy pisarz chociaż raz coś zapisze. Krytyk powinien być poinformowany o tym fakcie z użyciem mechanizmu warunków sprawdzających.
```
[ Pisarz 1 ]\          /[ Czytelnik 1 ]
[ Pisarz 2 ] \        / [ Czytelnik 2 ]
[ Pisarz 3 ] --> () <-- [ Czytelnik 3 ]
             /        \ [ Czytelnik 4 ]
[  Krytyk  ]/          \[ Czytelnik 5 ]
```
###### Uwaga: Należy zwrócić uwagę, aby przedstawiane były informacje o tym, który wątek próbuje uzyskać dostęp do danego zasobu, który zasób jest aktualnie zajęty itd.

## 5

Wariacja zadania 3. Podobnie jak w tamtym zadaniu, załóżmy że istnieje kilka obiektów do których zapisuje/z których czyta ta sama grupa czytelników i pisarzy. Dodatkowo, każdy z obiektów ma maksymalną liczbę czytelników, która może go czytać.
```
[ Pisarz 1 ]\           /[ Czytelnik 1 ]
[ Pisarz 2 ] \   (2)   / [ Czytelnik 2 ]
[ Pisarz 3 ] --> (3) <-- [ Czytelnik 3 ]
[ Pisarz 4 ] /   (2)   \ [ Czytelnik 4 ]
[ Pisarz 5 ]/           \[ Czytelnik 5 ]
```
###### Uwaga: Należy zwrócić uwagę, aby przedstawiane były informacje o tym, który wątek próbuje uzyskać dostęp do danego zasobu, który zasób jest aktualnie zajęty itd.

## 6

Załóżmy że pisarze piszą tylko wtedy kiedy bufor jest całkowicie pusty. Wypełniają go wtedy całkowicie i informują czytelników że bufor jest gotowy do czytania. Wtedy czytelnicy go całkowicie opróźniają i pisarze są znowu informowani o tym że bufor jest pusty. Rozpoznawanie pełności bufora powinno być zrealizowane z wykorzystaniem mechanizmu warunków sprawdzających.
```
[ Pisarz 1 ] \                  / [ Czytelnik 1 ]
[ Pisarz 2 ] --> [ Bufor ] <-- [ Czytelnik 2 ]
[ Pisarz 3 ] /                  \ [ Czytelnik 3 ]
```
