#pragma once
#include <iostream>

using namespace std;
// ������ ����� 4 ����� (��� 32 ����)
#define BLOCK_SIZE 4
// ���������� ��� vect ��� 4-�������� ������
typedef uint8_t vect[BLOCK_SIZE];
static unsigned char Pi[8][16] =                        //������� ��������������
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

static const unsigned char test_key[32] = {
    0x61, 0x63, 0x65, 0x66,
    0x62, 0x64, 0x67, 0x68,
    0x7a, 0x78, 0x76, 0x75,
    0x73, 0x71, 0x6f, 0x6d,
    0x6e, 0x70, 0x72, 0x74,
    0x77, 0x79, 0x6b, 0x6c,
    0x41, 0x33, 0x43, 0x38,
    0x34, 0x59, 0x35, 0x5a
};

vect iter_key[32]; // ������������ ����� ����������


static void GOST_Magma_Add(const uint8_t* a, const uint8_t* b, uint8_t* c)  //�������� �� ������ 2
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        c[i] = a[i] ^ b[i];
}

static void GOST_Magma_Add_32(const uint8_t* a, const uint8_t* b, uint8_t* c)  //�������� �� ������ 2^32
{
    int i;
    unsigned int internal = 0;
    for (i = 3; i >= 0; i--)
    {
        internal = a[i] + b[i] + (internal >> 8);
        c[i] = internal & 0xff;
    }
}


static void GOST_Magma_T(const uint8_t* in_data, uint8_t* out_data)
{
    uint8_t first_part_byte, sec_part_byte;
    int i;
    for (i = 0; i < 4; i++)
    {
        // ��������� ������ 4-������ ����� �����
        first_part_byte = (in_data[i] & 0xf0) >> 4;
        // ��������� ������ 4-������ ����� �����
        sec_part_byte = (in_data[i] & 0x0f);
        // ��������� ������ � ������������ � �������� �����������
        first_part_byte = Pi[i * 2][first_part_byte];
        sec_part_byte = Pi[i * 2 + 1][sec_part_byte];
        // ���������� ��� 4-������ ����� ������� � ����
        out_data[i] = (first_part_byte << 4) | sec_part_byte;
    }
}

void GOST_Magma_Expand_Key(const uint8_t* key)
{
    // ��������� ������ 32-������ ��������� � ������� ���������� � ������� �� �������
    memcpy(iter_key[0], key, 4);
    memcpy(iter_key[1], key + 4, 4);
    memcpy(iter_key[2], key + 8, 4);
    memcpy(iter_key[3], key + 12, 4);
    memcpy(iter_key[4], key + 16, 4);
    memcpy(iter_key[5], key + 20, 4);
    memcpy(iter_key[6], key + 24, 4);
    memcpy(iter_key[7], key + 28, 4);

    // ��������� ���������� ������� ���� ��� ��� ����
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

    // ��������� ������ 32-������ ��������� � ������� ���������� � �������� �� ������
    memcpy(iter_key[24], key + 28, 4);
    memcpy(iter_key[25], key + 24, 4);
    memcpy(iter_key[26], key + 20, 4);
    memcpy(iter_key[27], key + 16, 4);
    memcpy(iter_key[28], key + 12, 4);
    memcpy(iter_key[29], key + 8, 4);
    memcpy(iter_key[30], key + 4, 4);
    memcpy(iter_key[31], key, 4);
}

static void GOST_Magma_g(const uint8_t* k, const uint8_t* a, uint8_t* out_data)
{
    uint8_t internal[4];
    uint32_t out_data_32;
    // ���������� �� ������ 32 ������ �������� ����� � ������������ ������
    GOST_Magma_Add_32(a, k, internal);
    // ���������� ���������� ���������� �������������� ����������
    GOST_Magma_T(internal, internal);
    // ��������������� �������������� ������ � ���� 32-������ �����
    out_data_32 = internal[0];
    out_data_32 = (out_data_32 << 8) + internal[1];
    out_data_32 = (out_data_32 << 8) + internal[2];
    out_data_32 = (out_data_32 << 8) + internal[3];
    // ���������� �������� ��� ����� �� 11 ��������
    out_data_32 = (out_data_32 << 11) | (out_data_32 >> 21);
    // ��������������� 32-������ ��������� ������ ������� � 4-�������� ������
    out_data[3] = out_data_32;
    out_data[2] = out_data_32 >> 8;
    out_data[1] = out_data_32 >> 16;
    out_data[0] = out_data_32 >> 24;
}

