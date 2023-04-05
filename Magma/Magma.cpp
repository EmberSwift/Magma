#include <iostream>

using namespace std;
// Размер блока 4 байта (или 32 бита)
#define BLOCK_SIZE 4
// Определяем тип vect как 4-байтовый массив
typedef uint8_t vect[BLOCK_SIZE];
static unsigned char Pi[8][16] =                        //таблицы преобразования
{
  {1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2},
  {8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7},
  {5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0},
  {7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12},
  {12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11},
  {11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0},
  {6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15},
  {12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1}
};


vect iter_key[32]; // Итерационные ключи шифрования


static void Add(const uint8_t* a, const uint8_t* b, uint8_t* c)  //Сложение по модулю 2
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        c[i] = a[i] ^ b[i];
}

static void Add_32(const uint8_t* a, const uint8_t* b, uint8_t* c)  //Сложение по модулю 2^32
{
    int i;
    unsigned int internal = 0;
    for (i = 3; i >= 0; i--)
    {
        internal = a[i] + b[i] + (internal >> 8);
        c[i] = internal & 0xff;
    }
}


static void T(const uint8_t* in_data, uint8_t* out_data)  //Электронная кодовая книга
{
    uint8_t first_part_byte, sec_part_byte;
    int i;
    for (i = 0; i < 4; i++)
    {
        // Извлекаем первую 4-битную часть байта
        first_part_byte = (in_data[i] & 0xf0) >> 4;
        // Извлекаем вторую 4-битную часть байта
        sec_part_byte = (in_data[i] & 0x0f);
        // Выполняем замену в соответствии с таблицей подстановок
        first_part_byte = Pi[i * 2][first_part_byte];
        sec_part_byte = Pi[i * 2 + 1][sec_part_byte];
        // «Склеиваем» обе 4-битные части обратно в байт
        out_data[i] = (first_part_byte << 4) | sec_part_byte;
    }
}

void Expand_Key(uint8_t* key)
{
    // Формируем восемь 32-битных подключей в порядке следования с первого по восьмой
    memcpy(iter_key[0], key, 4);
    memcpy(iter_key[1], key + 4, 4);
    memcpy(iter_key[2], key + 8, 4);
    memcpy(iter_key[3], key + 12, 4);
    memcpy(iter_key[4], key + 16, 4);
    memcpy(iter_key[5], key + 20, 4);
    memcpy(iter_key[6], key + 24, 4);
    memcpy(iter_key[7], key + 28, 4);

    // Повторяем предыдущий кусочек кода еще два раза
    memcpy(iter_key[8], key, 4);
    memcpy(iter_key[9], key + 4, 4);
    memcpy(iter_key[10], key + 8, 4);
    memcpy(iter_key[11], key + 12, 4);
    memcpy(iter_key[12], key + 16, 4);
    memcpy(iter_key[13], key + 20, 4);
    memcpy(iter_key[14], key + 24, 4);
    memcpy(iter_key[15], key + 28, 4);

    memcpy(iter_key[16], key, 4);
    memcpy(iter_key[17], key + 4, 4);
    memcpy(iter_key[18], key + 8, 4);
    memcpy(iter_key[19], key + 12, 4);
    memcpy(iter_key[20], key + 16, 4);
    memcpy(iter_key[21], key + 20, 4);
    memcpy(iter_key[22], key + 24, 4);
    memcpy(iter_key[23], key + 28, 4);

    // Формируем восемь 32-битных подключей в порядке следования с восьмого по первый
    memcpy(iter_key[24], key + 28, 4);
    memcpy(iter_key[25], key + 24, 4);
    memcpy(iter_key[26], key + 20, 4);
    memcpy(iter_key[27], key + 16, 4);
    memcpy(iter_key[28], key + 12, 4);
    memcpy(iter_key[29], key + 8, 4);
    memcpy(iter_key[30], key + 4, 4);
    memcpy(iter_key[31], key, 4);
}

