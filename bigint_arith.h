#include "bigint_util.h"
//===============================================================================================//
//                                       사용자용 함수 구역
//                                     사용자가 직접 사용할 함수
//
int BI_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); // 덧셈 함수
int BI_Sub(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); // 뺄셈 함수
int BI_Mul(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); // 곱셈 함수
int BI_Sqr(BIGINT* bi_Dst, BIGINT* bi_Src1); // 제곱 함수

// !!!!! 시프트 함수를 사용자가 쓰려나??
int BI_Shift_RL_r_bits_x(BIGINT* bi_Src, int r); /**** A << r bits ****/
int BI_Shift_LR_r_bits_x(BIGINT* bi_Src, int r); /**** A >> r bits ****/
int BI_Shift_RL_r_bits_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int r); /**** A << r bits ****/
int BI_Shift_LR_r_bits_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int r); /**** A >> r bits ****/
// !!!!! 배열만 받아서 시프트하는 연산도 구현필요할듯

// !!!!! 지수승 함수를 사용자가 쓰려나??
int BI_Exp_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int n); /**** C = A^n ****/

//===============================================================================================//
//                                       개발자용 함수 구역
//                                  사용자는 직접 사용하지 않는 함수
//
//################################################################################################# 
//                                           뺄셈 관련 함수 
//#################################################################################################
int bi_Sub_w(word* Dst_bData, word Src1_Data, word Src2_Data, int carry_in); /**** 단일 워드 뺄셈 ****/
int bi_Sub(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); /**** 다중 워드 뺄셈 ****/

//################################################################################################# 
//                                           덧셈 관련 함수 
//#################################################################################################
int bi_Add_w(word* Dst_bData, word Src1_Data, word Src2_Data, int carry_in); /**** 단일 워드 덧셈 ****/
int bi_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); /**** 다중 워드 덧셈 ****/

//################################################################################################# 
//                                           곱셈 관련 함수 
//#################################################################################################
unsigned long long bi_Mul_w(word Src1_Data, word Src2_Data); /**** 단일 워드 곱셈 ****/
int bi_Mul_Schoolbook(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); /**** 스쿨북 곱셈 ****/
int bi_Mul_PS(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); /**** PS 곱셈 ****/
//int bi_Mul_Karatsuba(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); /**** 카라추바 곱셈 ****/

//################################################################################################# 
//                                           제곱 관련 함수 
//#################################################################################################
unsigned long long bi_Sqr_w(word Src1_Data); /**** 단일 워드 제곱 ****/
int bi_Sqr_PS(BIGINT* bi_Dst, BIGINT* bi_Src1); /**** PS 제곱 ****/

//################################################################################################# 
//                                           shift 관련 함수 
//#################################################################################################
int bi_Shift_RL_r_words(BIGINT* bi_Src, int r); /**** A << r words ****/
int bi_Shift_LR_r_words(BIGINT* bi_Src, int r); /**** A >> r words ****/

//################################################################################################# 
//                                           지수승 관련 함수 
//#################################################################################################
int bi_Exp_LR_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int n); /**** Left-to-Right exponentiation  ****/
