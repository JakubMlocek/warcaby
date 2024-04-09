#include <stdio.h>
#include <stdlib.h>

#define ROZMIAR_PLANSZY 8

void inicjalizujPlansze(char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY]) {
    for (int i = 0; i < ROZMIAR_PLANSZY; i++) {
        for (int j = 0; j < ROZMIAR_PLANSZY; j++) {
            if ((i + j) % 2 == 0) 
                plansza[i][j] = ' '; // Puste pole
            else {
                if (i < 3) 
                    plansza[i][j] = 'X'; // Pionki gracza 1
                else if (i > 4) 
                    plansza[i][j] = 'O'; // Pionki gracza 2
                else 
                    plansza[i][j] = ' '; // Puste pole
            }
        }
    }
}

void wyswietlPlansze(char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY]) {
    // Wyświetlenie nagłówka z numerami kolumn
    printf("  ");
    for (int k = 0; k < ROZMIAR_PLANSZY; k++) {
        printf("%d ", k + 1);
    }
    printf("\n");

    for (int i = 0; i < ROZMIAR_PLANSZY; i++) {
        // Wyświetlenie numeru wiersza na początku każdego wiersza
        printf("%d ", i + 1);
        for (int j = 0; j < ROZMIAR_PLANSZY; j++) {
            printf("%c ", plansza[i][j]);
        }
        printf("\n");
    }
}


void wykonajRuch(char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY], int x1, int y1, int x2, int y2) {
    char gracz = plansza[x1][y1];
    // Sprawdzenie, czy ruch jest na puste pole
    if (plansza[x2][y2] != ' ') {
        printf("Ruch na zajęte pole!\n");
        return;
    }

    // Sprawdzenie kierunku ruchu dla 'X' i 'O'
    if ((gracz == 'X' && x2 <= x1) || (gracz == 'O' && x2 >= x1)) {
        printf("Nieprawidłowy kierunek ruchu!\n");
        return;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    // Ruch o jedno pole po przekątnej
    if (dx == 1 && dy == 1) {
        plansza[x2][y2] = gracz;
        plansza[x1][y1] = ' ';
    } 
    // Bicie (przeskok o dwa pola po przekątnej z usunięciem piona przeciwnika)
    else if (dx == 2 && dy == 2 && plansza[(x1 + x2) / 2][(y1 + y2) / 2] != ' ' && plansza[(x1 + x2) / 2][(y1 + y2) / 2] != gracz) {
        plansza[x2][y2] = gracz;
        plansza[x1][y1] = ' ';
        plansza[(x1 + x2) / 2][(y1 + y2) / 2] = ' '; // Usunięcie piona przeciwnika
    } else {
        printf("Nieprawidłowy ruch!\n");
    }
}


int main() {
    char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY];
    int x1, y1, x2, y2;

    inicjalizujPlansze(plansza);
    wyswietlPlansze(plansza);

    // Prosty cykl gry dla 2 ruchów (dla przykładu)
    for (int i = 0; i < 2; i++) {
        printf("Podaj ruch w formacie 'x1 y1 x2 y2' (koordynaty od 1 do 8): ");
        scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
        // Dostosowanie indeksów do zerowego indeksowania tablicy
        wykonajRuch(plansza, x1 - 1, y1 - 1, x2 - 1, y2 - 1);
        wyswietlPlansze(plansza);
    }

    return 0;
}
