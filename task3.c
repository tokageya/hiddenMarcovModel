#include "hidden_markov_model.h"

#define TEST_LENGTH (35) // 生成する記号列の個数

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
    fprintf(stderr, "Error : in main.\n");
    fclose(fp);
    return 1;
  }
  fclose(fp);

  srand((unsigned)time(NULL)); // 乱数のシードを設定

  char symbolStringSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1];
  int stateOrderSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1];

  int nowStringLenCount = 0; // ここに生成された記号列の長さの総和を格納

  for(int count=0; count<TEST_LENGTH; count++){
    if( !generate_symbolString_with_stateOrder(hmm, symbolStringSet[count], MAX_SYMBOL_STRING_LEN, stateOrderSet[count]) ){
      fprintf(stderr, "Error : in main, function \"generate_symbolString_with_stateOrder\" is not correct.\n");
      return 1;
    }

    nowStringLenCount += (int)strlen(symbolStringSet[count]);

    printf("symbolStringSet [%3d] : \"%s\"\n", count, symbolStringSet[count]);
    printf("stateOrderSet   [%3d] : \"", count);
    
    for(int i=0; i<strlen(symbolStringSet[count]); i++){
      printf("%d", stateOrderSet[count][i]);
    }
    printf("\"\n");
  }

  HMM *hmm2; // 学習したHMM
  hmm2 = (HMM*)malloc(sizeof(HMM));

  init_HMM(hmm2);
  hmm2->statesNum = hmm->statesNum;
  hmm2->sigmasNum = hmm->sigmasNum;
  for(int i=0; i<hmm->sigmasNum; i++){
    hmm2->sigma[i] = hmm->sigma[i];
  }

  if( !learning_HMM_based_on_sum_stateOrder(hmm2, TEST_LENGTH, symbolStringSet, stateOrderSet) ){
    fprintf(stderr, "Error : in main.\n");
    return 1;
  }

  printf("\n------------ correct hmm ----------------\n\n");
  print_HMM(hmm);
  printf("\n\n\n------------ learning_HMM hmm2 ----------------\n\n");
  print_HMM(hmm2);
  
  double sumDiffP;
  sumDiffP = probability_diff_sum(hmm, hmm2);
  printf("==================\n");
  printf("nowStringLenCount = %d\n", nowStringLenCount);
  printf("sumDiffP = %lf\n", sumDiffP);
  printf("==================\n");


  free(hmm);
  free(hmm2);

  return 0;
}