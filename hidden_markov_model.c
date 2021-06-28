/**********************************************************\

  hidden_markov_model.c : 二次元配列を用いて遷移確率を表す 

\**********************************************************/

#include "hidden_markov_model.h"

/* STATE_EXIST : hmm 上に状態 s が存在するかどうかを返す */
#define STATE_EXIST(hmm, s) ( (0 <= s) && (s < hmm->statesNum) )

/* SIGMA_EXIST : hmm 上に記号 sigma に対応するインデックス i が存在するかどうかを返す */
#define SIGMA_EXIST(hmm, i) ( (0 <= i) && (i < hmm->sigmasNum) )

/* CORRECT_PROBABILITY_RANGE : p が確率として正しい値かどうかを返す */
#define CORRECT_PROBABILITY_RANGE(p) ( (0.0 <= p) && (p <= 1.0) )

/* RAND : 0～1までのdouble型の乱数を生成する */
#define RAND ((double)rand() / RAND_MAX)

/* FIRST_STATE : hmm の初期状態を返す */
#define FIRST_STATE(hmm) (0)

/* END_STATE :  hmm の最終状態を返す */
#define END_STATE(hmm) (hmm->statesNum-1)

/* DOUBLE_MIN : double 型の最小値 */
#define DOUBLE_MIN (-1.797693e+308)



/***********************************************************\
    隠れマルコフモデルを表す構造体 HMM の基本的な操作関数の定義
\***********************************************************/

/* init_HMM : HMM *hmm の0初期化 */
void init_HMM(HMM *hmm){
  hmm->statesNum = 0;
  hmm->sigmasNum = 0;
  for(int i=0; i<MAX_STATES; i++){
    for(int j=0; j<MAX_STATES; j++){
      hmm->delta[i][j] = 0.0;
    }
    for(int j=0; j<MAX_SIGMAS; j++){
      hmm->e[i][j] = 0.0;
    }
  }
  for(int i=0; i<MAX_SIGMAS; i++){
    hmm->sigma[i] = '\0';
  }
  return;
}

/* del_HMM : HMM *hmm のメモリ解放(使わない) */
void del_HMM(HMM *hmm){
  free(hmm);
  return;
}

/* copy_HMM : HMM *hmm1 の内容を HMM *hmm2 にコピー */
void copy_HMM(HMM *hmm1, HMM *hmm2){
  init_HMM(hmm2); // hmm2 を初期化
  hmm2->statesNum = hmm1->statesNum;
  hmm2->sigmasNum = hmm1->sigmasNum;
  for(int i=0; i<hmm1->statesNum; i++){
    for(int j=0; j<hmm1->statesNum; j++){
      hmm2->delta[i][j] = hmm1->delta[i][j];
    }
    for(int j=0; j<hmm1->sigmasNum; j++){
      hmm2->e[i][j] = hmm1->e[i][j];
    }
  }
  for(int i=0; i<hmm1->sigmasNum; i++){
    hmm2->sigma[i] = hmm1->sigma[i];
  }
  return;
}

/* set_sigma : HMM の有限アルファベットに記号を加える */
int set_sigma(HMM *hmm, char c){
  if( (hmm->sigmasNum+1) > MAX_SIGMAS) {fprintf(stderr, "Error : in function \"set_sigma\", MAX_SIGMAS over.\n"); return ERROR;}
  hmm->sigma[hmm->sigmasNum] = c;
  hmm->sigmasNum ++;
  return SUCCESS;
}

/* set_states : HMM に状態数 n を与えて状態 0～n-1 を作成する */
int set_states(HMM *hmm, int n){
  if(n > MAX_STATES) {fprintf(stderr, "Error : in function \"set_states\", %d(=n) exceeds MAX_STATES\n",n); return ERROR;}
  hmm->statesNum = n;
  return SUCCESS;
}

/* set_e : HMM の状態 state における記号 sigma に対応するインデックス sigmaIndex の出現確率 probability を与える */
int set_e(HMM *hmm, int state, int sigmaIndex, double probability){
  if( !STATE_EXIST(hmm,state) ) {fprintf(stderr, "Error : in function \"set_e\", state %d(=state) doesn't exist in Hidden Marcov Model.\n", state); return ERROR;}
  if( !SIGMA_EXIST(hmm,sigmaIndex) ) {fprintf(stderr, "Error : in function \"set_e\", sigma index %d(=sigmaIndex) doesn't exist.\n", sigmaIndex); return ERROR;}
  if( !CORRECT_PROBABILITY_RANGE(probability) ) {fprintf(stderr, "Error : in function \"set_e\", probability %lf(=probability) is out of range.\n", probability); return ERROR;}
  hmm->e[state][sigmaIndex] = probability;
  return SUCCESS;
}