static void GOST_Magma_G(const uint8_t* k, const uint8_t* a, uint8_t* out_data)
{
    uint8_t a_0[4]; // ������ �������� �����
    uint8_t a_1[4]; // ����� �������� �����
    uint8_t G[4];

    int i;
    // ����� 64-������ �������� ���� �� ��� �����
    for (i = 0; i < 4; i++)
    {
        a_0[i] = a[4 + i];
        a_1[i] = a[i];
    }

    // ���������� �������������� g
    GOST_Magma_g(k, a_0, G);
    // ������ ��������� �������������� g � ����� ��������� �����
    GOST_Magma_Add(a_1, G, G);

    for (i = 0; i < 4; i++)
    {
        // ����� � ����� �������� �������� �� ������
        a_1[i] = a_0[i];
        // ����� ��������� GOST_Magma_Add � ������ �������� �����
        a_0[i] = G[i];
    }

    // ������ ������ � ����� ����� ����� � ���� �����
    for (i = 0; i < 4; i++)
    {
        out_data[i] = a_1[i];
        out_data[4 + i] = a_0[i];
    }
}
static void GOST_Magma_G_Fin(const uint8_t* k, const uint8_t* a, uint8_t* out_data)
{
    uint8_t a_0[4]; // ������ �������� �����
    uint8_t a_1[4]; // ����� �������� �����
    uint8_t G[4];

    int i;
    // ����� 64-������ �������� ���� �� ��� �����
    for (i = 0; i < 4; i++)
    {
        a_0[i] = a[4 + i];
        a_1[i] = a[i];
    }

    // ���������� �������������� g
    GOST_Magma_g(k, a_0, G);
    // ������ ��������� �������������� g � ����� ��������� �����
    GOST_Magma_Add(a_1, G, G);
    // ����� ��������� GOST_Magma_Add � ����� �������� �����
    for (i = 0; i < 4; i++)
        a_1[i] = G[i];

    // ������ ������ � ����� ����� ����� � ���� �����
    for (i = 0; i < 4; i++)
    {
        out_data[i] = a_1[i];
        out_data[4 + i] = a_0[i];
    }
}

void GOST_Magma_Encrypt(const uint8_t* blk, uint8_t* out_blk)
{
    int i;
    // ������ �������������� G
    GOST_Magma_G(iter_key[0], blk, out_blk);
    // ����������� (�� ������� �� �������� ������) �������������� G
    for (i = 1; i < 31; i++)
        GOST_Magma_G(iter_key[i], out_blk, out_blk);
    // ��������� (�������� ������) �������������� G
    GOST_Magma_G_Fin(iter_key[31], out_blk, out_blk);
}

void GOST_Magma_Decrypt(const uint8_t* blk, uint8_t* out_blk)
{
    int i;
    // ������ �������������� G � ��������������
    // �������� ������� ������������� �����
    GOST_Magma_G(iter_key[31], blk, out_blk);
    // ����������� (�� ������� �� �������� ������) �������������� G
    // (������������ ����� ���� � �������� �������)
    for (i = 30; i > 0; i--)
        GOST_Magma_G(iter_key[i], out_blk, out_blk);
    // ��������� (�������� ������) �������������� G
    // � �������������� ������� ������������� �����
    GOST_Magma_G_Fin(iter_key[0], out_blk, out_blk);
}
void thmain()
{
    uint8_t out_blk[4];
    GOST_Magma_Expand_Key(test_key);
    unsigned char encrypt_test_string[8] = {
    0x32, 0x34, 0x33, 0x61, 0x66, 0x67, 0x21, 0x79
    };
    GOST_Magma_Encrypt(encrypt_test_string, out_blk);
    cout << out_blk << endl;
    // static const unsigned char decrypt_test_string[8] = {
    // 0x3d, 0xca, 0xd8, 0xc2, 0xe5, 0x01, 0xe9, 0x4e
    // };
     //  GOST_Magma_Decrypt(decrypt_test_string, out_blk);
     //  cout << out_blk << endl;

}

