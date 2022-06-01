# System zarządzania dostępem oparty o karty mifare
Oparty o ESP32 oraz moduł rfid RC522

## Todo
    [ ] napisać funkcje
    [ ] musze ogarnąć tabelki
    [ ] trzeba ogarnąć eprom żeby zapisywał stany
    [x] stronka ma być nie responsywna - wszystko bd rilołdowane bo nie chce się bawić z javom
    [ ] dorzucić csska

## Stages of grief
    1. Wejście w tryb programowania (dodawania kart do dozwolonych)
    2. Wyjście z tego trybu (urządzenie samo wyjdzie po każdej dodanej karcie)
    3. Otwarcie drzwi jednorazowo
    4. Otwarcie drzwi na stałe
    5. Wyłączenie otwarcia na stałe
    6. Zmiana nazwy i hasła sieci (to nie na stronie) 
    7. Programowanie czasu otwarcia drzwi (na ile się mają otwierać po zbliżeniu karty) i tryb bistabilny
    8. Listowanie wszystkich dodanych kart
    9. Usuwanie kart 