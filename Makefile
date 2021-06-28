# Makefile

CC=gcc
CFLAGS=-Wall -std=c99 -I.
SOURCE=hidden_markov_model.c

# テスト #
hmm_test: hmm_test.c $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $@.c $(SOURCE) -lm

# 課題1 #
task1: task1.c $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $@.c $(SOURCE) -lm

# 課題2 #
task2: task2.c $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $@.c $(SOURCE) -lm

# 課題3 #
task3: task3.c $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $@.c $(SOURCE) -lm

# 課題4 #
task4: task4.c $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $@.c $(SOURCE) -lm
