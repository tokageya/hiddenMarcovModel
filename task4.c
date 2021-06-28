#include "hidden_markov_model.h"

/* VITERBI_INIT_COUNT : viterbi学習を行う回数 */
#define VITERBI_INIT_COUNT (100)

/* probability_diff_sum : 2つの HMM の各確率パラメータの差の合計を求める */
double probability_diff_sum(HMM *hmm1, HMM *hmm2){
  double sumDiffP = 0.0;
  for(int nowState=0; nowState<hmm1->statesNum; nowState++){
    for(int nextState=0; nextState<hmm1->statesNum; nextState++){
      sumDiffP += fabs(hmm1->delta[nowState][nextState] - hmm2->delta[nowState][nextState]);
    }
    for(int symbolIndex=0; symbolIndex<hmm1->sigmasNum; symbolIndex++){
      sumDiffP += fabs(hmm1->e[nowState][symbolIndex] - hmm2->e[nowState][symbolIndex]);
    }
  }
  return sumDiffP;
}

int main(int argc, char *argv[]){
  FILE *fp;
  HMM *hmm;
  hmm = (HMM*)malloc(sizeof(HMM));

  if((fp = fopen(argv[1], "r")) == NULL){
    fprintf(stderr, "Error : file can not be opened.\n");
    return 1;
  }
  
  if( !read_HMM(fp, hmm) ){
    fprintf(stderr, "Error : in main, function \"read_HMM\" is not correct.\n");
    fclose(fp);
    return 1;
  }
  fclose(fp);

  // 手順1. 隠れマルコフモデルを用いて記号列とそれに対応する状態遷移列を 1000 個生成する //
  srand((unsigned)time(NULL)); // 乱数のシードを設定
  char symbolStringSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1]; // +1 は終端文字
  int stateOrderSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1]; // +1 は最終状態
  for(int count=0; count<MAX_SET; count++){
    if( !generate_symbolString_with_stateOrder(hmm, symbolStringSet[count], MAX_SYMBOL_STRING_LEN, stateOrderSet[count]) ){
      fprintf(stderr, "Error : in main, function \"generate_symbolString_with_stateOrder\" is not correct.\n");
      return 1;
    }
  }

  // 手順2. 記号列のみからのViterbi学習を100回実行して、もっとも確率パラメータが図2の隠れマルコフモデルに近いものを求める //

  HMM *nearestHmm; // hmm に一番近いような確率パラメータとなる HMM
  nearestHmm = (HMM*)malloc(sizeof(HMM));
  copy_HMM(hmm, nearestHmm);

  double minSumP = 1.797693e+308; // hmm と nearestHmm の確率パラメータの合計の最小値


  for(int predictCount=0; predictCount<VITERBI_INIT_COUNT; predictCount++){
    printf(" ===== in main, now predictCount = %3d ===== \n", predictCount);
    HMM *tmpHmm; // 学習で求めた HMM
    tmpHmm = (HMM*)malloc(sizeof(HMM));
    copy_HMM(hmm, tmpHmm);
    if( !learning_HMM_only_sum_symbolStrings(tmpHmm, symbolStringSet, hmm->statesNum, MAX_SET) ){
      fprintf(stderr, "Error : in main.\n");
      return 1;
    }
    double tmpSumP = probability_diff_sum(hmm, tmpHmm);
    if(tmpSumP < minSumP){
      copy_HMM(tmpHmm, nearestHmm);
      minSumP = tmpSumP;
    }
    free(tmpHmm);   
  }
  printf(" ===== predict end ===== \n");
  printf("\n------------ correct hmm ----------------\n\n");
  print_HMM(hmm);
  printf("\n\n\n------------ nearestHmm ----------------\n\n");
  print_HMM(nearestHmm);

  printf("==================\n");
  printf("minSumP = %lf\n", minSumP);
  printf("==================\n");
  
  free(hmm);
  free(nearestHmm);

  return 0;
}
