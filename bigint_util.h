#include "bigint_def.h"

//################################################################################################# 
//                                  BIGINT 생성 및 소멸관련 함수
//#################################################################################################
/**** 길이 정보를 받아서 데이터를 초기화하는 빅인티저 생성 함수 ****/
void bi_create(BIGINT* bi_Dst, int sign, word* bData);
/**** 16진수 문자열 -> 빅인티저 구조체 함수 ****/
int bi_set_by_string(BIGINT* bi_Dst, int sign, word* Data, char* str);
/**** 불필요한 0 지우는 함수 ****/
int bi_refine(BIGINT* bi_Src);

//################################################################################################# 
//                                  BIGINT 구조체를 활용하는 유틸 함수
//#################################################################################################
/**** 빅인티저 복사 함수 ****/ 
// BI_Src를 BI_Dst에 복사. BI_Dst <- BI_Src Copy Biginteger
int bi_copy_bigint(BIGINT* bi_Dst, BIGINT* bi_Src); // 부호 유지
int bi_copy2_bigint(BIGINT* bi_Dst, BIGINT* bi_Src); // 부호 변경
/**** 빅인티저 출력 함수 ****/ 
void bi_print_bigint(BIGINT* bi_Src);
/**** 빅인티저 길이 반환 함수 ****/ 
int bi_get_length(BIGINT* bi_Src);
/**** 두개의 빅인티저 길이 비교 함수 ****/ //bi_Src1의 길이가 더 길면 1, bi_Src2의 길이가 더 길면 -1, 길이가 같으면 0 출력
int bi_compare_length(BIGINT* bi_Src1, BIGINT* bi_Src2);
/**** 두개의 빅인티저 길이 중 긴 값 리턴 함수 ****/ 
int bi_get_max_length(BIGINT* bi_Src1, BIGINT* bi_Src2);
/**** 두개의 빅인티저 절대값 비교 함수 ****/ // bi_Src1의 절댓값이 더 크면 1, bi_Src2의 절댓값이 더 크면 -1, 같으면 0 리턴
int bi_compare_ABS(BIGINT* bi_Src1, BIGINT* bi_Src2);
/**** 두개의 빅인티저 값 비교 함수 ****/ // 부호 정보 고려. bi_Src1가 크면 1, bi_Src2가 크면 -1, 같으면 0
int bi_compare_bigint(BIGINT* bi_Src1, BIGINT* bi_Src2);
/**** 빅인티저가 0,1인지 판별하는 함수 ****/ // true 1, false 0 리턴
int bi_is_zero(BIGINT* bi_Src);
int bi_is_one(BIGINT* bi_Src);
int bi_set_zero(BIGINT* bi_Src);
int bi_set_one(BIGINT* bi_Src);
//void bi_assign(BIGINT* bi_Dst,  BIGINT* bi_Src);
//void bi_set_zero(BIGINT* bi_Src);
/**** 랜덤한 빅인티저 생성 함수 ****/
// wordlen 길이의 랜덤한 빅인티저 생성
// int bi_create_random_bigint(BIGINT* bi_Dst, int wordlen);