/* set_delta : HMM に状態 stateI から状態 stateJ への状態遷移確率を与える */
int set_delta(HMM *hmm, int stateI, int stateJ, double transitionP){
  if( !STATE_EXIST(hmm,stateI) ) {fprintf(stderr, "Error : in function \"set_delta\", state %d(=stateI) doesn't exist in Hidden Marcov Model.\n", stateI); return ERROR;}
  if( !STATE_EXIST(hmm,stateJ) ) {fprintf(stderr, "Error : in function \"set_delta\", state %d(=stateJ) doesn't exist in Hidden Marcov Model.\n", stateJ); return ERROR;}
  if( !CORRECT_PROBABILITY_RANGE(transitionP) ) {fprintf(stderr, "Error : in function \"set_delta\", probability %lf(=transitionP) is out of range.\n", transitionP); return ERROR;}
  hmm->delta[stateI][stateJ] = transitionP;
  return SUCCESS;
}

/* print_HMM : HMMの表示 */
void print_HMM(HMM *hmm){
  printf("hmm->sigmasNum = %d, hmm->statesNum = %d\n",hmm->sigmasNum, hmm->statesNum);
  printf("hmm->sigma = [ ");
  for(int i=0; i<hmm->sigmasNum; i++) printf("%c, ",hmm->sigma[i]);
  printf("]\n");
  printf("\n=============== hmm->delta ===============\n[\n");
  for(int i=0; i<hmm->statesNum; i++){
    printf("  [ ");
    for(int j=0; j<hmm->statesNum; j++) printf("%1.3lf, ", hmm->delta[i][j]);
    printf("],\n");
  }
  printf("]\n");

  printf("\n=============== hmm->e ===============\n[\n");
  for(int i=0; i<hmm->statesNum; i++){
    printf("  [ ");
    for(int j=0; j<hmm->sigmasNum; j++) printf("%1.3lf, ", hmm->e[i][j]);
    printf("],\n");
  }
  printf("]\n");
  return;
}

/* read_HMM : HMMをファイルから読み込んで作成する */
int read_HMM(FILE *fp, HMM *hmm){
	char line[FILE_BUF_SIZE];
  char buf[FILE_BUF_SIZE];
  int len, pos;
  
  init_HMM(hmm);
  while(fgets(line, FILE_BUF_SIZE, fp) != NULL){
    
    if(line[strlen(line) - 1] == '\n') line[strlen(line)-1] = '\0'; // 行末の余分な改行文字を削除

    len = strlen(line);
    pos=0;
    while(line[pos] != ' '){
      buf[pos] = line[pos];
      pos++;
      if(len <= pos){
        fprintf(stderr, "Error : in function \"read_HMM\", the command is not correct.\n");
        return ERROR;
      }
    }
    buf[pos] = '\0';
    pos ++;
    
    /* --- buf の文字列データに応じて次の作業を決める --- */

    // "sigma" : 記号を追加 // 
    if(!strcmp(buf,"sigma")){
      while(line[pos] != '\0'){
        if(line[pos] != ' '){
          if( !set_sigma(hmm, line[pos]) ){
            fprintf(stderr, "Error : in function \"read_HMM\", command state is not correct.\n");
            return ERROR;
          }
        }
        pos ++;
      }
    }
    
    // "stnum" : 状態数を記録 //
    else if(!strcmp(buf,"stnum")){
      if( ((int)line[pos]) < ((int)'0') || (  ((int)'9') < ((int)line[pos]) ) ){
        fprintf(stderr, "Error : in function \"read_HMM\", stnum number \'%c\' is not correct.\n", line[pos]);
        return ERROR;
      }
      if( !set_states(hmm, ((int)line[pos] - 48) ) ){
        fprintf(stderr, "Error : in function \"read_HMM\", command state is not correct.\n");
        return ERROR;
      }
    }

    // "state" : 状態での記号の出現確率の記録 //
    else if(!strcmp(buf,"state")){
      int state = ((int)line[pos]) - ((int)'0');
      pos++;
      int sigmaI = 0;
      int bufPos;
      while(line[pos] != '\0'){
        bufPos = 0;
        pos++;
        while(line[pos] != ' ' && line[pos] != '\0'){
          buf[bufPos] = line[pos];
          bufPos++;
          pos++;
        }
        buf[bufPos] = '\0';
        if( !set_e(hmm, state, sigmaI, atof(buf)) ){
          fprintf(stderr, "Error : in function \"read_HMM\", command state is not correct.\n");
          return ERROR;
        }
        sigmaI++;
      }
    }
    
    // "delta" : 状態遷移確率の記録 //
    else if(!strcmp(buf,"delta")){
      int stateI, stateJ;
      stateI = ((int)line[pos]) - ((int)'0');
      pos += 2;
      stateJ = ((int)line[pos]) - ((int)'0');
      pos += 2;
      
      int bufPos = 0;
      while(line[pos] != ' ' && line[pos] != '\0'){
        buf[bufPos] = line[pos];
        bufPos++;
        pos++;
      }
      buf[bufPos] = '\0';
      if( !set_delta(hmm, stateI, stateJ, atof(buf)) ){
        fprintf(stderr, "Error : in function \"read_HMM\", command delta is not correct.\n");
        return ERROR;
      }
    }
    else{
      fprintf(stderr, "Error : in function \"read_HMM\", the command \"%s\" are not correct.\n", buf);
      return ERROR;
    }
  }

	return SUCCESS;
}



