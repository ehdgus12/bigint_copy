//// 빅인티저 구조체 정의하는 헤더파일 ////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//################################################################################################# 
//                                  BIGINT에 사용되는 정의들
//#################################################################################################

/************** BIGINT 부호 **************/
// NON_NEGATIVE 1, NEGATIVE -1이라고 해도 괜찮을듯
#define NON_NEGATIVE    1
#define ZERO            0
#define NEGATIVE        -1

// word 하나당 32 비트 저장... 2048bit / 32 : 64 
#define MAX_BINT_LEN 64

/************** 단위 **************/
typedef unsigned int word;
typedef unsigned long long ull;

//#define WORDSIZE   sizeof(word)*8

/************** 함수들의 작동 여부 확인을 위한 definitions **************/
#define FUNC_SUCCESS               0 // 함수 정상 작동
#define FUNC_ERROR                 -1 // 함수 이상 작동
#define POINTER_ERROR              1 // 이미 사용중인 포인터 
#define NULL_POINTER_ERROR         2 // 널 포인터 에러.
#define SIZE_ERROR                 3 // 덧, 뺄셈 사이즈 에러.
#define INVALID_CHAR_ERROR         4 // 잘못된 문자열 들어갔을 때 생기는 에러.

/************** BIGINT 구조체 정의 **************/
typedef struct{
    int sign; // 
    int bLen; // 배열 A의 길이
    word* bData; // 배열 A의 주소
} BIGINT;

//################################################################################################# 
//                                  BIGINT 생성 및 소멸관련 함수
//#################################################################################################

/**** 길이 정보를 받아서 데이터를 초기화하는 빅인티저 생성 함수 ****/
void bi_create(BIGINT* bi_Dst, int sign, word* bData)
{
    bi_Dst->sign = sign; // NON_NEGATIVE로 초기화
    bi_Dst->bLen = 0;
    bi_Dst->bData = bData;

}

/**** 16진수 문자열 -> 빅인티저 구조체 함수 ****/
int bi_set_by_string(BIGINT* bi_Dst, int sign, word* Data, char* str){
    int str_len = strlen(str); // 입력된 문자열의 길이 계산
    int word_len = (str_len + 7) / 8; // 문자열 길이에 따라서 필요한 워드 개수 계산
    // 현재 word(unsigned int)가 4바이트니 8자리씩 끊어서 워드 하나 할당으로 함-> 
    // 8 대신 word와 관련있는 표현으로 바꿔보기 ex) 2*sizeof(word)

    // 빅인티저 생성
    bi_Dst->sign = sign; // NON_NEGATIVE로 초기화
    bi_Dst->bLen = word_len;
    bi_Dst->bData = Data;

    // 문자열을 16진수에서 정수로 변환하여 빅인티저 배열에 저장하는 단계
    int index = str_len - 1; // index : 전체 문자열의 index-> 끝부터 시작

    // 문자열 길이 및 워드길이 확인용
    //printf("str_len : %d /// ", str_len);
    //printf("word_len : %d\n", word_len);

    // 문자를 8개씩 처리하여 unsigned int 정수로 변환
    // 가장 오른쪽 워드(낮은 값)부터 작업
    // 즉, 받은 전체 문자열이 6606BC465BF5ABCD일 때 5BF5ABCD 먼저 처리, 이후 6606BC46 처리하는 방식->
    for (int i = 0; i < word_len - 1; i++) { 
        word value = 0; // 현재 처리중인 16진수 숫자 누적할 변수-> 
        // ex) 5BF5ABCD에서 CD처리 중이면 CD값을 정수로 변환하여 value에 넣을 것임->
        int digits_to_read = 8; // 16진수 문자를 8개(4바이트)씩 처리할 것임-> 
        // 즉, 받은 전체 문자열이 6606BC465BF5ABCD일 때 6606BC46 5BF5ABCD 단위로 처리->

        for (int j = 0; j < digits_to_read; j++) {
            char c = str[index];
            int digit_value;

            if (c >= '0' && c <= '9') {
                digit_value = c - '0';
            } else if (c >= 'A' && c <= 'F') {
                digit_value = c - 'A' + 10;
            } else if (c >= 'a' && c <= 'f') {
                digit_value = c - 'a' + 10;
            } else {
                // 예외 처리: 유효하지 않은 문자
                return INVALID_CHAR_ERROR;
            }

            value |= (digit_value << (j * 4)); // 4비트씩 왼쪽으로 쉬프트하면서 값 계산
            index--;
        }

        bi_Dst->bData[i] = value;
        //데이터 잘 들어가는지 확인용 코드
        //printf("bData[%d] : %08X ///\n ", i, bi_Dst->bData[i]);
    }

    // 마지막 워드 부분 처리->
    word value = 0; 
    int digits_to_read = 8; 

    for (int j = 0; j < digits_to_read; j++) {

        if (index < 0){ // index 0 보다 작은 경우 : 문자열 끝난 경우
            value |= (0 << (j * 4));
        }

        else{
            char c = str[index];
            int digit_value;

            if (c >= '0' && c <= '9') {
                digit_value = c - '0';
            } else if (c >= 'A' && c <= 'F') {
                digit_value = c - 'A' + 10;
            } else if (c >= 'a' && c <= 'f') {
                digit_value = c - 'a' + 10;
            } else {
                // 예외 처리: 유효하지 않은 문자
                return INVALID_CHAR_ERROR;
            }
            value |= (digit_value << (j * 4)); // 4비트씩 왼쪽으로 쉬프트하면서 값 계산
        }
        index--;
    }

    bi_Dst->bData[word_len-1] = value;
    //데이터 잘 들어가는지 확인용 코드
    //printf("bData[last] : %08X ", (*bi_Dst)->bData[word_len-1]);
    //printf("\n");
    return FUNC_SUCCESS;
}


