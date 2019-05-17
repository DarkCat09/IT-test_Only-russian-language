#include "TXLib.h"
#include <iostream>
#include <fstream>
using namespace std;

const COLORREF BISQUECOLOR = RGB(224, 203, 105);
const int SELECT_WIDTH = 43;
const int SELECT_HEIGHT = 44;

int ans = 0;
int scores = 0;
int userAnswers[12];
bool testIsComplete = false;
char* backgndImage = new char[50];

struct Quest
{
    string str;
    string answers[4];
    int rightAnswer;
    char pictureN[5];
    HDC pict;
};

struct Screen
{
    int x;
    int y;
    int middleX;
    int middleY;
};

void background(COLORREF color);
void drawMenu(Screen canvas);
void getQuestionsAndAnswers(Quest* qs_array);

int main()
{
    txCreateWindow (800, 600);
    char qNumber_str[5];
    Quest questions[11];

    ifstream bckgnd("backgroundOfTest.conf");
    char buff[50];

    bckgnd.getline(buff, 50);
    backgndImage = buff;

    bckgnd.close();

    HDC pictSelected     = txLoadImage("pictures\\selected.bmp");
    HDC pictIsntSelected = txLoadImage("pictures\\notselected.bmp");
    HDC nextButton       = txLoadImage("pictures\\nextbutton.bmp");
    HDC frameForAnswers  = txLoadImage("pictures\\framesForAnswers\\frame.bmp");
    HDC backgroundPic    = txLoadImage(("pictures\\backgnds\\" + (string)backgndImage + ".bmp").c_str());

    Screen canvas = {
        txGetExtentX(),     txGetExtentY(),
        txGetExtentX() / 2, txGetExtentY() / 2
    };

    getQuestionsAndAnswers(questions);

    txBegin();

    drawMenu(canvas);

    RECT startButton = {
        canvas.middleX - 100,
        canvas.middleY -  50,
        canvas.middleX + 100,
        canvas.middleY +  50
    };

    RECT next = {
        txGetExtentX() - 103,
        txGetExtentY() - 35,
        txGetExtentX() - 20,
        txGetExtentY() - 10
    };

    while (!(In(txMousePos(), startButton) && txMouseButtons() & 1)) {
        txSleep(10);
    }
    txSleep(500);

    RECT answerButtons[4] = {
        {
            80,  150 + 100 * 0,
            480, 200 + 100 * 0
        },

        {
            80,  150 + 100 * 1,
            480, 200 + 100 * 1
        },

        {
            80,  150 + 100 * 2,
            480, 200 + 100 * 2
        },

        {
            80,  150 + 100 * 3,
            480, 200 + 100 * 3
        }
    };


    while (!GetAsyncKeyState(VK_ESCAPE)) {

        for (int question = 0; question < 11; question++) {
            txBegin();
            sprintf(qNumber_str, "%d.", (question + 1));
            background(TX_WHITE);

            txBitBlt(txDC(), 0, 0, 800, 600, backgroundPic, 0, 0);

            txSetColor(TX_BLACK);
            txSelectFont("Calibri", 26);
            txBitBlt(txDC(), 0, 0, 800, 600, questions[question].pict, 0, 0);

            txSetFillColor(TX_WHITE);
            txRectangle(100, 100, 750, 130);
            txTextOut(100, 100, (qNumber_str + questions[question].str + "?").c_str());

            txBitBlt(txDC(), txGetExtentX() - 103, txGetExtentY() - 35, 83, 25, nextButton, 0, 0);

            for (int answerNumber = 0; answerNumber <= 3; answerNumber++) {
                txSetColor(TX_BLACK);
                txSetFillColor(TX_WHITE);

                txBitBlt(txDC(), 80, 150 + 100 * answerNumber, 400, 50, frameForAnswers, 0, 0);

                txTextOut(100, 160 + 100 * answerNumber, questions[question].answers[answerNumber].c_str());
                txTransparentBlt(txDC(), 480 - 50, 152 + 100 * answerNumber, SELECT_WIDTH, SELECT_HEIGHT, pictIsntSelected, 0, 0, TX_WHITE);
            }

            userAnswers[question] = 0;
            while (userAnswers[question] < 1) {
                txSleep(10);

                for (int answer_number = 0; answer_number <= 3; answer_number++) {

                    if (In(txMousePos(), answerButtons[answer_number]) && txMouseButtons() & 1) {
                        if (ans > 0) {
                            for (int ansNum = 0; ansNum <= 3; ansNum++) {
                                txTransparentBlt(txDC(), 480 - 50, 152 + 100 * ansNum, SELECT_WIDTH, SELECT_HEIGHT, pictIsntSelected, 0, 0, TX_WHITE);
                            }
                        }

                        while (txMouseButtons() & 1) {
                            txSleep(10);
                        }

                        ans = answer_number + 1;
                        txTransparentBlt(txDC(), 480 - 50, 152 + 100 * answer_number, SELECT_WIDTH, SELECT_HEIGHT, pictSelected, 0, 0, TX_WHITE);
                    }
                }

                if (((In(txMousePos(), next) && txMouseButtons() & 1) || GetAsyncKeyState(VK_RETURN)) && ans > 0) {
                    userAnswers[question] = ans;
                    while (txMouseButtons() & 1) {
                        txSleep(10);
                    }
                }

            }
            ans = 0;

            txEnd();
        }

        txSleep(1000);

        testIsComplete = true;
        break;
    }

    if (testIsComplete) {
        for (int indexOfQuestion = 0; indexOfQuestion < 11; indexOfQuestion++) {
            if (userAnswers[indexOfQuestion] == questions[indexOfQuestion].rightAnswer) {
                scores++;
            }
        }
    }

    txSetColor(TX_WHITE, 2);
    txSetFillColor(TX_LIGHTGRAY);
    Win32::RoundRect(txDC(), txGetExtentX()/2 - 300, txGetExtentY()/2 - 180, txGetExtentX()/2 + 300, txGetExtentY()/2 + 170, 30, 30);

    char scoreStr[5];
    sprintf(scoreStr, "%d", scores);
    txTextOut(txGetExtentX()/2 - 180, txGetExtentY()/2 - 10, ("Результат: " + (string)scoreStr + " из 11.").c_str());

    txSleep(500);

    txEnd();

    txDeleteDC(pictSelected);
    txDeleteDC(pictIsntSelected);
    txDeleteDC(nextButton);
    txDeleteDC(frameForAnswers);

    for (int ind = 0; ind < 11; ind++) {
        txDeleteDC(questions[ind].pict);
    }

    txDeleteDC(backgroundPic);

    return 0;
}