static void g(const uint8_t* k, const uint8_t* a, uint8_t* out_data)
{
    uint8_t internal[4];
    uint32_t out_data_32;
    // Складываем по модулю 32 правую половину блока с итерационным ключом
    Add_32(a, k, internal);
    // Производим преобразование результата
    T(internal, internal);
    // Преобразовываем четырехбайтный вектор в одно 32-битное число
    out_data_32 = internal[0];
    out_data_32 = (out_data_32 << 8) + internal[1];
    out_data_32 = (out_data_32 << 8) + internal[2];
    out_data_32 = (out_data_32 << 8) + internal[3];
    // Циклически сдвигаем все влево на 11 разрядов
    out_data_32 = (out_data_32 << 11) | (out_data_32 >> 21);
    // Преобразовываем 32-битный результат сдвига обратно в 4-байтовый вектор
    out_data[3] = out_data_32;
    out_data[2] = out_data_32 >> 8;
    out_data[1] = out_data_32 >> 16;
    out_data[0] = out_data_32 >> 24;
}

static void G(const uint8_t* k, const uint8_t* a, uint8_t* out_data)
{
    uint8_t a_0[4]; // Правая половина блока
    uint8_t a_1[4]; // Левая половина блока
    uint8_t G[4];

    int i;
    // Делим 64-битный исходный блок на две части
    for (i = 0; i < 4; i++)
    {
        a_0[i] = a[4 + i];
        a_1[i] = a[i];
    }

    // Производим преобразование g
    g(k, a_0, G);
    // Ксорим результат преобразования g с левой половиной блока
    Add(a_1, G, G);

    for (i = 0; i < 4; i++)
    {
        // Пишем в левую половину значение из правой
        a_1[i] = a_0[i];
        // Пишем результат Add в правую половину блока
        a_0[i] = G[i];
    }

    // Сводим правую и левую части блока в одно целое
    for (i = 0; i < 4; i++)
    {
        out_data[i] = a_1[i];
        out_data[4 + i] = a_0[i];
    }
}
static void G_Fin(const uint8_t* k, const uint8_t* a, uint8_t* out_data)
{
    uint8_t a_0[4]; // Правая половина блока
    uint8_t a_1[4]; // Левая половина блока
    uint8_t G[4];

    int i;
    // Делим 64-битный исходный блок на две части
    for (i = 0; i < 4; i++)
    {
        a_0[i] = a[4 + i];
        a_1[i] = a[i];
    }

    // Производим преобразование g
    g(k, a_0, G);
    // Ксорим результат преобразования g с левой половиной блока
    Add(a_1, G, G);
    // Пишем результат Add в левую половину блока
    for (i = 0; i < 4; i++)
        a_1[i] = G[i];

    // Сводим правую и левую части блока в одно целое
    for (i = 0; i < 4; i++)
    {
        out_data[i] = a_1[i];
        out_data[4 + i] = a_0[i];
    }
}

void Encrypt(const uint8_t* blk, uint8_t* out_blk)
{
    int i;
    // Первое преобразование G
    G(iter_key[0], blk, out_blk);
    // Последующие (со второго по тридцать первое) преобразования G
    for (i = 1; i < 31; i++)
        G(iter_key[i], out_blk, out_blk);
    // Последнее (тридцать второе) преобразование G
    G_Fin(iter_key[31], out_blk, out_blk);
}

void Decrypt(const uint8_t* blk, uint8_t* out_blk)
{
    int i;
    // Первое преобразование G с использованием
    // тридцать второго итерационного ключа
    G(iter_key[31], blk, out_blk);
    // Последующие (со второго по тридцать первое) преобразования G
    // (итерационные ключи идут в обратном порядке)
    for (i = 30; i > 0; i--)
        G(iter_key[i], out_blk, out_blk);
    // Последнее (тридцать второе) преобразование G
    // с использованием первого итерационного ключа
    G_Fin(iter_key[0], out_blk, out_blk);
}
void main()
{
    setlocale(LC_ALL, "rus");
    uint8_t out_blk[8];
    unsigned char key[] = "12345678901234567890123456789012";
    Expand_Key(key);
    cout << "Ключ шифрования: " << key << endl;
    unsigned char string[] = "abcdefgh";
    cout << "Исходная строка: ";
    for (int i = 0; i < 8; i++)
        cout << string[i];
    cout << endl;

    Encrypt(string, out_blk);
    cout << "Зашифрованная строка: ";
    for (int i = 0; i<8; i++)
        cout << out_blk[i];
    cout << endl;

    cout << "Расшифрованная строка: ";
    Decrypt(out_blk, out_blk);
    for (int i = 0; i < 8; i++)
        cout << out_blk[i];
    cout << endl;

}

