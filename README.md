# Husarion Telegraf

Projekt na zajęcia "Mechatronic Design" na AGH 🟩⬛🟥.

## Spis Treści

- [Opis](#opis)
- [Wymagania](#wymagania)
- [Instalacja](#instalacja)
- [Użycie](#użycie)
  - [Inicjalizacja](#inicjalizacja)
  - [Obsługa enkodera wejściowego](#obsługa-enkodera-wejściowego)
  - [Wyświetlanie danych na kole wyjściowym](#wyświetlanie-danych-na-kole-wyjściowym)
  - [Konwersja danych binarnych](#konwersja-danych-binarnych)
  - [Reset pozycji i bufora](#reset-pozycji-i-bufora)
  - [Debugowanie](#debugowanie)
- [Autor](#autor)
- [Licencja](#licencja)

## Opis

Projekt **Husarion Telegraf** pozwala na kontrolę i monitorowanie danych z platformy Husarion. Wykorzystuje enkodery, aby śledzić pozycje kół robota, przetwarzać dane binarne i wyświetlać wynik na kole wyjściowym. Dodatkowo umożliwia reset pozycji oraz funkcję debugowania. Projekt napisany w C++.

## Wymagania

- Platforma Husarion z STM32
- Husarion SDK
- Platforma Husarion IDE lub lokalne środowisko C++ (np. GCC, CMake)
- Kabel USB do programowania i debugowania

## Instalacja

1. Sklonuj repozytorium:
    ```sh
    git clone https://github.com/Niewiaro/Husarion-Telegraf.git
    cd Husarion-Telegraf
    ```

2. Skonfiguruj środowisko zgodnie z dokumentacją Husarion:
    - W przypadku Husarion IDE, otwórz projekt i przeprowadź proces kompilacji i wysyłki.
    - Jeśli używasz lokalnego środowiska, zainstaluj wymagane biblioteki i użyj CMake do kompilacji.

## Użycie

### Inicjalizacja

Skrypt inicjalizuje stany początkowe enkoderów, ustawia domyślne wartości przesunięć oraz przygotowuje wątki odpowiedzialne za obsługę enkoderów i reset pozycji.

```cpp
void init()
{
    input_wheel_start_state = hMot1.getEncoderCnt();
    output_wheel_start_state = hMot2.getEncoderCnt();
    actuator_start_state = hMot3.getEncoderCnt();

    for (int i = 0; i < binary_array_size; ++i)
    {
        binary_array[i] = 1;
    }
    binary_array_index = 0;

    sys.taskCreate(input_wheel_encoder);
    sys.taskCreate(clear_button_thread_loop);
}
```

### Obsługa enkodera wejściowego

Funkcja `input_wheel_encoder` monitoruje zmiany pozycji enkodera i na podstawie przesunięcia względem pozycji początkowej ustawia wartości w tablicy `binary_array`.

```cpp
void input_wheel_encoder()
{
    while (input_wheel_encoder_run)
    {
        input_wheel_curent_state = hMot1.getEncoderCnt();

        if (!input_wheel_home_position_run &&
            abs(input_wheel_start_state - input_wheel_curent_state) > input_wheel_offset &&
            binary_array_index < binary_array_size)
        {
            binary_array[binary_array_index] = (input_wheel_curent_state > input_wheel_start_state) ? 1 : 0;
            binary_array_index++;
        }
        sys.delay(input_wheel_encoder_delay);
    }
}
```

### Wyświetlanie danych na kole wyjściowym

Funkcja `output_wheel_show` obraca koło wyjściowe na podstawie przetworzonej wartości binarnej i aktywuje siłownik.

```cpp
void output_wheel_show()
{
    int position = output_number * output_wheel_step;
    hMot2.rotAbs(position, output_wheel_power, true, INFINITE);
    hMot3.rotAbs(actuator_start_state + actuator_target_state, actuator_power, true, INFINITE);
}
```

### Konwersja danych binarnych

Funkcja `binaryToDecimal` konwertuje dane binarne z tablicy na liczbę dziesiętną i weryfikuje, czy wynik nie przekracza ustalonego limitu.

```cpp
int binaryToDecimal(const bool* binary, int size) {
    int decimal = 0;
    for (int i = 0; i < size; ++i) {
        decimal += binary[i] * (1 << (size - 1 - i));
    }
    if (decimal > border_top) {
        Serial.printf("ERROR: Value out of range\r\n");
        return 0;
    }
    return decimal;
}
```

### Reset pozycji i bufora

Funkcja `input_wheel_home_position` resetuje pozycję koła wejściowego do wartości początkowej. Funkcja `clear_buffor` umożliwia reset danych binarnych w przypadku błędu lub potrzeby ponownego uruchomienia.

```cpp
void input_wheel_home_position()
{
    hMot1.rotAbs(input_wheel_start_state, input_wheel_power, true, INFINITE);
}

void clear_buffor(bool all = false)
{
    if (all) {
        for (int i = 0; i < binary_array_size; ++i) {
            binary_array[i] = 1;
        }
        binary_array_index = 0;
    } else {
        if (binary_array_index > 0) {
            binary_array[--binary_array_index] = 1;
        }
    }
}
```

### Debugowanie

Funkcja `debug_info` wyświetla dane diagnostyczne z enkoderów w regularnych odstępach czasu.

```cpp
void debug_info()
{
    while (debug)
    {
        Serial.printf("hMot1: %d\thMot2: %d\thMot3: %d\r\n", hMot1.getEncoderCnt(), hMot2.getEncoderCnt(), hMot3.getEncoderCnt());
        sys.delay(debug_info_delay);
    }
}
```

## Autor

Jakub Niewiarowski

## Licencja

Projekt jest licencjonowany na zasadach licencji MIT. Szczegóły znajdują się w pliku [LICENSE](LICENSE). 
