#include "hidden_markov_model.h"

int main(int argc, char *argv[]){
  FILE *fp;
  HMM *hmm;
  hmm = (HMM*)malloc(sizeof(HMM));

  // 手順1. ファイルから隠れマルコフモデルの定義を格納 //
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

  // 手順2. 隠れマルコフモデルの定義からランダムに記号列を生成する(状態遷移も保持) //
  char symbolStringSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1];
  int stateOrderSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1];

  int nowStringLenCount = 0; // ここに生成された記号列の長さの総和を格納
  int setNum;
  for(int count=0; count<MAX_SET; count++){
    if( !generate_symbolString_with_stateOrder(hmm, symbolStringSet[count], MAX_SYMBOL_STRING_LEN, stateOrderSet[count]) ){
      fprintf(stderr, "Error : in main, function \"generate_symbolString_with_stateOrder\" is not correct.\n");
    }
    nowStringLenCount += (int)strlen(symbolStringSet[count]);
    if(nowStringLenCount >= 100){
      setNum = count+1;
      break;
    }
  }

  printf("nowStringLenCount : %d\nsetNum : %d\n",nowStringLenCount,setNum);

  // 手順3. 記号列から状態遷移列を予測する //
  double maxProbability;
  int predictStateOrderSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1];
  for(int count=0; count<setNum; count++){
    if( !viterbi(hmm, symbolStringSet[count], predictStateOrderSet[count], &maxProbability) ){
      fprintf(stderr, "Error : in main, function \"viterbi\" is not correct.\n");
      return 1;
    }

    printf("symbolStringSet      [%2i] : \"%s\"\n", count, symbolStringSet[count]);
    printf("stateOrderSet        [%2i] : \"", count);
    for(int i=0; i<(int)strlen(symbolStringSet[count]); i++){
      printf("%d", stateOrderSet[count][i]);
    }
    printf("\"\n");

    printf("predictStateOrderSet [%2i] : \"", count);
    for(int i=0; i<(int)strlen(symbolStringSet[count]); i++){
      printf("%d",predictStateOrderSet[count][i]);
    }
    printf("\"\n");
    
  }

  // 手順4. 予測結果と実際に生成された状態を比較して正答率を求める //
  int allCount = 0;
  int correctCount=0;
  for(int count=0; count<MAX_SET; count++){
    for(int i=0; i<strlen(symbolStringSet[count]); i++){
      allCount++;
      if(stateOrderSet[count][i] == predictStateOrderSet[count][i]) correctCount++;
    }
  }
  printf("percentage of correct : %3.3lf%%\n", ( (double)correctCount / (double)allCount ) * 100  );

  free(hmm);

  return 0;
}