/**** 불필요한 0 지우는 함수****/
int bi_refine(BIGINT* bi_Src){
    if (bi_Src == NULL) return NULL_POINTER_ERROR; 

    while (bi_Src->bLen > 0){
        if (bi_Src->bData[bi_Src->bLen-1] != 0) break; // 0이 아니면 지우지 않음
        bi_Src->bLen--;
    }

    if (bi_Src->bLen == 0) bi_Src->sign = ZERO;
    
    return FUNC_SUCCESS;

}
///////////////////////////////////////////////////////////////////////////////////////////////////

//################################################################################################# 
//                                  BIGINT 구조체를 활용하는 유틸 함수
//#################################################################################################
/**** 빅인티저 출력 함수 ****/ 
void bi_print_bigint(BIGINT* bi_Src) {
    if(bi_Src->sign == ZERO || bi_Src == 0){
        printf("is zero\n");
        return;
    }
    if (bi_Src->sign == NEGATIVE) printf("-");
    
    for (int i = bi_Src->bLen - 1; i >= 0; i--) {
        printf("%08X ", bi_Src->bData[i]);
    }
    printf("\n");
}

/**** 빅인티저 복사 함수 ****/ 
// BI_Src를 BI_Dst에 복사-> BI_Dst <- BI_Src Copy Biginteger
// 부호 유지
int bi_copy_bigint(BIGINT* bi_Dst, BIGINT* bi_Src){
    if(bi_Dst == NULL) {
        //printf("카피 오류 : 포인터 없음");
        return NULL_POINTER_ERROR; // 포인터 에러 
    }

    bi_Dst->bLen = bi_Src->bLen;
    bi_Dst->sign = bi_Src->sign;

    for (int i = 0; i < bi_Src->bLen; i++){
        bi_Dst->bData[i] = bi_Src->bData[i];
    }

    //printf("복사 결과 [bi_Dst] : ");
    //bi_print_bigint(bi_Dst);
    
    return FUNC_SUCCESS;

}
// 부호 변경
int bi_copy2_bigint(BIGINT* bi_Dst, BIGINT* bi_Src){
    if(bi_Dst == NULL) {
        printf("카피 오류 : 포인터 없음");
        return NULL_POINTER_ERROR; // 포인터 에러 
    }
    
    bi_Dst->bLen = bi_Src->bLen;
    bi_Dst->sign = -(bi_Src->sign);
    
    for (int i = 0; i < bi_Src->bLen; i++){
        bi_Dst->bData[i] = bi_Src->bData[i];
    }

    printf("복사 결과 [bi_Dst] : ");
    bi_print_bigint(bi_Dst);
    
    return FUNC_SUCCESS;

}

/**** 빅인티저 길이 반환 함수 ****/ 
int bi_get_length(BIGINT* bi_Src){
    return bi_Src->bLen;
}
/**** 두개의 빅인티저 길이 비교 함수 ****/ 
// bi_Src1의 길이가 더 길면 1, bi_Src2의 길이가 더 길면 -1, 길이가 같으면 0 출력
int bi_compare_length(BIGINT* bi_Src1, BIGINT* bi_Src2){
    if (bi_get_length(bi_Src1) > bi_get_length(bi_Src2)) return 1;
    else if (bi_get_length(bi_Src1) < bi_get_length(bi_Src2)) return -1;
    else return 0;
}
/**** 두개의 빅인티저 길이 중 긴 값 리턴 함수 ****/ 
int bi_get_max_length(BIGINT* bi_Src1, BIGINT* bi_Src2){
    int result = bi_compare_length(bi_Src1, bi_Src2);
    if (result == 1) return bi_Src1->bLen;
    else return bi_Src2->bLen;
}
/**** 두개의 빅인티저 절대값 비교 함수 ****/ 
// bi_Src1의 절댓값이 더 크면 1, bi_Src2의 절댓값이 더 크면 -1, 같으면 0 리턴
int bi_compare_ABS(BIGINT* bi_Src1, BIGINT* bi_Src2){
    // x의 워드 길이가 더 큰 경우
    if (bi_Src1->bLen > bi_Src2->bLen) return 1;
    // y의 워드 길이가 더 큰 경우
    else if (bi_Src1->bLen < bi_Src2->bLen) return -1;
    // 워드 길이가 같은 경우
    for (int j = bi_Src1->bLen-1; j >= 0; j--){
        if (bi_Src1->bData[j] > bi_Src2->bData[j]) return 1;
        else if (bi_Src1->bData[j] < bi_Src2->bData[j]) return -1;
    }
    return 0;
}
/**** 두개의 빅인티저 값 비교 함수 ****/ 
// 부호 정보 고려-> bi_Src1가 크면 1, bi_Src2가 크면 -1, 같으면 0
int bi_compare_bigint(BIGINT* bi_Src1, BIGINT* bi_Src2){
    // 부호 다른 경우
    if (bi_Src1->sign > bi_Src2->sign) return 1;
    if (bi_Src1->sign < bi_Src2->sign) return -1;
    
    // 부호 같은 경우
    int ret = bi_compare_ABS(bi_Src1, bi_Src2); // 절댓값 비교함수
    if (bi_Src1->sign == NON_NEGATIVE) return ret;
    else return ret * (-1);
}
/**** 빅인티저가 0, 1인지 판별하는 함수 ****/
// true 1, false 0 
int bi_is_zero(BIGINT* bi_Src){
    if (bi_Src->sign == ZERO && bi_Src->bLen == 0) return 1;
    return 0;
}
int bi_is_one(BIGINT* bi_Src){
    
    if (bi_Src->sign == NEGATIVE || bi_Src->bData[0] != 1) return 0;

    for (int i = bi_Src->bLen - 1; i >= 1; i--) {
        if (bi_Src->bData[i] != 0) return 0;
    }
    return 1;
}


