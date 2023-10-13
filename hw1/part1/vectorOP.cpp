#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask traverseMask, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    traverseMask = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, traverseMask); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, traverseMask); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, traverseMask);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  
  // Const
  __pp_vec_float clampMaximum = _pp_vset_float(9.999999f);
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one = _pp_vset_int(1);

  // Variable
  __pp_vec_float valuesVector, outputVector;
  __pp_vec_int exponentsVector;
  __pp_mask traverseMask, zeroLikeMask, remainMask, newZeroLikeMask, nonNewZeroLike, resultNeedClampMask;
  
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // 定義了本次迴圈應該走訪的Mask長度
    int traverseLength = (N - i) < VECTOR_WIDTH ? (N - i) : VECTOR_WIDTH;
    traverseMask = _pp_init_ones(traverseLength);

    // 把values跟exponents值讀進Vector裡
    _pp_vload_float(valuesVector, values + i, traverseMask);
    _pp_vload_int(exponentsVector, exponents + i, traverseMask);

    // 取得exponents是0的Mask並且把對應的output設成1.f
    _pp_veq_int(zeroLikeMask, zero, exponentsVector, traverseMask);
    _pp_vset_float(outputVector, 1.f, zeroLikeMask);

    // 取得剩下需要進行exponential運算的Mask
    remainMask = _pp_mask_not(zeroLikeMask);
    remainMask = _pp_mask_and(remainMask, traverseMask);

    // 先進行1次方的運算 => 讓output的值等於value
    _pp_vmove_float(outputVector, valuesVector, remainMask);

    // 跑一個loop直到vector裡面的所有exponents都變成0
    while (_pp_cntbits(remainMask) != 0) {

      // 將exponents的值 - 1並取得最新變成0的Mask
      _pp_vsub_int(exponentsVector, exponentsVector, one, remainMask);
      _pp_veq_int(newZeroLikeMask, zero, exponentsVector, remainMask);

      // 計算還是有 > 0的Mask
      nonNewZeroLike = _pp_mask_not(newZeroLikeMask);
      remainMask = _pp_mask_and(nonNewZeroLike, remainMask);
      
      // 對於那些index計算 output = output * value
      _pp_vmult_float(outputVector, outputVector, valuesVector, remainMask);
    }

    // 取得超過clampValue的Mask
    _pp_vlt_float(resultNeedClampMask, outputVector, clampMaximum, traverseMask);
    resultNeedClampMask = _pp_mask_not(resultNeedClampMask);
    resultNeedClampMask = _pp_mask_and(resultNeedClampMask, traverseMask);
    
    // 將那些Index的output進行轉換
    _pp_vset_float(outputVector, 9.999999f, resultNeedClampMask);

    // 最後把運算完的output寫回去記憶體
    _pp_vstore_float(output + i, outputVector, traverseMask);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //

  // Variable
  __pp_mask traverseMask = _pp_init_ones();
  __pp_vec_float valuesVector;
  __pp_vec_float storeVector = _pp_vset_float(0.f);
  float result = 0.f;

  // 先將每個vector疊在一個store vector上
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_float(valuesVector, values + i, traverseMask);
    _pp_vadd_float(storeVector, storeVector, valuesVector, traverseMask);
  }

  // 最後再把該store vector做總和
  for (int j = VECTOR_WIDTH; j > 1; j /= 2){
      _pp_hadd_float(storeVector, storeVector);
      _pp_interleave_float(storeVector, storeVector);
  }
    
  return storeVector.value[0];
}