/*************************************************\
  課題1: HMM の定義から記号列を生成する関数の定義
\*************************************************/

/* generate_symbolString : 隠れマルコフモデルの定義に基づいて, ランダムに記号列を生成する */
int generate_symbolString(HMM *hmm, char *symbolString, int maxStringLen){
  int nowState = 0; // 現在の状態
  int nowStringLen = 0; // 現在の記号列の長さ

  
  // 手順1. 初期状態から任意の状態に移る //
  {
    int tmpState; // tmp State
    double sumP = 0.0; // sum Probability
    double tmpP = RAND; // tmp Probability 

    for(tmpState=0; tmpState<hmm->statesNum; tmpState++){
      sumP += hmm->delta[nowState][tmpState];
      if(tmpP < sumP) break;
    }
    nowState = tmpState;
  }

  
  // 手順2. !(終端状態に達する) or 記号列の長さがlenに達する まで以下を行う //
  while( (nowState != END_STATE(hmm)) && nowStringLen < maxStringLen ){
    
    // 手順2-1. 今の状態で出力操作を行う //
    {
      int tmpSigmaI; // tmp Sigma Index
      double sumP = 0.0; // sum Probability
      double tmpP = RAND; // tmp Probability

      for(tmpSigmaI=0; tmpSigmaI<hmm->sigmasNum; tmpSigmaI++){
        sumP += hmm->e[nowState][tmpSigmaI];
        if(tmpP < sumP) break;
      }
      symbolString[nowStringLen] = hmm->sigma[tmpSigmaI];
      nowStringLen++;
    }

    // 手順2-2. 次の状態に移る //
    {
      int tmpState; // tmp State
      double sumP = 0.0; // sum Probability
      double tmpP = RAND; // tmp Probability

      for(tmpState=0; tmpState<hmm->statesNum; tmpState++){
        sumP += hmm->delta[nowState][tmpState];
        if(tmpP < sumP) break;
      }
      nowState = tmpState;
    }
  }
  symbolString[nowStringLen] = '\0';

  return SUCCESS;
}

/* generate_symbolString_with_stateOrder : 隠れマルコフモデルの定義に基づいて, ランダムに記号列を生成する(状態遷移も記す) */
int generate_symbolString_with_stateOrder(HMM *hmm, char *symbolString, int maxStringLen, int *stateOrder){
  int nowState = 0; // 現在の状態
  int nowStateNum = 0; // 今までに経た状態数
  int nowStringLen = 0; // 現在の記号列の長さ
  
  // 手順1. 初期状態から任意の状態に移る //
  {
    int tmpState; // tmp State
    double sumP = 0.0; // sum Probability
    double tmpP = RAND; // tmp Probability 

    for(tmpState=0; tmpState<hmm->statesNum; tmpState++){
      sumP += hmm->delta[nowState][tmpState];
      if(tmpP <= sumP) break;
    }
    nowState = tmpState;
  }

  
  // 手順2. !(終端状態に達する) or 記号列の長さがlenに達する まで以下を行う //
  while( (nowState != END_STATE(hmm)) && nowStringLen < maxStringLen ){
    
    // 手順2-1. 今の状態で出力操作を行う. このとき, stateOrder に状態を記録しておく. //
    {
      int tmpSigmaI; // tmp Sigma Index
      double sumP = 0.0; // sum Probability
      double tmpP = RAND; // tmp Probability

      for(tmpSigmaI=0; tmpSigmaI<hmm->sigmasNum; tmpSigmaI++){
        sumP += hmm->e[nowState][tmpSigmaI];
        if(tmpP <= sumP) break;
      }
      symbolString[nowStringLen] = hmm->sigma[tmpSigmaI];
      nowStringLen++;
      stateOrder[nowStateNum] = nowState;
      nowStateNum++;
    }

    // 手順2-2. 次の状態に移る //
    {
      int tmpState; // tmp State
      double sumP = 0.0; // sum Probability
      double tmpP = RAND; // tmp Probability

      for(tmpState=0; tmpState<hmm->statesNum; tmpState++){
        sumP += hmm->delta[nowState][tmpState];
        if(tmpP <= sumP) break;
      }
      nowState = tmpState;
    }
  }
  if( nowState != END_STATE(hmm) ) nowState = END_STATE(hmm);
  symbolString[nowStringLen] = '\0';
  stateOrder[nowStateNum] = nowState;

  return SUCCESS;
}