//===============================================================================================//

//===============================================================================================//
//                                       사용자용 함수 구역
//                                     사용자가 직접 사용할 함수
//
int BI_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); // 덧셈 함수
int BI_Sub(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); // 뺄셈 함수
int BI_Mul(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2); // 곱셈 함수

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


//===============================================================================================//
int BI_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    // case 1: bi_Src1이 0인 경우
    if (bi_Src1->sign == ZERO) return bi_copy_bigint(bi_Dst, bi_Src2);
    // case 2: bi_Src2가 0인 경우
    if (bi_Src2->sign == ZERO) return bi_copy_bigint(bi_Dst, bi_Src1);
    // case 3: 부호 다른 경우
    if (bi_Src1->sign == NON_NEGATIVE && bi_Src2->sign == NEGATIVE) {
        // 음수가 더 절댓값이 큰 경우
        if (bi_compare_ABS(bi_Src1, bi_Src2) == -1){ 
            bi_Dst->sign = NEGATIVE;
            bi_Dst->bLen = bi_Src2->bLen;
            return bi_Sub(bi_Dst, bi_Src2, bi_Src1);
        }
        else {
            bi_Dst->sign = NON_NEGATIVE;
            bi_Dst->bLen = bi_Src1->bLen;
            return bi_Sub(bi_Dst, bi_Src1, bi_Src2);
        }
    }
    if (bi_Src1->sign == NEGATIVE && bi_Src2->sign == NON_NEGATIVE) {
        if (bi_compare_ABS(bi_Src1, bi_Src2) == 1) {
            bi_Dst->sign = NEGATIVE;
            bi_Dst->bLen = bi_Src1->bLen;
            return bi_Sub(bi_Dst, bi_Src1, bi_Src2);     
        }
        else {
            bi_Dst->sign = NON_NEGATIVE;
            bi_Dst->bLen = bi_Src2->bLen;
            return bi_Sub(bi_Dst, bi_Src2, bi_Src1);
        }
        
    }
    // case 4: 부호 같은 경우
    else {
        bi_Dst->sign = bi_Src1->sign;
        if (bi_Src1->bLen >= bi_Src2->bLen) {
            bi_Dst->bLen = (bi_Src1->bLen)+1;
            return bi_Add(bi_Dst, bi_Src1, bi_Src2);
        }
        else {
            bi_Dst->bLen = (bi_Src2->bLen)+1;
            return bi_Add(bi_Dst, bi_Src2, bi_Src1);
        }
    }        
}

int bi_Add_w(word* Dst_bData, word Src1_Data, word Src2_Data, int carry_in){
    int carry_out = 0;

    *Dst_bData = (Src1_Data + Src2_Data) & 0xFFFFFFFF;
    if (*Dst_bData < Src1_Data) carry_out = 1;

    *Dst_bData = (*Dst_bData + carry_in) & 0xFFFFFFFF;
    if (*Dst_bData < carry_in) carry_out += 1;

    return carry_out; // 0 또는 1
}

int bi_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    // 길이가 정상적으로 들어오지 않은 경우
    if (bi_Src1->bLen < bi_Src2->bLen) return SIZE_ERROR;
    //=================================== 길이가 정상적으로 들어온 경우 ===================================//
    int carry = 0;
    int len_long = bi_Src1->bLen; 
    int len_short = bi_Src2->bLen;
    int i = 0;
    //=================================== bi_Src1,2 길이 같은 경우 ===================================//
    if(len_long == len_short){
        //bi_Dst->bLen = len_long;
        for ( ; i < len_long; i++){
            carry = bi_Add_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], carry);
        }
        if (carry){
            bi_Dst->bData[bi_Dst->bLen-1] = carry;
        }
    }
    //=================================== bi_Src1가 더 긴 경우 ===================================//
    else {
        //bi_Dst->bLen = len_long; // 결과 값은 긴쪽 길이로 설정.
        //====================== 우선 짧은 길이만큼 덧셈 ======================//
        for( ; i < len_short; i++){
            carry = bi_Add_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], carry);
        }
        //========== 캐리가 없는 경우 ==========//
        if (carry == 0){
            for( ; i < len_long; i++){
                bi_Dst->bData[i] = bi_Src1->bData[i]; 
            }
        }
        //========== 캐리가 있는 경우 ==========//
        else {
            for ( ; i < len_long; i++){
                bi_Dst->bData[i] = (bi_Src1->bData[i] + carry) & 0xFFFFFFFF;
                carry = (bi_Dst->bData[i] < carry) ? 1 : 0;   
            }
        }
        //====================== 길이가 더 늘어나는지 판단하는 부분 ======================//
        if (carry){
            bi_Dst->bData[bi_Dst->bLen-1] = carry;
        }
    }
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}

