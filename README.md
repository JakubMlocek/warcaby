﻿# Warcaby - Beniamin Jankowski, Jakub Młocek - CBZ III

# Link do repozytorium: https://github.com/JakubMlocek/warcaby

## Warcaby - zaimplementowane zasady gry

**Cel gry**: zbić wszystkie pionki przeciwnika

**Liczba graczy:** 2

**Zasady**:

Gracze rywalizują na planszy 8x8 pól, gdzie pionki mogą poruszać się jedynie na skos o jedno pole do przodu. Zbicie pionka przeciwnika polega na przeskoczeniu go, gdy ten sąsiaduje po przekątnej. Warunkiem zbicie jest wolne miejsce po przekątnej za potencjalnym zbitym pionkiem przeciwnika. Możliwe jest bicie wielkorotne wielu pionków zgodnie z poprzednim opisem. Można bić zarówno do tyłu jak i do przodu.

W przypadku, gdy ostatnią pozycją pionka jest ostatni rząd wg gracza (najbliższej przeciwnikia), pionek zamienia się w damkę. Jest on uprawniona o przesuwanie się o dowolnie dużą liczbą pól po skosie do przodu, jak i do tyłu w zakresie planszy. W przypadku próby zbicia, damka stawia się o jedno miejsce za zbitym pionkiem.

Wygrywa gracz, który pierwszy zbije wszystkie pionki przeciwnika

## Rozwiązanie implementacyjne

1. Serwer 

Serwer pośredniczy w wymianie danych przez graczy. Wpierw, serwer wysyła kod graczowi, który może być akceptowany przez gracza. W przypadku, gdy gracz wyślę inny kod, serwer szuka danej gry i dzięki kodowi, łączy się z nią. Gra nie rozpocznie się póki do gry nie dołączy dokładnie dwóch graczy.

Serwer losuje gracza zaczynającego oraz gra się rozpoczyna, gdzie serwer wysyła rozkład planszy obu graczom.

2. Klient

W przypadku ustalenia już gry, klientowi wyświetla się plansza oraz informacja o danym przebiegu gry (liczba zbitych pionków) oraz kogo jest teraz ruch. Serwer oczekuje zatem podania pionka oraz następnmych koordynatów ruchu pionka. 

Gdy klient zdecyduje się na ruch, wysyłany jest obecny stan planszy do serwera, gdzie ten przesyła ją dalej do drugiego gracza. Pierwszy gracz, który wysłał planszę widzi tak samo długo planszę wysłaną, póki nie uzyska od serwera (czyli drugiego gracza) nowej wraz ze zrobionym ruchem drugiego gracza.

Gra zatem toczy się na zasadzie:

1. Serwer wysyła równocześnie plansze początkową obu graczom oraz informację o graczu rozpoczynającym.
2. Gracz wysyła swój ruch na serwer, gdzie ten analizowany jest przez niego, tzn. wysyłany do drugiego gracza.
3. W sytuacji, gdy któryś z graczy wygra, serwer analizuje to oraz pokazuje wyniki/końcową planszę obu użytkownikom, po czym kończy obecną sesję gry.

## Rozwiązanie graficzne

Gra opiera się na bardzo prostym modelu graficznym, jako wyświetlanie znaków ASCII w polu terminala. Taka informacja w postaci listy jest też przekazywana w grze jako klient-serwer.

## Wymagania

Klienci (oraz serwer) muszą być w jednej sieci, gdzie nawiązują połączenie z serwerem już na starcie aplikacji `warcaby_klient`. Program `warcaby_serwer` działa *cały czas*, gdzie łączenie się z konkretną grą przez klientów odbywa się za pomocą podanego przez serwer (lub klienta) kodu.

## Aspekty techniczne

Serwer jest w stanie ciągłego nasłuchiwania, gdzie będzie on miał statyczny adres IP. W przypadku wymian klient-serwer będzie to komunikacja unicast za pomocą protokołu transportowego TCP. Dane wysyłane poprzez serwer będą w ustalonym formacie TLV, gdzie sam serwer jest sekwencyjny, czeka na opdowiedź klienta. Serwer ma działać nieprzerwanie (tak jak np. procesy deamon na systemach linux) jako ciągłe oczekiwanie na ruchy graczy lub na zaczęcie nowej gry. 
