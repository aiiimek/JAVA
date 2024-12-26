#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct {
    char model[50];
    int Moc;
    char Wydajnosc[15];
    double Cena;
} Zasilacz;

FILE* dbFile = NULL;
char currentDbName[100] = "";

void menu();
void otworzBazeDanych();
void stworzBazeDanych();
void przegladBazy();
void dodajWpis();
void modyfikujWpis();
void usunWpis();
void sortujBaze(FILE* fp, int pole, int liczbaElementowPliku);
void usunBaze();
void gotoxy(short x, short y);
void dodajDomyslneZasilacze();
Zasilacz wczytajStruct();
void drukujStruct(Zasilacz psu);
int sprawdzNazweBazy(char* nazwa);
void pauseScreen(const char* message);

int main() {
    while (1) {
        menu();
    }
    return 0;
}

void menu() {
    system("cls");
    printf("\n\n\n\n\n\n\n\n\n\n\n");
    printf("                    ========== Minibaza danych zasilaczy komputerowych ==========\n");
    printf("                    1. Otworz baze danych\n");
    printf("                    2. Stworz nowa baze\n");
    printf("                    3. Przeglad bazy\n");
    printf("                    4. Dodaj wpis\n");
    printf("                    5. Modyfikuj wpis\n");
    printf("                    6. Usun wpis\n");
    printf("                    7. Sortuj baze\n");
    printf("                    8. Usun baze\n");
    printf("                    9. Zakoncz program\n");
    printf("                    =============================================================\n");
    printf("                    Wybierz opcje: ");

    char opcja = getchar();
    getchar();  // consume newline character
    switch (opcja) {
        case '1': otworzBazeDanych(); break;
        case '2': stworzBazeDanych(); break;
        case '3': przegladBazy(); break;
        case '4': dodajWpis(); break;
        case '5': modyfikujWpis(); break;
        case '6': usunWpis(); break;
        case '7':
            if (dbFile == NULL) {
                printf("Najpierw otworz baze danych.\n");
                pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
                break;
            }
            printf("Podaj kryterium sortowania (1 - Model, 2 - Moc, 3 - Cena): ");
            int criterion;
            scanf("%d", &criterion);
            fseek(dbFile, 0, SEEK_END);
            long size = ftell(dbFile) / sizeof(Zasilacz);
            sortujBaze(dbFile, criterion, size);
            pauseScreen("Baza danych posortowana pomyslnie. Nacisnij dowolny klawisz, aby kontynuowac...");
            break;
        case '8': usunBaze(); break;
        case '9': exit(0); break;
        default: printf("Nieprawidlowa opcja, sprobuj jeszcze raz.\n"); break;
    }
}