/*********************************************************\
  課題2: HMM と記号列から状態パスを推測する関数の定義
\*********************************************************/

/* char_to_symbolStringIndex : 記号列上の文字から対応するインデックス値を求める */
int char_to_symbolStringIndex(HMM *hmm, char c, int *symbolIndex){
  for(int i = 0; i < hmm->sigmasNum; i++){
    if(hmm->sigma[i] == c){
      *symbolIndex = i;
      return SUCCESS;
    }
  }
  return ERROR;
}

/* viterbi_without_stateOrder : Viterbiのアルゴリズムに則って, 記号列から最大の状態パスの推測を行い, 状態パスの最大確率値を求める */
int viterbi_without_stateOrder(HMM *hmm, char *symbolString, double *maxP){
  int m = strlen(symbolString); // 入力となる記号列の長さ
  double vP[m+1][hmm->statesNum]; // viterbi Probability. 各状態における最大確率値を表す二次元配列. 
                                  // vP[(現時点での状態遷移数(=現時点での記号列の長さ))][(次の状態)] で次の状態に至るようなパスの最大確率値を表す.

  // 手順1. 初期化 //
  vP[0][0] = log(1.0);
  for(int k=1; k<hmm->statesNum; k++) {vP[0][k] = DOUBLE_MIN;} // 各状態での確率値を初期化

  // 手順2. 1文字目から終端文字までの各状態においての最大確率値を順番に求める //
  for(int i=1; i<=strlen(symbolString); i++){ // i : 現時点での状態遷移数  1 ～ strlen(symbolString)
    for(int k=0; k<hmm->statesNum; k++){ // k : 状態 0 ～ (hmm->statesNum-1)

      // 手順2-1. 最大確率値を求める //
      double maxPassP = DOUBLE_MIN; // 直前の状態が j, そこから状態 k に至るパスの確率の最大値

      int symbolIndex;
      char symbol = symbolString[i-1];

      if( !char_to_symbolStringIndex(hmm, symbol, &symbolIndex ) ) {
        fprintf(stderr, "Error : in function \"viterbi\", symbol %c(=symbolString[%d]) is not found in sigma.\n", symbolString[i], i); 
        return ERROR;
      }

      for(int j=0; j<hmm->statesNum; j++){ // j : i の直前の状態 0 ～ (hmm->statesNum-1)
        double tmpPassP;
        if( isinf(vP[i-1][j]) && isinf(log(hmm->delta[j][k])) ){
          tmpPassP = DOUBLE_MIN;
        }
        else if( isinf(vP[i-1][j]) ){
          tmpPassP = log(hmm->delta[j][k]);
        }
        else if( isinf(log(hmm->delta[j][k])) ){
          tmpPassP = vP[i-1][j];
        }
        else{ 
          tmpPassP = vP[i-1][j] + log(hmm->delta[j][k]); // tmp Pass Probability 
          if(isinf(tmpPassP)){
            tmpPassP = DOUBLE_MIN;
          }
        }

        if(maxPassP < tmpPassP) {
          maxPassP = tmpPassP;
        }
      }

      /* 手順2-2. vP の値を更新 */
      double logTmpP= maxPassP + log(hmm->e[k][symbolIndex]);
      if( isinf(logTmpP) ){
        logTmpP = DOUBLE_MIN;
      }
      vP[i][k] = logTmpP;
    }
  }

  // 手順3. 終端式 //
  double maxPassP = DOUBLE_MIN;
  for(int j=0; j<hmm->statesNum; j++){
    double tmpPassP;
    if( isinf(vP[m][j]) && isinf(log(hmm->delta[j][END_STATE(hmm)])) ){
      tmpPassP = DOUBLE_MIN;
    }
    else if( isinf(vP[m][j]) ){
      tmpPassP = log(hmm->delta[j][END_STATE(hmm)]);
    }
    else if( isinf(log(hmm->delta[j][END_STATE(hmm)])) ){
      tmpPassP = vP[m][j];
    }
    else{
      tmpPassP = vP[m][j] + log(hmm->delta[j][END_STATE(hmm)]);
      if( isinf(tmpPassP) ){
        tmpPassP = DOUBLE_MIN;
      }
    }
    
    if(maxPassP < tmpPassP){
      maxPassP = tmpPassP;
    }
  }
  *maxP = exp(maxPassP);

  return SUCCESS;
}

