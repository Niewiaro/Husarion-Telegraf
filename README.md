# Husarion Telegraf

Projekt na zajęcia "Mechatronic Desigh" na AGH 🟩⬛🟥.

## Spis Treści

- [Opis](#opis)
- [Wymagania](#wymagania)
- [Instalacja](#instalacja)
- [Użycie](#użycie)
  - [Inicjalizacja robota](#inicjalizacja-robota)
  - [Obsługa enkodera](#obsługa-enkodera)
  - [Konwersja danych binarnych](#konwersja-danych-binarnych)
  - [Reset pozycji](#reset-pozycji)
- [Autor](#autor)
- [Licencja](#licencja)

## Opis

Projekt **Husarion Telegraf** umożliwia kontrolę i analizę danych z robota Husarion. System wykorzystuje enkoder do monitorowania pozycji kół robota, przetwarza dane binarne oraz umożliwia reset pozycji. Projekt napisany w C++.

## Wymagania

- Husarion ROSbot lub kompatybilny
- Husarion SDK
- Platforma Husarion IDE lub lokalne środowisko C++ dla urządzeń Husarion

## Instalacja

1. Sklonuj repozytorium:
    ```sh
    git clone https://github.com/Niewiaro/Husarion-Telegraf.git
    cd Husarion-Telegraf
    ```

2. Zainstaluj wymagane zależności przez Husarion IDE lub lokalnie przez CMake (zgodnie z instrukcjami Husarion).

## Użycie

### Inicjalizacja robota

Skrypt inicjalizuje pozycję enkodera koła robota, ustawia domyślne przesunięcie i przygotowuje do działania funkcję monitorującą zmiany pozycji.

```cpp
void init()
{
    input_wheel_start_state = hMot1.getEncoderCnt();
    input_wheel_offset = 10;
    input_wheel_curent_state = input_wheel_start_state;

    for (int i = 0; i < 4; ++i)
    {
        binary_array[i] = 1;
    }
    binary_array_index = 0;

    Serial.printf("input_wheel_state: %d\r\n", input_wheel_start_state);
    sys.taskCreate(input_wheel_encoder);
}
```

### Obsługa enkodera

Funkcja `input_wheel_encoder` monitoruje zmiany pozycji koła i ustawia wartości binarne w tablicy `binary_array` na podstawie przesunięcia.

```cpp
void input_wheel_encoder()
{
    // Pętla monitorująca stan enkodera i zapamiętująca wartości binarne
}
```

### Konwersja danych binarnych

Funkcja `binaryToDecimal` konwertuje tablicę `binary_array` na liczbę dziesiętną. Funkcja przydaje się do odczytywania pozycji binarnej w postaci liczby dziesiętnej.

```cpp
int binaryToDecimal(const bool* binary, int size) {
    int decimal = 0;
    for (int i = 0; i < size; ++i) {
        decimal += binary[i] * (1 << (size - 1 - i)); // konwersja binarnej do dziesiętnej
    }
    return decimal;
}
```

### Reset pozycji

Aby zresetować pozycję koła do początkowej wartości `input_wheel_start_state`, wywoływana jest funkcja `input_wheel_home_position`.

```cpp
void input_wheel_home_position()
{
    hLED2.on();
    hMot1.rotAbs(input_wheel_start_state, 200, false, INFINITE); // obrót do pozycji 0
    hLED2.off();
}
```

## Autor

Jakub Niewiarowski

## Licencja

Projekt jest licencjonowany na zasadach licencji MIT. Szczegóły znajdują się w pliku [LICENSE](LICENSE).
