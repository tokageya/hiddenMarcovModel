#include "hidden_markov_model.h"

/* SET_COUNT : 生成するランダム記号列の個数 */
#define SET_COUNT 1000

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

  // 手順1. hmm をもとに s に記号列を生成する //
  srand((unsigned)time(NULL)); // 乱数のシードを設定

  char generatedSymbolString[SET_COUNT][MAX_SYMBOL_STRING_LEN+1]; // 生成された記号列
  int stateOrder[SET_COUNT][MAX_SYMBOL_STRING_LEN+1]; // 状態遷移

  for(int count=0; count<SET_COUNT; count++){
    if( !generate_symbolString_with_stateOrder(hmm, generatedSymbolString[count], MAX_SYMBOL_STRING_LEN, stateOrder[count]) ){
      fprintf(stderr, "Error : in main, s = \"%s\".\n", generatedSymbolString[count]);
      return 1;
    }
  }


  /* begin debug print
  printf("strlen(symbolStringset[%2d])  : %zu\n", count, strlen(symbolStringSet[count]));
  printf("symbolStringset[%2d]          : \"%s\"\n", count, symbolStringSet[count]);
  printf("stateOrderSet[%2d] : \"", count);

  for(int i=0; i<strlen(symbolStringSet[count]); i++){
    printf("%d", stateOrderSet[count][i]);
  }
  printf("\"\n");
  // end Debug print */




  HMM *hmm2;
  hmm2 = (HMM*)malloc(sizeof(HMM));

  init_HMM(hmm2);
  hmm2->statesNum = hmm->statesNum;
  hmm2->sigmasNum = hmm->sigmasNum;
  for(int i=0; i<hmm->sigmasNum; i++){
    hmm2->sigma[i] = hmm->sigma[i];
  }

  if( !( learning_HMM_based_on_sum_stateOrder(hmm2, SET_COUNT, generatedSymbolString, stateOrder) ) ){
    fprintf(stderr, "Error : in main.\n");
    return 1;
  }

  printf("\n------------ correct hmm ----------------\n\n");
  print_HMM(hmm);
  printf("\n\n\n------------ learning_HMM hmm2 ----------------\n\n");
  print_HMM(hmm2);

  free(hmm);
  free(hmm2);

  return 0;
}