## Moduły
Do zadań dostarczone są dwa gotowe moduły.

##### `simple_sync` zawiera kod modułu jądra, który posiada następującą funkcjonalność:

korzysta z urządzenia `/dev/simple` (jeśli nie istnieje, należy je stworzyć za pomocą dostarczonego skryptu);
zwraca sekwencję łańcuchów znaków: `-0123456789-ABCDEFGHIJ-` (np. poprzez `cat /dev/simple`);
sekwencja znaków jest tworzona (z opóźnieniami) z wykorzystaniem licznika dostępnego dla całego modułu;
powyższy fakt powoduje że uruchomienie (np. w dwóch oknach terminala) czytania z urządzenia `/dev/simple` skutkuje zwracaniem niewłaściwej sekwencji znaków.

##### `linked_buffer` zawiera kod modułu jądra obsługujący dynamiczny bufor oparty o listę dowiązaną. Bufor ten:

korzysta z urządzenia `/dev/linked` (jeśli nie istnieje, należy je stworzyć za pomocą dostarczonego skryptu);
można do niego zapisać dowolną sekwencję znaków (np. za pomocą `echo "w szczebrzeszynie chrzaszcz brzmi w trzcinie" > /dev/linked`);
zwraca wcześniej zapisane ciągi znaków podczas czytania (np. poprzez `cat /dev/simple`);
wysłanie do niego sekwencji znaków `xxx&` powoduje wyczyszczenie bufora, np.: `echo -n 'xxx&' > /dev/linked`;
plik `/proc/linked` zwraca informacje o aktualnej długości tekstu w buforze.
Dane w tym buforze przechowywane są w częściach o równej wielkości (`char[]`), a te – na liście dowiązanej. Jeżeli na buforze wywołamy jednocześnie operacje zapisywania długich łańcuchów znaków, to dane zostaną wymieszane.

## Zadanie 1 (semafory)
Oba moduły zsynchronizuj za pomocą semaforów.

## Zadanie 2 (muteksy)
Oba moduły zsynchronizuj za pomocą muteksów.

## Zadanie 3 (spinlock)
Oba moduły zsynchronizuj za pomocą spinlocków.

## Zadanie 4 (RCU)
Moduł `linked_buffer` zsynchronizuj za pomocą RCU.