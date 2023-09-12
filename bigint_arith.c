#include "bigint_arith.h"
//===============================================================================================//
//                                       사용자용 함수 구역
//                                     사용자가 직접 사용할 함수
//
/* Shift Right-to-Left R-words */
// 최대 범위 넘어가는거 에러뜨도록 해야하나?
/* bLen이 0이 될 경우, 데이터 변화가 없음. 0 판별할 때 부호정보 대신 길이 정보만 사용해야 함.*/
/* 남는 데이터는 어떻게 처리하나? 그냥 냅두나? */
int bi_Shift_RL_r_words(BIGINT* bi_Src, int r){
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen + r > MAX_BINT_LEN) return SIZE_ERROR;
    int i;
    for (i = bi_Src->bLen - 1; i >= 0; i--){    /* a[bLen + r - 1] ... a[r] 채우기 */
        bi_Src->bData[i + r] = bi_Src->bData[i];
    }
    bi_Src->bLen = bi_Src->bLen + r;
    for (i = 0; i < r; i++){    /* a[r - 1] ... a[0] 채우기 */
        bi_Src->bData[i] = 0;
    }
    return FUNC_SUCCESS;
}
/* Shift Left-to-Right R-words */
int bi_Shift_LR_r_words(BIGINT* bi_Src, int r){
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen - r < 0) return SIZE_ERROR;

    bi_Src->bLen = bi_Src->bLen - r;

    for (int i = 0; i < bi_Src->bLen; i++){
        bi_Src->bData[i] = bi_Src->bData[r + i];
    }

    return FUNC_SUCCESS;
}

/**** A << r bits ****/
int BI_Shift_RL_r_bits_x(BIGINT* bi_Src, int r) {
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen == 0) return FUNC_SUCCESS;

    int last_bLen = bi_Src->bLen;
    int word_shift = r / (sizeof(word)*8); // r 비트를 word(32비트)로 나눈 횟수
    int bit_shift = r % (sizeof(word)*8); // 나머지 비트 이동
    
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
        for (int i = last_bLen-1; i >= 1; i--){
            word shitfed_word = (bi_Src->bData[i] << bit_shift) 
                | (bi_Src->bData[i-1] >> (sizeof(word)*8 - bit_shift));
            bi_Src->bData[i + word_shift] = shitfed_word; 
        }
        bi_Src->bData[word_shift] = bi_Src->bData[0] << bit_shift;
        for (int j = word_shift - 1; j >= 0; j--){
            bi_Src->bData[j] = 0;
        }
    }
    return FUNC_SUCCESS;
}
/**** A >> r bits ****/
int BI_Shift_LR_r_bits_x(BIGINT* bi_Src, int r) {
    if (bi_Src == NULL) return POINTER_ERROR;
    if (bi_Src->bLen == 0) return FUNC_SUCCESS;

    int last_bLen = bi_Src->bLen;
    int word_shift = r / (sizeof(word)*8); // r 비트를 word(32비트)로 나눈 횟수
    int bit_shift = r % (sizeof(word)*8); // 나머지 비트 이동

    bi_Src->bLen = bi_Src->bLen - word_shift;

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
            word shitfed_word = (bi_Src->bData[word_shift+1+i] << ((sizeof(word)*8) - bit_shift)) 
                | (bi_Src->bData[word_shift+i] >> bit_shift);
            bi_Src->bData[i] = shitfed_word;
        }
        bi_Src->bData[bi_Src->bLen - 1] = bi_Src->bData[last_bLen-1] >> bit_shift;
    }
    bi_refine(bi_Src);

    return FUNC_SUCCESS;
}
/**** A << r bits ****/
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
            word shitfed_word = (bi_Src->bData[i] << bit_shift) 
                | (bi_Src->bData[i-1] >> (sizeof(word)*8 - bit_shift));
            bi_Dst->bData[i + word_shift] = shitfed_word; 
        }
        bi_Dst->bData[word_shift] = bi_Src->bData[0] << bit_shift;
        for (int j = word_shift - 1; j >= 0; j--){
            bi_Dst->bData[j] = 0;
        }
    }
    return FUNC_SUCCESS;
}
/**** A >> r bits ****/
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
            word shitfed_word = (bi_Src->bData[word_shift+1+i] << ((sizeof(word)*8) - bit_shift)) 
                | (bi_Src->bData[word_shift+i] >> bit_shift);
            bi_Dst->bData[i] = shitfed_word;
        }
        bi_Dst->bData[bi_Dst->bLen - 1] = bi_Src->bData[SLen-1] >> bit_shift;
    }
    bi_refine(bi_Dst);

    return FUNC_SUCCESS;
}

/**** Left-to-Right exponentiation  ****/
int bi_Exp_LR_zx(BIGINT* bi_Dst, BIGINT* bi_Src, int n){
    if (bi_Src == NULL || bi_Dst == NULL) return POINTER_ERROR;
    bi_Dst->bLen = 1;
    bi_Dst->bData[0] = 1;
    int n2 = n;

    int length = 0;
    while (n2 > 0) {
        n2 >>= 1;
        length++; 
    }

    while (length >= 0){
        bi_Sqr_PS(bi_Dst, bi_Dst);
        if(((n >> (length-1)) & 0x1) == 1) BI_Mul(bi_Dst, bi_Dst, bi_Src);
        length--;
    }

    return FUNC_SUCCESS;
}