/* viterbi : Viterbiのアルゴリズムに則って, 記号列から最大の状態パスの推測を行い, 状態パスの最大確率値を求める(状態パスも格納する) */
int viterbi(HMM *hmm, char *symbolString, int *stateOrder, double *maxP){
  //int n = hmm->statesNum-2; // 中間状態数 n (= 初期状態と最終状態以外の状態数. 資料でのnと同様)
  int m = (int)strlen(symbolString); // 入力となる記号列の長さ
  double vP[m+1][hmm->statesNum]; // viterbi Probability. 各状態における最大確率値を表す二次元配列. 
                                  // vP[(現時点での状態遷移数(=現時点での記号列の長さ))][(次の状態)] で次の状態に至るようなパスの最大確率値を表す.
  int lastState[m+1][hmm->statesNum]; // last State. 各状態遷移数においての最大確率となる直前の状態を保持
                                      // lastState[(現時点での状態遷移数(=現時点での記号列の長さ))][(次の状態)] で次の状態に至るようなパスの最大確率値を表す.
  
  // 手順1. 初期化 //
  vP[0][0] = log(1.0);
  for(int k=1; k<hmm->statesNum; k++) {vP[0][k] = DOUBLE_MIN;} // 各状態での確率値を初期化

  // 手順2. 1文字目から終端文字までの各状態においての最大確率値を順番に求める //
  for(int i=1; i<=(int)strlen(symbolString); i++){ // i : 現時点での状態遷移数  1 ～ strlen(symbolString)
    for(int k=0; k<hmm->statesNum; k++){ // k : 状態 0 ～ (hmm->statesNum-1)

      // 手順2-1. 最大確率値を求める //
      int maxLastState = 0; // vP に最大値を取らせる直前の状態 ( 最終的に求まる j の値 )
      double maxPassP = DOUBLE_MIN; // 直前の状態が j, そこから状態 k に至るパスの確率の最大値

      int symbolIndex;
      char symbol = symbolString[i-1];

      if( !char_to_symbolStringIndex(hmm, symbol, &symbolIndex ) ) {
        fprintf(stderr, "Error : in function \"viterbi\", symbol %c(=symbolString[%d]) is not found in sigma.\n", symbol, i-1); 
        return ERROR;
      }

      for(int j=0; j<hmm->statesNum; j++){ // j : i の直前の状態 0 ～ (hmm->statesNum-1)
        double tmpPassP;
        if( isinf(vP[i-1][j]) && isinf(log(hmm->delta[j][k])) ){
          tmpPassP = DOUBLE_MIN;
        }
        else if( isinf(vP[i-1][j]) ){
          tmpPassP = log(hmm->delta[j][k]);
        }
        else if( isinf(log(hmm->delta[j][k])) ){
          tmpPassP = vP[i-1][j];
        }
        else{ 
          tmpPassP = vP[i-1][j] + log(hmm->delta[j][k]); // tmp Pass Probability 
          if(isinf(tmpPassP)){
            tmpPassP = DOUBLE_MIN;
          }
        }

        if(maxPassP < tmpPassP) {
          maxPassP = tmpPassP;
          maxLastState = j;
        }
      }

      /* 手順2-2. vP の値を更新( lastState の値も更新 ) */
      double logTmpP= maxPassP + log(hmm->e[k][symbolIndex]);
      if( isinf(logTmpP) ){
        logTmpP = DOUBLE_MIN;
      }
      vP[i][k] = logTmpP;
      lastState[i][k] = maxLastState;
    }
  }

  // 手順3. 終端式 //
  int maxLastState = 0; // vP に最大値を取らせる直前の状態 ( 最終的に求まる j の値 )
  double maxPassP = DOUBLE_MIN;
  for(int j=0; j<hmm->statesNum; j++){
    double tmpPassP;
    if( isinf(vP[m][j]) && isinf(log(hmm->delta[j][END_STATE(hmm)])) ){
      tmpPassP = DOUBLE_MIN;
    }
    else if( isinf(vP[m][j]) ){
      tmpPassP = log(hmm->delta[j][END_STATE(hmm)]);
    }
    else if( isinf(log(hmm->delta[j][END_STATE(hmm)])) ){
      tmpPassP = vP[m][j];
    }
    else{
      tmpPassP = vP[m][j] + log(hmm->delta[j][END_STATE(hmm)]);
      if( isinf(tmpPassP) ){
        tmpPassP = DOUBLE_MIN;
      }
    }
    
    if(maxPassP < tmpPassP){
      maxPassP = tmpPassP;
      maxLastState = j;
    }
  }
  *maxP = exp(maxPassP);

  // 手順4. 求まった最大確率値となる状態パスを求める //
  for(int j=m; j>0; j--){
    stateOrder[j-1] = maxLastState;
    maxLastState = lastState[j][maxLastState];
  }

  return SUCCESS;
}