void otworzBazeDanych() {
    printf("Podaj nazwe pliku bazy danych (np. baza01.dat): ");
    char fileName[100];
    scanf("%99s", fileName);

    if (dbFile != NULL) {
        printf("Baza danych jest juz otwarta. Zamknij aktualna baze przed otwarciem nowej.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    dbFile = fopen(fileName, "rb+");
    if (dbFile == NULL) {
        printf("Nie udalo sie otworzyc pliku.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    strcpy(currentDbName, fileName);
    printf("Baza danych %s otwarta pomyslnie.\n", fileName);
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void stworzBazeDanych() {
    printf("Podaj nazwe nowego pliku bazy danych (np. baza01.dat): ");
    char fileName[100];
    scanf("%99s", fileName);

    if (dbFile != NULL) {
        printf("Baza danych jest juz otwarta. Zamknij aktualna baze przed utworzeniem nowej.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    dbFile = fopen(fileName, "wb+");
    if (dbFile == NULL) {
        printf("Nie udalo sie utworzyc pliku.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    strcpy(currentDbName, fileName);
    printf("Nowa baza danych %s utworzona pomyslnie.\n", fileName);

    // Dodanie domyslnych zasilaczy
    dodajDomyslneZasilacze();
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void przegladBazy() {
    if (dbFile == NULL) {
        printf("Najpierw otworz baze danych.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    rewind(dbFile);
    Zasilacz psu;
    int index = 1;
    printf("========== Przeglad bazy danych ==========\n");
    while (fread(&psu, sizeof(Zasilacz), 1, dbFile) == 1) {
        printf("%d. Model: %s, Moc: %dW, Wydajnosc: %s, Cena: %.2f\n",
               index++, psu.model, psu.Moc, psu.Wydajnosc, psu.Cena);
    }
    printf("==========================================\n");
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void dodajDomyslneZasilacze() {
    Zasilacz domyslneZasilacze[5] = {
        {"Corsair RM850x", 850, "80+ Gold", 130.00},
        {"EVGA SuperNOVA 750 G3", 750, "80+ Gold", 120.00},
        {"Seasonic Prime TX-1000", 1000, "80+ Titanium", 250.00},
        {"Cooler Master V850", 850, "80+ Gold", 140.00},
        {"Be Quiet! Straight Power 11", 750, "80+ Gold", 160.00}
    };

    for (int i = 0; i < 5; i++) {
        fwrite(&domyslneZasilacze[i], sizeof(Zasilacz), 1, dbFile);
    }
}

void dodajWpis() {
    if (dbFile == NULL) {
        printf("Najpierw otworz baze danych.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    Zasilacz psu = wczytajStruct();

    fseek(dbFile, 0, SEEK_END);
    fwrite(&psu, sizeof(Zasilacz), 1, dbFile);
    printf("Wpis dodany pomyslnie.\n");
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void modyfikujWpis() {
    if (dbFile == NULL) {
        printf("Najpierw otworz baze danych.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    przegladBazy();
    printf("Podaj numer wpisu do modyfikacji: ");
    int index;
    scanf("%d", &index);

    Zasilacz psu;
    fseek(dbFile, (index - 1) * sizeof(Zasilacz), SEEK_SET);
    fread(&psu, sizeof(Zasilacz), 1, dbFile);

    psu = wczytajStruct();

    fseek(dbFile, (index - 1) * sizeof(Zasilacz), SEEK_SET);
    fwrite(&psu, sizeof(Zasilacz), 1, dbFile);
    printf("Wpis zmodyfikowany pomyslnie.\n");
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void usunWpis() {
    if (dbFile == NULL) {
        printf("Najpierw otworz baze danych.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    przegladBazy();
    printf("Podaj numer wpisu do usuniecia: ");
    int index;
    scanf("%d", &index);

    FILE *tempFile = fopen("temp.dat", "wb+");
    if (tempFile == NULL) {
        printf("Nie udalo sie utworzyc pliku tymczasowego.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    rewind(dbFile);
    Zasilacz psu;
    int currentIndex = 1;
    while (fread(&psu, sizeof(Zasilacz), 1, dbFile) == 1) {
        if (currentIndex != index) {
            fwrite(&psu, sizeof(Zasilacz), 1, tempFile);
        }
        currentIndex++;
    }

    fclose(dbFile);
    fclose(tempFile);

    remove(currentDbName);
    rename("temp.dat", currentDbName);
    dbFile = fopen(currentDbName, "rb+");
    printf("Wpis usuniety pomyslnie.\n");
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void sortujBaze(FILE* fp, int pole, int liczbaElementowPliku) {
    Zasilacz bufs1, bufs2;
    int n = liczbaElementowPliku, zam;
    int i, wynik;
    fseek(fp, 0, SEEK_SET);
    do {
        zam = 0;
        for (i = 0; i < n - 1; i++) {
            fseek(fp, i * sizeof(Zasilacz), SEEK_SET);
            fread(&bufs1, sizeof(Zasilacz), 1, fp);
            fread(&bufs2, sizeof(Zasilacz), 1, fp);
            if (pole == 1) wynik = strcmp(bufs1.model, bufs2.model);
            if (pole == 2) wynik = bufs1.Moc > bufs2.Moc;
            if (pole == 3) wynik = bufs1.Cena > bufs2.Cena;
            if (wynik > 0) {
                fseek(fp, i * sizeof(Zasilacz), SEEK_SET);
                fwrite(&bufs2, sizeof(Zasilacz), 1, fp);
                fwrite(&bufs1, sizeof(Zasilacz), 1, fp);
                zam = 1;
            }
        }
    } while (zam);
    printf("Baza danych posortowana pomyslnie.\n");
}

void usunBaze() {
    if (dbFile == NULL) {
        printf("Najpierw otworz baze danych.\n");
        pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
        return;
    }

    fclose(dbFile);
    remove(currentDbName);
    dbFile = NULL;
    printf("Baza danych %s usunieta pomyslnie.\n", currentDbName);
    pauseScreen("Nacisnij dowolny klawisz, aby kontynuowac...");
}

void gotoxy(short x, short y) {
    COORD pos = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

Zasilacz wczytajStruct() {
    Zasilacz psu;
    printf("Podaj model: ");
    scanf("%49s", psu.model);
    printf("Podaj moc (W): ");
    scanf("%d", &psu.Moc);
    printf("Podaj wydajnosc: ");
    scanf("%14s", psu.Wydajnosc);
    printf("Podaj cene: ");
    scanf("%lf", &psu.Cena);
    return psu;
}

void drukujStruct(Zasilacz psu) {
    printf("\nModel: %s", psu.model);
    printf("\nMoc: %dW", psu.Moc);
    printf("\nWydajnosc: %s", psu.Wydajnosc);
    printf("\nCena: %.2f\n", psu.Cena);
}

int sprawdzNazweBazy(char* nazwa) {
    int len = strlen(nazwa);
    if (len < 9 || len > 10) return 0;
    if (strncmp(nazwa, "baza", 4) != 0) return 0;
    if (nazwa[len-4] != '.') return 0;
    if (strncmp(&nazwa[len-3], "dat", 3) != 0) return 0;
    for (int i = 4; i < len-4; i++) {
        if (!isdigit(nazwa[i])) return 0;
    }
    return 1;
}

void pauseScreen(const char* message) {
    printf("%s", message);
    getchar();
}