int BI_Sub(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){

    // case 1: bi_Src1이 0인 경우, 부호 변경하여 복사
    if (bi_Src1->sign == ZERO) return bi_copy2_bigint(bi_Dst, bi_Src2);
    // case 2: bi_Src2가 0인 경우, 부호 유지하며 복사
    if (bi_Src1->sign == ZERO) return bi_copy_bigint(bi_Dst, bi_Src1);
    // case 3: bi_Src1, bi_Src2가 같은 경우
    if (bi_compare_bigint(bi_Src1, bi_Src2) == 0) {
        bi_Dst->sign = ZERO;
        bi_Dst->bLen = 0;
        return FUNC_SUCCESS;
    }
    // case 4: 부호 다른 경우
    // 양수 - 음수
    if (bi_Src1->sign == NON_NEGATIVE && bi_Src2->sign == NEGATIVE) {
        bi_Dst->sign = NON_NEGATIVE;
        if(bi_Src1->bLen >= bi_Src2->bLen) {
            bi_Dst->bLen = (bi_Src1->bLen)+1;
            return bi_Add(bi_Dst, bi_Src1, bi_Src2);
        }
        else {
            bi_Dst->bLen = (bi_Src2->bLen)+1;
            return bi_Add(bi_Dst, bi_Src2, bi_Src1);
        }
    }
    // 음수 - 양수
    if (bi_Src1->sign == NEGATIVE && bi_Src2->sign == NON_NEGATIVE) {
        bi_Dst->sign = NEGATIVE;
        if(bi_Src1->bLen >= bi_Src2->bLen) {
            bi_Dst->bLen = (bi_Src1->bLen)+1;
            return bi_Add(bi_Dst, bi_Src1, bi_Src2);
        }
        else {
            bi_Dst->bLen = (bi_Src2->bLen)+1;
            return bi_Add(bi_Dst, bi_Src2, bi_Src1);
        }
    }    
    // case 5: 부호 같은 경우
    // 양수 - 양수
    if (bi_Src1->sign == NON_NEGATIVE && bi_Src2->sign == NON_NEGATIVE) {
        if (bi_compare_ABS(bi_Src1, bi_Src2) == -1){ // 뒤에가 절대값이 더 큰 값인 경우
            bi_Dst->sign = NEGATIVE;
            bi_Dst->bLen = bi_Src2->bLen;
            return bi_Sub(bi_Dst, bi_Src2, bi_Src1);
        }
        else {
            bi_Dst->sign = NON_NEGATIVE;
            bi_Dst->bLen = bi_Src1->bLen;
            return bi_Sub(bi_Dst, bi_Src1, bi_Src2);
        }
        
    }
    // 음수 - 음수
    if (bi_Src1->sign == NEGATIVE && bi_Src2->sign == NEGATIVE) {
        if (bi_compare_ABS(bi_Src1, bi_Src2) == 1) { // 앞에가 절대값이 더 큰 값인 경우
            bi_Dst->sign = NEGATIVE;
            bi_Dst->bLen = bi_Src1->bLen;
            return bi_Sub(bi_Dst, bi_Src1, bi_Src2);     
        }
        else {
            bi_Dst->bLen = bi_Src2->bLen;
            return bi_Sub(bi_Dst, bi_Src2, bi_Src1);
        }
    }
    return FUNC_ERROR;
}

int bi_Sub_w(word* Dst_bData, word Src1_Data, word Src2_Data, int borrow_in) {
    int borrow_out = 0;

    *Dst_bData = (Src1_Data - borrow_in) & 0xFFFFFFFF;
    if(Src1_Data < borrow_in) borrow_out = 1;
    if(*Dst_bData < Src2_Data) borrow_out += 1;

    *Dst_bData = (*Dst_bData - Src2_Data) & 0xFFFFFFFF;

    return borrow_out; // 0 또는 1
}

int bi_Sub(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2) {

    if (bi_Src1->bLen < bi_Src2->bLen) return SIZE_ERROR;

    //=================================== 이외의 경우 ===================================//
    int borrow = 0;
    int len_long = bi_Src1->bLen;
    int len_short = bi_Src2->bLen;
    int i = 0;

    //=================================== bi_Src1,2 길이 같은 경우 ===================================//
    if (len_long == len_short) {
        for (; i < len_long; i++) {
            borrow = bi_Sub_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], borrow);
        }
    }

    //=================================== bi_Src1가 더 긴 경우 ===================================//
    else {
        //====================== 우선 짧은 길이만큼 뺄셈 ======================//
        for (; i < len_short; i++) {
            borrow = bi_Sub_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], borrow);
        }
        ///========== 캐리가 없는 경우 ==========//
        if (borrow == 0) {
            for( ; i < len_long; i++){
                bi_Dst->bData[i] = bi_Src1->bData[i]; 
            }
        }
        //========== 캐리가 있는 경우 ==========//
        // 캐리가 있는 경우의 버전 2
        else {
            for (; i < len_long; i++) {
                bi_Dst->bData[i] = (bi_Src1->bData[i] - borrow) & 0xFFFFFFFF;
                borrow = (bi_Src1->bData[i] == 0 ) ? 1 : 0;
            }
        }
    }
    // 앞에서부터 word가 0인 부분 삭제
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}