/***********************************************************\
  課題3: 状態パスがわかる場合の隠れマルコフモデルを学習する関数の定義
\***********************************************************/

/* learning_HMM_based_on_stateOrder : 記号列と状態パスから隠れマルコフモデルの各確率を求める (※hmm には statesNum, sigma, sigmasNum は事前に与えておく) */
int learning_HMM_based_on_stateOrder(HMM *hmm, char *symbolString, int *stateOrder){
  int transitionCount[hmm->statesNum][hmm->statesNum]; // 各状態における次の各状態への遷移回数
  int transitionCountAll[hmm->statesNum]; // 各状態に置けるすべての状態遷移回数の合計
  int symbolCount[hmm->statesNum][hmm->sigmasNum]; // 各状態における各記号の出現回数
  int symbolCountAll[hmm->statesNum]; // 各状態に置ける全ての記号の出現回数の合計

  // 手順1. 初期化 //
  for(int i=0; i<hmm->statesNum; i++){
    for(int j=0; j<hmm->statesNum; j++){
      transitionCount[i][j] = 0;
    }
    transitionCountAll[i] = 0;
  }
  for(int i=0; i<hmm->statesNum; i++){
    for(int j=0; j<hmm->sigmasNum; j++){
      symbolCount[i][j] = 0;
    }
    symbolCountAll[i] = 0;
  }

  // 手順2. 各状態に置ける記号出現回数と状態遷移回数を記録 //
  // 手順2-0. 初期状態からの遷移をカウントする
  transitionCount[0][stateOrder[0]] ++;
  transitionCountAll[0]++;
  for(int turn=0; turn<strlen(symbolString); turn++){
    int nowState = stateOrder[turn];

    // 手順2-1. 現在の状態で出現した記号をカウント //
    int symbolIndex;
    char symbol = symbolString[turn];
    
    if( !char_to_symbolStringIndex(hmm, symbol, &symbolIndex ) ) {
      fprintf(stderr, "Error : in function \"learning_HMM\", symbol %c(=symbolString[%d]) is not found in sigma.\n", symbolString[turn], turn); 
      return ERROR;
    }
    symbolCount[nowState][symbolIndex] ++;
    symbolCountAll[nowState] ++;

    // 手順2-2. 現在の状態から次の状態への状態遷移をカウント //
    int nextState = stateOrder[turn+1]; 
    transitionCount[nowState][nextState] ++;
    transitionCountAll[nowState] ++;
    
  }

  // 手順3. 記録した回数から各確率値を求める //
  for(int nowState=0; nowState<hmm->statesNum; nowState++){
    // 手順3-1. 記録した状態遷移回数から状態遷移確率を与える //
    for(int nextState=0; nextState<hmm->statesNum; nextState++){
      if(transitionCountAll[nowState] == 0.0) continue;
      double transitionP = (((double)transitionCount[nowState][nextState]) / transitionCountAll[nowState]);
      if( !set_delta(hmm, nowState, nextState, transitionP) ){
        fprintf(stderr, "Error : in function \"learning_HMM\", nowState = %d, nextState = %d, transitionP = %lf\n",nowState, nextState, transitionP);
        return ERROR;
      }
    }
    // 手順3-2. 記録した記号出現回数から記号の出現確率を与える //
    for(int symbolIndex=0; symbolIndex<hmm->sigmasNum; symbolIndex++){
      if(symbolCountAll[nowState] == 0.0) continue;
      double appearP = ( ((double)symbolCount[nowState][symbolIndex]) / symbolCountAll[nowState] );
      if( !set_e(hmm, nowState, symbolIndex, appearP) ){
        fprintf(stderr, "Error : in function \"learning_HMM\", nowState = %d, symbolIndex = %d, appearP = %lf\n",nowState, symbolIndex, appearP);
        return ERROR;
      }
    }

  }

  return SUCCESS;
}

