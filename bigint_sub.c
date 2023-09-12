#include "bigint_arith.h"
//===============================================================================================//
//                                       사용자용 함수 구역
//                                     사용자가 직접 사용할 함수
//
int BI_Sub(BIGINT* bi_Dst, BIGINT* bi_Src1, BIGINT* bi_Src2){
    if (bi_Src1 == NULL || bi_Src2 == NULL || bi_Dst == NULL) return POINTER_ERROR;
    // case 1: bi_Src1이 0인 경우, 부호 변경하여 복사
    if (bi_Src1->bLen == 0) return bi_copy2_bigint(bi_Dst, bi_Src2);
    // case 2: bi_Src2가 0인 경우, 부호 유지하며 복사
    if (bi_Src1->bLen == 0) return bi_copy_bigint(bi_Dst, bi_Src1);
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
//===============================================================================================//

//===============================================================================================//
//                                       개발자용 함수 구역
//                                  사용자는 직접 사용하지 않는 함수
//
/**** bi_sub 내부에 들어갈 뺄셈 함수, 단일 워드 뺄셈 ****/
// 리턴 값 : 캐리, Src1의 길이가 Src2의 길이보다 크거나 같도록 넣을 예정.
int bi_Sub_w(word* Dst_bData, word Src1_Data, word Src2_Data, int borrow_in) {
    int borrow_out = 0;

    *Dst_bData = (Src1_Data - borrow_in) & 0xFFFFFFFF;
    if(Src1_Data < borrow_in) borrow_out = 1;
    if(*Dst_bData < Src2_Data) borrow_out += 1;

    *Dst_bData = (*Dst_bData - Src2_Data) & 0xFFFFFFFF;

    return borrow_out; // 0 또는 1
}

/**** BI_Sub 내부에 들어갈 뺄셈 함수, 다중 워드 뺄셈 ****/
// 리턴 값 : 함수의 정상 작동 여부, 둘다 양수로 보고 계산, Src1의 길이가 Src2의 길이보다 크거나 같도록 넣을 예정.
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
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}
//===============================================================================================//
