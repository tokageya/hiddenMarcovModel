#include "hidden_markov_model.h"

int main(int argc, char *argv[]){
  FILE *fp;
  HMM *hmm; // 隠れマルコフモデルの定義を格納
  hmm = (HMM*)malloc(sizeof(HMM));

  // 手順1. ファイルから隠れマルコフモデルの定義を読み込む //
  // 手順1-1. ファイルを開く //
  if((fp = fopen(argv[1], "r")) == NULL){
    fprintf(stderr, "Error : file can not be opened.\n");
    return 1;
  }
  // 手順1-2. 隠れマルコフモデルの定義をhmmに格納 //
  if( !read_HMM(fp, hmm) ){
    fprintf(stderr, "Error : in main.\n");
    fclose(fp);
    return 1;
  }
  fclose(fp);

  srand((unsigned)time(NULL)); // 乱数のシードを設定

  // 手順2. 隠れマルコフモデルの定義から記号列をランダムに書き込む //
  char generatedString[MAX_SYMBOL_STRING_LEN+1]; // 生成される記号列を格納. +1 は終端文字
  int stateOrder[MAX_SYMBOL_STRING_LEN+1]; // 状態遷移

  generate_symbolString_with_stateOrder(hmm, generatedString, MAX_SYMBOL_STRING_LEN, stateOrder);

  printf("generatedString         : \"%s\"\n",generatedString);
  printf("stateOrder              : \"");
  for(int i=0; i<strlen(generatedString); i++){
    printf("%d", stateOrder[i]);
  }
  printf("\"\n");
  printf("strlen(generatedString) : %zu\n",strlen(generatedString));
  
  free(hmm);

  return 0;
}