/* learning_HMM_based_on_sum_stateOrder : 複数の記号列と状態パスから隠れマルコフモデルの各確率を求める (※hmm には statesNum, sigma, sigmasNum は事前に与えておく) */
int learning_HMM_based_on_sum_stateOrder(HMM *hmm, int symbolStringNum, char symbolString[MAX_SET][MAX_SYMBOL_STRING_LEN+1], int stateOrder[MAX_SET][MAX_SYMBOL_STRING_LEN+1]){
  int transitionCount[hmm->statesNum][hmm->statesNum]; // 各状態における次の各状態への遷移回数
  int transitionCountAll[hmm->statesNum]; // 各状態に置けるすべての状態遷移回数の合計
  int symbolCount[hmm->statesNum][hmm->sigmasNum]; // 各状態における各記号の出現回数
  int symbolCountAll[hmm->statesNum]; // 各状態に置ける全ての記号の出現回数の合計
  
  // 手順1. 初期化 //
  for(int i=0; i<hmm->statesNum; i++){
    for(int j=0; j<hmm->statesNum; j++){
      transitionCount[i][j] = 0;
    }
    transitionCountAll[i] = 0;
  }
  for(int i=0; i<hmm->statesNum; i++){
    for(int j=0; j<hmm->sigmasNum; j++){
      symbolCount[i][j] = 0;
    }
    symbolCountAll[i] = 0;
  }

  // 手順2. 各状態に置ける記号出現回数と状態遷移回数を記録 //
  for(int nowSymbolString = 0; nowSymbolString < symbolStringNum; nowSymbolString++){
    // 手順2-0. 初期状態からの遷移をカウントする
    transitionCount[0][stateOrder[nowSymbolString][0]] ++;
    transitionCountAll[0]++;
    for(int turn=0; turn<((int)strlen(symbolString[nowSymbolString])); turn++){
      int nowState = stateOrder[nowSymbolString][turn];

      // 手順2-1. 現在の状態で出現した記号をカウント //
      int symbolIndex;
      char symbol = symbolString[nowSymbolString][turn];

      if( !char_to_symbolStringIndex(hmm, symbol, &symbolIndex ) ) {
        fprintf(stderr, "Error : in function \"learning_HMM\", symbol %c(=symbolString[%d]) is not found in sigma.\n", symbolString[nowSymbolString][turn], turn); 
        return ERROR;
      }
      symbolCount[nowState][symbolIndex] ++;
      symbolCountAll[nowState] ++;

      // 手順2-2. 現在の状態から次の状態への状態遷移をカウント //
      int nextState;
      if(turn == (int)strlen(symbolString[nowSymbolString])-1) nextState = END_STATE(hmm);
      else nextState = stateOrder[nowSymbolString][turn+1]; 
      transitionCount[nowState][nextState] ++;
      transitionCountAll[nowState] ++;
      
    }
  }

  // 手順3. 記録した回数から各確率値を求める //
  for(int nowState = 0; nowState < hmm->statesNum; nowState++){
    // 手順3-1. 記録した状態遷移回数から状態遷移確率を与える //
    for(int nextState = 0; nextState < hmm->statesNum; nextState++){
      if(transitionCountAll[nowState] == 0) continue;
      double transitionP = (((double)transitionCount[nowState][nextState]) / transitionCountAll[nowState]);
      if( !set_delta(hmm, nowState, nextState, transitionP) ){
        fprintf(stderr, "Error : in function \"learning_HMM\", nowState = %d, nextState = %d, transitionP = %lf\n",nowState, nextState, transitionP);
        return ERROR;
      }
    }
    // 手順3-2. 記録した記号出現回数から記号の出現確率を与える //
    for(int symbolIndex=0; symbolIndex<hmm->sigmasNum; symbolIndex++){
      if(symbolCountAll[nowState] == 0) continue;
      double appearP = ( ((double)symbolCount[nowState][symbolIndex]) / symbolCountAll[nowState] );
      if( !set_e(hmm, nowState, symbolIndex, appearP) ){
        fprintf(stderr, "Error : in function \"learning_HMM\", nowState = %d, symbolIndex = %d, appearP = %lf\n",nowState, symbolIndex, appearP);
        return ERROR;
      }
    }
  }

  return SUCCESS;
}



/***************************************************************\
    課題4: 状態パスがわからない場合の隠れマルコフモデルを学習する関数の定義
\***************************************************************/

