#include "bigint_arith.h"
//===============================================================================================//
//                                       사용자용 함수 구역
//                                     사용자가 직접 사용할 함수

int BI_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    if (bi_Src1 == NULL || bi_Src2 == NULL || bi_Dst == NULL) return POINTER_ERROR;
    // case 1: bi_Src1이 0인 경우
    if (bi_Src1->bLen == 0) return bi_copy_bigint(bi_Dst, bi_Src2);
    // case 2: bi_Src2가 0인 경우
    if (bi_Src2->bLen == 0) return bi_copy_bigint(bi_Dst, bi_Src1);
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
//===============================================================================================//

//===============================================================================================//
//                                       개발자용 함수 구역
//                                  사용자는 직접 사용하지 않는 함수
//
/**** bi_add 내부에 들어갈 덧셈 함수, 단일 워드 덧셈 ****/
// 리턴 값 : 캐리, Src1의 길이가 Src2의 길이보다 크거나 같도록함. 
int bi_Add_w(word* Dst_bData, word Src1_Data, word Src2_Data, int carry_in){
    int carry_out = 0;

    *Dst_bData = (Src1_Data + Src2_Data) & 0xFFFFFFFF;
    if (*Dst_bData < Src1_Data) carry_out = 1;

    *Dst_bData = (*Dst_bData + carry_in) & 0xFFFFFFFF;
    if (*Dst_bData < carry_in) carry_out += 1;

    return carry_out; // 0 또는 1
}

/**** BI_Add 내부에 들어갈 덧셈 함수, 다중 워드 덧셈 ****/
// 리턴 값 : 함수의 정상 작동 여부, Src1의 길이가 Src2의 길이보다 크거나 같도록함.
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
        for ( ; i < len_long; i++){
            carry = bi_Add_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], carry);
        }
        if (carry){
            bi_Dst->bData[bi_Dst->bLen-1] = carry;
        }
    }
    //=================================== bi_Src1가 더 긴 경우 ===================================//
    else {
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

//===============================================================================================//


// 매개변수 및 인자 수정버전

/*
int bi_Add(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){

    if (bi_Src1->bLen < bi_Src2->bLen) return SIZE_ERROR;

    //=================================== 이외의 경우 ===================================//
    int carry = 0;
    int len_long = bi_Src1->bLen; 
    int len_short = bi_Src2->bLen;
    int i = 0;
    //=================================== bi_Src1,2 길이 같은 경우 ===================================//
    if(len_long == len_short){
        printf("길이가 같은 경우의 덧셈\n");
        for ( ; i < len_long; i++){
            carry = bi_Add_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], carry);
        }
        printf("최종 캐리 : %d\n", carry);
        if (carry){
            bi_Dst->bData[bi_Dst->bLen-1] = carry;
        }
        bi_print_bigint(bi_Dst);
    }
    //=================================== bi_Src1가 더 긴 경우 ===================================//
    else {
        printf("bi_Src1의 길이가 더 긴 경우의 덧셈\n");
        bi_Dst->bLen = len_long; // 결과 값은 긴쪽 길이로 설정.
        //====================== 우선 짧은 길이만큼 덧셈 ======================//
        for( ; i < len_short; i++){
            carry = bi_Add_w(&bi_Dst->bData[i], bi_Src1->bData[i], bi_Src2->bData[i], carry);
        }
        printf("bi_Src2의 길이까지의 덧셈 후 나온 캐리 : %d\n", carry);
        // i = len_short인 상태
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
    printf("덧셈 함수 내에서 최종 덧셈 결과 : ");
    bi_print_bigint(bi_Dst);
    printf("\n");
    return FUNC_SUCCESS;
}
*/

/*
int bi_Add2(word* Dst_bData, word* Src1_bData, word* Src2_bData, int Src1Len, int Src2Len){
    //=================================== 이외의 경우 ===================================//
    int carry = 0;
    int i = 0;
    //=================================== bi_Src1,2 길이 같은 경우 ===================================//
    if(Src1Len == Src2Len){
        // bi_Dst의 bLen은 만들어서 넣기
        for ( ; i < Src1Len; i++){
            carry = bi_Add_w(Dst_bData[i], Src1_bData[i], Src2_bData[i], carry);
        }
        if (carry){
            Dst_bData[i] = carry;
        }
    }
    //=================================== bi_Src1가 더 긴 경우 ===================================//
    else {
        //====================== 우선 짧은 길이만큼 덧셈 ======================//
        for( ; i < Src2Len; i++){
            carry = bi_Add_w(Dst_bData[i], Src1_bData[i], Src2_bData[i], carry);
        }
        // i = Src2Len인 상태
        //========== 캐리가 없는 경우 ==========//
        if (carry == 0){
            for( ; i < Src1Len; i++){
                Dst_bData[i] = Src1_bData[i]; 
            }
        }
        //========== 캐리가 있는 경우 ==========//
        else {
            for ( ; i < Src1Len; i++){
                Dst_bData[i] = (Src1_bData[i] + carry) & 0xFFFFFFFF;
                carry = (Dst_bData[i] < carry) ? 1 : 0;   
            }
        }
        //====================== 길이가 더 늘어나는지 판단하는 부분 ======================//
        if (carry){
            Dst_bData[i] = carry;
        }
    }
    return FUNC_SUCCESS;
}
int BI_Add2(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    
    // case 1: bi_Src1이 0인 경우
    if (bi_is_zero(bi_Src1)) return bi_copy_bigint(bi_Dst, bi_Src2);

    // case 2: bi_Src2가 0인 경우
    if (bi_is_zero(bi_Src2)) return bi_copy_bigint(bi_Dst, bi_Src1);

    // case 3: 부호 다른 경우
    if (bi_Src1->sign == NON_NEGATIVE && bi_Src2->sign == NEGATIVE) {
        // 음수가 더 절댓값이 큰 경우
        if (bi_compare_ABS(bi_Src1, bi_Src2) == -1){ 
            bi_Dst->sign = NEGATIVE;
            return bi_Sub(bi_Dst, bi_Src2, bi_Src1);
        }
        else {
            return bi_Sub(bi_Dst, bi_Src1, bi_Src2);
        }
    }

    if (bi_Src1->sign == NEGATIVE && bi_Src2->sign == NON_NEGATIVE) {

        if (bi_compare_ABS(bi_Src1, bi_Src2) == 1) {
            bi_Dst->sign = NEGATIVE;
            return bi_Sub(bi_Dst, bi_Src1, bi_Src2);     
        }
        else {
            return bi_Sub(bi_Dst, bi_Src2, bi_Src1);
        }
        
    }
    // case 4: 부호 같은 경우
    else 
        if (bi_Src1->sign == NEGATIVE) bi_Dst->sign = NEGATIVE;
        if (bi_Src1->bLen >= bi_Src2->bLen) 
            return bi_Add2(bi_Dst->bData, bi_Src1->bData, bi_Src2->bData, bi_Src1->bLen, bi_Src2->bLen);
        else return bi_Add2(bi_Dst->bData, bi_Src2->bData, bi_Src1->bData, bi_Src2->bLen, bi_Src1->bLen);
}
*/