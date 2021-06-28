#ifndef HIDDEN_MARKOV_MODEL_H
#define HIDDEN_MARKOV_MODEL_H

#include <stdio.h>
#include <string.h> // 文字列操作(strlen 等)
#include <stdlib.h> // malloc, rand 等
#include <unistd.h> // sleep
#include <time.h> // time
#include <math.h> // log, fabs


/* MAX_SIGMAS : 出現する記号の最大数 */
#define MAX_SIGMAS (8)

/* MAX_STATES : モデル上の状態の最大数 */
#define MAX_STATES (10) 

/* FILE_BUF_SIZE : ファイルからモデルの入力時に使用するバッファサイズ */
#define FILE_BUF_SIZE (256)

/* MAX_SYMBOL_STRING_LEN : 記号列の最大サイズ */
#define MAX_SYMBOL_STRING_LEN (100)

/* H_S : 学習時の確率パラメータの変動の最大値(learning_HMM_only_sum_symbolStrings_tmpにのみ使用) */
#define H_S (0.01)

/* MAX_SET : 課題3,4で使用する記号列のセット数 */
#define MAX_SET (1000)

/* SUCCESS, ERROR : 関数実行のエラー検出用 */
#define SUCCESS (1)
#define ERROR (0)

/* HMM : Hidden Marcov Model, 隠れマルコフモデル全体を表す構造体 */
typedef struct {
  double delta[MAX_STATES][MAX_STATES]; // 状態遷移確率を表す二次元配列. delta[(現在の状態)][(次の状態)] で遷移確率にアクセスできる. デフォルトで0.0
  int statesNum; // モデル上の状態の総数
  char sigma[MAX_SIGMAS]; // 有限アルファベットに含まれる記号を表す. デフォルトで'\0'
  double e[MAX_STATES][MAX_SIGMAS]; // 各状態における記号の出現確率を表す. e[(状態)][(記号に対応するインデックス値)]で出現確率にアクセスできる. デフォルトで0.0
  int sigmasNum; // モデル上の記号の総数
} HMM;

/* HMM の基本的な操作関数 */
void init_HMM(HMM *hmm);
void copy_HMM(HMM *hmm1, HMM *hmm2);
void print_HMM(HMM *hmm);
int read_HMM(FILE *fp, HMM *hmm);

/* HMM から記号列をランダム生成 (課題1) */
int generate_symbolString(HMM *hmm, char *symbolString, int maxStringLen);
int generate_symbolString_with_stateOrder(HMM *hmm, char *symbolString, int maxStringLen, int *stateOrder);

/* HMM と記号列から確率が一番高い状態遷移を予測 (課題2) */
int viterbi_without_stateOrder(HMM *hmm, char *symbolString, double *maxP);
int viterbi(HMM *hmm, char *symbolString, int *stateOrder, double *maxP);

/* 記号列と状態遷移から HMM の各確率を予測 (課題3) */
int learning_HMM_based_on_stateOrder(HMM *hmm, char *symbolString, int *stateOrder);
int learning_HMM_based_on_sum_stateOrder(HMM *hmm, int symbolStringNum, char symbolString[MAX_SET][MAX_SYMBOL_STRING_LEN+1], int stateOrder[MAX_SET][MAX_SYMBOL_STRING_LEN+1]);

/* 記号列のみから HMM の各確率を予測 (課題4) */
int learning_HMM_only_sum_symbolStrings(HMM *hmm, char symbolString[MAX_SET][MAX_SYMBOL_STRING_LEN+1], int statesNum, int symbolStringNum);

#endif
