/**
 * 少しだけ C  ----------------------------------------------------------
 * 
 * 何年ぶりなんだろうか、忘れた。
 * お前の事ははっきり言えば、嫌いだ。
 * それは、お前の所為じゃない、オレの問題だ。
 * お前はなにも悪くない。
 * 
 * では、初心者なので Hello World から。
*/

# include <stdio.h>
# include <math.h>
# include <stdlib.h>

/**
 * 2の階乗を行う。
 * 危なそうなので強制的に10乗までしかやらない。
*/
double factorial(int n) {
    if(n > 10) {
        printf("Boo Boo Boo. Please more small number.\n");
        return -1.0;
    }
    double d = 1;
    for(double i = 1;i <= n ;i++) {
        d*=2;
    }
    return d;
}

int main( int argc, const char *argv[] ) {
    // double y = 4.0;
    if( argc != 2 ) {
        printf("Hello World\t %d\n",2023);
        printf("Hey you, Next time, Please input argument by number.\n");
        return 0;
    }
    int n = atoi(argv[1]);
    // べき乗関数
    double powret = pow(2.0,4.0);   // 指数関数、y を変数にするとコンパイルエラー、なぜだ、ハノイの塔を恐れているのか？
    printf("powret is %f \n",powret);

    double retfac = factorial(n);
    if( retfac > -1.0 ) {
        printf("retfac is %f \n",retfac);
    }
    return 0;
}