/* learning_HMM_only_sum_symbolStrings : 複数の記号列から隠れマルコフモデルの各確率を求める (※ hmm には sigma, sigmasNum は事前に与えておく) */
int learning_HMM_only_sum_symbolStrings(HMM *hmm, char symbolString[MAX_SET][MAX_SYMBOL_STRING_LEN+1], int statesNum, int symbolStringNum){

  set_states(hmm, statesNum);

  // 手順1. ランダムに確率パラメータを設定 //

  // 手順1-1. 状態遷移確率をランダムに与える //
  for(int nowState = 0; nowState < hmm->statesNum; nowState++){
    double tmpSumP = 0.0; // tmp Sum Propability
    double remainingP = 1.0; // remaining Probability (= 1.0 - tmpSumP)
    for(int nextState = 0; nextState < hmm->statesNum; nextState++){
      if(nowState == END_STATE(hmm)){ set_delta(hmm, nowState, nextState, 0.0); continue; }
      if(nextState == FIRST_STATE(hmm)){ set_delta(hmm, nowState, nextState, 0.0); continue; }
      double tmpRand = remainingP * RAND;
      set_delta(hmm, nowState, nextState, tmpRand);
      tmpSumP += tmpRand;
      remainingP = 1.0 - tmpSumP;
    }
  }

  // 手順1-2. 各状態に置ける記号の出現確率をランダムに与える //
  for(int nowState = 0; nowState < hmm->statesNum; nowState++){
    double tmpSumP = 0.0; // tmp Sum Probability
    double remainingP = 1.0; // remaining Probability (= 1.0 - tmpSumP)
    for(int symbolIndex = 0; symbolIndex < hmm->sigmasNum; symbolIndex++){
      if(nowState == FIRST_STATE(hmm) || nowState == END_STATE(hmm)){ set_e(hmm, nowState, symbolIndex, 0.0); continue; }
      double tmpRand = remainingP * RAND;
      set_e(hmm, nowState, symbolIndex, tmpRand);
      tmpSumP += tmpRand;
      remainingP = 1.0 - tmpSumP;
    }
  }

  // 状態パスが更新されなくなるまで繰り返す //
  int updatedCount = 0; // 更新した回数
  int beforeStateOrderSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1]; // 一個前の状態パスの情報を保持
  while( 1 ){
    // 手順2. 複数の記号列のみに対して Viterbi アルゴリズムを実行して複数の状態パスを推定 //
    if(updatedCount >= 1000) { fprintf(stderr, "updatedCount is 1000 over.\n"); return ERROR; } // 一定以上更新しても終わらなかったらエラーを返す
    int stateOrderSet[MAX_SET][MAX_SYMBOL_STRING_LEN+1]; // viterbi によって得られる状態遷移を格納
    double maxP; // stateOrderSet の状態遷移となる確率
    for(int viterbiCount=0; viterbiCount<symbolStringNum; viterbiCount++){
      if( !viterbi(hmm, symbolString[viterbiCount], stateOrderSet[viterbiCount], &maxP) ){
        fprintf(stderr, "Error : in function \"learning_HMM_only_symbolString\", symbolString = \"%s\", statesNum = %d\n", symbolString[viterbiCount], statesNum);
        return ERROR;
      }
    }

    // 手順2.5. 推定された状態パスが変更されなかったら抜ける //
    if(updatedCount != 0){
      int endLearnFlag = 1; // 学習終了フラグ
      for(int n = 0; n < symbolStringNum; n++){
        for(int turn = 0; turn < (int)strlen(symbolString[n]); turn++){
          if(beforeStateOrderSet[n][turn] != stateOrderSet[n][turn]){
            endLearnFlag = 0;
            break;
          }
        }
        if(!endLearnFlag) break;
      }
      // 推定された状態パスが全く変更を受けてなかったらbreak
      if(endLearnFlag) {
        break;
      }
      // 現在の状態パス集合をコピー
      for(int n = 0; n < symbolStringNum; n++){
        for(int turn = 0; turn < (int)strlen(symbolString[n]); turn++){
          beforeStateOrderSet[n][turn] = stateOrderSet[n][turn];
        }
      }
    }

    // 手順3. 推定された複数の状態パスを用いて課題3の学習手法を適用して確率パラメータを予測 //
    if( !learning_HMM_based_on_sum_stateOrder(hmm, symbolStringNum, symbolString, stateOrderSet) ){
      fprintf(stderr, "Error : in function \"learning_HMM_only_symbolString\", function \"learning_HMM_based_on_stateOrder\" is not correct.\n");
      return ERROR;
    }
    updatedCount++;
  }
  return SUCCESS;
}