unsigned long long bi_Mul_w(word Src1_Data, word Src2_Data){
    unsigned int A1, A0, B1, B0;
    unsigned int T1, T0, T;
    unsigned int C1, C0;
    unsigned long long C;

    A1 = (Src1_Data >> 16);
    A0 = Src1_Data & ((1 << 16) - 1);
    B1 = (Src2_Data >> 16);
    B0 = Src2_Data & ((1 << 16) - 1);

    T1 = A1*B0;
    T0 = A0*B1;

    T0 = (T1 + T0) & 0xFFFFFFFF;
    T1 = T0 < T1; // T1 : 0 또는 1.
  
    C1 = A1*B1;
    C0 = A0*B0;
    
    T = C0;
    C0 = (C0 + (T0 << 16)) & 0xFFFFFFFF;
    C1 = C1 + (T1 << 16) + (T0 >> 16) + (C0 < T);

    C = ((unsigned long long)C1 << 32) + C0;
    //printf("C = %llx ", C);
    return C;
    
}
int bi_Mul_Schoolbook(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    // bi_Dst 0으로 초기화한 상태
    int i, j;
    unsigned long long UV;
    for (i = 0; i < bi_Src1->bLen; i++){
        unsigned int U = 0x00;
        for (j = 0; j< bi_Src2->bLen; j++){
            UV = bi_Dst->bData[i+j] + (unsigned long long)(bi_Src1->bData[i])*bi_Src2->bData[j] + U;
            //UV = bi_Dst->bData[i+j] + bi_Mul_w(bi_Src1->bData[i], bi_Src2->bData[j]) + U;
            U = UV >> 32;
            unsigned int V = UV & 0xFFFFFFFF;
            bi_Dst->bData[i+j] = V;
        }
        bi_Dst->bData[i+j] = U;
    }

    if (bi_Src1->sign != bi_Src2->sign) bi_Dst->sign = NEGATIVE;
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}
int bi_Mul_PS(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){ // 잘못됨
    word R0 = 0, R1 = 0, R2 = 0;
    int i, k;
    unsigned int U,V;
    unsigned long long UV;
    //printf("bi_Src1 워드 길이 : %d\n", bi_Src1->bLen);
    //printf("k 최대 크기 : 0~%d\n", bi_Src1->bLen + bi_Src2->bLen - 2);
    for (k = 0; k < bi_Src1->bLen + bi_Src2->bLen-1; k++){ //for (k = 0; k < 2*(bi_Src1->bLen)-1; k++) 
        //unsigned int carry = 0;
        //printf("%08X \n", bi_Src1->bData[i]);
        for (i = 0; i <= k && i < bi_Src1->bLen; i++) { // k가 8넘어가면서 오류남. //k-i < bi_Src1->bLen 이걸 어디에 넣어야함.
            if (k-i < bi_Src2->bLen) {
                unsigned int carry = 0;
                UV = (unsigned long long)(bi_Src1->bData[i])*bi_Src2->bData[k-i];
                U = UV >> 32;
                V = UV & 0xFFFFFFFF;
                carry = bi_Add_w(&R0, R0, V, 0);
                carry = bi_Add_w(&R1, R1, U, carry);
                R2 = R2 + carry;
            }
        }
        //printf("%08X ", R0);
        bi_Dst->bData[k] = R0;
        R0 = R1;
        R1 = R2;
        R2 = 0;
    }
    //printf("%08X \n", R0);
    bi_Dst->bData[k] = R0;
    if (bi_Src1->sign != bi_Src2->sign) bi_Dst->sign = NEGATIVE;
    bi_refine(bi_Dst);
    return FUNC_SUCCESS;
}
int BI_Mul(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    bi_Dst->bLen = bi_Src1->bLen + bi_Src2->bLen;
    // 두 빅인티저 중 0이 있는 경우
    if (bi_is_zero(bi_Src1) || bi_is_zero(bi_Src2)) {
        bi_refine(bi_Dst);
        return FUNC_SUCCESS;
    }
    // 두 빅인티저 중 +- 1이 있는 경우.
    if (bi_is_one(bi_Src1)) {
        for(int i = 0; i < bi_Src2->bLen; i++){
                bi_Dst->bData[i] = bi_Src2->bData[i]; 
            }
        if (bi_Src1->sign != bi_Src2->sign) bi_Dst->sign = NEGATIVE;
        bi_refine(bi_Dst);    
        return FUNC_SUCCESS;
        }
    if (bi_is_one(bi_Src2)) {
        for(int i = 0; i < bi_Src1->bLen; i++){
                bi_Dst->bData[i] = bi_Src1->bData[i]; 
            }
        if (bi_Src1->sign != bi_Src2->sign) bi_Dst->sign = NEGATIVE;
        bi_refine(bi_Dst);  
        return FUNC_SUCCESS;
        }
    
    // 그 외 경우
    bi_Mul_PS(bi_Dst, bi_Src1, bi_Src2); // 이 부분이 bi_Mul_Schoolbook, bi_Mul_PS, bi_Mul_Karatsuba.
    
    return FUNC_SUCCESS;
}

int BI_Sqr(BIGINT* bi_Dst, BIGINT* bi_Src1){
    if (bi_Src1 == NULL || bi_Dst == NULL) return POINTER_ERROR;

    bi_Dst->bLen = bi_Src1->bLen + bi_Src1->bLen;
    bi_Dst->sign = NON_NEGATIVE;

    if (bi_is_zero(bi_Src1)){   /* bi_Src1이 0인 경우*/
        bi_Dst->sign = ZERO;
        bi_Dst->bLen = 0;
        return FUNC_SUCCESS;
    }
    if (bi_is_one(bi_Src1)){    /* bi_Src1이 (+,-)1인 경우*/
        bi_Dst->bData[0] = 1;
        return FUNC_SUCCESS;
    }
    bi_Sqr_PS(bi_Dst, bi_Src1); /* 그 외 경우*/

    return FUNC_SUCCESS;
}

unsigned long long bi_Sqr_w(word Src1_Data){
    unsigned int A1, A0;
    unsigned long long T;
    unsigned int C1, C0;
    unsigned long long C;

    A1 = (Src1_Data >> 16);
    A0 = Src1_Data & ((1 << 16) - 1);
    printf("A1 = %08X ", A1);
    printf("A0 = %08X\n", A0);

    C1 = A1*A1;
    C0 = A0*A0;
    printf("C1 = A1*A1 %08X ", C1);
    printf("C0 = A0*A0 %08X\n", C0);
    
    C = ((unsigned long long)C1 << 32) + C0;
    printf("C = ((unsigned long long)C1 << 32) + C0 = %llX\n", C);

    T = A0*A1;
    printf("T = A0*A1 = %llX\n", T);
    T = T << 17; //T = T << (16 +1)
    printf("T = (unsigned long long)T << 17 = %llX\n", T);
    C = C + T;

    return C;
    
}
int bi_Sqr_PS(BIGINT* bi_Dst, BIGINT* bi_Src1){
    // 길이 및 부호 설정 부분 
    bi_Dst->bLen = 2*bi_Src1->bLen;
    bi_Dst->sign = NON_NEGATIVE;

    word R0 = 0;
    word R1 = 0;
    word R2 = 0;
    int k = 0;
    int i = 0;
    unsigned int U,V;
    unsigned long long UV;
    unsigned long long UV2;

    for (k = 0; k < 2*(bi_Src1->bLen) - 1; k++){ // 2*(bi_Src1->bLen) 이게 맞는지 확인 필요.
        for (i = 0; i <= k/2 && i < bi_Src1->bLen; i++ ) { // i <= k/2
            if (k-i < bi_Src1->bLen){
                unsigned int carry = 0;
                UV = (unsigned long long)(bi_Src1->bData[i])*bi_Src1->bData[k-i];
                if (i < k-i) { 
                    UV2 = (2*UV) & 0xFFFFFFFFFFFFFFFF;
                    if (UV2 < UV) carry = 1; 
                    R2 = R2 + carry;
                    UV = UV2;
                    //break;                
                }
                U = UV >> 32;
                V = UV & 0xFFFFFFFF;
                carry = bi_Add_w(&R0, R0, V, 0);
                carry = bi_Add_w(&R1, R1, U, carry);
                R2 = R2 + carry;
            }
        }
        bi_Dst->bData[k] = R0;
        R0 = R1;
        R1 = R2;
        R2 = 0;
        //printf("%08X ", bi_Dst->bData[k]);
    }
    bi_Dst->bData[k] = R0;
    //printf("%08X ", bi_Dst->bData[k]);

    //printf("\n왜 오류\n");
    //bi_print_bigint(bi_Dst);
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}