void background(COLORREF color)
{
    txSetFillColor(color);
    txClear();
}

void drawMenu(Screen canvas)
{
    txSelectFont("Calibri", 40);
    txSetColor(TX_WHITE);
    txTextOut(canvas.middleX - 50, canvas.middleY - 150, "IT-тест");

    txSetColor(TX_BLACK);
    txSetFillColor(BISQUECOLOR);

    txRectangle(
        canvas.middleX - 100,
        canvas.middleY -  50,
        canvas.middleX + 100,
        canvas.middleY +  50
    );
}

void getQuestionsAndAnswers(Quest* qs_array)
{
    ifstream qaFile("qa.txt");
    char buff[100];
    string stroka;

    for (int index = 0; index < 11; index++) {
        getline (qaFile, stroka);

        char* text = new char[200];
        strcpy(text, stroka.c_str());
        qs_array[index].str = text;

        for (int indexOfAnswer = 0; indexOfAnswer < 4; indexOfAnswer++) {
            qaFile.getline(buff, 100);
            string text = buff;
            qs_array[index].answers[indexOfAnswer] = text;
        }

        qaFile.getline(buff, 100);
        qs_array[index].rightAnswer = atoi(buff);
    }
    qaFile.close();

    for (int quest_n = 0; quest_n < 11; quest_n++) {
        sprintf(qs_array[quest_n].pictureN, "%d", quest_n);

        char* picAdress = new char[195];
        strcpy(picAdress, "pictures\\picturesForQuestions\\");
        strcat(picAdress, qs_array[quest_n].pictureN);
        strcat(picAdress, ".bmp");
        qs_array[quest_n].pict = txLoadImage(picAdress);
    }
}

