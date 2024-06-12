#include <stdio.h>
#include <stdlib.h>
#include "warcaby.h"

#define ROZMIAR_PLANSZY 8

int main()
{
    char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY];
    char gracz = 'X'; // Początkowy gracz
    int x1, y1, x2, y2;

    inicjalizujPlansze(plansza);
    wyswietlPlansze(plansza);

    for (int i = 0; i < 5; i++) {
        printf("Ruch gracza %c. Podaj ruch w formacie 'x1 y1 x2 y2' (koordynaty od 1 do 8): ", gracz);
        scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
        wykonajRuch(plansza, x1 - 1, y1 - 1, x2 - 1, y2 - 1, &gracz);
        wyswietlPlansze(plansza);
    }
    return 0;
}