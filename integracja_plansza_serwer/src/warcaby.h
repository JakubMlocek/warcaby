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
                else if (i > ROZMIAR_PLANSZY - 4)
                    plansza[i][j] = 'O'; // Pionki gracza 2
                else
                    plansza[i][j] = ' '; // Puste pole
            }
        }
    }
}


void wyswietlPlansze(char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY]) {
    // Wyświetlenie nagłówka z etykietami kolumn
    printf("   ");
    for (int k = 0; k < ROZMIAR_PLANSZY; k++) {
        printf("  y%d ", k + 1);
    }
    printf("\n");

    // Wyświetlenie linii oddzielającej nagłówek od planszy
    printf("   +");
    for (int k = 0; k < ROZMIAR_PLANSZY; k++) {
        printf("----+");
    }
    printf("\n");

    for (int i = 0; i < ROZMIAR_PLANSZY; i++) {
        // Wyświetlenie etykiety wiersza na początku każdego wiersza
        printf("x%d |", i + 1);
        for (int j = 0; j < ROZMIAR_PLANSZY; j++) {
            printf(" %c  |", plansza[i][j]);
        }
        printf("\n");
        // Wyświetlenie linii oddzielającej wiersze
        printf("   +");
        for (int k = 0; k < ROZMIAR_PLANSZY; k++) {
            printf("----+");
        }
        printf("\n");
    }
}

void wykonajRuch(char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY], int x1, int y1, int x2, int y2, char* gracz) {
    if (plansza[x1][y1] != *gracz) {
        printf("To nie jest twoj pionek!\n");
        return;
    }

    if (plansza[x2][y2] != ' ') {
        printf("Ruch na zajęte pole!\n");
        return;
    }

    if ((*gracz == 'X' && x2 <= x1) || (*gracz == 'O' && x2 >= x1)) {
        printf("Nieprawidłowy kierunek ruchu!\n");
        return;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if (dx == 1 && dy == 1) {
        plansza[x2][y2] = *gracz;
        plansza[x1][y1] = ' ';
        *gracz = (*gracz == 'X') ? 'O' : 'X'; // Zmiana gracza
    } else if (dx == 2 && dy == 2 && plansza[(x1 + x2) / 2][(y1 + y2) / 2] != ' ' && plansza[(x1 + x2) / 2][(y1 + y2) / 2] != *gracz) {
        plansza[x2][y2] = *gracz;
        plansza[x1][y1] = ' ';
        plansza[(x1 + x2) / 2][(y1 + y2) / 2] = ' '; // Usunięcie piona przeciwnika
        *gracz = (*gracz == 'X') ? 'O' : 'X'; // Zmiana gracza
    } else {
        printf("Nieprawidłowy ruch!\n");
    }
}