int BI_Shift_RL_r_bits_x(BIGINT* bi_Src, int r) {
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen == 0) return FUNC_SUCCESS;

    int last_bLen = bi_Src->bLen;
    int word_shift = r / (sizeof(word)*8); // r 비트를 word(32비트)로 나눈 횟수
    int bit_shift = r % (sizeof(word)*8); // 나머지 비트 이동
    //printf("word shift = %d ", word_shift);
    //printf("bit shift = %d\n", bit_shift);
    
    if (bit_shift == 0) {          /* case 1) r = wk (k<n) */
        bi_Src->bLen = last_bLen + word_shift;
        for (int i = last_bLen - 1; i >= 0; i--){    /* a[bLen + word_shift - 1] ... a[word_shift] 채우기 */
            bi_Src->bData[word_shift + i] = bi_Src->bData[i];
        }
        for (int j = 0; j < word_shift; j++){    /* a[r - 1] ... a[0] 채우기 */
            bi_Src->bData[j] = 0;
        }
    }
    else {                         /* case 2) r = wk + r' */
        bi_Src->bLen = last_bLen + word_shift + 1;        
        bi_Src->bData[bi_Src->bLen - 1] = bi_Src->bData[last_bLen - 1] >> (sizeof(word)*8 - bit_shift);
        //printf("%08X \n", bi_Src->bData[bi_Src->bLen - 1]);
        for (int i = last_bLen-1; i >= 1; i--){
            //printf("%08X 얘를 움직임", (bi_Src->bData[i]));
            //printf("%08X ", (bi_Src->bData[i] << bit_shift));
            //printf("%08X 얘를 움직임", (bi_Src->bData[i-1]));
            //printf("%08X ->", (bi_Src->bData[i-1] >> (sizeof(word)*8 - bit_shift)));    
            
            word shitfed_word = (bi_Src->bData[i] << bit_shift) | (bi_Src->bData[i-1] >> (sizeof(word)*8 - bit_shift));
            bi_Src->bData[i + word_shift] = shitfed_word;
            //printf("%08X\n", bi_Src->bData[i + word_shift]);
            
        }
        //printf("끝");
        bi_Src->bData[word_shift] = bi_Src->bData[0] << bit_shift;
        for (int j = word_shift - 1; j >= 0; j--){
            bi_Src->bData[j] = 0;
            //printf("%08X ", bi_Src->bData[j]);
        }
    }    
    return FUNC_SUCCESS;
}
int BI_Shift_LR_r_bits_x(BIGINT* bi_Src, int r) {
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen == 0) return FUNC_SUCCESS;
    
    int last_bLen = bi_Src->bLen;
    int word_shift = r / (sizeof(word)*8); // r 비트를 word(32비트)로 나눈 횟수
    int bit_shift = r % (sizeof(word)*8); // 나머지 비트 이동
    printf("word shift = %d ", word_shift);
    printf("bit shift = %d\n", bit_shift);    

    bi_Src->bLen = bi_Src->bLen - word_shift;
    printf("비트 크기 : %lu\n", last_bLen * (sizeof(word)*8));
    printf("바뀐 길이 : %d\n", bi_Src->bLen);
    if (r >= last_bLen * (sizeof(word)*8)) {  /* case 1) r >= wn */
        bi_Src->bLen = 0;
        bi_Src->sign = ZERO;
        return FUNC_SUCCESS;
    }
    else if (bit_shift == 0) {          /* case 2) r = wk (k<n) */
        for (int i = 0; i < bi_Src->bLen; i++){
            bi_Src->bData[i] = bi_Src->bData[word_shift + i];
        }
    }
    else {                              /* case 3) r = wk + r' */
        for (int i = 0; i < bi_Src->bLen - 1; i++){
            word shitfed_word = (bi_Src->bData[word_shift+1+i] << ((sizeof(word)*8) - bit_shift)) | (bi_Src->bData[word_shift+i] >> bit_shift);
            bi_Src->bData[i] = shitfed_word;
        }
        printf("%08X \n", bi_Src->bData[last_bLen - 1]);
        bi_Src->bData[bi_Src->bLen - 1] = bi_Src->bData[last_bLen-1] >> bit_shift;
        printf("%08X \n", bi_Src->bData[bi_Src->bLen - 1]);
    }
    bi_refine(bi_Src);

    return FUNC_SUCCESS;
}
int BI_Shift_RL_r_bits_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int r) {
    if (bi_Src == NULL || bi_Dst == NULL) return POINTER_ERROR;
    if (bi_Src->bLen == 0) return FUNC_SUCCESS;

    int SLen = bi_Src->bLen;
    int word_shift = r / (sizeof(word)*8); // r 비트를 word(32비트)로 나눈 횟수
    int bit_shift = r % (sizeof(word)*8); // 나머지 비트 이동

    bi_Dst->sign = bi_Src->sign;
    if (bit_shift == 0) {          /* case 1) r = wk (k<n) */
        bi_Dst->bLen = SLen + word_shift;
        for (int i = SLen - 1; i >= 0; i--){    /* a[bLen + word_shift - 1] ... a[word_shift] 채우기 */
            bi_Dst->bData[word_shift + i] = bi_Src->bData[i];
        }
        for (int j = 0; j < word_shift; j++){    /* a[r - 1] ... a[0] 채우기 */
            bi_Dst->bData[j] = 0;
        }
    }
    else {                         /* case 2) r = wk + r' */
        bi_Dst->bLen = SLen + word_shift + 1;        
        bi_Dst->bData[bi_Dst->bLen - 1] = bi_Src->bData[SLen - 1] >> (sizeof(word)*8 - bit_shift);
        for (int i = SLen-1; i >= 1; i--){
            word shitfed_word = (bi_Src->bData[i] << bit_shift) | (bi_Src->bData[i-1] >> (sizeof(word)*8 - bit_shift));
            bi_Dst->bData[i + word_shift] = shitfed_word; 
        }
        bi_Dst->bData[word_shift] = bi_Src->bData[0] << bit_shift;
        for (int j = word_shift - 1; j >= 0; j--){
            bi_Dst->bData[j] = 0;
        }
    }
    return FUNC_SUCCESS;
}
int BI_Shift_LR_r_bits_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int r) {
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen == 0) return FUNC_SUCCESS;

    int SLen = bi_Src->bLen;
    int word_shift = r / (sizeof(word)*8); // r 비트를 word(32비트)로 나눈 횟수
    int bit_shift = r % (sizeof(word)*8); // 나머지 비트 이동
    
    bi_Dst->sign = bi_Src->sign;
    bi_Dst->bLen = bi_Src->bLen - word_shift;

    if (r >= SLen * (sizeof(word)*8)) {  /* case 1) r >= wn */
        bi_Dst->bLen = 0;
        bi_Dst->sign = ZERO;
        return FUNC_SUCCESS;
    }
    else if (bit_shift == 0) {          /* case 2) r = wk (k<n) */
        for (int i = 0; i < bi_Dst->bLen; i++){
            bi_Dst->bData[i] = bi_Src->bData[word_shift + i];
        }
    }
    else {                              /* case 3) r = wk + r' */
        for (int i = 0; i < bi_Dst->bLen - 1; i++){
            word shitfed_word = (bi_Src->bData[word_shift+1+i] << ((sizeof(word)*8) - bit_shift)) | (bi_Src->bData[word_shift+i] >> bit_shift);
            bi_Dst->bData[i] = shitfed_word;
        }
        bi_Dst->bData[bi_Dst->bLen - 1] = bi_Src->bData[SLen-1] >> bit_shift;
    }
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}

