#include "bigint_util.h"
//################################################################################################# 
//                                  BIGINT 생성 및 소멸관련 함수
//#################################################################################################

/**** 길이 정보를 받아서 데이터를 초기화하는 빅인티저 생성 함수 ****/
void bi_create(BIGINT* bi_Dst, int sign, word* bData)
{
    bi_Dst->sign = sign; // ZERO로 초기화
    bi_Dst->bLen = 0;
    bi_Dst->bData = bData;
}

/**** 16진수 문자열 -> 빅인티저 구조체 함수 ****/
int bi_set_by_string(BIGINT* bi_Dst, int sign, word* Data, char* str)
{
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
    printf("str_len : %d /// ", str_len);
    printf("word_len : %d\n", word_len);

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
        printf("bData[%d] : %08X /// ", i, bi_Dst->bData[i]);
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
        printf("카피 오류 : 포인터 없음");
        return NULL_POINTER_ERROR; // 포인터 에러 
    }

    bi_Dst->bLen = bi_Src->bLen;
    bi_Dst->sign = bi_Src->sign;

    for (int i = 0; i < bi_Src->bLen; i++){
        bi_Dst->bData[i] = bi_Src->bData[i];
    }

    printf("복사 결과 [bi_Dst] : ");
    bi_print_bigint(bi_Dst);
    
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
// true 1, false 0,-1 
int bi_is_zero(BIGINT* bi_Src){
    if (bi_Src->bLen == 0 && bi_Src->sign == ZERO) return 1;
    else if(bi_Src->bLen == 0) {
        printf("길이는 0, 부호는 ZERO가 아님\n");
        return -1;
    }
    else return 0;
}
int bi_is_one(BIGINT* bi_Src){
    
    if (bi_Src->sign == NEGATIVE || bi_Src->bData[0] != 1) return 0;

    for (int i = bi_Src->bLen - 1; i >= 1; i--) {
        if (bi_Src->bData[i] != 0) return 0;
    }
    return 1;
}
/**** 0 만드는 함수****/
int bi_set_zero(BIGINT* bi_Src){
    if (bi_Src == NULL) return POINTER_ERROR;
    bi_Src->sign = ZERO;
    bi_Src->bLen = 0;
    return FUNC_SUCCESS;
}
/**** 1 만드는 함수****/
int bi_set_one(BIGINT* bi_Src){
    if (bi_Src == NULL) return POINTER_ERROR;
    bi_Src->sign = NON_NEGATIVE;
    bi_Src->bLen = 1;
    bi_Src->bData[0] = 0x1; 
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