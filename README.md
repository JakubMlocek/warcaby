# Warcaby - Beniamin Jankowski

## Warcaby - szczególne zasady gry, które zostały zaimplementowane

**Cel gry**: zbić wszystkie pionki przeciwniki
**Liczba graczy:** 2
**Zasady**:

Gracze rywalizują na planszy 8x8 pól, gdzie pionki mogą poruszać się jedynie na skos o jedno pole do przodu. Zbicie pionka przeciwnika polega na przeskoczeniu go, gdy ten sąsiaduje po przekątnej. Warunkiem zbicie jest wolne miejsce po przekątnej za potencjalnym zbitym pionkiem przeciwnika. Możliwe jest bicie wielkorotne wielu pionków zgodnie z poprzednim opisem. Można bić zarówno do tyłu jak i do przodu.

W przypadku, gdy ostatnią pozycją pionka jest ostatni rząd wg gracza (najbliższej przeciwnikia), pionek zamienia się w damkę. Jest on uprawniona o przesuwanie się o dowolnie dużą liczbą pól po skosie do przodu, jak i do tyłu w zakresie planszy. W przypadku próby zbicia, damka stawia się o jedno miejsce za zbitym pionkiem.

Wygrywa gracz, który pierwszy zbije wszystkie pionki przeciwnika

## Rozwiązanie implementacyjne