int bitLength(int n) {
    int length = 0;

    while (n > 0) {
        n >>= 1; // n을 오른쪽으로 1 비트 시프트
        length++;
    }

    return length;
}

int bi_Exp_LR_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int n){
    if (bi_Src == NULL || bi_Dst == NULL) return POINTER_ERROR;
    bi_Dst->bLen = 1;
    bi_Dst->bData[0] = 1;
    int length = bitLength(n);

    while (length > 0){
        BIGINT temp1, temp2;
        word temp1_bdata[MAX_BINT_LEN] = {0, }, temp2_bdata[MAX_BINT_LEN] = {0, };
        bi_create(&temp1, ZERO, temp1_bdata);
        bi_create(&temp2, ZERO, temp2_bdata);        
        bi_Sqr_PS(&temp1, bi_Dst);
        //BI_Sqr(&temp1, bi_Dst);
        if(((n >> (length-1)) & 1) == 1){
            BI_Mul(&temp2, &temp1, bi_Src);
            bi_copy_bigint(bi_Dst, &temp2);
        }
        else bi_copy_bigint(bi_Dst, &temp1);
        //printf(((n >> (length-1)) & 0x1));
        length--;
    }

    return FUNC_SUCCESS;
}


//===============================================================================================//

//===============================================================================================//
int main(){
    BIGINT b0, b1, b2, b3, b4, b5, b6, b7, b8;
    BIGINT r0, r1, r2, r3, r4, r5, r6, r7, r8;
    word b0_bdata[MAX_BINT_LEN], b1_bdata[MAX_BINT_LEN], b2_bdata[MAX_BINT_LEN],
        b3_bdata[MAX_BINT_LEN], b4_bdata[MAX_BINT_LEN], b5_bdata[MAX_BINT_LEN], 
        b6_bdata[MAX_BINT_LEN], b7_bdata[MAX_BINT_LEN], b8_bdata[MAX_BINT_LEN];

    word r0_bdata[MAX_BINT_LEN] = {0, }, r1_bdata[MAX_BINT_LEN] = {0, }, r2_bdata[MAX_BINT_LEN] = {0, },
        r3_bdata[MAX_BINT_LEN] = {0, }, r4_bdata[MAX_BINT_LEN] = {0, }, r6_bdata[MAX_BINT_LEN]= {0, },
        r7_bdata[MAX_BINT_LEN] = {0, }, r8_bdata[MAX_BINT_LEN] = {0, }, r5_bdata[MAX_BINT_LEN] = {0, };
    
    bi_create(&r0, ZERO, r0_bdata);
    bi_create(&r1, ZERO, r1_bdata);
    bi_create(&r2, ZERO, r2_bdata);
    bi_create(&r3, ZERO, r3_bdata);
    bi_create(&r4, ZERO, r4_bdata);

    /*
    printf("=============  빅인티저 생성 검증 시작 ============== \n");
    //bi_set_by_string(&b0, NON_NEGATIVE, b0_bdata, "00000000");
    bi_set_by_string(&b1, NON_NEGATIVE, b1_bdata, "FFFFFFFFFFFFFFFF");
    bi_set_by_string(&b2, NEGATIVE, b2_bdata, "FFFFFFFFFFFFFFFF");
    bi_set_by_string(&b3, NON_NEGATIVE, b3_bdata, "1");
    bi_set_by_string(&b4, NEGATIVE, b4_bdata, "1");
    
    bi_print_bigint(&r0);
    bi_print_bigint(&b1);
    bi_print_bigint(&b2);
    printf("=============  빅인티저 생성 검증 종료 ============== \n\n");
    
    printf("=============  빅인티저 복사 검증 시작 ============== \n");
    bi_copy_bigint(&r1, &r0);
    printf("=============  빅인티저 복사 검증 종료 ============== \n\n");

    printf("=============  빅인티저 크기 검증 시작 ============== \n");
    printf("크기 비교 결과 : ");
    printf("%d\n", bi_compare_bigint(&b2, &b1));
    printf("=============  빅인티저 크기 검증 종료 ============== \n\n");

    printf("=============  빅인티저 is_zero 검증 시작 ============== \n");
    printf("0 판독 결과 : ");
    printf("%d\n", bi_is_zero(&r1));
    printf("=============  빅인티저 is_zero 검증 종료 ============== \n\n");
     
    printf("=============  빅인티저 생성 검증 종료 ============== \n");

    printf("=============  빅인티저 덧셈 검증 시작 ============== \n");
    BI_Sub(&r0, &b1, &b1); // FFFFFFFF FFFFFFFF - FFFFFFFF FFFFFFFF
    BI_Sub(&r1, &b1, &b2); // FFFFFFFF FFFFFFFF - (-FFFFFFFF FFFFFFFF) 
    BI_Sub(&r2, &b1, &b3); // FFFFFFFF FFFFFFFF - 1 -> FFFFFFFF FFFFFFFE 
    BI_Sub(&r3, &b2, &b4); // (-FFFFFFFF FFFFFFFF) - (-1) -> -FFFFFFFF FFFFFFFE 
    BI_Sub(&r4, &r2, &r3); // FFFFFFFF FFFFFFFE - -FFFFFFFF FFFFFFFE 
    
    bi_print_bigint(&r0);
    bi_print_bigint(&r1);
    bi_print_bigint(&r2);
    bi_print_bigint(&r3);
    bi_print_bigint(&r4); 
    printf("=============  빅인티저 덧셈  검증 종료 ============== \n\n");
    
    printf("=============  빅인티저 곱셈  검증 시작 ============== \n\n");
    bi_set_by_string(&b5, NON_NEGATIVE, b5_bdata, "BD91C935C85617B079C6F2728B987CE488BB17B4644D5F8B9C23AF955AB74663");
    bi_set_by_string(&b6, NEGATIVE, b6_bdata,"09745AE9B45ECD62652851E0D1EF8A496B9C64E24CE34D0DED336E398902D302");
    bi_create(&r5, ZERO, r5_bdata);
    bi_print_bigint(&b5);
    bi_print_bigint(&b6);
    bi_print_bigint(&r5);

    BI_Mul(&r5, &b5, &b6);
    bi_print_bigint(&b5);
    bi_print_bigint(&b6);
    bi_print_bigint(&r5);

    printf("=============  빅인티저 곱셈  검증 종료 ============== \n\n");
    
    printf("=============  빅인티저 제곱  검증 시작 ============== \n\n");
    bi_set_by_string(&b5, NON_NEGATIVE, b5_bdata, "1234567890abcdef1234567890abcdef1234567890abcdef");
    bi_create(&r6, ZERO, r6_bdata);
    bi_Sqr_PS(&r6, &b5);
    bi_print_bigint(&r6);
    printf("=============  빅인티저 제곱  검증 종료 ============== \n\n");

    
    printf("=============  빅인티저 시프트 연산  검증 시작 ============== \n\n");
    bi_set_by_string(&b7, NON_NEGATIVE, b7_bdata, "1234567890abcdef1234567890abcdef1234567890abcdef");
    printf("시프트 전 : ");    
    bi_print_bigint(&b7);
    int r = 48*4-1;
    bi_shift_LR_r_bits_x(&b7, r);
    bi_print_bigint(&b7);    

    bi_create(&r7, ZERO, r7_bdata);
    bi_set_by_string(&b8, NON_NEGATIVE, b8_bdata, "1234567890abcdef1234567890abcdef1234567890abcdef");
    bi_shift_LR_r_bits_zx(&r7, &b8, r);
    bi_print_bigint(&r7);    
    printf("=============  빅인티저 A 시프트 연산 검증 종료 ============== \n\n");
    */

    printf("=============  빅인티저 지수승 연산 검증 시작 ============== \n\n");
    bi_create(&r7, ZERO, r7_bdata);
    bi_set_by_string(&b8, NON_NEGATIVE, b8_bdata, "1234567890abcdef1234567890abcdef1234567890abcdef");
    int n = 10; // MAX_BINT_LEN 때문에 숫자 커지면 배열에서 참조못함.
    bi_Exp_LR_zx(&r7, &b8, n);
    bi_print_bigint(&r7);
    printf("빅인티저 길이 = %d\n", bi_get_length(&r7));
    printf("=============  빅인티저 지수승 연산 검증 종료 ============== \n\n");
    
    return 0;
}
